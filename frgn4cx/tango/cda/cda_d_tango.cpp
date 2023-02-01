#include <fcntl.h>
#include <errno.h>
#include <ctype.h>
#include <string.h>
#include <time.h>

#include "misc_macros.h"
#include "misclib.h"
#include "cxscheduler.h"

#include "cda.h"
#include "cdaP.h"

#include "cda_d_tango.h"

#include <tango.h>


#define USE_PUSH_MODEL 0
#define DO_CATCH       1

//////////////////////////////////////////////////////////////////////

class cda_d_tango_EventCallBack : public Tango::CallBack
{
    void push_event(Tango::EventData *);

public:
    cda_hwcnref_t  hwr;

    cda_d_tango_EventCallBack(cda_hwcnref_t  the_hwr) {hwr = the_hwr;}
};

//////////////////////////////////////////////////////////////////////

static int cda_d_tango_debug = 1;

static void report_exception(const char *caller, CORBA::Exception &e)
{
    fprintf(stderr, "%s %s: exception, details follow:\n", strcurtime(), caller);
    fprintf(stderr, "--------------------\n");
    Tango::Except::print_exception(e);
    fprintf(stderr, "--------------------\n");
}

//////////////////////////////////////////////////////////////////////

// Google for "c wrapper for c++ class"
// https://nachtimwald.com/2017/08/18/wrapping-c-objects-in-c/


