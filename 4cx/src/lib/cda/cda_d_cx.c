#include <errno.h>
#include <ctype.h>
#include <string.h>

#include "misc_macros.h"
#include "misclib.h"
#include "cxscheduler.h"

#include "cxlib.h"
#include "cda.h"
#include "cdaP.h"

#include "cda_d_cx.h"


static int IsATemporaryCxError(void)
{
    return
        IS_A_TEMPORARY_CONNECT_ERROR()  ||
        errno == CENOHOST    ||
        errno == CETIMEOUT   ||  errno == CEMANYCLIENTS  ||
        errno == CESRVNOMEM  ||  errno == CEMCONN;
}

enum
{
//    RSLV_TYPE_HWID = 1,  // server.hwid -- no resolving required
    RSLV_TYPE_NAME = 2,  // server.name -- resolve name->chnd within server
    RSLV_TYPE_GLBL = 3,  // global_name -- first find server-owner, than resolve within server
};

enum
{
    RSLV_STATE_UNKNOWN = 0, // _GLBL with no known relation to server
    RSLV_STATE_SERVER  = 1, // _GLBL with known server or _NAME; chnd unknown
    RSLV_STATE_ABSENT  = 2, // Any type which isn't found
    RSLV_STATE_FOUND1  = 3, // Any type after CXC_NT_OPEN_FOUND_STAGE1 and before CXC_NT_OPEN_FOUND_STAGE2
    RSLV_STATE_DONE    = 4, // Any type resolved to known chnd
};

enum
{
    MODE_ON_UPDATE = 1 << 0,  // } Note: these 2 flags are INTENTIONALLY set to 2 lower bits,
    MODE_NOMONITOR = 1 << 1,  // }       for them to be usable as array index
    MODE_RQ_LOCK   = 1 << 2,

    MODE_MON_TYPE_mask = MODE_ON_UPDATE | MODE_NOMONITOR,
};
// Usage: mode2upd_cond[hi->mode & MODE_MON_TYPE_mask]
static int mode2upd_cond[4] =
{
    CX_UPD_COND_ON_CYCLE,  // on_update=0 nomonitor=0
    CX_UPD_COND_ON_UPDATE, // on_update=1 nomonitor=0
    CX_UPD_COND_NEVER,     // on_update=0 nomonitor=1
    CX_UPD_COND_NEVER,     // on_update=1 nomonitor=1
};

enum
{
    SRVTYPE_DATA_IO  = 0,
    SRVTYPE_RESOLVER = 1,
};


typedef struct
{
    int            in_use;

    cda_dataref_t  dataref; // "Backreference" to corr.entry in the global table
    char          *name;

    int            chnd;
    int            rslv_type;
    int            rslv_state;
    int            mode;

    cda_hwcnref_t  next;    // Link to next hwr of sid
    cda_hwcnref_t  prev;    // Link to previous hwr of sid
} hwrinfo_t;

typedef struct _cda_d_cx_privrec_t_struct
{
    cda_srvconn_t  sid;
    int            being_processed;
    int            being_destroyed;

    int            srvtype;
    int            cd;
    int            is_suffering;
    int            was_data_somewhen;
    int            state;
    sl_tid_t       rcn_tid;

    struct _cda_d_cx_privrec_t_struct
                  *resolver;

    cda_hwcnref_t  frs_hwr;
    cda_hwcnref_t  lst_hwr;
} cda_d_cx_privrec_t;

//////////////////////////////////////////////////////////////////////

enum
{
    HWR_MIN_VAL   = 1,
    HWR_MAX_COUNT = 1000000,  // An arbitrary limit
    HWR_ALLOC_INC = 100,
};


static hwrinfo_t     *hwrs_list;
static int            hwrs_list_allocd;

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
    safe_free(hi->name);
    hi->in_use = 0;
}

//--------------------------------------------------------------------

static void AddHwrToSrv  (cda_d_cx_privrec_t *me, cda_hwcnref_t hwr)
{
  hwrinfo_t     *hi   = AccessHwrSlot(hwr);

    hi->prev = me->lst_hwr;
    hi->next = -1;

    if (me->lst_hwr >= 0)
        AccessHwrSlot(me->lst_hwr)->next = hwr;
    else
        me->frs_hwr                      = hwr;
    me->lst_hwr = hwr;
}

static void DelHwrFromSrv(cda_d_cx_privrec_t *me, cda_hwcnref_t hwr)
{
  hwrinfo_t     *hi   = AccessHwrSlot(hwr);
  cda_hwcnref_t  prev = hi->prev;
  cda_hwcnref_t  next = hi->next;

    if (prev < 0) me->frs_hwr = next; else AccessHwrSlot(prev)->next = next;
    if (next < 0) me->lst_hwr = prev; else AccessHwrSlot(next)->prev = prev;

    hi->prev = hi->next = -1;
}

