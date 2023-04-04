#include "cxsd_driver.h"

#include "drv_i/frolov_time24k4_drv_i.h"


enum
{
    // F=0,16
    A_STAT        = 0,
    A_ADDR        = 1,
    A_TIME_n_base = 2,

    // F=1,17
    // no A1?
    A_MASK        = 2,
    A_STATUS      = 3,
    A_STATE       = 4,
};

enum
{
    // A_MASK
    MASK_IGN_STARTS = 1 << 4,

    // A_STATUS
    STATUS_EXT_FRQ  = 1 << 0,
    STATUS_ONESHOT  = 1 << 1,
    STATUS_MASK_L   = 1 << 2,
    STATUS_STAT_OFF = 1 << 3,

    // A_STATE
    STATE_MEASURING = 1 << 0,
    STATE_LAM_TRIG  = 1 << 1,
    STATE_STAT_MAX  = 1 << 2,
    STATE_MEMO_MAX  = 1 << 3,
};


typedef struct
{
    int       devid;
    int       N;

    int       cur_mask;
    int       rqd_mask;
    rflags_t  mask_rflags;

    uint8     stat_data[FROLOV_TIME24K4_MAX_NUMPTS * FROLOV_TIME24K4_NUM_LINES];
} frolov_time24k4_privrec_t;


static void SetMask(frolov_time24k4_privrec_t *me, int mask)
{
  int w;

    me->cur_mask = mask;
    w = mask;
    me->mask_rflags = x2rflags(DO_NAF(CAMAC_REF, me->N, A_MASK,  17, &w));
}

static void StartSingleMeasurement(frolov_time24k4_privrec_t *me)
{
  int  w;
  int  junk;

    // F25A1 -- "reset"?
    DO_NAF(CAMAC_REF, me->N, 1, 25, &junk);

    // Unblock operation
    w = me->cur_mask &~ MASK_IGN_STARTS;
    DO_NAF(CAMAC_REF, me->N, A_MASK, 17, &w);
}

static void ReadAndReturnTimes(frolov_time24k4_privrec_t *me)
{
  int        times_addrs [FROLOV_TIME24K4_NUM_LINES];
  cxdtype_t  times_dtypes[FROLOV_TIME24K4_NUM_LINES];
  int        times_nelems[FROLOV_TIME24K4_NUM_LINES];
  int32      times_vals  [FROLOV_TIME24K4_NUM_LINES];
  void      *times_vals_p[FROLOV_TIME24K4_NUM_LINES];
  rflags_t   times_rflags[FROLOV_TIME24K4_NUM_LINES];

  int        nl;
  int        w;
  int        status;

    for (nl = 0;  nl < FROLOV_TIME24K4_NUM_LINES;  nl++)
    {
        status = DO_NAF(CAMAC_REF, me->N, A_TIME_n_base + nl, 0, &w);

        times_addrs [nl] = FROLOV_TIME24K4_CHAN_TIME_n_base + nl;
        times_dtypes[nl] = CXDTYPE_INT32;
        times_nelems[nl] = 1;
        times_vals  [nl] = w;
        times_vals_p[nl] = times_vals + nl;
        times_rflags[nl] = x2rflags(status);
    }

    ReturnDataSet(me->devid,
                  FROLOV_TIME24K4_NUM_LINES,
                  times_addrs, times_dtypes, times_nelems,
                  times_vals_p, times_rflags, NULL);
}