extern "C"
{


//////////////////////////////////////////////////////////////////////

enum
{
    CHTYPE_ATTRIBUTE = 1,
    CHTYPE_PROPERTY  = 2,
    CHTYPE_COMMAND   = 3,
    CHTYPE_RESULT    = 4,
};

enum
{   /* this enumeration according to man(3) pipe */
    PIPE_RD_SIDE = 0,
    PIPE_WR_SIDE = 1,
};

typedef int dpx_id_t;

typedef struct
{
    int            in_use;

    cda_dataref_t  dataref; // "Backreference" to corr.entry in the global table
    cxdtype_t      dtype;
    int            chtype;
    dpx_id_t       dpx;

    char          *dup_name;
    char          *chn_name; // Note: should NOT be freed, because points inside dup_name[]
    //!!! TANGO ID or pointer
    int            subscr_event_id;
    cda_d_tango_EventCallBack *cb;

    /*!!! Should remove next/prev & Co. when USE_PUSH_MODEL in effect */
    cda_hwcnref_t  next;    // Link to next hwr of sid
    cda_hwcnref_t  prev;    // Link to previous hwr of sid

    struct _cda_d_tango_privrec_t_struct *me; // Backreference to a containing server

    // Copied from cxsd_fe_epics.c::moninfo_t; should better keep in sync
    void          *current_val;      // Pointer to current-value-buffer (if !=NULL)...
    int            current_nelems;   // ...and # of units in it
    CxAnyVal_t     valbuf;           // Buffer for small-sized values; current_val=NULL in such cases
    cxdtype_t      current_dtype;    // Current data type
    rflags_t       current_rflags;
    cx_time_t      current_timestamp;

#if USE_PUSH_MODEL 
    pthread_mutex_t val_mutex;
    int             val_mutex_inited;
#endif /* USE_PUSH_MODEL */
} hwrinfo_t;

typedef struct
{
    int            in_use;

    char          *dev_name;
    Tango::DeviceProxy *dev;

    dpx_id_t       next;    // Link to next dpx of sid
    dpx_id_t       prev;    // Link to previous dpx of sid

    dpx_id_t       nx2r;    // Link to NeXt dpx in to-Reconnect list
    dpx_id_t       pr2r;    // Link to PRevious dpx in to-Reconnect list

    struct _cda_d_tango_privrec_t_struct *me; // Backreference to a containing server
} dpxinfo_t; // "dpx" -- DeviceProXy

typedef struct _cda_d_tango_privrec_t_struct
{
    cda_srvconn_t  sid;

    dpx_id_t       frs_dpx;
    dpx_id_t       lst_dpx;

    cda_hwcnref_t  frs_hwr;
    cda_hwcnref_t  lst_hwr;

#if USE_PUSH_MODEL
    int            event_pipe[2];
    sl_fdh_t       event_rfdh;
#else
#endif /* USE_PUSH_MODEL */
} cda_d_tango_privrec_t;

//////////////////////////////////////////////////////////////////////

enum
{
    DPX_MIN_VAL   = 1,
    DPX_MAX_COUNT = 1000000,  // An arbitrary limit
    DPX_ALLOC_INC = 100,

    HWR_MIN_VAL   = 1,
    HWR_MAX_COUNT = 1000000,  // An arbitrary limit
    HWR_ALLOC_INC = 100,
};

static dpxinfo_t     *dpxs_list;
static int            dpxs_list_allocd;

static hwrinfo_t     *hwrs_list;
static int            hwrs_list_allocd;

static dpx_id_t       frs_2rcn = -1;
static dpx_id_t       lst_2rcn = -1;

//--------------------------------------------------------------------

// GetDpxSlot()
GENERIC_SLOTARRAY_DEFINE_GROWING(static, Dpx, dpxinfo_t,
                                 dpxs, in_use, 0, 1,
                                 DPX_MIN_VAL, DPX_ALLOC_INC, DPX_MAX_COUNT,
                                 , , void)

static void RlsDpxSlot(dpx_id_t dpx)
{
  dpxinfo_t *di = AccessDpxSlot(dpx);

    if (di->in_use == 0) return; /*!!! In fact, an internal error */
    safe_free(di->dev_name); di->dev_name = NULL;

    //!!! Clear TANGO resources
    if (di->dev != NULL)
    {
        delete di->dev;
        di->dev = NULL;
    }

    di->in_use = 0;
}

//--------------------------------------------------------------------

static void AddDpxToSrv  (cda_d_tango_privrec_t *me, dpx_id_t dpx)
{
  dpxinfo_t     *di   = AccessDpxSlot(dpx);

    di->prev = me->lst_dpx;
    di->next = -1;

    if (me->lst_dpx >= 0)
        AccessDpxSlot(me->lst_dpx)->next = dpx;
    else
        me->frs_dpx                      = dpx;
    me->lst_dpx = dpx;

    di->me = me;
}

static void DelDpxFromSrv(cda_d_tango_privrec_t *me, dpx_id_t dpx)
{
  dpxinfo_t *di   = AccessDpxSlot(dpx);
  dpx_id_t   prev = di->prev;
  dpx_id_t   next = di->next;

    if (prev < 0) me->frs_dpx = next; else AccessDpxSlot(prev)->next = next;
    if (next < 0) me->lst_dpx = prev; else AccessDpxSlot(next)->prev = prev;

    di->prev = di->next = -1;

    di->me = NULL;
}

static void AddDpxToRcn  (dpx_id_t dpx)
{
  dpxinfo_t     *di   = AccessDpxSlot(dpx);

    di->pr2r = lst_2rcn;
    di->nx2r = -1;

    if (lst_2rcn >= 0)
        AccessDpxSlot(lst_2rcn)->nx2r = dpx;
    else
        frs_2rcn                      = dpx;
    lst_2rcn = dpx;
}

static void DelDpxFromRcn(dpx_id_t dpx)
{
  dpxinfo_t *di   = AccessDpxSlot(dpx);
  dpx_id_t   pr2r = di->pr2r;
  dpx_id_t   nx2r = di->nx2r;

    if (pr2r < 0) frs_2rcn = nx2r; else AccessDpxSlot(pr2r)->nx2r = nx2r;
    if (nx2r < 0) lst_2rcn = pr2r; else AccessDpxSlot(nx2r)->pr2r = pr2r;

    di->pr2r = di->nx2r = -1;
}

//--------------------------------------------------------------------

// GetHwrSlot()
GENERIC_SLOTARRAY_DEFINE_GROWING(static, Hwr, hwrinfo_t,
                                 hwrs, in_use, 0, 1,
                                 HWR_MIN_VAL, HWR_ALLOC_INC, HWR_MAX_COUNT,
                                 , , void)

static void RlsHwrSlot(cda_hwcnref_t hwr)
{
  hwrinfo_t *hi = AccessHwrSlot(hwr);

    if (hi->in_use == 0) return; /*!!! In fact, an internal error */
    safe_free(hi->dup_name); hi->dup_name = NULL;

    //!!! Clear TANGO resources
    if (hi->subscr_event_id != -1)
    {
    }
    if (hi->cb != NULL)
    {
        delete hi->cb; hi->cb = NULL;
    }

    safe_free(hi->current_val); hi->current_val = NULL;

#if USE_PUSH_MODEL
    if (hi->val_mutex_inited)
        pthread_mutex_destroy(&(hi->val_mutex));
    hi->val_mutex_inited = 0;
#endif /* USE_PUSH_MODEL */

    hi->in_use = 0;
}

//--------------------------------------------------------------------

static void AddHwrToSrv  (cda_d_tango_privrec_t *me, cda_hwcnref_t hwr)
{
  hwrinfo_t     *hi   = AccessHwrSlot(hwr);

    hi->prev = me->lst_hwr;
    hi->next = -1;

    if (me->lst_hwr >= 0)
        AccessHwrSlot(me->lst_hwr)->next = hwr;
    else
        me->frs_hwr                      = hwr;
    me->lst_hwr = hwr;

    hi->me = me;
}

static void DelHwrFromSrv(cda_d_tango_privrec_t *me, cda_hwcnref_t hwr)
{
  hwrinfo_t     *hi   = AccessHwrSlot(hwr);
  cda_hwcnref_t  prev = hi->prev;
  cda_hwcnref_t  next = hi->next;

    if (prev < 0) me->frs_hwr = next; else AccessHwrSlot(prev)->next = next;
    if (next < 0) me->lst_hwr = prev; else AccessHwrSlot(next)->prev = prev;

    hi->prev = hi->next = -1;

    hi->me = NULL;
}

//////////////////////////////////////////////////////////////////////

static int  store_current_value (hwrinfo_t *hi, Tango::DeviceAttribute *attr_value)
{
  void          *value_p;

  int            data_type;
  cxdtype_t      dtype;

    value_p = hi->current_val;
    if (value_p == NULL) value_p = &(hi->valbuf);

    data_type = attr_value->get_type();

    /*!!! A very BIG FAT note:
          we are trying to read value of a type DIFFERENT
          from what was used for current_val allocation,
          which potentially can result in buffer overflow.
          Currently, for SCALARS, it is safe, because reading
          is performed into valbuf, which is of type CxAnyVal_t,
          which, by design, is able to hold a value of ANY type.
          But for VECTORS (and STRINGS) this wouldn't work. */

#if DO_CATCH
    try
    {
#endif
        switch (hi->dtype)
        {
            case Tango::DEV_SHORT:  *(attr_value) >> *(( int16*)value_p);   dtype = CXDTYPE_INT16;  break;
            case Tango::DEV_USHORT: *(attr_value) >> *((uint16*)value_p);   dtype = CXDTYPE_UINT16; break;
            case Tango::DEV_LONG:   *(attr_value) >> *(( int32*)value_p);   dtype = CXDTYPE_INT32;  break;
            case Tango::DEV_ULONG:  *(attr_value) >> *((uint32*)value_p);   dtype = CXDTYPE_UINT32; break;
            /* Note: since "'long long int' is not an enumeration type",
                     we can NOT use 64-bit ints */
            //case Tango::DEV_LONG64:  *(attr_value) >> *(( int64*)value_p);  dtype = CXDTYPE_INT64;  break;
            //case Tango::DEV_ULONG64: *(attr_value) >> *((uint64*)value_p);  dtype = CXDTYPE_UINT64; break;
            case Tango::DEV_FLOAT:  *(attr_value) >> *((float32*)value_p);  dtype = CXDTYPE_SINGLE; break;
            case Tango::DEV_DOUBLE: *(attr_value) >> *((float64*)value_p);  dtype = CXDTYPE_DOUBLE; break;
            // case Tango::DEV_UCHAR:   ???
            // case Tango::DEV_STRING:  ???
            // case Tango::DEV_BOOLEAN: convert to int32?
            // case Tango::DEV_STATE:   convert to int32?
            // case Tango::DEV_ENUM:    convert to int32?
            // case Tango::DEV_INT:     what type is it???
            default: return -1;
        }
        hi->current_dtype  = dtype;
        hi->current_nelems = 1;
        hi->current_rflags = 0;

        return 0;
#if DO_CATCH
    }
    catch (CORBA::Exception &e)
    {
        if (cda_d_tango_debug)
            report_exception("push_event: reading via >>", e);
    }
    catch (...)
    {
        if (cda_d_tango_debug)
            fprintf(stderr, "%s: reading via >> raised an exception\n", __FUNCTION__);
    }
#endif

    return -1;
}

static void return_current_value(hwrinfo_t *hi)
{
  void *value_p;

    value_p = hi->current_val;
    if (value_p == NULL) value_p = &(hi->valbuf);
    cda_dat_p_update_dataset(hi->me->sid, 1, &(hi->dataref),
                             &value_p, &(hi->current_dtype), &(hi->current_nelems),
                             &(hi->current_rflags), NULL/*&(hi->current_timestamp)*/, CDA_DAT_P_IS_UPDATE);
}

#if USE_PUSH_MODEL
static void event2pipe_proc(int uniq, void *privptr1,
                            sl_fdh_t fdh, int fd, int mask, void *privptr2)
{
  cda_d_tango_privrec_t *me = (cda_d_tango_privrec_t *)privptr1;

  int                    repcount;
  int                    r;
  cda_hwcnref_t          hwr;
  hwrinfo_t             *hi;

    for (repcount = 100;  repcount > 0;  repcount--)
    {
        r = read(fd, &hwr, sizeof(hwr));
        if (r != sizeof(hwr))
        {
            hi = AccessHwrSlot(hwr);
            if (hwr < 0  ||  hwr >= hwrs_list_allocd  ||
                hi->in_use == 0) goto NEXT_REPCOUNT;

            pthread_mutex_lock  (&(hi->val_mutex));
            return_current_value(hi);
            pthread_mutex_unlock(&(hi->val_mutex));
        }

 NEXT_REPCOUNT:;
    }
}
#endif /* USE_PUSH_MODEL */

static int determine_name_type(const char *name,
                               char srvrspec[], size_t srvrspec_size,
                               const char **channame_p)
{
  int         srv_len;
  const char *colon_p;
  const char *p;
  char       *vp;

    /* Defaults */
    srv_len = 0;
    *channame_p = name;

    /* Doesn't start with alphanumeric? */
    if (!isalnum(*name)) goto DO_RET;

    /* Is there a colon at all? */
    colon_p = strchr(name, ':');
    if (colon_p == NULL) goto DO_RET;

    /* Check than EVERYTHING preceding ':' can constitute a hostname */
    for (p = name;  p < colon_p;  p++)
        if (*p == '\0'  ||
        (!isalnum(*p)  &&  *p != '.'  &&  *p != '-')) goto DO_RET;
    /* Okay, skip ':' */
    p++;
    /* ':' should be followed by a digit */
    if (*p == '\0'  ||  !isdigit(*p)) goto DO_RET;
    /* Okay, skip digits... */
    while (*p != '\0'  &&  isdigit(*p)) p++;
    //
    if (*p != '.'  &&  *p != '/') goto DO_RET;

    ////////////////////
    srv_len = p - name;
    *channame_p = p + 1;

 DO_RET:
    if (srv_len > 0)
    {
        if (srv_len > srvrspec_size - 1)
            srv_len = srvrspec_size - 1;
        memcpy(srvrspec, name, srv_len);
    }
    srvrspec[srv_len] = '\0';
    for (vp = srvrspec;  *vp != '\0';  vp++) *vp = tolower(*vp);

    return srv_len != 0;
}

static int  create_dpx_dev(dpx_id_t dpx, dpxinfo_t *di, int is_suffering)
{
#if DO_CATCH
    try
    {
#endif
        di->dev = new Tango::DeviceProxy(di->dev_name);
#if DO_CATCH
    }
    catch (CORBA::Exception &e)
    {
        if (cda_d_tango_debug)
            report_exception("new Tango::DeviceProxy()", e);
        errno = EIO;
    }
    catch (...)
    {
        if (cda_d_tango_debug)
            fprintf(stderr, "%s: DeviceProxy creation raised an exception; dev=%p\n", __FUNCTION__, di->dev);
        errno = EIO;
    }
#endif

    if (di->dev != NULL)
    {
        if (is_suffering)
            cda_dat_p_report(di->me->sid, "DeviceProxy(\"%s\") created",
                             di->dev_name);
        return 0;
    }
    else
    {
        if (!is_suffering)
            cda_dat_p_report(di->me->sid, "failed to create DeviceProxy(\"%s\")",
                             di->dev_name);
        return +1;
    }
}

static int create_hwr_subscription(cda_hwcnref_t hwr)
{
  hwrinfo_t             *hi = AccessHwrSlot(hwr);
  dpxinfo_t             *di = AccessDpxSlot(hi->dpx);;

#if DO_CATCH
    try
    {
#endif
#if USE_PUSH_MODEL
        hi->subscr_event_id = di->dev->subscribe_event(hi->chn_name,
                                                       Tango::CHANGE_EVENT,
                                                       hi->cb,
                                                       1);
#else
        hi->subscr_event_id = di->dev->subscribe_event(hi->chn_name,
                                                       Tango::CHANGE_EVENT,
                                                       Tango::ALL_EVENTS,
                                                       1);
#endif /* USE_PUSH_MODEL */
#if DO_CATCH
    }
    catch (CORBA::Exception &e)
    {
        if (cda_d_tango_debug)
            report_exception("subscribe_event()", e);
        return -1;
    }
    catch (...)
    {
        if (cda_d_tango_debug)
            fprintf(stderr, "%s(%s|%s): subscribe_event raised an exception\n", __FUNCTION__, di->dev_name, hi->chn_name);
        return -1;
    }
#endif

    /*!!! Here should request initial read */

    return 0;
}

static int  cda_d_tango_new_chan(cda_dataref_t ref, const char *name,
                                 int options,
                                 cxdtype_t dtype, int max_nelems)
{
  cda_d_tango_privrec_t *me;

  int                    w_srv;
  char                   srvrspec[200];
  const char            *channame;
  size_t                 channamelen;
  size_t                 devnameofs;
  size_t                 totransofs;
  char                  *dup_name = NULL;
  char                  *p;
  int                    nsls;
  char                  *sl3;

  const char            *at_p;
  char                   at_c;
  const char            *params;
  int                    chtype;

  cda_hwcnref_t          hwr;
  hwrinfo_t             *hi;

  char                  *dev_name = NULL;
  dpx_id_t               dpx;
  dpxinfo_t             *di;

  size_t                 csize;                // Channel data size

  int                    saved_errno;

fprintf(stderr, "name=<%s>\n", name);
    /* Check/filter the dtype first */
    // As of 02.08.2019 -- we support scalars only
    if (max_nelems != 1)
    {
        cda_dat_p_report(-1, "%s(\"%s\"):max_nelems=%d requested while only ==1 (scalars) is supported",
                         __FUNCTION__, name, max_nelems);
        errno = EINVAL;
        return CDA_DAT_P_ERROR;
    }
    if (dtype != CXDTYPE_INT16  &&  dtype != CXDTYPE_UINT16  &&
        dtype != CXDTYPE_INT32  &&  dtype != CXDTYPE_UINT32  &&
        /* Note: since "'long long int' is not an enumeration type",
                 we can NOT use 64-bit ints */
        //dtype != CXDTYPE_INT64  &&  dtype != CXDTYPE_UINT64  &&
        dtype != CXDTYPE_SINGLE  &&
        dtype != CXDTYPE_DOUBLE)
    {
        cda_dat_p_report(-1, "%s(\"%s\"):dtype=%d (repr=%d, size=%zd) is unsupported",
                         __FUNCTION__, name, dtype, reprof_cxdtype(dtype), sizeof_cxdtype(dtype));
        errno = EINVAL;
        return CDA_DAT_P_ERROR;
    }

    w_srv = determine_name_type(name, srvrspec, sizeof(srvrspec), &channame);
    if (!w_srv)
    {
#if 1
        strcpy(srvrspec, "(INDIRECT)");
#else
        /*!!! Note: we REQUIRE a name with "HOST:PORT/" */
        errno = EINVAL;
        cda_set_err("HOST:PORT is required in channel spec");
        return CDA_DAT_P_ERROR;
#endif
    }

    /*!!! Perform complete parsing -- {DOMAIN/FAMILY/MEMBER}/{CHANNEL}[@T] */
    at_p = strchr(channame, '@');
    if (at_p != NULL)
    {
        channamelen = at_p - channame;
        params      = at_p + 1;
    }
    else
    {
        channamelen = strlen(channame);
        params      = "";
    }

    if (channamelen < 1)
    {
        errno = EINVAL;
        cda_set_err("empty CHANNEL name");
        return CDA_DAT_P_ERROR;
    }

    at_c = tolower(*params);
    if      (at_c == 'a'  ||  at_c == '\0') chtype = CHTYPE_ATTRIBUTE;
    else if (at_c == 'p')                   chtype = CHTYPE_PROPERTY;
    else if (at_c == 'c')                   chtype = CHTYPE_COMMAND;
    else if (at_c == 'r')                   chtype = CHTYPE_RESULT;
    else
    {
        errno = EINVAL;
        cda_set_err("invalid channel-type char after '@'");
        return CDA_DAT_P_ERROR;
    }

    /* Re-add "HOST:PORT/" (if present) to channel name */
    devnameofs = channame - name;
    channamelen += devnameofs;
    channame = name;

    totransofs = w_srv? strlen(srvrspec) : 0;

    /* Store name for future references and translate from CX to TANGO style */
    dup_name = (char*)malloc(channamelen + 1);
    if (dup_name == NULL) return CDA_DAT_P_ERROR;
    memcpy(dup_name, channame, channamelen); dup_name[channamelen] = '\0';
    /* a. Convert all '.' to '/' */
    for (p = dup_name + totransofs;            *p != '\0';  p++)
        if (*p == '.') *p = '/';
    /* b. Count '/'s */
    for (p = dup_name + devnameofs, nsls = 0;  *p != '\0';  p++)
    {
        if (*p == '/')
        {
            nsls++;
            if (nsls == 3) sl3 = p;
        }
        /* A hack to alow specifying '=' for "#dbase=no" -- otherwise
           cdaclient recognizes '=' as a separator in the NAME=VALUE pair */
        if (*p == '%') *p = '=';
    }
    if (nsls != 3)
    {
        errno = EINVAL;
        cda_set_err("invalid channel name format");
        goto ERREXIT_FREE_DUP_NAME;
    }
    *sl3 = '\0';
    if (strlen(sl3 + 1) < 1)
    {
        errno = EINVAL;
        cda_set_err("last channel name component is empty");
        goto ERREXIT_FREE_DUP_NAME;
    }

    /* Okay -- here the name is checked and we may allocate data structures */
fprintf(stderr, "\tdup_name=<%s> sl3+1=<%s>\n", dup_name, sl3+1);

    // a. A server
    me = (cda_d_tango_privrec_t *)cda_dat_p_get_server(ref, &CDA_DAT_P_MODREC_NAME(tango), srvrspec, CDA_DAT_P_GET_SERVER_OPT_NONE);
    if (me == NULL) goto ERREXIT_FREE_DUP_NAME;

    // b. A device
    dev_name = strdup(dup_name); // Note: this is WITHOUT the "/CHANNEL" component, because that was trimmed by '\0' above
    if (dev_name == NULL) goto ERREXIT_FREE_DUP_NAME;
    for (dpx = me->frs_dpx;  dpx >= 0;  dpx = di->next)
    {
        di = AccessDpxSlot(dpx);
        fprintf(stderr, "\t[%d]<%s>, <%s>\n", dpx, di->dev_name, dev_name);
        if (strcasecmp(dev_name, di->dev_name) == 0) break;
    }
    // If not found -- create one
    if (dpx < 0)
    {
        dpx = GetDpxSlot();
        if (dpx < 0) goto ERREXIT_FREE_DEV_NAME;
        di = AccessDpxSlot(dpx);
        // Fill data
        di->next     = -1;
        di->prev     = -1;
        di->nx2r     = -1;
        di->pr2r     = -1;
        di->dev_name = dev_name;
        dev_name = NULL; // Protect the value from free()ing

        AddDpxToSrv(me, dpx);

        create_dpx_dev(dpx, di, 0);
        if (di->dev == NULL)
            AddDpxToRcn(dpx);
    }
    else
    {
        di = AccessDpxSlot(dpx);
        free(dev_name); dev_name = NULL; // It is no longer needed
    }

    // c. A channel
    hwr = GetHwrSlot();
    if (hwr < 0) goto ERREXIT_FREE_DUP_NAME;
    hi = AccessHwrSlot(hwr);
    // Fill data
    hi->next     = -1;
    hi->prev     = -1;
    hi->dataref  = ref;
    hi->dtype    = dtype;
    hi->dup_name = dup_name;
    hi->chn_name = sl3 + 1;
    hi->subscr_event_id = -1;

    // Allocate a buffer for data if needed
    csize = sizeof_cxdtype(dtype) * max_nelems;
    if (csize > sizeof(hi->valbuf))
    {
        if ((hi->current_val = malloc(csize)) == NULL)
        {
            saved_errno = errno;
            RlsHwrSlot(hwr);
            errno = saved_errno;
            return CDA_DAT_P_ERROR;
        }
    }

#if USE_PUSH_MODEL
    if ((saved_errno = pthread_mutex_init(&(hi->val_mutex), NULL)) != 0)
    {
        RlsHwrSlot(hwr);
        errno = saved_errno;
        return CDA_DAT_P_ERROR;
    }
    hi->val_mutex_inited = 1;
#endif /* USE_PUSH_MODEL */

    hi->chtype   = chtype;
    hi->dpx      = dpx;
    hi->cb       = new cda_d_tango_EventCallBack(hwr);
    if (hi->cb == NULL)
    {
        RlsHwrSlot(hwr);
        return CDA_DAT_P_ERROR;
    }
    // Subscribe to events, if possible
    if (di->dev != NULL)
    {
        
        if (create_hwr_subscription(hwr) < 0)
        {
            RlsHwrSlot(hwr);
            errno = EIO;
            return CDA_DAT_P_ERROR;
        }
    }

    AddHwrToSrv(me, hwr);
    cda_dat_p_set_hwr  (ref, hwr);

////fprintf(stderr, "sid=%d dpx=%d hwr=%d\n", me->sid, dpx, hwr);
//    goto ERREXIT_FREE_DUP_NAME;
    return CDA_DAT_P_NOTREADY;

 ERREXIT_FREE_DEV_NAME:
    saved_errno = errno;
    free(dev_name);
    errno = saved_errno;
 ERREXIT_FREE_DUP_NAME:
    saved_errno = errno;
    free(dup_name);
    errno = saved_errno;
    return CDA_DAT_P_ERROR;
}

static int  cda_d_tango_snd_data(void *pdt_privptr, cda_hwcnref_t hwr,
                                 cxdtype_t dtype, int nelems, void *value)
{
  cda_d_tango_privrec_t *me = (cda_d_tango_privrec_t *)pdt_privptr;
  hwrinfo_t             *hi = AccessHwrSlot(hwr);
  dpxinfo_t             *di;
  Tango::DeviceAttribute*da = NULL;

    if (nelems != 1)
    {
        cda_dat_p_report(-1, "%s(\"%s\"):nelems=%d requested while only ==1 (scalars) is supported",
                         __FUNCTION__, hi->chn_name, nelems);
        errno = EINVAL;
        return CDA_PROCESS_ERR;
    }
#if DO_CATCH
    try
    {
#endif
        switch (dtype)
        {
            case CXDTYPE_INT16:  da = new Tango::DeviceAttribute(hi->chn_name, *((  int16*)value)); break;
            case CXDTYPE_UINT16: da = new Tango::DeviceAttribute(hi->chn_name, *(( uint16*)value)); break;
            case CXDTYPE_INT32:  da = new Tango::DeviceAttribute(hi->chn_name, *((  int32*)value)); break;
            case CXDTYPE_UINT32: da = new Tango::DeviceAttribute(hi->chn_name, *(( uint32*)value)); break;
            /* Note: since "'long long int' is not an enumeration type",
                     we can NOT use 64-bit ints */
            //case CXDTYPE_INT64:  da = new Tango::DeviceAttribute(hi->chn_name, *((  int64*)value)); break;
            //case CXDTYPE_UINT64: da = new Tango::DeviceAttribute(hi->chn_name, *(( uint64*)value)); break;
            case CXDTYPE_SINGLE: da = new Tango::DeviceAttribute(hi->chn_name, *((float32*)value)); break;
            case CXDTYPE_DOUBLE: da = new Tango::DeviceAttribute(hi->chn_name, *((float64*)value)); break;
            default:
                errno = EINVAL;
                return CDA_PROCESS_ERR;
        }
#if DO_CATCH
    }
    catch (CORBA::Exception &e)
    {
        if (cda_d_tango_debug)
            report_exception("snd: new Tango::DeviceAttribute()", e);
        errno = EIO;
        return CDA_PROCESS_ERR;
    }
    catch (...)
    {
        if (cda_d_tango_debug)
            fprintf(stderr, "%s(%s): DeviceAttribute creation raised an exception\n", __FUNCTION__, hi->chn_name);
        errno = EIO;
        return CDA_PROCESS_ERR;
    }
#endif

    di = AccessDpxSlot(hi->dpx);
#if DO_CATCH
    try
    {
#endif
        di->dev->write_attribute(*da);
#if DO_CATCH
    }
    catch (CORBA::Exception &e)
    {
        delete da; da = NULL;
        if (cda_d_tango_debug)
            report_exception("snd: write_attribute()", e);
        errno = EIO;
        return CDA_PROCESS_ERR;
    }
    catch (...)
    {
        delete da; da = NULL;
        if (cda_d_tango_debug)
            fprintf(stderr, "%s(%s): write_attribute raised an exception\n", __FUNCTION__, hi->chn_name);
        errno = EIO;
        return CDA_PROCESS_ERR;
    }
#endif

    delete da; da = NULL;

    return CDA_PROCESS_DONE;
}

//////////////////////////////////////////////////////////////////////

static int  cda_d_tango_new_srv (cda_srvconn_t  sid, void *pdt_privptr,
                                 int            uniq,
                                 const char    *srvrspec,
                                 const char    *argv0,
                                 int            srvtype  __attribute((unused)))
{
  cda_d_tango_privrec_t *me = (cda_d_tango_privrec_t *)pdt_privptr;

////fprintf(stderr, "ZZZ %s(%s)\n", __FUNCTION__, srvrspec);
    me->sid = sid;

    me->frs_dpx = -1;
    me->lst_dpx = -1;

    me->frs_hwr = -1;
    me->lst_hwr = -1;

#if USE_PUSH_MODEL
    me->event_pipe[PIPE_RD_SIDE] = me->event_pipe[PIPE_WR_SIDE] = -1;
    me->event_rfdh = -1;

    if (pipe(me->event_pipe) < 0)
    {
        cda_set_err("pipe() failed");
        return CDA_DAT_P_ERROR;
    }
    set_fd_flags(me->event_pipe[PIPE_RD_SIDE], O_NONBLOCK, 1);
    set_fd_flags(me->event_pipe[PIPE_WR_SIDE], O_NONBLOCK, 1);
    if ((me->event_rfdh = sl_add_fd(uniq, me,
                                    me->event_pipe[PIPE_RD_SIDE], SL_RD,
                                    event2pipe_proc, NULL)) < 0)
    {
        cda_set_err("sl_add_fd() failed");
        close(me->event_pipe[PIPE_RD_SIDE]); me->event_pipe[PIPE_RD_SIDE] = -1;
        close(me->event_pipe[PIPE_WR_SIDE]); me->event_pipe[PIPE_WR_SIDE] = -1;
        return CDA_DAT_P_ERROR;
    }
#else
#endif /* USE_PUSH_MODEL */

    return CDA_DAT_P_OPERATING;

}

static int  cda_d_tango_del_srv (cda_srvconn_t  sid, void *pdt_privptr)
{
  cda_d_tango_privrec_t *me = (cda_d_tango_privrec_t *)pdt_privptr;

#if USE_PUSH_MODEL
    if (me->event_rfdh > 0)
    {
        sl_del_fd(me->event_rfdh); me->event_rfdh = -1;
    }

    if (me->event_pipe[PIPE_RD_SIDE] > 0)
    {
        close(me->event_pipe[PIPE_RD_SIDE]); me->event_pipe[PIPE_RD_SIDE] = -1;
    }
    if (me->event_pipe[PIPE_WR_SIDE] > 0)
    {
        close(me->event_pipe[PIPE_WR_SIDE]); me->event_pipe[PIPE_WR_SIDE] = -1;
    }
#else
#endif /* USE_PUSH_MODEL */
    return CDA_DAT_P_DEL_SRV_SUCCESS;  
}

//////////////////////////////////////////////////////////////////////

#if !USE_PUSH_MODEL

enum {TANGO_HEARTBEAT_USECS = 100*1000*0 + 5000000*1}; // Use 5s for debugging
enum {TANGO_RCN_FRQDIV      = 100*0+1};

static int       is_initialized;
static sl_tid_t  tango_hbt_tid = -1;
static int       tango_rcn_ctr = 0;

static int  CheckEventsIterator(hwrinfo_t *hi, void *privptr)
{
  dpxinfo_t     *di   = AccessDpxSlot(hi->dpx);

#if DO_CATCH
    try
    {
#endif
        if (di->dev != NULL)
            di->dev->get_events(hi->subscr_event_id, hi->cb);
#if DO_CATCH
    }
    catch (CORBA::Exception &e)
    {
        if (cda_d_tango_debug)
            report_exception("get_events()", e);
    }
    catch (...)
    {
        if (cda_d_tango_debug)
            fprintf(stderr, "%s: get_events() raised an exception\n", __FUNCTION__);
    }
#endif

    return 0;
}
static int  DoSubscribeIterator(hwrinfo_t *hi, void *privptr)
{
  dpx_id_t  dpx = ptr2lint(privptr);

    if (hi->dpx == dpx)
    {
        create_hwr_subscription(hi - hwrs_list /* "hi2hwr()" */);
    }

    return 0;
}
static void TangoHeartbeatProc(int uniq, void *unsdptr,
                               sl_tid_t tid, void *privptr)
{
  dpx_id_t   dpx;
  dpx_id_t   nx2r;
  dpxinfo_t *di;

    tango_hbt_tid = sl_enq_tout_after(0/*!!!uniq*/, NULL,
                                      TANGO_HEARTBEAT_USECS, TangoHeartbeatProc, NULL);

    ForeachHwrSlot(CheckEventsIterator, NULL);

    tango_rcn_ctr++;
    if (tango_rcn_ctr >= TANGO_RCN_FRQDIV)
    {
        tango_rcn_ctr = 0;
        for (dpx = frs_2rcn;  dpx >= 0;  dpx = nx2r)
        {
            di   = AccessDpxSlot(dpx);
            nx2r = di->nx2r;
            // "Try to reconnect"
            if (create_dpx_dev(dpx, di, 1) == 0)
            {
                DelDpxFromRcn(dpx);
                ForeachHwrSlot(DoSubscribeIterator, lint2ptr(dpx));
            }
        }
    }
}

#endif /* !USE_PUSH_MODEL */

static int  cda_d_tango_init_m(void)
{
  int         r;
  const char *dbg;

    if ((dbg = getenv("CDA_D_TANGO_DEBUG")) != NULL  &&
        (*dbg == '1'  ||  *dbg == 'y'  ||  *dbg == 'Y'))
        cda_d_tango_debug = 1;

    r = cda_register_dat_plugin(&(CDA_DAT_P_MODREC_NAME(tango)));
    if (r < 0) return -1;

#if !USE_PUSH_MODEL
    tango_hbt_tid = sl_enq_tout_after(0/*!!!uniq*/, NULL,
                                      TANGO_HEARTBEAT_USECS, TangoHeartbeatProc, NULL);
                                   
    is_initialized = 1;
#endif
    
    return 0;
}

static void cda_d_tango_term_m(void)
{
#if !USE_PUSH_MODEL
    if (!is_initialized) return;
    if (tango_hbt_tid >= 0)
    {
        sl_deq_tout(tango_hbt_tid); tango_hbt_tid = -1;
    }
#endif
    
    cda_deregister_dat_plugin(&(CDA_DAT_P_MODREC_NAME(tango)));
}

//////////////////////////////////////////////////////////////////////

CDA_DEFINE_DAT_PLUGIN(tango, "TANGO data-access plugin",
                      cda_d_tango_init_m, cda_d_tango_term_m,
                      sizeof(cda_d_tango_privrec_t),
                      CDA_DAT_P_FLAG_NONE,
                      '.', ':', '@',
                      cda_d_tango_new_chan, NULL,
                      NULL,
                      cda_d_tango_snd_data, NULL,
                      cda_d_tango_new_srv,  cda_d_tango_del_srv,
                      NULL, NULL,
                      NULL, NULL);


} /* extern "C" */

