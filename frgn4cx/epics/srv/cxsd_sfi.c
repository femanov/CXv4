#include "cxsd_hw.h"
#include "cxsd_hwP.h"
#include "cxsd_frontend.h"
#include "cxsd_modmgr.h"
#include "cxsd_logger.h"
#include "cxsd_access.h"

#include "cxsd_sfi.h"

#include "math.h"  // for round()


enum {RDS_MAX_COUNT = 20};
enum
{
    I_PHYS_COUNT = 2,
};

enum
{
    MON_KIND_VAL = 0,
    MON_KIND_RAW = 1,
};

enum
{
    MONITOR_EVMASK =
        CXSD_HW_CHAN_EVMASK_UPDATE   |
        CXSD_HW_CHAN_EVMASK_RDSCHG
};

//////////////////////////////////////////////////////////////////////

static cxsd_sfi_mon_update_cb_t mon_update = NULL;

void cxsd_sfi_set_mon_update(cxsd_sfi_mon_update_cb_t cb)
{
    mon_update = cb;
}

//////////////////////////////////////////////////////////////////////

typedef struct
{
    int            in_use;
    int            kind;
    cxsd_cpntid_t  cpid;
    cxsd_gchnid_t  gcid;
    const char    *name;
    cxdtype_t      ext_dtype;

    void          *PV_ptr;

    int            phys_count;
    int            phys_n_allocd;    // in PAIRS -- i.e., malloc(sizeof(double)*phys_n_allocd*2)
    double         imm_phys_rds[I_PHYS_COUNT * 2];
    double        *alc_phys_rds;

    void          *current_val;      // Pointer to current-value-buffer (if !=NULL)...
    int            current_nelems;   // ...and # of units in it
    CxAnyVal_t     valbuf;           // Buffer for small-sized values; current_val=NULL in such cases
    cxdtype_t      current_dtype;    // Current data type
    rflags_t       current_rflags;
    cx_time_t      current_timestamp;
    int            val_is_present;   // Either valbuf or current_val contain a valid, ready-for-use value
} moninfo_t;

static moninfo_t       *sfi_monitors_list           = NULL;
static int              sfi_monitors_list_allocd    = 0;

static cxsd_gchnid_t   *sfi_periodics               = NULL;
static int              sfi_periodics_allocd        = 0;
static int              sfi_periodics_used          = 0;
static int              sfi_periodics_needs_rebuild = 0;

// GetMonSlot()
GENERIC_SLOTARRAY_DEFINE_GROWING(static, Mon, moninfo_t,
                                 sfi_monitors, in_use, 0, 1,
                                 0, 100, 0,
                                 , , void)

static void MonEvproc(int            uniq,
                      void          *privptr1,
                      cxsd_gchnid_t  gcid,
                      int            reason,
                      void          *privptr2);

static void RlsMonSlot(int id)
{
  moninfo_t *mp = AccessMonSlot(id);

    if (mp->in_use == 0) return; /*!!! In fact, an internal error */
    mp->in_use = 0;

    CxsdHwDelChanEvproc(0, NULL,
                        mp->gcid, MONITOR_EVMASK,
                        MonEvproc, lint2ptr(id));

    if (mp->cpid >= 0)
    {
        sfi_periodics_used--;
        sfi_periodics_needs_rebuild = 1;
    }

    mp->PV_ptr = NULL;

    safe_free(mp->name);
    safe_free(mp->alc_phys_rds);
    safe_free(mp->current_val);
}

//////////////////////////////////////////////////////////////////////