//--------------------------------------------------------------------

static void DestroyCxPrivrec(cda_d_cx_privrec_t *me)
{
  cda_hwcnref_t       hwr;
  hwrinfo_t          *hi;
  cda_hwcnref_t       next;

    for (hwr = me->frs_hwr;  hwr >= 0;  hwr = next)
    {
        hi = AccessHwrSlot(hwr);
        next = hi->next;

        DelHwrFromSrv(me, hwr);
        RlsHwrSlot(hwr);
    }

    if (me->cd >= 0) cx_close(me->cd); me->cd      = -1;
    sl_deq_tout(me->rcn_tid);          me->rcn_tid = -1;
}

//--------------------------------------------------------------------

static inline cda_d_cx_privrec_t * get_DATA_IO_server (cda_dataref_t  source_ref,
                                                       const char    *srvrspec)
{
    return cda_dat_p_get_server(source_ref, &CDA_DAT_P_MODREC_NAME(cx),
                                srvrspec,
                                SRVTYPE_DATA_IO | CDA_DAT_P_GET_SERVER_OPT_NONE);
}

static inline cda_d_cx_privrec_t * get_RESOLVER_server(cda_dataref_t  source_ref)
{
    return cda_dat_p_get_server(source_ref, &CDA_DAT_P_MODREC_NAME(cx),
                                "RESOLVER",
                                SRVTYPE_RESOLVER | CDA_DAT_P_GET_SERVER_OPT_NOLIST);
}

//////////////////////////////////////////////////////////////////////


static int determine_name_type(const char *name,
                               char srvrspec[], size_t srvrspec_size,
                               char **channame_p)
{
  int         srv_len;
  const char *colon_p;
  const char *p;
  char       *vp;

    /* Defaults */
    srv_len = 0;
    *channame_p = name;

    /* Doesn't start with alphanumeric (or just ":...")? */
    if (*name != ':'  &&  !isalnum(*name)) goto DO_RET;

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
    /* ...with an optional leading '-' */
    if (*p == '-') p++;
    if (*p == '\0'  ||  !isdigit(*p)) goto DO_RET;
    /* Okay, skip digits... */
    while (*p != '\0'  &&  isdigit(*p)) p++;
    //
    if (*p != '.') goto DO_RET;

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

static int  cda_d_cx_new_chan(cda_dataref_t ref, const char *name,
                              int options,
                              cxdtype_t dtype, int max_nelems)
{
  cda_d_cx_privrec_t *me;

  char                srvrspec[200];
  const char         *channame;
  int                 w_srv;
  const char         *at_p;
  size_t              channamelen;
  const char         *params;

  cda_hwcnref_t       hwr;
  hwrinfo_t          *hi;

  char               *errp;

    w_srv = determine_name_type(name, srvrspec, sizeof(srvrspec), &channame);
    ////fprintf(stderr, "\t%s(%d, \"%s\")\n\t\t[%s]%s\n", __FUNCTION__, ref, name, srvrspec, channame);

    if (strcasecmp(srvrspec, "unknown") == 0)
    {
        srvrspec[0] = '\0';
        w_srv = 0;
    }

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
        cda_set_err("empty CHANNEL name");
        return CDA_DAT_P_ERROR;
    }

    /* Alloc and fill a slot */
    hwr = GetHwrSlot();
    if (hwr < 0) return CDA_DAT_P_ERROR;
    hi = AccessHwrSlot(hwr);
    // Fill data
    hi->next    = -1;
    hi->prev    = -1;
    hi->dataref = ref;
    hi->name = malloc(channamelen + 1);
    if (hi->name == NULL)
    {
        RlsHwrSlot(hwr);
        return CDA_DAT_P_ERROR;
    }
    memcpy(hi->name, channame, channamelen); hi->name[channamelen] = '\0';
    if ((tolower(*params) == 'u')  ||
        (options & CDA_DATAREF_OPT_ON_UPDATE) != 0) hi->mode |= MODE_ON_UPDATE;
    if ((options & CDA_DATAREF_OPT_EXCLUSIVE) != 0) hi->mode |= MODE_RQ_LOCK;
    if ((options & CDA_DATAREF_OPT_NOMONITOR) != 0) hi->mode |= MODE_NOMONITOR;
    cda_dat_p_set_hwr(ref, hwr);

    if (w_srv)
    {
        hi->rslv_type = RSLV_TYPE_NAME;

        me = get_DATA_IO_server(ref, srvrspec);
        if (me == NULL)
        {
            RlsHwrSlot(hwr);
            return CDA_DAT_P_ERROR;
        }
        AddHwrToSrv(me, hwr);

        if (me->state == CDA_DAT_P_OPERATING)
        {
            cx_begin  (me->cd);
            hi->rslv_state = RSLV_STATE_SERVER;
            cx_ch_open(me->cd, hi->name, 0/*fail_on_err*/,
                       mode2upd_cond[hi->mode & MODE_MON_TYPE_mask], hwr, 0);
            cx_run    (me->cd);
        }
    }
    else
    {
        ////fprintf(stderr, "<%s>\n", hi->name);
        hi->rslv_type  = RSLV_TYPE_GLBL;
        hi->rslv_state = RSLV_STATE_UNKNOWN;

        me = get_RESOLVER_server(ref);
        if (me == NULL)
        {
            RlsHwrSlot(hwr);
            return CDA_DAT_P_ERROR;
        }
        AddHwrToSrv(me, hwr);
        cda_dat_p_report_rslvstat(hi->dataref, CDA_RSLVSTAT_SEARCHING);
    }

    return CDA_DAT_P_NOTREADY;
}

