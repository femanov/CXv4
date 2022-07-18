#include <ctype.h>

#include "cxsd_driver.h"
#include "cda.h"

#include "cxsd_hwP.h"


typedef struct
{
    cda_context_t  cid;
    cda_dataref_t  chan_r;
} privrec_t;

static void chan_evproc(int            devid,
                        void          *devptr,
                        cda_dataref_t  ref,
                        int            reason,
                        void          *info_ptr,
                        void          *privptr2)
{
  privrec_t  *me = devptr;
  cxdtype_t   dtype;
  int         nelems;
  void       *buf;
  rflags_t    rflags;
  cx_time_t   timestamp;
  cx_time_t   fresh_age;
  const char *src_p;

  int         rest;

  uint8       min_uint8,   max_uint8;
  int8        min_int8,    max_int8;
  uint16      min_uint16,  max_uint16;
  int16       min_int16,   max_int16;
  uint32      min_uint32,  max_uint32;
  int32       min_int32,   max_int32;
  uint64      min_uint64,  max_uint64;
  int64       min_int64,   max_int64;
  float32     min_float32, max_float32;
  float64     min_float64, max_float64;

  int64       sum_i64;
  float64     sum_f64;
  double      avg_f64;

  cxdtype_t   dtype_vals [3];
  void       *val_p_vals [3];
  rflags_t    rflags_vals[3];
  cx_time_t   tmstp_vals [3];

  static int  chan_addrs [3] = {0, 1, 2};
  static int  nelems_vals[3] = {1, 1, 1};

  static int  chan_zero = 0;

#define DO_CALCS(dt, sum) \
{ \
  dt *p; \
 \
    if (nelems > 0) \
        for (rest = nelems - 1, p = buf, sum = __CX_CONCATENATE(min_,dt) = __CX_CONCATENATE(max_,dt) = *p++; \
             rest > 0; \
             rest--,            p++) \
        { \
            sum += *p; \
            if (__CX_CONCATENATE(min_,dt) > *p) __CX_CONCATENATE(min_,dt) = *p; \
            if (__CX_CONCATENATE(max_,dt) < *p) __CX_CONCATENATE(max_,dt) = *p; \
        } \
    else \
    { \
        __CX_CONCATENATE(min_,dt) = __CX_CONCATENATE(max_,dt) = 0; \
        sum = 0; \
    } \
 \
    val_p_vals[0] = &(__CX_CONCATENATE(min_,dt)); \
    val_p_vals[1] = &(__CX_CONCATENATE(max_,dt)); \
}

    if      (reason == CDA_REF_R_UPDATE)
    {
        /* Note: 
             we use "ref" instead of "me->chan_r" because for "insrv::" refs
             this is first called during initialization,
             upon cda_add_chan(), when me->chan_r isn't filled yet
             and is still ==-1 */
        dtype  = cda_current_dtype_of_ref (ref);
        nelems = cda_current_nelems_of_ref(ref);
        cda_acc_ref_data                  (ref, &buf,     NULL);
        cda_get_ref_stat                  (ref, &rflags, &timestamp);

        /* Perform calculations */
        switch (dtype)
        {
            case CXDTYPE_UINT8:  DO_CALCS(uint8,   sum_i64); avg_f64 = nelems > 0? ((float64)sum_i64 / nelems) : 0; break;
            case CXDTYPE_INT8:   DO_CALCS(int8,    sum_i64); avg_f64 = nelems > 0? ((float64)sum_i64 / nelems) : 0; break;
            case CXDTYPE_UINT16: DO_CALCS(uint16,  sum_i64); avg_f64 = nelems > 0? ((float64)sum_i64 / nelems) : 0; break;
            case CXDTYPE_INT16:  DO_CALCS(int16,   sum_i64); avg_f64 = nelems > 0? ((float64)sum_i64 / nelems) : 0; break;
            case CXDTYPE_UINT32: DO_CALCS(uint32,  sum_i64); avg_f64 = nelems > 0? ((float64)sum_i64 / nelems) : 0; break;
            case CXDTYPE_INT32:  DO_CALCS(int32,   sum_i64); avg_f64 = nelems > 0? ((float64)sum_i64 / nelems) : 0; break;
            case CXDTYPE_UINT64: DO_CALCS(uint64,  sum_i64); avg_f64 = nelems > 0? ((float64)sum_i64 / nelems) : 0; break;
            case CXDTYPE_INT64:  DO_CALCS(int64,   sum_i64); avg_f64 = nelems > 0? ((float64)sum_i64 / nelems) : 0; break;
            case CXDTYPE_SINGLE: DO_CALCS(float32, sum_f64); avg_f64 = nelems > 0? (         sum_f64 / nelems) : 0; break;
            case CXDTYPE_DOUBLE: DO_CALCS(float64, sum_f64); avg_f64 = nelems > 0? (         sum_f64 / nelems) : 0; break;
            default: return;
        }

        /* Return values */
        dtype_vals [0] = dtype_vals [1]                  = dtype;
                                          dtype_vals [2] = CXDTYPE_DOUBLE;
                                          val_p_vals [2] = &avg_f64;
        rflags_vals[0] = rflags_vals[1] = rflags_vals[2] = rflags;
        tmstp_vals [0] = tmstp_vals [1] = tmstp_vals [2] = timestamp;
        ReturnDataSet(devid, 3,
                      chan_addrs, dtype_vals, nelems_vals,
                      val_p_vals, rflags_vals, tmstp_vals);
    }
    else if (reason == CDA_REF_R_FRESHCHG)
    {
        /* Note: <0 -- error, ==0 -- no fresh_age specified, >0 -- specified */
        if (cda_fresh_age_of_ref(ref, &fresh_age) > 0)
            SetChanFreshAge(devid, 0, 1, fresh_age);
        /* Note 2: is this a valid approach?  If 
                   1) some fresh age is specified;
                   2) reconnect happens,
                   3) ...with no fresh age specified in the new incarnation
                   4) thus the OLD fresh age remains in effect...
           Is it fixable at all? */
    }
    else if (reason == CDA_REF_R_RSLVSTAT)
    {
        if (ptr2lint(info_ptr) == CDA_RSLVSTAT_NOTFOUND)
        {
            if (cda_src_of_ref(ref, &src_p) < 0) src_p = "???";
            DoDriverLog(devid, 0, "target channel \"%s\" not found", src_p);
        }
    }
}