static void ReadAndReturnStats(frolov_time24k4_privrec_t *me)
{
  int        stats_addrs [FROLOV_TIME24K4_NUM_LINES];
  cxdtype_t  stats_dtypes[FROLOV_TIME24K4_NUM_LINES];
  int        stats_nelems[FROLOV_TIME24K4_NUM_LINES];
  void      *stats_vals_p[FROLOV_TIME24K4_NUM_LINES];
  rflags_t   stats_rflags[FROLOV_TIME24K4_NUM_LINES];

  enum      {STAT_SEG_SIZE = 128};
  int        rdata[STAT_SEG_SIZE];

  int        w;
  int        junk;
  int        status_reg;
  uint8     *wp;
  int       *rp;
  int        nl;
  int        nseg;
  int        x;

    // Mask L
    DO_NAF(CAMAC_REF, me->N, A_STATUS, 1,  &status_reg);
    w = status_reg | STATUS_MASK_L;
    DO_NAF(CAMAC_REF, me->N, A_STATUS, 17, &w);

    // F25A2 -- "reset"?
    DO_NAF(CAMAC_REF, me->N, 2, 25, &junk);

    for (nl = 0, wp = me->stat_data;  nl < FROLOV_TIME24K4_NUM_LINES;  nl++)
    {
        stats_addrs [nl] = FROLOV_TIME24K4_CHAN_STAT_n_base + nl;
        stats_dtypes[nl] = CXDTYPE_UINT8;
        stats_nelems[nl] = FROLOV_TIME24K4_MAX_NUMPTS;
        stats_vals_p[nl] = wp;
        stats_rflags[nl] = 0;

        // Set read address for this line
        w = nl * FROLOV_TIME24K4_MAX_NUMPTS;
        DO_NAF(CAMAC_REF, me->N, A_ADDR, 1, &w);
        // and read the data segment-by-segment
        for (nseg = 0;  nseg < FROLOV_TIME24K4_MAX_NUMPTS / STAT_SEG_SIZE;  nseg++)
        {
            DO_NAFB(CAMAC_REF, me->N, A_STAT, 0, rdata, STAT_SEG_SIZE);
            for (x = 0, rp = rdata;  x < STAT_SEG_SIZE;  x++)
                *wp++ = *rp++;
        }
    }

    // Restore status reg value (usually unmask L)
    DO_NAF(CAMAC_REF, me->N, A_STATUS, 17, &status_reg);

    ReturnDataSet(me->devid,
                  FROLOV_TIME24K4_NUM_LINES,
                  stats_addrs, stats_dtypes, stats_nelems,
                  stats_vals_p, stats_rflags, NULL);
}

static void ClearStats(frolov_time24k4_privrec_t *me)
{
  int  w;
  int  junk;

    DO_NAF(CAMAC_REF, me->N, 2, 25, &junk); // Preparation: Reset LAM, STAT_MAX, MEMO_MAX triggers
    DO_NAF(CAMAC_REF, me->N, 3, 25, &junk); // Clear memory
    SleepBySelect(2000); // >1.31ms
#if 0  // Added by mistake, due to poor formatting of documentation?
    DO_NAF(CAMAC_REF, me->N, 2, 25, &junk); // Preparation: Reset LAM, STAT_MAX, MEMO_MAX triggers
    DO_NAF(CAMAC_REF, me->N, 1, 25, &junk); // Reset LAM trigger (?)
#elif 0
    DO_NAF(CAMAC_REF, me->N, 2, 25, &junk); // Preparation: Reset LAM, STAT_MAX, MEMO_MAX triggers
    // Reset "disable statistics" bit?
    DO_NAF(CAMAC_REF, me->N, A_STATUS,  1,  &w);
    w &=~ STATUS_STAT_OFF;
    DO_NAF(CAMAC_REF, me->N, A_STATUS,  17, &w);
#endif
}

//////////////////////////////////////////////////////////////////////

static void LAM_CB(int devid, void *devptr)
{
  frolov_time24k4_privrec_t *me = (frolov_time24k4_privrec_t *)devptr;

fprintf(stderr, "LAM\n");
    ReadAndReturnTimes(me);
    if (me->rqd_mask != me->cur_mask) SetMask(me, me->rqd_mask);
    StartSingleMeasurement(me);
}