//////////////////////////////////////////////////////////////////////

#include <signal.h>
void cda_d_tango_EventCallBack::push_event(Tango::EventData *myevent)
{
  hwrinfo_t     *hi = AccessHwrSlot(hwr);
#if USE_PUSH_MODEL
  int            r;

    // Store data
    pthread_mutex_lock  (&(hi->val_mutex));
    r = store_current_value (hi, myevent->attr_value);
    pthread_mutex_unlock(&(hi->val_mutex));

    // Send a notification
    if (r == 0) write(hi->me->event_pipe[PIPE_WR_SIDE], &hwr, sizeof(hwr));
#else
//fprintf(stderr, "%s: dev=%p attr_value=%p ", __FUNCTION__, myevent->device, myevent->attr_value); cerr << "attr_name=" << myevent->attr_name << " event=" << myevent->event; fprintf(stderr, " err=%d\n\t", myevent->err); cout<<myevent->errors[0].desc<<"\n"; //for (int err = 0;  err < myevent->errors.size();  err++) ;
    if (myevent->err)
    {
        if (cda_d_tango_debug)
        {
            fprintf(stderr, "%s %s(hwr=%d:\"%s!%s\"): ", strcurtime(), __FUNCTION__, hwr, hi->dup_name, hi->chn_name);
            cerr << myevent->errors[0].desc;
            fprintf(stderr, "\n");
        }
        return;
    }

    if (store_current_value (hi, myevent->attr_value) == 0)
        return_current_value(hi);
#endif /* USE_PUSH_MODEL */
}