static void cda_d_cx_del_chan(void *pdt_privptr, cda_hwcnref_t hwr)
{
  cda_d_cx_privrec_t *me = pdt_privptr;
  hwrinfo_t          *hi = AccessHwrSlot(hwr);

    if (hwr < HWR_MIN_VAL  ||  hwr >= hwrs_list_allocd  ||
        hi->in_use == 0)
    {
        /*!!!Bark? */
        return;
    }

    if (me->state == CDA_DAT_P_OPERATING)
    {
        /* Send "release" */
        cx_begin   (me->cd);
        cx_ch_close(me->cd, hi->chnd, hwr, 0);
        cx_run     (me->cd);
    }

    DelHwrFromSrv(me, hwr); /*!!! Should check if hwr belongs to me! */
    RlsHwrSlot(hwr);
}

static int  cda_d_cx_snd_data(void *pdt_privptr, cda_hwcnref_t hwr,
                              cxdtype_t dtype, int nelems, void *value)
{
  cda_d_cx_privrec_t *me = pdt_privptr;
  hwrinfo_t          *hi = AccessHwrSlot(hwr);

    /* A safety net */
    if (hwr < HWR_MIN_VAL  ||  hwr >= hwrs_list_allocd  ||
        hi->in_use == 0)
    {
        /* Ouch... A cda_core error? */
        cda_dat_p_report(-1,
                         "%s(me=%p): attempt to write to illegal hwr=%d (hwrs_list_allocd=%d)\n",
                         __FUNCTION__, me, hwr, hwrs_list_allocd);
        return CDA_PROCESS_SEVERE_ERR;
    }

    if (me->state      == CDA_DAT_P_OPERATING  &&
        hi->rslv_state >= RSLV_STATE_FOUND1)
    {
        if (cx_begin   (me->cd)               < 0  ||
            cx_ch_rq_wr(me->cd, hi->chnd,
                        dtype, nelems, value) < 0  ||
            cx_run     (me->cd)               < 0)
            return CDA_PROCESS_ERR;
    }

    return CDA_PROCESS_DONE;
}

static int  cda_d_cx_lock_op (void *pdt_privptr,
                              int   count, cda_hwcnref_t *hwrs,
                              int   operation, int lockset_id)
{
  cda_d_cx_privrec_t *me = pdt_privptr;
  int                 do_lock = (operation & CX_LOCK_WR) != 0;
  hwrinfo_t          *hi;
  int                 n;

    // Stage 1: remember the desired-lock-state
    for (n = 0;  n < count;  n++)
        if (hwrs[n] >= HWR_MIN_VAL  &&  hwrs[n] < hwrs_list_allocd  &&
            (hi = AccessHwrSlot(hwrs[n]))->in_use)
        {
            if (do_lock) hi->mode |=  MODE_RQ_LOCK;
            else         hi->mode &=~ MODE_RQ_LOCK;
        }

    if (me->state != CDA_DAT_P_OPERATING) return 0;

    cx_begin(me->cd);
    for (n = 0;  n < count;  n++)
        if (hwrs[n] >= HWR_MIN_VAL  &&  hwrs[n] < hwrs_list_allocd  &&
            (hi = AccessHwrSlot(hwrs[n]))->in_use  &&
            hi->rslv_state >= RSLV_STATE_FOUND1)
            cx_ch_rq_l_o(me->cd, hi->chnd, operation);
    cx_run(me->cd); /*!!! Maybe count number of requests and if ==0 then cancel request? */

    return 0;
}