static int frolov_time24k4_init_d(int devid, void *devptr,
                                  int businfocount, int *businfo,
                                  const char *auxinfo)
{
  frolov_time24k4_privrec_t *me = (frolov_time24k4_privrec_t*)devptr;

  int         w;
  int         junk;
  const char *errstr;

    me->devid  = devid;
    me->N      = businfo[0];

    me->mask_rflags = x2rflags(DO_NAF(CAMAC_REF, me->N, A_MASK,  1, &w));
    w &= 0xF; // 0b1111
    me->rqd_mask = me->cur_mask = w;
    // Temporary block operation
    w |= MASK_IGN_STARTS;
    DO_NAF                           (CAMAC_REF, me->N, A_MASK, 17, &w);
    // Set mode (leaving clock source (frequency) unmodified)
    DO_NAF(CAMAC_REF, me->N, A_STATUS,  1,  &w);
    w = (w & STATUS_EXT_FRQ) | STATUS_ONESHOT | STATUS_STAT_OFF;
    DO_NAF(CAMAC_REF, me->N, A_STATUS,  17, &w);
    // Perform various resets:
//    DO_NAF(CAMAC_REF, me->N, 0, 10, &junk); // Reset LAM, allow single-shot
    DO_NAF(CAMAC_REF, me->N, 2, 25, &junk); //
    DO_NAF(CAMAC_REF, me->N, 1, 25, &junk); //
//    DO_NAF(CAMAC_REF, me->N, , , &junk); //
//    DO_NAF(CAMAC_REF, me->N, , , &junk); //

    SetChanReturnType(devid, FROLOV_TIME24K4_CHAN_TIME_n_base, FROLOV_TIME24K4_NUM_LINES, IS_AUTOUPDATED_YES);
    SetChanReturnType(devid, FROLOV_TIME24K4_CHAN_STAT_n_base, FROLOV_TIME24K4_NUM_LINES, IS_AUTOUPDATED_YES);

    SetChanReturnType(devid, FROLOV_TIME24K4_CHAN_NUMPTS,   1, IS_AUTOUPDATED_TRUSTED);
    SetChanReturnType(devid, FROLOV_TIME24K4_CHAN_RANGEMIN, 1, IS_AUTOUPDATED_TRUSTED);
    SetChanReturnType(devid, FROLOV_TIME24K4_CHAN_RANGEMAX, 1, IS_AUTOUPDATED_TRUSTED);
    ReturnInt32Datum (devid, FROLOV_TIME24K4_CHAN_NUMPTS,   FROLOV_TIME24K4_MAX_NUMPTS, 0);
    ReturnInt32Datum (devid, FROLOV_TIME24K4_CHAN_RANGEMIN, 0,                          0);
    ReturnInt32Datum (devid, FROLOV_TIME24K4_CHAN_RANGEMAX, 255,                        0);

    if ((errstr = WATCH_FOR_LAM(devid, devptr, me->N, LAM_CB)) != NULL)
    {
        DoDriverLog(devid, DRIVERLOG_ERRNO, "WATCH_FOR_LAM(): %s", errstr);
        return -CXRF_DRV_PROBL;
    }

    StartSingleMeasurement(me);

    return DEVSTATE_OPERATING;
}