static int GetChanInfo(const char    *name,
                       cxsd_cpntid_t *cpid_p,
                       cxsd_gchnid_t *gcid_p,
                       int           *phys_count_p,
                       double        *rds_buf,
                       int            rds_buf_cap)
{
  int            kind;
  char           namebuf[1000];
  const char    *src_p;
  char          *dst_p;
  char          *dot_p;
  size_t         namelen;
  char           ch;

  cxsd_cpntid_t  cpid;
  cxsd_gchnid_t  gcid;
  int            phys_count;
 
    if (name == NULL) return -1;

    // a. Copy name to namebuf, replacing ':'s with '.'s
    //    and noticing the position of last '.'
    for (src_p = name, dst_p = namebuf, namelen = 0, dot_p = NULL;
         *src_p != '\0'  &&             namelen < (sizeof(namebuf) - 1);
         src_p++,      dst_p++,         namelen++)
    {
        ch = *src_p;
        if (ch == ':') ch = '.';
        *dst_p = ch;
        if (ch == '.') dot_p = dst_p;
    }
    *dst_p = '\0';
    // b. Check for special suffixes
    kind = MON_KIND_VAL;
    if (dot_p != NULL  &&  dot_p != name)
    {
        if      (strcasecmp(dot_p + 1, "_raw")      == 0)
            kind = MON_KIND_RAW;
        /*
        else if (strcasecmp(dot_p + 1, "_rangemin") == 0)
            kind = MON_KIND_RANGEMIN;
        */
        else
            goto LEAVE_NAME_AS_IS;

        *dot_p = '\0';

 LEAVE_NAME_AS_IS:;
    }

        /* Does this name exist? */
    if ((cpid = CxsdHwResolveChan(namebuf, &gcid,
                                  &phys_count, rds_buf, rds_buf_cap,
                                  NULL, NULL, NULL, NULL,
                                  NULL, NULL, NULL, NULL)) < 0)
        return -1;

    if (kind != MON_KIND_VAL) phys_count = 0;

    if (cpid_p       != NULL) *cpid_p       = cpid;
    if (gcid_p       != NULL) *gcid_p       = gcid;
    if (phys_count_p != NULL) *phys_count_p = phys_count;

    return kind;
}

//////////////////////////////////////////////////////////////////////

static void StoreRDs(moninfo_t *mp, int phys_count, double *rds)
{
    if (phys_count > 0)
    {
        if (mp->alc_phys_rds != NULL  ||  phys_count > I_PHYS_COUNT)
        {
            if (GrowUnitsBuf(&(mp->alc_phys_rds), &(mp->phys_n_allocd),
                             phys_count, sizeof(double) * 2) < 0)
            {
                /*!!!Bark? */
                return;
            }
        }
        memcpy(mp->alc_phys_rds != NULL? mp->alc_phys_rds : mp->imm_phys_rds,
               rds, phys_count * sizeof(double) * 2);
    }
    mp->phys_count = phys_count;
}

static void SendMonitor(int mid)
{
  moninfo_t      *mp    = AccessMonSlot(mid);

    mp->val_is_present = 0;

    if (mon_update != NULL) mon_update(mid, mp->PV_ptr);
}

static void MonEvproc(int            uniq,
                      void          *privptr1,
                      cxsd_gchnid_t  gcid,
                      int            reason,
                      void          *privptr2)
{
  int          id = ptr2lint(privptr2);
  moninfo_t   *mp = AccessMonSlot(id);

  cxsd_gchnid_t  dummy_gcid;
  int            phys_count;
  double         rds_buf[RDS_MAX_COUNT*2];

    if      (reason == CXSD_HW_CHAN_R_UPDATE)
    {
        SendMonitor(id);
        //if (mon_update != NULL) mon_update(id, mp->PV_ptr);
    }
    else if (reason == CXSD_HW_CHAN_R_RDSCHG)
    {
        if (CxsdHwGetCpnProps(mp->cpid, &dummy_gcid,
                              &phys_count, rds_buf, RDS_MAX_COUNT,
                              NULL, NULL, NULL, NULL,
                              NULL, NULL, NULL, NULL) < 0) return;
        StoreRDs(mp, phys_count, rds_buf);
    }
}

static int CxsdHwIsChanValReady(int gcid)
{
  cxsd_hw_chan_t        *chn_p = cxsd_hw_channels + gcid;

    if (gcid <= 0  ||  gcid >= cxsd_hw_numchans) return -1;

    return
        chn_p->is_internal
        ||
        (chn_p->timestamp.sec != CX_TIME_SEC_NEVER_READ
         &&
         (chn_p->rw  ||
          (chn_p->is_autoupdated       &&
           chn_p->fresh_age.sec  == 0  &&
           chn_p->fresh_age.nsec == 0)
         )
        );
}


//////////////////////////////////////////////////////////////////////