//////////////////////////////////////////////////////////////////////

static void MarkAllAsDefunct(cda_d_cx_privrec_t *me)
{
  cda_hwcnref_t       hwr;
  hwrinfo_t          *hi;
  cda_hwcnref_t       next;

    if(0)fprintf(stderr, "%s: %s\n", strcurtime(), __FUNCTION__);
    if (me->was_data_somewhen)
    {
        for (hwr = me->frs_hwr;  hwr >= 0;  hwr = next)
        {
            hi = AccessHwrSlot(hwr);
            next = hi->next;
            cda_dat_p_defunct_dataset(me->sid, 1, &(hi->dataref));
            cda_dat_p_report_dataset_lockstat (me->sid,
                                               1, &(hi->dataref),
                                               0);
        }
        cda_dat_p_update_server_cycle(me->sid);
    }
}

//////////////////////////////////////////////////////////////////////

static void ReconnectProc(int uniq, void *unsdptr,
                          sl_tid_t tid, void *privptr);

static void ScheduleSearch(cda_d_cx_privrec_t *me, int usecs);

static void ScheduleReconnect(cda_d_cx_privrec_t *me, int us)
{
    if (me->rcn_tid >= 0) sl_deq_tout(me->rcn_tid);
    me->rcn_tid = sl_enq_tout_after(cda_dat_p_suniq_of_sid(me->sid), NULL, /*!!!uniq*/
                                    us, ReconnectProc, me);
}

static void SuccessProc(cda_d_cx_privrec_t *me)
{
  cda_hwcnref_t       hwr;
  hwrinfo_t          *hi;
  cda_hwcnref_t       next;

    if (me->is_suffering)
    {
        cda_dat_p_report(me->sid, "connected.");
        me->is_suffering = 0;
    }
    
    sl_deq_tout(me->rcn_tid); me->rcn_tid = -1;

    cx_begin(me->cd);
    for (hwr = me->frs_hwr;  hwr >= 0;  hwr = next)
    {
        hi = AccessHwrSlot(hwr);
        next = hi->next;
        if      (hi->rslv_type == RSLV_TYPE_NAME  ||
                 hi->rslv_type == RSLV_TYPE_GLBL/*!!! WHAT does "GLBL" do here? */)
        {
            hi->rslv_state = RSLV_STATE_SERVER;
            cx_ch_open(me->cd, hi->name, 0/*fail_on_err*/,
                       mode2upd_cond[hi->mode & MODE_MON_TYPE_mask], hwr, 0);
        }
    }
    if (cx_run(me->cd) < 0) return;

    // "Ping" our resolver
    if (me->resolver != NULL)
        ScheduleSearch(me->resolver, 1);

    cda_dat_p_set_server_state(me->sid, me->state = CDA_DAT_P_OPERATING);
}

static void FailureProc(cda_d_cx_privrec_t *me, int reason)
{
  int                 ec = errno;
  int                 us;
  enum {MAX_US_X10 = 2*1000*1000*1000};

  cda_d_cx_privrec_t *rs;

  cda_hwcnref_t       hwr;
  hwrinfo_t          *hi;
  cda_hwcnref_t       next;

    us = 1*1000*1000;
    if (ec == CENOHOST)
    {
        if (us > MAX_US_X10 / 10)
            us = MAX_US_X10;
        else
            us *= 10;
    }

    if (!me->is_suffering)
    {
        cda_dat_p_report(me->sid, "%s: %s; will reconnect.",
                         cx_strreason(reason), cx_strerror(ec));
        me->is_suffering = 1;
    }

    /* Notify cda */
    cda_dat_p_set_server_state(me->sid, me->state = CDA_DAT_P_NOTREADY);

    /* Modify channels' resolve states */
    rs = me->resolver;
    for (hwr = me->frs_hwr;  hwr >= 0;  hwr = next)
    {
        hi = AccessHwrSlot(hwr);
        next = hi->next;
        hi->chnd = -1;
        if      (hi->rslv_type == RSLV_TYPE_GLBL)
        {
            if (rs == NULL)
            {
                rs = get_RESOLVER_server(hi->dataref);
                if (rs == NULL)
                {
                    /* "Unable to get a resolver" -- probably
                       an impossible situation, since one MUST exist as
                       a prerequisite for GLBL channels */
                    continue;
                }
                me->resolver = rs;
            }

            DelHwrFromSrv(me, hwr);
            AddHwrToSrv  (rs, hwr);
            hi->rslv_state = RSLV_STATE_UNKNOWN;
            cda_dat_p_set_ready(hi->dataref, 0);
            cda_dat_p_report_rslvstat(hi->dataref, CDA_RSLVSTAT_SEARCHING);
        }
        else if (hi->rslv_type == RSLV_TYPE_NAME)
        {
            hi->rslv_state = RSLV_STATE_SERVER;
            cda_dat_p_set_ready(hi->dataref, 0);
        }
    }
    
    /* Forget old connection */
    if (me->cd >= 0) cx_close(me->cd);
    me->cd = -1;

    MarkAllAsDefunct(me);

    /* And organize a timeout in a second... */
    ScheduleReconnect(me, us);
}

