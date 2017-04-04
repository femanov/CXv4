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
    RSLV_TYPE_HWID = 1,  // server.hwid -- no resolving required
    RSLV_TYPE_NAME = 2,  // server.name -- resolve name->hwid within server
    RSLV_TYPE_GLBL = 3,  // global_name -- first find server-owner, than resolve within server
};

enum
{
    RSLV_STATE_UNKNOWN = 0, // _GLBL with no known relation to server
    RSLV_STATE_SERVER  = 1, // _GLBL with known server or _NAME; hwid unknown
    RSLV_STATE_ABSENT  = 2, // Any type which isn't found
    RSLV_STATE_DONE    = 3, // Any type resolved to known hwid
};


typedef struct
{
    int            in_use;

    cda_dataref_t  dataref; // "Backreference" to corr.entry in the global table
    char          *name;

    int            hwid;
    int            rslv_type;
    int            rslv_state;
    int            on_update;

    cda_hwcnref_t  next;    // Link to next hwr of sid
    cda_hwcnref_t  prev;    // Link to previous hwr of sid
} hwrinfo_t;

typedef struct
{
    cda_srvconn_t  sid;
    int            cd;
    int            is_suffering;
    int            was_data_somewhen;
    int            state;
    sl_tid_t       rcn_tid;

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

static cda_d_cx_privrec_t  udp_resolver_rec;

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
//////////////////////////////////////////////////////////////////////

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
            (!isalnum(*p)  &&  *p != '.')  &&  *p != '-') goto DO_RET;
    /* Okay, skip ':' */
    p++;
    /* ':' should be followed by a digit */
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
                              cxdtype_t dtype, int nelems)
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
    hi->on_update = ((tolower(*params) == 'u')  ||
                     (options & CDA_DATAREF_OPT_ON_UPDATE) != 0);
    cda_dat_p_set_hwr(ref, hwr);

    if (w_srv)
    {
        me = cda_dat_p_get_server(ref, &CDA_DAT_P_MODREC_NAME(cx), srvrspec);
        if (me == NULL)
        {
            RlsHwrSlot(hwr);
            return CDA_DAT_P_ERROR;
        }
        AddHwrToSrv(me, hwr);

        hi->hwid = strtol(hi->name, &errp, 10);
        if (errp == hi->name  ||  *errp != '\0')
            hi->rslv_type = RSLV_TYPE_NAME;
        else
            hi->rslv_type = RSLV_TYPE_HWID;

        if (me->state == CDA_DAT_P_OPERATING)
        {
            cx_begin(me->cd);
            if      (hi->rslv_type == RSLV_TYPE_NAME)
            {
                hi->rslv_state = RSLV_STATE_SERVER;
                cx_rslv(me->cd, hi->name, hwr, 0);
            }
            else
            {
                hi->rslv_state = RSLV_STATE_DONE;
                cx_rd_cur(me->cd, 1, &(hi->hwid), &hwr, NULL);
                cx_setmon(me->cd, 1, &(hi->hwid), &hwr, NULL, hi->on_update);
                /*!!! Obtain parameters */
                cda_dat_p_set_ready(hi->dataref, 1);
            }
            cx_run(me->cd);
        }
    }
    else
    {
        hi->rslv_type  = RSLV_TYPE_GLBL;
        hi->rslv_state = RSLV_STATE_UNKNOWN;
        cx_resolve(name);
    }

    return CDA_DAT_P_NOTREADY;
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
        hi->rslv_state == RSLV_STATE_DONE)
    {
        if (cx_begin(me->cd)         < 0  ||
            cx_rq_wr(me->cd, 1, &(hi->hwid), &hwr, NULL, 
            &dtype, &nelems, &value) < 0  ||
            cx_run  (me->cd)         < 0)
            return CDA_PROCESS_ERR;
    }

    return CDA_PROCESS_DONE;
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
        }
        cda_dat_p_update_server_cycle(me->sid);
    }
}

//////////////////////////////////////////////////////////////////////

static void ReconnectProc(int uniq, void *unsdptr,
                          sl_tid_t tid, void *privptr);

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
        if      (hi->rslv_type == RSLV_TYPE_NAME)
        {
            hi->rslv_state = RSLV_STATE_SERVER;
            cx_rslv(me->cd, hi->name, hwr, 0);
        }
        else if (hi->rslv_type == RSLV_TYPE_HWID)
        {
            hi->rslv_state = RSLV_STATE_DONE;
            cx_rd_cur(me->cd, 1, &(hi->hwid), &hwr, NULL);
            cx_setmon(me->cd, 1, &(hi->hwid), &hwr, NULL, hi->on_update);
            /*!!! Obtain parameters */
            cda_dat_p_set_ready(hi->dataref, 1);
        }
    }
    cx_run(me->cd);

    cda_dat_p_set_server_state(me->sid, me->state = CDA_DAT_P_OPERATING);
}