static void frolov_time24k4_rw_p(int devid, void *devptr,
                                 int action,
                                 int count, int *addrs,
                                 cxdtype_t *dtypes, int *nelems, void **values)
{
  frolov_time24k4_privrec_t *me = (frolov_time24k4_privrec_t*)devptr;

  int              n;     // channel N in addrs[]/.../values[] (loop ctl var)
  int              chn;   // channel

  int32            value;
  rflags_t         rflags;

  int              nl;
  int              w;

    for (n = 0;  n < count;  n++)
    {
        chn = addrs[n];
        if (action == DRVA_WRITE)
        {
            if (nelems[n] != 1  ||
                (dtypes[n] != CXDTYPE_INT32  &&  dtypes[n] != CXDTYPE_UINT32))
                goto NEXT_CHANNEL;
            value = *((int32*)(values[n]));
            ////fprintf(stderr, " write #%d:=%d\n", chn, val);
        }
        else
            value = 0xFFFFFFFF; // That's just to make GCC happy

        if (0)
        {}
        else if (chn >= FROLOV_TIME24K4_CHAN_MASK_n_base  &&
                 chn <  FROLOV_TIME24K4_CHAN_MASK_n_base + FROLOV_TIME24K4_NUM_LINES)
        {
            nl = chn - FROLOV_TIME24K4_CHAN_MASK_n_base;
            if (action == DRVA_WRITE)
            {
                if (value != 0) me->rqd_mask |=  (1 << nl);
                else            me->rqd_mask &=~ (1 << nl);
                if (0) SetMask(me, me->rqd_mask);
            }
            value  = (me->rqd_mask >> nl) & 1;
            rflags = me->mask_rflags;
        }
        else if (chn == FROLOV_TIME24K4_CHAN_CLK_MODE)
        {
            rflags = 0;
            if (action == DRVA_WRITE)
            {
                rflags |= x2rflags(DO_NAF(CAMAC_REF, me->N, A_STATUS, 1,  &w));
                if (value != 0) w |=  STATUS_EXT_FRQ;
                else            w &=~ STATUS_EXT_FRQ;
                rflags |= x2rflags(DO_NAF(CAMAC_REF, me->N, A_STATUS, 17, &w));
            }
            rflags     |= x2rflags(DO_NAF(CAMAC_REF, me->N, A_STATUS, 1,  &w));
            value = (w & STATUS_EXT_FRQ) != 0;
        }
#if 1
        else if (chn == 49)
        {
            if (action == DRVA_WRITE  &&  value == CX_VALUE_COMMAND)
            {
                ReadAndReturnTimes(me);
                if (me->rqd_mask != me->cur_mask) SetMask(me, me->rqd_mask);
                StartSingleMeasurement(me);
            }
            rflags = 0;
            value  = 0;
        }
        else if (chn == 99)
        {
#if 0
          int status;
            status = DO_NAF(CAMAC_REF, me->N, 0,       8, &w);
            rflags = x2rflags(status);
            value  = (status & CAMAC_Q)   != 0;
#else
            rflags = x2rflags(DO_NAF(CAMAC_REF, me->N, A_STATE, 1, &w));
            value  = (w & STATE_LAM_TRIG) != 0;
#endif
        }
#endif
        else if ((chn >= FROLOV_TIME24K4_CHAN_TIME_n_base  &&
                  chn <  FROLOV_TIME24K4_CHAN_TIME_n_base + FROLOV_TIME24K4_NUM_LINES)  ||
                 (chn >= FROLOV_TIME24K4_CHAN_STAT_n_base  &&
                  chn <  FROLOV_TIME24K4_CHAN_STAT_n_base + FROLOV_TIME24K4_NUM_LINES))
            goto NEXT_CHANNEL;
        else if (chn == FROLOV_TIME24K4_CHAN_NUMPTS    ||
                 chn == FROLOV_TIME24K4_CHAN_RANGEMIN  ||
                 chn == FROLOV_TIME24K4_CHAN_RANGEMAX)
            goto NEXT_CHANNEL;
        else
        {
            value   = chn;
            rflags  = CXRF_UNSUPPORTED;
        }

        ReturnInt32Datum(devid, chn, value, rflags);

 NEXT_CHANNEL:;
    }
}


DEFINE_CXSD_DRIVER(frolov_time24k4, "Frolov TIME24K4",
                   NULL, NULL,
                   sizeof(frolov_time24k4_privrec_t), NULL,
                   1, 1,
                   NULL, 0,
                   NULL,
                   -1, NULL, NULL,
                   frolov_time24k4_init_d, NULL, frolov_time24k4_rw_p);