static int IsSuitableForExport(cxsd_gchnid_t gcid)
{
  cxdtype_t      dtype;

    dtype = cxsd_hw_channels[gcid].dtype;

    // For now, only scalars
//    if (cxsd_hw_channels[gcid].max_nelems != 1) return 0;

    // Limit types...
    dtype = cxsd_hw_channels[gcid].dtype;
    if (reprof_cxdtype(dtype) != CXDTYPE_REPR_INT    &&
        reprof_cxdtype(dtype) != CXDTYPE_REPR_FLOAT  &&
        reprof_cxdtype(dtype) != CXDTYPE_REPR_TEXT)
        return 0;
    // int64 is NOT supported by EPICS3
    if (reprof_cxdtype(dtype) == CXDTYPE_REPR_INT  &&
        sizeof_cxdtype(dtype) > sizeof(int32))
        return 0;

    return 1;
}

int  cxsd_sfi_pvExistTest(const char *name)
{
  cxsd_gchnid_t  gcid;

    if (GetChanInfo(name, NULL, &gcid, NULL, NULL, 0) < 0) return 0;

    return IsSuitableForExport(gcid);
}

static int mon_eq_checker(moninfo_t *mp, void *privptr)
{
    return strcasecmp(mp->name, privptr) == 0;
}
int  cxsd_sfi_pvAttach   (const char *name, int *mid_p)
{
  int             mid;
  moninfo_t      *mp;
  cxsd_hw_chan_t *chn_p;
  cxdtype_t       chn_dtype;

  int             kind;
  cxsd_cpntid_t   cpid;
  cxsd_gchnid_t   gcid;
  int             phys_count;
  double          rds_buf[RDS_MAX_COUNT*2];
  int             evmask;

  size_t         csize;                // Channel data size

    /* Does this name exist? */
    if ((kind = GetChanInfo(name, &cpid, &gcid,
                            &phys_count, rds_buf, RDS_MAX_COUNT)) < 0)
        return -1;

    chn_p = cxsd_hw_channels + gcid;
    chn_dtype = chn_p->dtype;

//fprintf(stderr, "\tgcid=%d cpid=%d\n", gcid, cpid);
    /* Isn't it already monitored? */
    mid = ForeachMonSlot(mon_eq_checker, name);
    if (mid >= 0)
    {
        *mid_p = mid;
        return CXSD_SFI_ALREADY;
    }

    /* Check compatibility */
    if (!IsSuitableForExport(gcid)) return CXSD_SFI_ERROR;

    /* Allocate a cell */
    mid = GetMonSlot();
    if (mid < 0) return -1;
    mp  = AccessMonSlot(mid);
    mp->cpid = -1; // Prevent RlsMonSlot() from "periodics_used--" if called before "periodics_used++"
    if ((mp->name = strdup(name)) == NULL)
    {
        RlsMonSlot(mid);
        return CXSD_SFI_noMemory;
    }

    /* Decide about "externally-visible dtype"... */
    mp->ext_dtype = chn_dtype;
    if (kind == MON_KIND_VAL  &&
        phys_count != 0       &&  
        (reprof_cxdtype(chn_dtype) == CXDTYPE_REPR_INT  ||
         reprof_cxdtype(chn_dtype) == CXDTYPE_REPR_FLOAT)  &&
        chn_dtype != CXDTYPE_SINGLE) mp->ext_dtype = CXDTYPE_DOUBLE;

    /* ...and allocate a buffer if required */
    csize = (sizeof_cxdtype(chn_dtype) > sizeof_cxdtype(mp->ext_dtype)? 
             sizeof_cxdtype(chn_dtype) : sizeof_cxdtype(mp->ext_dtype)) * chn_p->max_nelems;
    if (csize > sizeof(mp->valbuf))
    {
        if ((mp->current_val = malloc(csize)) == NULL)
        {
            RlsMonSlot(mid);
            return CXSD_SFI_noMemory;
        }
        bzero(mp->current_val, csize);
    }

    /* Add a cell to periodics[] */
    if (sfi_periodics_allocd <= sfi_periodics_used  &&
        GrowUnitsBuf(&(sfi_periodics),
                     &(sfi_periodics_allocd),
                     sfi_periodics_allocd + 100,
                     sizeof(*(sfi_periodics))) < 0)
    {
        RlsMonSlot(mid);
        return CXSD_SFI_noMemory;
    }
    sfi_periodics_used++;
    sfi_periodics_needs_rebuild = 1;
                        
    /* Fill */
    mp->kind = kind;
    mp->cpid = cpid;
    mp->gcid = gcid;
    StoreRDs(mp, phys_count, rds_buf);

    /* Call "initial read" */
    /* Note: it is crucial for this to be BEFORE CxsdHwAddChanEvproc(),
             so that in case of immediate-answer there is no evproc yet to be called,
             thus no update will be posted for a not-yet-finished-to-register PV */
    CxsdHwDoIO(0/*!!!ID!!!*/, DRVA_READ,
               1, &gcid,
               NULL, NULL, NULL);

    evmask = MONITOR_EVMASK;
    if (kind != MON_KIND_VAL  &&
        kind != MON_KIND_RAW) evmask &=~ CXSD_HW_CHAN_EVMASK_UPDATE;
    if (kind != MON_KIND_VAL) evmask &=~ CXSD_HW_CHAN_EVMASK_RDSCHG;

    /* Subscribe to updates */
    if (CxsdHwAddChanEvproc(0, NULL,
                            mp->gcid, evmask,
                            MonEvproc, lint2ptr(mid)) < 0)
    {
        RlsMonSlot(mid);
        return CXSD_SFI_noMemory;
    }

    *mid_p = mid;

    return CXSD_SFI_SUCCESS;
}