static void FailureProc(cda_d_cx_privrec_t *me, int reason)
{
  int                 ec = errno;
  int                 us;
  enum {MAX_US_X10 = 2*1000*1000*1000};

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
    for (hwr = me->frs_hwr;  hwr >= 0;  hwr = next)
    {
        hi = AccessHwrSlot(hwr);
        next = hi->next;
        if      (hi->rslv_type == RSLV_TYPE_GLBL)
        {
            DelHwrFromSrv(me,                hwr);
            AddHwrToSrv  (&udp_resolver_rec, hwr);
            hi->rslv_state = RSLV_STATE_UNKNOWN;
            cda_dat_p_set_ready(hi->dataref, 0);
        }
        else if (hi->rslv_type == RSLV_TYPE_NAME)
        {
            hi->rslv_state = RSLV_STATE_SERVER;
            cda_dat_p_set_ready(hi->dataref, 0);
        }
        /* Nothing to do with  == RSLV_TYPE_HWID */
    }
    
    /* Forget old connection */
    if (me->cd >= 0) cx_close(me->cd);
    me->cd = -1;

    MarkAllAsDefunct(me);

    /* And organize a timeout in a second... */
    if (me->rcn_tid >= 0) sl_deq_tout(me->rcn_tid);
    me->rcn_tid = sl_enq_tout_after(0, NULL, /*!!!uniq*/
                                    us, ReconnectProc, me);
}

static void ProcessCxlibEvent(int uniq, void *unsdptr,
                              int cd, int reason, const void *info,
                              void *privptr)
{
  cda_d_cx_privrec_t *me = (cda_d_cx_privrec_t *)privptr;
  int                 ec = errno;

  cx_newval_info_t   *nvi;
  cx_rslv_info_t     *rsi;
  cx_fresh_age_info_t *fai;
  cx_rds_info_t      *rdi;
  cx_strs_info_t     *sti;
  cx_quant_info_t    *qui;

  cda_hwcnref_t       hwr;
  hwrinfo_t          *hi;

    ////fprintf(stderr, "%s: reason=%d\n", __FUNCTION__, reason);
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
                hi->in_use)
            {
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

        case CAR_RSLV_RESULT:
            rsi = info;
            hwr = rsi->param1;
            ////fprintf(stderr, "rci:hwr=%d\n", hwr);
            hi  = AccessHwrSlot(hwr);
            if (/* "CheckHwr()" */
                hwr >= HWR_MIN_VAL  &&  hwr < hwrs_list_allocd  &&
                hi->in_use)
            {
                if (hi->rslv_state == RSLV_STATE_SERVER)
                {
                    if (rsi->hwid > 0)
                    {
                        ////fprintf(stderr, "<%s> :%d\n", hi->name, rsi->hwid);
                        hi->rslv_state = RSLV_STATE_DONE;
                        hi->hwid = rsi->hwid;
                        cx_begin(me->cd);
                        cx_rd_cur(me->cd, 1, &(hi->hwid), &hwr, NULL);
                        cx_setmon(me->cd, 1, &(hi->hwid), &hwr, NULL, hi->on_update);
                        cx_run(me->cd);
                        /*!!! Obtain parameters */
                        cda_dat_p_set_ready (hi->dataref, 1);
                    }
                    else
                    {
                        ////fprintf(stderr, "NOTFOUND %s\n", hi->name);
                        hi->rslv_state = RSLV_STATE_ABSENT;
                        cda_dat_p_set_notfound(hi->dataref);
                    }
                }
                else
                {
                    fprintf(stderr, "RSLV_RESULT: hwr=%d state=%d\n", hwr, hi->rslv_state);
                    /*??? debug message about an error?*/
                }
            }
            break;

        case CAR_FRESH_AGE:
            fai = info;
            hwr = fai->param1;
            hi  = AccessHwrSlot(hwr);
            if (/* "CheckHwr()" */
                hwr >= HWR_MIN_VAL  &&  hwr < hwrs_list_allocd  &&
                hi->in_use)
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
                hi->in_use)
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
                hi->in_use)
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
                hi->in_use)
            {
                cda_dat_p_set_quant(hi->dataref, qui->q, qui->q_dtype);
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

    me->cd  = DoConnect(me);
    if (me->cd < 0)
        FailureProc(me, CAR_CONNFAIL);
}

static int  cda_d_cx_new_srv (cda_srvconn_t  sid, void *pdt_privptr,
                              int            uniq,
                              const char    *srvrspec,
                              const char    *argv0)
{
  cda_d_cx_privrec_t *me = pdt_privptr;
  int                 ec;

////fprintf(stderr, "ZZZ %s(%s)\n", __FUNCTION__, srvrspec);
    me->sid = sid;
    me->state   = CDA_DAT_P_NOTREADY;
    me->rcn_tid = -1;
    me->frs_hwr = -1;
    me->lst_hwr = -1;

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

    return me->state;
}

static void cda_d_cx_del_srv (cda_srvconn_t  sid, void *pdt_privptr)
{
  cda_d_cx_privrec_t *me = pdt_privptr;
  
    if (me->cd >= 0)
        cx_close(me->cd);
    sl_deq_tout(me->rcn_tid); me->rcn_tid = -1;
}

//////////////////////////////////////////////////////////////////////

CDA_DEFINE_DAT_PLUGIN(cx, "CX data-access plugin",
                      NULL, NULL,
                      sizeof(cda_d_cx_privrec_t),
                      '.', ':', '@',
                      cda_d_cx_new_chan, NULL,
                      cda_d_cx_snd_data, NULL,
                      cda_d_cx_new_srv,  cda_d_cx_del_srv);