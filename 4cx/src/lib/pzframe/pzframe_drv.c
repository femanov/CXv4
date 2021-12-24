#include "timeval_utils.h"

#include "pzframe_drv.h"


psp_lkp_t pzframe_drv_run_mode_lkp[] =
{
    {"on_request",  PZFRAME_RUN_MODE_ON_REQUEST},
    {"on_run",      PZFRAME_RUN_MODE_ON_RUN},
    {"auto_re_run", PZFRAME_RUN_MODE_AUTO_RE_RUN},
    {"disabled",    PZFRAME_RUN_MODE_DISABLED},
    {NULL, 0}
};


static void pzframe_tout_p(int devid, void *devptr,
                           sl_tid_t tid,
                           void *privptr);


static void ReturnMeasurements(pzframe_drv_t *pdr, int do_read,
                               rflags_t add_rflags)
{
    if (do_read) pdr->read_measurements(pdr);
    pdr->prepare_retbufs               (pdr, add_rflags);
    ReturnDataSet(pdr->devid,
                  pdr->retbufs.count,
                  pdr->retbufs.addrs,  pdr->retbufs.dtypes, pdr->retbufs.nelems,
                  pdr->retbufs.values, pdr->retbufs.rflags, pdr->retbufs.timestamps);
}

static void SetDeadline(pzframe_drv_t *pdr)
{
  struct timeval   msc; // timeval-representation of MilliSeConds
  struct timeval   dln; // DeadLiNe

    if (pdr->param_waittime >= 0  &&
        pdr->value_waittime >  0)
    {
        msc.tv_sec  =  pdr->value_waittime / 1000;
        msc.tv_usec = (pdr->value_waittime % 1000) * 1000;
        timeval_add(&dln, &(pdr->measurement_start), &msc);
        
        pdr->tid = sl_enq_tout_at(pdr->devid, NULL, &dln,
                                  pzframe_tout_p, pdr);
    }
}

static void PerformTimeoutActions(pzframe_drv_t *pdr, int do_return)
{
    pdr->measuring_now = PZFRAME_MEASURING_NOT;
    if (pdr->tid >= 0)
    {
        sl_deq_tout(pdr->tid);
        pdr->tid = -1;
    }
    pdr->abort_measurements(pdr);
    if (do_return)
        ReturnMeasurements (pdr, 0, CXRF_IO_TIMEOUT);
    else
        pzframe_drv_req_mes(pdr);
}

static void  RunMeasurements(pzframe_drv_t *pdr)
{
  int  r;

    r = pdr->start_measurements(pdr);
    if (r == PZFRAME_R_READY)
        ReturnMeasurements(pdr, 1, 0);
    else
    {
        pdr->measuring_now = PZFRAME_MEASURING_RUN;
        gettimeofday(&(pdr->measurement_start), NULL);
        SetDeadline(pdr);

        if (pdr->param_istart >= 0  &&
            (pdr->value_istart & CX_VALUE_LIT_MASK) != 0)
            pdr->trggr_measurements(pdr);
    }
}

static void pzframe_tout_p(int devid, void *devptr __attribute__((unused)),
                           sl_tid_t tid __attribute__((unused)),
                           void *privptr)
{
  pzframe_drv_t *pdr = (pzframe_drv_t *)privptr;

    pdr->tid = -1;
    if (pdr->measuring_now  != PZFRAME_MEASURING_RUN  ||
        pdr->param_waittime <  0  ||
        pdr->value_waittime <= 0)
    {
        DoDriverLog(devid, DRIVERLOG_WARNING,
                    "strange timeout: measuring_now=%d, waittime=%d",
                    pdr->measuring_now,
                    pdr->param_waittime < 0 ? -123456789 : pdr->value_waittime);
        return;
    }

    PerformTimeoutActions(pdr, 1);
}

//////////////////////////////////////////////////////////////////////

void  pzframe_drv_init(pzframe_drv_t *pdr, int devid,
                       int                           param_shot,
                       int                           param_istart,
                       int                           param_waittime,
                       int                           param_stop,
                       int                           param_elapsed,
                       int                           param_run_mode,
                       int                           param_run,
                       int                           reserved_param1,
                       int                           reserved_param2,
                       pzframe_start_measurements_t  start_measurements,
                       pzframe_trggr_measurements_t  trggr_measurements,
                       pzframe_abort_measurements_t  abort_measurements,
                       pzframe_read_measurements_t   read_measurements,
                       pzframe_prepare_retbufs_t     prepare_retbufs,
                       void                         *reserved_func_ptr1,
                       void                         *reserved_func_ptr2)
{
    pdr->devid              = devid;

    pdr->param_shot         = param_shot;
    pdr->param_istart       = param_istart;
    pdr->param_waittime     = param_waittime;
    pdr->param_stop         = param_stop;
    pdr->param_elapsed      = param_elapsed;
    pdr->param_run_mode     = param_run_mode;
    pdr->param_run          = param_run;
    pdr->reserved_param1    = reserved_param1;
    pdr->reserved_param2    = reserved_param2;
    pdr->start_measurements = start_measurements;
    pdr->trggr_measurements = trggr_measurements;
    pdr->abort_measurements = abort_measurements;
    pdr->read_measurements  = read_measurements;
    pdr->prepare_retbufs    = prepare_retbufs;
    pdr->reserved_func_ptr1 = reserved_func_ptr1;
    pdr->reserved_func_ptr2 = reserved_func_ptr2;

    pdr->state              = 0; // In fact, unused
    pdr->measuring_now      = PZFRAME_MEASURING_NOT;
    pdr->tid                = -1;
}