int  cxsd_sfi_set_PV_ptr (int mid, void *PV_ptr)
{
  moninfo_t *mp = AccessMonSlot(mid);

    /*!!!Check!!!*/

    mp->PV_ptr = PV_ptr;

    return 0;
}

int  cxsd_sfi_get_PV_ptr (int mid, void **PV_ptr_p)
{
  moninfo_t *mp = AccessMonSlot(mid);

    /*!!!Check!!!*/

    *PV_ptr_p = mp->PV_ptr;

    return 0;
}

const char *
     cxsd_sfi_get_name   (int mid)
{
  moninfo_t      *mp    = AccessMonSlot(mid);

    /*!!!Check!!!*/

    return mp->name;
}

int  cxsd_sfi_get_info   (int mid,
                          cxdtype_t *dtype_p, int *max_nelems_p, int *is_rw_p)
{
  moninfo_t      *mp    = AccessMonSlot(mid);
  cxsd_hw_chan_t *chn_p;

    /*!!!Check!!!*/

    chn_p = cxsd_hw_channels + mp->gcid;

    if (dtype_p      != NULL) *dtype_p      = mp->ext_dtype;
    if (max_nelems_p != NULL) *max_nelems_p = chn_p->max_nelems;
    if (is_rw_p      != NULL) *is_rw_p      = chn_p->rw  &&
                                              (mp->kind == MON_KIND_VAL  ||  mp->kind == MON_KIND_RAW);

    return 0;
}