static void ProcessCxlibEvent(int uniq, void *unsdptr,
                              int cd, int reason, const void *info,
                              void *privptr)
{
  cda_d_cx_privrec_t *me = (cda_d_cx_privrec_t *)privptr;
  int                 ec = errno;

  cda_d_cx_privrec_t *rs;

  const cx_newval_info_t    *nvi;
  const cx_fresh_age_info_t *fai;
  const cx_rds_info_t       *rdi;
  const cx_strs_info_t      *sti;
  const cx_quant_info_t     *qui;
  const cx_range_info_t     *rni;
  const cx_lockstat_info_t  *loi;
  const cx_ch_open_info_t   *opi;

  cda_hwcnref_t       hwr;
  hwrinfo_t          *hi;

    ////fprintf(stderr, "%s: reason=%d\n", __FUNCTION__, reason);
    me->being_processed++;

    switch (reason)
    {
        case CAR_NEWDATA:
            me->was_data_somewhen = 1;
            nvi = info;
            hwr = nvi->param1;
            ////fprintf(stderr, "nvi:hwr=%d\n", hwr);
            hi  = AccessHwrSlot(hwr);
            if (/* "CheckHwr()" */
                hwr >= HWR_MIN_VAL  &&  hwr < hwrs_list_allocd  &&
                hi->in_use  &&
                hi->rslv_state >= RSLV_STATE_FOUND1)
            {
                ////fprintf(stderr, "dtype=%d nelems=%d rflags=%d is_update=%d\n", nvi->dtype, nvi->nelems, nvi->rflags, nvi->is_update);
                cda_dat_p_update_dataset(me->sid,
                                         1, &(hi->dataref),
                                         &(nvi->data),
                                         &(nvi->dtype),
                                         &(nvi->nelems),
                                         &(nvi->rflags),
                                         &(nvi->timestamp),
                                         nvi->is_update);
            }
            break;

        case CAR_CYCLE:
            cda_dat_p_update_server_cycle(me->sid);
            break;

        case CAR_CH_OPEN_RESULT:
            opi = info;
            hwr = opi->param1;
            hi  = AccessHwrSlot(hwr);
            if (/* "CheckHwr()" */
                hwr >= HWR_MIN_VAL  &&  hwr < hwrs_list_allocd  &&
                hi->in_use  &&
                // Check if that's an answer to THIS channel
                opi->name != NULL  &&  strcasecmp(opi->name, hi->name) == 0)
            {
                if (hi->rslv_state == RSLV_STATE_SERVER  ||  hi->rslv_state == RSLV_STATE_FOUND1)
                {
                    if      (opi->status == 0)
                    {
                        hi->rslv_state = RSLV_STATE_FOUND1;
                        hi->chnd = opi->chnd;
                        cda_dat_p_set_hwinfo(hi->dataref,
                                             opi->rw, opi->dtype, opi->max_nelems,
                                             opi->hwid);
                        cda_dat_p_report_rslvstat(hi->dataref, CDA_RSLVSTAT_FOUND);
                        /* Try to put a lock, if required */
                        /* Note: this must be the LAST action,
                           otherwise all threee calls should be checked for "< 0" */
                        if ((hi->mode & MODE_RQ_LOCK) != 0)
                        {
                            cx_begin(me->cd);
                            cx_ch_rq_l_o(me->cd, hi->chnd, CX_LOCK_WR | CX_LOCK_ALLORNOTHING);
                            cx_run(me->cd);
                        }
                    }
                    else if (opi->status > 0)
                    {
                        hi->rslv_state = RSLV_STATE_DONE;
                        cda_dat_p_set_ready (hi->dataref, 1);
                    }
                    else
                    {
                        ////fprintf(stderr, "NOTFOUND P%s\n", hi->name);

                        if (hi->rslv_type == RSLV_TYPE_GLBL)
                        {
                            // The following code is a copy from FailureProc()
                            rs = me->resolver;
                            if (rs == NULL)
                            {
                                rs = get_RESOLVER_server(hi->dataref);
                                if (rs == NULL)
                                {
                                    /* "Unable to get a resolver" -- probably
                                       an impossible situation, since one MUST exist as
                                       a prerequisite for GLBL channels */
                                    return;
                                }
                                me->resolver = rs;
                            }

                            DelHwrFromSrv(me, hwr);
                            AddHwrToSrv  (rs, hwr);
                            hi->rslv_state = RSLV_STATE_UNKNOWN;
                            cda_dat_p_set_ready(hi->dataref, 0);
                            cda_dat_p_report_rslvstat(hi->dataref, CDA_RSLVSTAT_SEARCHING);
                        }
                        else
                        {
                            hi->rslv_state = RSLV_STATE_ABSENT;
                            cda_dat_p_report_rslvstat(hi->dataref, CDA_RSLVSTAT_NOTFOUND);
                        }
                    }
                }
                else
                {
                    //fprintf(stderr, "RSLV_RESULT: hwr=%d state=%d\n", hwr, hi->rslv_state);
                    cda_ref_p_report(hi->dataref, "duplicate CH_OPEN_RESULT/status=%d (state=%d, type=%d) for <%s>", opi->status, hi->rslv_state, hi->rslv_type, hi->name);
                }
            }
            break;

        case CAR_FRESH_AGE:
            fai = info;
            hwr = fai->param1;
            hi  = AccessHwrSlot(hwr);
            if (/* "CheckHwr()" */
                hwr >= HWR_MIN_VAL  &&  hwr < hwrs_list_allocd  &&
                hi->in_use  &&
                hi->rslv_state >= RSLV_STATE_FOUND1)
            {
                cda_dat_p_set_fresh_age(hi->dataref, fai->fresh_age);
            }
            break;

        case CAR_RDS:
            rdi = info;
            hwr = rdi->param1;
            hi  = AccessHwrSlot(hwr);
            if (/* "CheckHwr()" */
                hwr >= HWR_MIN_VAL  &&  hwr < hwrs_list_allocd  &&
                hi->in_use  &&
                hi->rslv_state >= RSLV_STATE_FOUND1)
            {
                cda_dat_p_set_phys_rds(hi->dataref, rdi->phys_count, rdi->rds);
            }
            break;

        case CAR_STRS:
            sti = info;
            hwr = sti->param1;
            hi  = AccessHwrSlot(hwr);
            if (/* "CheckHwr()" */
                hwr >= HWR_MIN_VAL  &&  hwr < hwrs_list_allocd  &&
                hi->in_use  &&
                hi->rslv_state >= RSLV_STATE_FOUND1)
            {
                cda_dat_p_set_strings(hi->dataref,
                                      sti->strings[0], sti->strings[1],
                                      sti->strings[2], sti->strings[3],
                                      sti->strings[4], sti->strings[5],
                                      sti->strings[6], sti->strings[7]);
            }
            break;

        case CAR_QUANT:
            qui = info;
            hwr = qui->param1;
            hi  = AccessHwrSlot(hwr);
            if (/* "CheckHwr()" */
                hwr >= HWR_MIN_VAL  &&  hwr < hwrs_list_allocd  &&
                hi->in_use  &&
                hi->rslv_state >= RSLV_STATE_FOUND1)
            {
                cda_dat_p_set_quant(hi->dataref, qui->q, qui->q_dtype);
            }
            break;

        case CAR_RANGE:
            rni = info;
            hwr = rni->param1;
            hi  = AccessHwrSlot(hwr);
            if (/* "CheckHwr()" */
                hwr >= HWR_MIN_VAL  &&  hwr < hwrs_list_allocd  &&
                hi->in_use  &&
                hi->rslv_state >= RSLV_STATE_FOUND1)
            {
                cda_dat_p_set_range(hi->dataref, rni->range, rni->range_dtype);
            }
            break;

        case CAR_LOCKSTAT:
            loi = info;
            hwr = loi->param1;
            hi  = AccessHwrSlot(hwr);
            if (/* "CheckHwr()" */
                hwr >= HWR_MIN_VAL  &&  hwr < hwrs_list_allocd  &&
                hi->in_use  &&
                hi->rslv_state >= RSLV_STATE_FOUND1)
            {
                cda_dat_p_report_dataset_lockstat(me->sid,
                                                  1, &(hi->dataref),
                                                  loi->lockstat);
            }
            break;

        case CAR_CONNECT:
            SuccessProc(me);
            break;

        case CAR_CONNFAIL:
        case CAR_ERRCLOSE:
        case CAR_KILLED:
            FailureProc(me, reason);
            break;

        case CAR_EACCESS:
            // What can we do here?
            cda_dat_p_report(me->sid, "%s: %s; will NOT reconnect.",
                             cx_strreason(reason), cx_strerror(ec));
            break;
    }

    me->being_processed--;
    if (me->being_processed == 0  &&  me->being_destroyed)
    {
        DestroyCxPrivrec(me);
        cda_dat_p_del_server_finish(me->sid);
    }
}