static int  vector_minmaxavg_init_d(int devid, void *devptr,
                                    int businfocount, int businfo[],
                                    const char *auxinfo)
{
  privrec_t  *me = devptr;
  const char *p;

  int         n_items;
  char       *endptr;

    me->cid    = -1;
    me->chan_r = -1;

    n_items = -1;

    /* Parse auxinfo */
    if (auxinfo == NULL  ||  *auxinfo == '\0')
    {
        DoDriverLog(devid, 0, "%s(): empty auxinfo", __FUNCTION__);
        return -CXRF_CFG_PROBL;
    }

    p = auxinfo;
    if (*p == '@')
    {
        p++;

        /* Optional NELEMS */
        if (isdigit(*p))
        {
            /* Note: the usual "if (endptr==p)" check is useless,
                     since 'p' points to a digit, thus strtol() will ALWAYS
                     yeild some number;
                     a check for n_items<0 is also useless because of
                     the same reason. */
            n_items = strtol(p, &endptr, 0);
            p = endptr;
            /* ...but 0xFFFFFFFF yeilds -1 on 32-bit systems;
               so, we MUST check for n_items<0! */
            if (n_items < 0)
            {
                DoDriverLog(devid, 0, "negative n_items (%d=0x%d)", n_items, n_items);
                return -CXRF_CFG_PROBL;
            }
        }

        /* Now, a MANDATORY ':' separator */
        if (*p != ':')
        {
            DoDriverLog(devid, 0, "':' expected after '@...' spec");
            return -CXRF_CFG_PROBL;
        }
        p++;

        /* And one more check for non-empty data-chan spec */
        if (*p == '\0')
        {
            DoDriverLog(devid, 0, "data-chan spec expected after '@...:'");
            return -CXRF_CFG_PROBL;
        }
    }

    /* Select n_items if not specified */
    if (n_items < 0) n_items = 1000;

    if ((me->cid = cda_new_context(devid, devptr,
                                   "insrv::", 0,
                                   NULL,
                                   0, NULL, 0)) < 0)
    {
        DoDriverLog(devid, 0, "cda_new_context(): %s", cda_last_err());
        return -CXRF_DRV_PROBL;
    }
    if ((me->chan_r = cda_add_chan(me->cid, NULL, p,
                                   CDA_DATAREF_OPT_NO_RD_CONV,
                                   CXDTYPE_UNKNOWN, n_items,
                                   CDA_REF_EVMASK_UPDATE | 
                                     CDA_REF_EVMASK_FRESHCHG |
                                     CDA_REF_EVMASK_RSLVSTAT,
                                   chan_evproc, NULL)) < 0)
    {
        DoDriverLog(devid, 0, "cda_new_chan(\"%s\"): %s",
                    p, cda_last_err());
        cda_del_context(me->cid);    me->cid    = -1;
        return -CXRF_DRV_PROBL;
    }
    SetChanReturnType(devid, 0, 1, IS_AUTOUPDATED_YES);
    
    return DEVSTATE_OPERATING;
}

static void vector_minmaxavg_term_d(int devid, void *devptr)
{
  privrec_t *me = devptr;

    cda_del_context(me->cid);
    me->cid    = -1;
    me->chan_r = -1;
}


DEFINE_CXSD_DRIVER(vector_minmaxavg, "Vector values min/max/avg calculator",
                   NULL, NULL,
                   sizeof(privrec_t), NULL,
                   0, 0,
                   NULL, 0,
                   NULL,
                   -1, NULL, NULL,
                   vector_minmaxavg_init_d, vector_minmaxavg_term_d, NULL);