static int eng2opr(cxdtype_t src_dtype, int src_nelems,     void *src_data,
                   int phys_count, double *phys_rds,
                   cxdtype_t dst_dtype, int dst_max_nelems, void *dst_data)
{
  int              nels;
  uint8           *src;
  uint8           *dst;

  int              srpr;
  size_t           ssiz;
  int              repr;
  size_t           size;
  int              is_text_int_compat;

  double           v;
  int              n;
  double          *rdp;

//fprintf(stderr, "%s src_dtype=%d src_nelems=%d dst_dtype=%d max_nelems=%d\n", __FUNCTION__, src_dtype, src_nelems, dst_dtype, dst_max_nelems);
    /* An individual check for CXDTYPE_UNKNOWN, since it can't be converted to anything */
    if (src_dtype == CXDTYPE_UNKNOWN) return -1;

    /* Check nelems */
    if      (src_nelems < 0)
    {
        /*!!!Bark? */
        return -1;
    }
    else if (dst_max_nelems == 1  &&  src_nelems != 1)
    {
        /*!!!Bark? */
        return -1;
    }
    else if (src_nelems > dst_max_nelems)
    {
        /* Too many?  Okay, just limit */
        /*!!!Bark? */
        src_nelems = dst_max_nelems;
    }
    nels = src_nelems;

    /* Store data */
    src = src_data;
    dst = dst_data;

    srpr = reprof_cxdtype(src_dtype);
    ssiz = sizeof_cxdtype(src_dtype);
    repr = reprof_cxdtype(dst_dtype);
    size = sizeof_cxdtype(dst_dtype);

    // Treat TEXT and INT of same size as compatible (e.g. INT8 and TEXT (byte and char))
    is_text_int_compat =
        (ssiz == size)  &&
        ((srpr == CXDTYPE_REPR_INT   &&  repr == CXDTYPE_REPR_TEXT)  ||
         (srpr == CXDTYPE_REPR_TEXT  &&  repr == CXDTYPE_REPR_INT));

    /* a. Incompatible? */
    if      (!is_text_int_compat     &&
             src_dtype != dst_dtype  &&
             (
              (srpr != CXDTYPE_REPR_INT  &&  srpr != CXDTYPE_REPR_FLOAT)
              ||
              (repr != CXDTYPE_REPR_INT  &&  repr != CXDTYPE_REPR_FLOAT)
             )
            )
    {
        return -2;
    }
    /* b. Simple copy? */
    else if (is_text_int_compat  ||
             (
              src_dtype == dst_dtype  &&
              (
               phys_count == 0
               ||
               (repr != CXDTYPE_REPR_INT  &&  repr != CXDTYPE_REPR_FLOAT)
              )
             )
            )
    {
        if (nels != 0) memcpy(dst, src, size * nels);
    }
    /* c. Conversion */
    else
    {
        rdp = phys_rds;
        rdp += phys_count * 2; // Is later compensated with "rdp -= n*2"

//fprintf(stderr, "dtype=%d nels=%d\n", dtypes[x], nels);
        while (nels > 0)
        {
            // Read datum, converting to double
            switch (src_dtype)
            {
                case CXDTYPE_INT32:  v = *((  int32*)src);     break;
                case CXDTYPE_UINT32: v = *(( uint32*)src);     break;
                case CXDTYPE_INT16:  v = *((  int16*)src);     break;
                case CXDTYPE_UINT16: v = *(( uint16*)src);     break;
                case CXDTYPE_DOUBLE: v = *((float64*)src);     break;
                case CXDTYPE_SINGLE: v = *((float32*)src);     break;
                case CXDTYPE_INT64:  v = *((  int64*)src);     break;
                case CXDTYPE_UINT64: v = *(( uint64*)src);     break;
                case CXDTYPE_INT8:   v = *((  int8 *)src);     break;
                default:             v = *(( uint8 *)src);     break;
            }
            src += ssiz;

//fprintf(stderr, "v=%8.3f", v);
            // Do conversion
            if (phys_count != 0)
            {
                n    = phys_count;
                rdp -= n*2;
                while (n > 0)
                {
//fprintf(stderr, " v= %8.3f / %8.3f - %8.3f\n", v, rdp[0],  rdp[1]);
                    v = v / rdp[0] - rdp[1];
                    rdp += 2;
                    n--;
                }
            }
//fprintf(stderr, " -> %8.3f\n", v);

            // Store datum, converting from double
            switch (dst_dtype)
            {
                case CXDTYPE_INT32:      *((  int32*)dst) = round(v); break;
                case CXDTYPE_UINT32:     *(( uint32*)dst) = round(v); break;
                case CXDTYPE_INT16:      *((  int16*)dst) = round(v); break;
                case CXDTYPE_UINT16:     *(( uint16*)dst) = round(v); break;
                case CXDTYPE_DOUBLE:     *((float64*)dst) =       v;  break;
                case CXDTYPE_SINGLE:     *((float32*)dst) =       v;  break;
                case CXDTYPE_INT64:      *((  int64*)dst) = round(v); break;
                case CXDTYPE_UINT64:     *(( uint64*)dst) = round(v); break;
                case CXDTYPE_INT8:       *((  int8 *)dst) = round(v); break;
                default:                 *(( uint8 *)dst) = round(v); break;
            }
            dst += size;

            nels--;
        }
    }

    return src_nelems;
}