static int  DoConnect(cda_d_cx_privrec_t *me)
{
  char        envname[300];
  char       *p;
  const char *spec;
  const char *tr_spec;

    spec = cda_dat_p_srvrn_of_sid(me->sid);
    if (spec != NULL)
    {
        check_snprintf(envname, sizeof(envname), "CX_TRANSLATE_%s", spec);

        for (p = envname; *p != '\0';  p++)
        {
            if (isalnum(*p)) *p = toupper(*p);
            else             *p = '_';
        }
        tr_spec = getenv(envname);
        if (tr_spec != NULL) spec = tr_spec;
    }

    return cx_open(cda_dat_p_suniq_of_sid(me->sid), NULL,
                   spec,                        0,
                   cda_dat_p_argv0_of_sid(me->sid), NULL,
                   ProcessCxlibEvent,           me);
}

static void ReconnectProc(int uniq, void *unsdptr,
                          sl_tid_t tid, void *privptr)
{
  cda_d_cx_privrec_t *me = privptr;

    me->rcn_tid = -1;

    me->being_processed++;

    me->cd  = DoConnect(me);
    if (me->cd < 0)
        FailureProc(me, CAR_CONNFAIL);

    me->being_processed--;
    if (me->being_processed == 0  &&  me->being_destroyed)
    {
        DestroyCxPrivrec(me);
        cda_dat_p_del_server_finish(me->sid);
    }
}