void  pzframe_drv_term(pzframe_drv_t *pdr)
{
    if (pdr->tid >= 0)
    {
        sl_deq_tout(pdr->tid);
        pdr->tid = -1;
    }
}

void  pzframe_drv_rw_p   (pzframe_drv_t *pdr,
                          int chan, int32 val, int action)
{
  struct timeval   now;

    if      (chan == pdr->param_shot)
    {
        ////DoDriverLog(pdr->devid, 0, "SHOT!");
        if (action == DRVA_WRITE  &&
            pdr->measuring_now == PZFRAME_MEASURING_RUN  &&
            val == CX_VALUE_COMMAND)
            pdr->trggr_measurements(pdr);
        ReturnInt32Datum(pdr->devid, chan, 0, 0);
    }
    else if (chan == pdr->param_stop)
    {
        if (action == DRVA_WRITE  &&
            pdr->measuring_now == PZFRAME_MEASURING_RUN  &&
            (val &~ CX_VALUE_DISABLED_MASK) == CX_VALUE_COMMAND)
            PerformTimeoutActions(pdr,
                                  (val & CX_VALUE_DISABLED_MASK) == 0);
        ReturnInt32Datum(pdr->devid, chan, 0, 0);
    }
    else if (chan == pdr->param_istart)
    {
        if (action == DRVA_WRITE)
        {
            pdr->value_istart = (val & CX_VALUE_LIT_MASK);
            if (pdr->measuring_now == PZFRAME_MEASURING_RUN  &&
                pdr->value_istart)
                pdr->trggr_measurements(pdr);
        }
        ReturnInt32Datum(pdr->devid, chan, pdr->value_istart, 0);
    }
    else if (chan == pdr->param_waittime)
    {
        if (action == DRVA_WRITE)
        {
            if (val < 0) val = 0;
            pdr->value_waittime = val;
        }
        ReturnInt32Datum(pdr->devid, chan, pdr->value_waittime, 0);
        /* Adapt to new waittime */
        if (action == DRVA_WRITE  &&
            pdr->measuring_now == PZFRAME_MEASURING_RUN)
        {
            if (pdr->tid >= 0)
            {
                sl_deq_tout(pdr->tid);
                pdr->tid = -1;
            }
            SetDeadline(pdr);
        }
    }
    else if (chan == pdr->param_elapsed)
    {
        if (pdr->measuring_now != PZFRAME_MEASURING_RUN)
            val = -1;
        else
        {
            gettimeofday(&now, NULL);
            timeval_subtract(&now, &now, &(pdr->measurement_start));
            val = now.tv_sec * 1000 + now.tv_usec / 1000;
        }
        ReturnInt32Datum(pdr->devid, chan, val, 0);
    }
    else if (chan == pdr->param_run_mode)
    {
        if (action == DRVA_WRITE)
        {
            if (val != PZFRAME_RUN_MODE_ON_REQUEST   &&
                val != PZFRAME_RUN_MODE_ON_RUN       &&
                val != PZFRAME_RUN_MODE_AUTO_RE_RUN  &&
                val != PZFRAME_RUN_MODE_DISABLED)
                val = PZFRAME_RUN_MODE_ON_REQUEST;
            pdr->run_mode = val;
            // Force start if now-current mode doesn't require RUN'ning
            if (pdr->measuring_now == PZFRAME_MEASURING_RQD  &&
                (val == PZFRAME_RUN_MODE_ON_REQUEST  ||
                 val == PZFRAME_RUN_MODE_AUTO_RE_RUN))
                RunMeasurements(pdr);
        }
        ReturnInt32Datum(pdr->devid, chan, pdr->run_mode, 0);
    }
    else if (chan == pdr->param_run)
    {
        if (action == DRVA_WRITE  &&
            pdr->measuring_now == PZFRAME_MEASURING_RQD  &&
            val == CX_VALUE_COMMAND)
        {
            RunMeasurements(pdr);
        }
        ReturnInt32Datum(pdr->devid, chan, 0, 0);
    }
    else
        ReturnInt32Datum(pdr->devid, chan, 0, CXRF_UNSUPPORTED);
}

void  pzframe_drv_req_mes(pzframe_drv_t *pdr)
{
    if (pdr->measuring_now != PZFRAME_MEASURING_NOT) return;

    if (pdr->run_mode == PZFRAME_RUN_MODE_ON_RUN  ||
        pdr->run_mode == PZFRAME_RUN_MODE_DISABLED)
    {
        pdr->measuring_now = PZFRAME_MEASURING_RQD;
        return;
    }

    RunMeasurements(pdr);
}

void  pzframe_drv_drdy_p (pzframe_drv_t *pdr, int do_return, rflags_t rflags)
{
    if (pdr->measuring_now != PZFRAME_MEASURING_RUN) return;
    pdr->measuring_now = PZFRAME_MEASURING_NOT;
    if (pdr->tid >= 0)
    {
        sl_deq_tout(pdr->tid);
        pdr->tid = -1;
    }
    if (do_return)
    {
        ReturnMeasurements (pdr, 1, rflags);
        if (pdr->run_mode == PZFRAME_RUN_MODE_AUTO_RE_RUN)
            pzframe_drv_req_mes(pdr);
    }
    else
        pzframe_drv_req_mes(pdr);
}