static int opr2eng(cxdtype_t src_dtype, int src_nelems,     void *src_data,
                   int phys_count, double *phys_rds,
                   cxdtype_t dst_dtype, int dst_max_nelems, void *dst_data)
{
  int              nels;
  uint8           *src;
  uint8           *dst;

  int              srpr;
  size_t           ssiz;
  int              repr;
  size_t           size;
  int              is_text_int_compat;

  double           v;
  int              n;
  double          *rdp;

//fprintf(stderr, "%s src_dtype=%d src_nelems=%d dst_dtype=%d max_nelems=%d\n", __FUNCTION__, src_dtype, src_nelems, dst_dtype, dst_max_nelems);
    /* An individual check for CXDTYPE_UNKNOWN, since it can't be converted to anything */
    if (src_dtype == CXDTYPE_UNKNOWN) return -1;

    /* Check nelems */
    if      (src_nelems < 0)
    {
        /*!!!Bark? */
        return -1;
    }
    else if (dst_max_nelems == 1  &&  src_nelems != 1)
    {
        /*!!!Bark? */
        return -1;
    }
    else if (src_nelems > dst_max_nelems)
    {
        /* Too many?  Okay, just limit */
        /*!!!Bark? */
        src_nelems = dst_max_nelems;
    }
    nels = src_nelems;

    /* Store data */
    src = src_data;
    dst = dst_data;

    srpr = reprof_cxdtype(src_dtype);
    ssiz = sizeof_cxdtype(src_dtype);
    repr = reprof_cxdtype(dst_dtype);
    size = sizeof_cxdtype(dst_dtype);

    // Treat TEXT and INT of same size as compatible (e.g. INT8 and TEXT (byte and char))
    is_text_int_compat =
        (ssiz == size)  &&
        ((srpr == CXDTYPE_REPR_INT   &&  repr == CXDTYPE_REPR_TEXT)  ||
         (srpr == CXDTYPE_REPR_TEXT  &&  repr == CXDTYPE_REPR_INT));

    /* a. Incompatible? */
    if      (!is_text_int_compat     &&
             src_dtype != dst_dtype  &&
             (
              (srpr != CXDTYPE_REPR_INT  &&  srpr != CXDTYPE_REPR_FLOAT)
              ||
              (repr != CXDTYPE_REPR_INT  &&  repr != CXDTYPE_REPR_FLOAT)
             )
            )
    {
        return -2;
    }
    /* b. Simple copy? */
    else if (is_text_int_compat  ||
             (
              src_dtype == dst_dtype  &&
              (
               phys_count == 0
               ||
               (repr != CXDTYPE_REPR_INT  &&  repr != CXDTYPE_REPR_FLOAT)
              )
             )
            )
    {
        if (nels != 0) memcpy(dst, src, size * nels);
    }
    /* c. Conversion */
    else
    {
        rdp = phys_rds;

//fprintf(stderr, "dtype=%d nels=%d\n", dtypes[x], nels);
        while (nels > 0)
        {
            // Read datum, converting to double
            switch (src_dtype)
            {
                case CXDTYPE_INT32:  v = *((  int32*)src);     break;
                case CXDTYPE_UINT32: v = *(( uint32*)src);     break;
                case CXDTYPE_INT16:  v = *((  int16*)src);     break;
                case CXDTYPE_UINT16: v = *(( uint16*)src);     break;
                case CXDTYPE_DOUBLE: v = *((float64*)src);     break;
                case CXDTYPE_SINGLE: v = *((float32*)src);     break;
                case CXDTYPE_INT64:  v = *((  int64*)src);     break;
                case CXDTYPE_UINT64: v = *(( uint64*)src);     break;
                case CXDTYPE_INT8:   v = *((  int8 *)src);     break;
                default:             v = *(( uint8 *)src);     break;
            }
            src += ssiz;

//fprintf(stderr, "v=%8.3f", v);
            // Do conversion
            if (phys_count != 0)
            {
                n    = phys_count;
                rdp += n*2; // Move to the end of {R,D}-list
                while (n > 0)
                {
                    rdp -= 2;
                    v = (v + rdp[1]) * rdp[0];
                    n--;
                }
            }
//fprintf(stderr, " -> %8.3f\n", v);

            // Store datum, converting from double
            switch (dst_dtype)
            {
                case CXDTYPE_INT32:      *((  int32*)dst) = round(v); break;
                case CXDTYPE_UINT32:     *(( uint32*)dst) = round(v); break;
                case CXDTYPE_INT16:      *((  int16*)dst) = round(v); break;
                case CXDTYPE_UINT16:     *(( uint16*)dst) = round(v); break;
                case CXDTYPE_DOUBLE:     *((float64*)dst) =       v;  break;
                case CXDTYPE_SINGLE:     *((float32*)dst) =       v;  break;
                case CXDTYPE_INT64:      *((  int64*)dst) = round(v); break;
                case CXDTYPE_UINT64:     *(( uint64*)dst) = round(v); break;
                case CXDTYPE_INT8:       *((  int8 *)dst) = round(v); break;
                default:                 *(( uint8 *)dst) = round(v); break;
            }
            dst += size;

            nels--;
        }
    }

    return src_nelems;
}