static void ProcessSrchEvent(int uniq, void *unsdptr,
                             int cd, int reason, const void *info,
                             void *privptr)
{
  cda_d_cx_privrec_t   *me  = (cda_d_cx_privrec_t *)privptr;
  const cx_srch_info_t *sip = info;

  cda_hwcnref_t         hwr;
  hwrinfo_t            *hi;

  char                  srvrspec[200];
  cda_d_cx_privrec_t   *ts; // "ts" -- Target Server

////fprintf(stderr, "%s \"%s\" param1=%d param2=%d %s:%d\n", __FUNCTION__, sip->name, sip->param1, sip->param2, sip->srv_addr, sip->srv_n);
    if (reason != CAR_SRCH_RESULT) return;

    me->being_processed++;

    hwr = sip->param1;
    hi  = AccessHwrSlot(hwr);
    if (/* "CheckHwr()" */
        hwr >= HWR_MIN_VAL  &&  hwr < hwrs_list_allocd  &&
        hi->in_use                                      &&
        /*!!! Must check that this hwr belongs to this server */ 1 &&
        hi->rslv_state == RSLV_STATE_UNKNOWN)
    {
        if (strcasecmp(sip->name, hi->name) == 0)
        {
            /* Okay, let's transplant it to the server */
            /* 1. Obtain a server */
            check_snprintf(srvrspec, sizeof(srvrspec),
                           "%s:%d", sip->srv_addr, sip->srv_n);
            ts = get_DATA_IO_server(hi->dataref, srvrspec);
////fprintf(stderr, "ts=%p\n", ts);
            if (ts == NULL)
            {
                /* Ouch... */
                /* !!!Print something to stderr? */
                goto END_PROCESSED;
            }

            /* 2. Do transfer */
            DelHwrFromSrv(me, hwr);
            AddHwrToSrv  (ts, hwr);

            /* 3. Modify state */
            hi->rslv_state = RSLV_STATE_SERVER;
////fprintf(stderr, "ts->state=%d\n", ts->state);
            if (ts->state == CDA_DAT_P_OPERATING)
            {
                cx_begin  (ts->cd);
                cx_ch_open(ts->cd, hi->name, 1/*fail_on_err*/,
                           mode2upd_cond[hi->mode & MODE_MON_TYPE_mask], hwr, 0);
                cx_run    (ts->cd);
            }
        }
        else
        {
            /* Non-matching name? Should we print some diagnostics,
               or is it just because of UDP-port reuse? */
        }
    }

 END_PROCESSED:
    me->being_processed--;
    if (me->being_processed == 0  &&  me->being_destroyed)
    {
        DestroyCxPrivrec(me);
        cda_dat_p_del_server_finish(me->sid);
    }
}