int  cxsd_sfi_get_data   (int mid,
                          cxdtype_t *dtype_p, int *nelems_p,       void **data_p,
                          rflags_t *rflags_p, cx_time_t *timestamp_p)
{
  moninfo_t      *mp    = AccessMonSlot(mid);
  cxsd_hw_chan_t *chn_p;

  cxdtype_t       dtype;
  int             nelems;
  void           *data;
  rflags_t        rflags;
  cx_time_t       timestamp;

  double          v;
  int             n;
  double         *rdp;

  void           *dst;

    /*!!!Check!!!*/

    chn_p = cxsd_hw_channels + mp->gcid;

#if 1
    if (mp->val_is_present)
    {
        dtype     = mp->current_dtype;
        nelems    = mp->current_nelems;
        data      = mp->current_val != NULL? mp->current_val : (void*)&(mp->valbuf);
        rflags    = mp->current_rflags;
        timestamp = mp->current_timestamp;
    }
    else
    {
        if (mp->kind == MON_KIND_VAL  ||
            mp->kind == MON_KIND_RAW)
        {
            if (mp->phys_count == 0)
            {
                dtype     = chn_p->current_dtype;
                nelems    = chn_p->current_nelems;
                data      = chn_p->current_val;
                rflags    = chn_p->rflags;
                timestamp = chn_p->timestamp;
                goto DATA_READY;
            }

            rdp = mp->alc_phys_rds;
            if (rdp == NULL) rdp = mp->imm_phys_rds;

            dst = mp->current_val;
            if (dst == NULL) dst = (void*)&(mp->valbuf);

            dtype = mp->ext_dtype;

            nelems = eng2opr(chn_p->current_dtype, chn_p->current_nelems, chn_p->current_val,
                             mp->phys_count, rdp,
                             mp->ext_dtype,        chn_p->max_nelems,     dst);
            if (nelems < 0) return nelems;

            mp->current_dtype     = dtype;
            mp->current_nelems    = nelems;
            data                  = dst;
            mp->current_rflags    = rflags    = chn_p->rflags;
            mp->current_timestamp = timestamp = chn_p->timestamp;
        }
        else return -1;

        mp->val_is_present = 1;
    }

 DATA_READY:;
    if (dtype_p     != NULL) *dtype_p     = dtype;
    if (nelems_p    != NULL) *nelems_p    = nelems;
    if (data_p      != NULL) *data_p      = data;
    if (rflags_p    != NULL) *rflags_p    = rflags;
    if (timestamp_p != NULL) *timestamp_p = timestamp;
#else
    if (mp->kind == MON_KIND_VAL  ||
        mp->kind == MON_KIND_RAW)
    {
        dtype     = chn_p->current_dtype;
        nelems    = chn_p->current_nelems;
        data      = chn_p->current_val;
        rflags    = chn_p->rflags;
        timestamp = chn_p->timestamp;

        if (mp->phys_count != 0)
        {
            // Get a double
            switch (dtype)
            {
                case CXDTYPE_INT32:  v = *((  int32*)data);     break;
                case CXDTYPE_UINT32: v = *(( uint32*)data);     break;
                case CXDTYPE_INT16:  v = *((  int16*)data);     break;
                case CXDTYPE_UINT16: v = *(( uint16*)data);     break;
                case CXDTYPE_DOUBLE: v = *((float64*)data);     break;
                case CXDTYPE_SINGLE: v = *((float32*)data);     break;
                case CXDTYPE_INT64:  v = *((  int64*)data);     break;
                case CXDTYPE_UINT64: v = *(( uint64*)data);     break;
                case CXDTYPE_INT8:   v = *((  int8 *)data);     break;
                default:             v = *(( uint8 *)data);     break;
            }

            // Perform RD-conversion
            /*!!! That's a SCALAR-only code!!! */
            rdp = mp->alc_phys_rds;
            if (rdp == NULL) rdp = mp->imm_phys_rds;
            n   = mp->phys_count;
            while (n > 0)
            {
                v = v / rdp[0] - rdp[1];
                rdp += 2;
                n--;
            }

            // Store result
            if (dtype == CXDTYPE_SINGLE)
            {
                mp->valbuf.f32 = v;
                data = &(mp->valbuf.f32);
            }
            else
            {
                dtype = CXDTYPE_DOUBLE;
                mp->valbuf.f64 = v;
                data = &(mp->valbuf.f64);
            }
        }
    }
    else return -1;

    if (dtype_p     != NULL) *dtype_p     = dtype;
    if (nelems_p    != NULL) *nelems_p    = nelems;
    if (data_p      != NULL) *data_p      = data;
    if (rflags_p    != NULL) *rflags_p    = rflags;
    if (timestamp_p != NULL) *timestamp_p = timestamp;
#endif

    return 0;
}

int  cxsd_sfi_do_write   (int mid, 
                          cxdtype_t  dtype,   int  nelems,   const void  *data)
{
  moninfo_t      *mp    = AccessMonSlot(mid);
  cxsd_hw_chan_t *chn_p;

  cxdtype_t       dst_dtype;
  double         *rdp;
  void           *dst;

  int             eff_nelems;

    /*!!!Check!!!*/

    chn_p = cxsd_hw_channels + mp->gcid;

    if (mp->kind == MON_KIND_VAL  ||
        mp->kind == MON_KIND_RAW)
    {
        if (mp->phys_count == 0)
        {
            return CxsdHwDoIO(0/*!!!ID!!!*/, DRVA_WRITE,
                              1, &(mp->gcid), &dtype, &nelems, &data);
        }

        // Drop the "present" flag immediately
        mp->val_is_present = 0;

        // Prepare parameters
        dst_dtype = chn_p->dtype;
        if (dst_dtype == CXDTYPE_UNKNOWN) return -1;

        rdp = mp->alc_phys_rds;
        if (rdp == NULL) rdp = mp->imm_phys_rds;

        dst = mp->current_val;
        if (dst == NULL) dst = (void*)&(mp->valbuf);

        // Convert
        eff_nelems = opr2eng(dtype,                nelems,                data,
                             mp->phys_count, rdp,
                             dst_dtype,            chn_p->max_nelems,     dst);
        if (eff_nelems < 0) return eff_nelems;

        // Send
        return CxsdHwDoIO(0/*!!!ID!!!*/, DRVA_WRITE,
                          1, &(mp->gcid), &dst_dtype, &eff_nelems, &dst);
    }
    else return -1;
}

void cxsd_sfi_term_f     (void)
{
    safe_free(sfi_periodics);
    sfi_periodics               = NULL;
    sfi_periodics_allocd        = 0;
    sfi_periodics_used          = 0;
    sfi_periodics_needs_rebuild = 0;

    safe_free(sfi_monitors_list);
    sfi_monitors_list        = NULL;
    sfi_monitors_list_allocd = 0;
}

static int RecordAMonitor(moninfo_t *mp, void *privptr)
{
  cxsd_gchnid_t **wp_p = privptr;

    **wp_p = mp->gcid;
    (*wp_p)++;

    return 0;
}
int  cxsd_sfi_request_subscription(void)
{
  cxsd_gchnid_t *wp;

    if (sfi_periodics_used == 0) return 0;

    if (sfi_periodics_needs_rebuild)
    {
        wp = sfi_periodics;
        ForeachMonSlot(RecordAMonitor, &wp);
        sfi_periodics_needs_rebuild = 0;
    }

//fprintf(stderr, "  sfi_periodics_used=%d\n", sfi_periodics_used);
    CxsdHwDoIO(0/*!!!ID!!!*/, DRVA_READ,
               sfi_periodics_used, sfi_periodics,
               NULL, NULL, NULL);

    return 0;
}