static void PeriodicSrchProc(int uniq, void *unsdptr,
                             sl_tid_t tid, void *privptr)
{
  cda_d_cx_privrec_t *me = privptr;
  cda_hwcnref_t       hwr;
  hwrinfo_t          *hi;
  int                 r;

    me->rcn_tid = -1;

    if (me->frs_hwr > 0)
    {
        cx_begin(me->cd);
        for (hwr = me->frs_hwr;  hwr >= 0;  hwr = hi->next)
        {
            hi = AccessHwrSlot(hwr);
            /* 1st try... */
            r = cx_srch(me->cd, hi->name, hwr, 0);
            /* ...failed? Okay, */
            if (r != 0)
            {
                /* Send current buffer, */
                cx_run  (me->cd);
                /* start new packet */
                cx_begin(me->cd);
                /* 2nd try */
                cx_srch(me->cd, hi->name, hwr, 0);
            }
        }
        cx_run  (me->cd);
    }

    ScheduleSearch(me, 10*1000000);
}
static void ScheduleSearch(cda_d_cx_privrec_t *me, int usecs)
{
    if (me->rcn_tid >= 0) sl_deq_tout(me->rcn_tid);
    me->rcn_tid = sl_enq_tout_after(cda_dat_p_suniq_of_sid(me->sid), NULL,
                                    usecs, PeriodicSrchProc, me);

}

static int  cda_d_cx_new_srv (cda_srvconn_t  sid, void *pdt_privptr,
                              int            uniq,
                              const char    *srvrspec,
                              const char    *argv0,
                              int            srvtype)
{
  cda_d_cx_privrec_t *me = pdt_privptr;
  int                 ec;

////fprintf(stderr, "ZZZ %s(%s)\n", __FUNCTION__, srvrspec);
    /* Common initialization */
    me->sid = sid;
    me->state   = CDA_DAT_P_NOTREADY;
    me->rcn_tid = -1;
    me->frs_hwr = -1;
    me->lst_hwr = -1;

    me->srvtype = srvtype;

    /* Type-dependent cations */
    if (srvtype == SRVTYPE_RESOLVER)
    {
        me->cd = cx_seeker(uniq, NULL,
                           cda_dat_p_argv0_of_sid(me->sid), NULL,
                           ProcessSrchEvent,                me);
        if (me->cd < 0)
        {
            ec = errno;
            cda_set_err("cx_seeker(\"%s\"): %s",
                        cda_dat_p_srvrn_of_sid(me->sid), cx_strerror(ec));
            return CDA_DAT_P_ERROR;
        }

        /* Periodic timeout for searching;
           1st time is after 1us, i.e. "immediately after main loop start" */
        ScheduleSearch(me, 1);
    }
    else
    {
        me->cd  = DoConnect(me);
        if (me->cd < 0)
        {
            if (!IsATemporaryCxError())
            {
                ec = errno;
                cda_set_err("cx_open(\"%s\"): %s",
                            cda_dat_p_srvrn_of_sid(me->sid), cx_strerror(ec));
                return CDA_DAT_P_ERROR;
            }
            
            FailureProc(me, CAR_CONNFAIL);
        }
    }

    return me->state;
}


static int  cda_d_cx_del_srv (cda_srvconn_t  sid, void *pdt_privptr)
{
  cda_d_cx_privrec_t *me = pdt_privptr;

    if (me->being_processed)
    {
        me->being_destroyed = 1;
        return CDA_DAT_P_DEL_SRV_DEFERRED;
    }
    else
    {
        DestroyCxPrivrec(me);
        return CDA_DAT_P_DEL_SRV_SUCCESS;
    }
}

static int  cda_d_cx_srv_ioctl(void *pdt_privptr,
                               int   opcode, void *info_ptr)
{
  cda_d_cx_privrec_t *me = pdt_privptr;

    if      (opcode == CDA_DAT_P_SRV_IOCTL_NR_RECONNECT)
    {
        if (me->cd < 0) ScheduleReconnect(me, 1);
    }
    else
    {
        errno = EINVAL;
        return -1;
    }
}

//////////////////////////////////////////////////////////////////////

CDA_DEFINE_DAT_PLUGIN(cx, "CX data-access plugin",
                      NULL, NULL,
                      sizeof(cda_d_cx_privrec_t),
                      CDA_DAT_P_FLAG_CHAN_TYPE_CHANGE_SUPPORTED,
                      '.', ':', '@',
                      cda_d_cx_new_chan, cda_d_cx_del_chan,
                      NULL,
                      cda_d_cx_snd_data, cda_d_cx_lock_op,
                      cda_d_cx_new_srv,  cda_d_cx_del_srv,
                      NULL, cda_d_cx_srv_ioctl,
                      NULL, NULL);
