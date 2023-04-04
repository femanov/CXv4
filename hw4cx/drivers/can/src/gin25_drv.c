#include "timeval_utils.h"

#include "cxsd_driver.h"
#include "cankoz_lyr.h"

#include "drv_i/gin25_drv_i.h"


/*=== GIN25 specifics ==============================================*/

enum
{
    DEVTYPE_GIN25 = -1
};

enum
{
    DESC_REQ_U_MES_n_base  = 0x01,
    DESC_SET_U_SET_n_base  = 0x11,
    DESC_GET_U_SET_n_base  = 0x21,
    DESC_CHARGE_CTL_n_base = 0x31,
    DESC_GET_STATUS_n_base = 0x41,
    DESC_RST_ERR_n_base    = 0x51,
    DESC_SET_AUTOREPORT    = 0xF0,

    MODE_MES_ONCE     = 0,
    MODE_MES_PERIODIC = 1,
};

enum
{
    MIN_TIMECODE = 0,
    MAX_TIMECODE = 99,
    DEF_TIMECODE = 4,
};

enum
{
    ALIVE_SECONDS   = 60,
    ALIVE_USECS     = ALIVE_SECONDS * 1000000,
};

//////////////////////////////////////////////////////////////////////

typedef struct
{
    cankoz_vmt_t    *lvmt;
    int              devid;
    int              handle;

    int              rd_rcvd;

    uint8            err_reg[GIN25_NUM_LINES];

    int              DEV_LINES;
    int              timecode;

    int              hw_ver;
    int              sw_ver;
} privrec_t;

static psp_paramdescr_t gin25_params[] =
{
    PSP_P_INT ("lines",    privrec_t, DEV_LINES, 1,            1,            3),
    PSP_P_INT ("timecode", privrec_t, timecode,  DEF_TIMECODE, MIN_TIMECODE, MAX_TIMECODE),
    PSP_P_END()
};

//////////////////////////////////////////////////////////////////////

static void SendPERIODIC(privrec_t *me)
{
  int  nl;

    for (nl = 0;  nl < me->DEV_LINES;  nl++)
        me->lvmt->q_enqueue_v(me->handle, SQ_ALWAYS,
                              SQ_TRIES_ONS, 0,
                              NULL, NULL,
                              0, 3,
                              DESC_REQ_U_MES_n_base + nl, me->timecode, MODE_MES_PERIODIC);
}

static void ReturnErrorBits(privrec_t *me, int nl, uint8 mask)
{
  int        err_addrs [1 + GIN25_CHAN_ERRBx_pl_count];
  cxdtype_t  err_dtypes[1 + GIN25_CHAN_ERRBx_pl_count];
  int        err_nelems[1 + GIN25_CHAN_ERRBx_pl_count];
  int32      err_vals  [1 + GIN25_CHAN_ERRBx_pl_count];
  void      *err_vals_p[1 + GIN25_CHAN_ERRBx_pl_count];
  rflags_t   err_rflags[1 + GIN25_CHAN_ERRBx_pl_count];

  int        n;
#define SET_RET_INFO(chan_n, val)      \
    do                                 \
    {                                  \
        err_addrs [n] = chan_n;        \
        err_dtypes[n] = CXDTYPE_INT32; \
        err_nelems[n] = 1;             \
        err_vals  [n] = (val);         \
        err_vals_p[n] = err_vals + n;  \
        err_rflags[n] = 0;             \
        n++;                           \
    }                                  \
    while (0)

  int        x;

    me->err_reg[nl] = mask;

    n = 0;

    SET_RET_INFO(GIN25_CHAN_ERR8B_n_base + nl, mask);

    for (x = 0;  x < GIN25_CHAN_ERRBx_pl_count;  x++)
        SET_RET_INFO(GIN25_CHAN_ERRBx_n_base + 
                     GIN25_CHAN_ERRBx_pl_count * nl + x, 
                     (mask & (1 << x)) != 0);

    ReturnDataSet(me->devid,
                  n,
                  err_addrs, err_dtypes, err_nelems,
                  err_vals_p, err_rflags, NULL);
}

//////////////////////////////////////////////////////////////////////

static void gin25_ff (int devid, void *devptr, int is_a_reset);
static void gin25_in (int devid, void *devptr,
                      int desc,  size_t dlc, uint8 *data);
static void gin25_alv(int devid, void *devptr,
                      sl_tid_t tid,
                      void *privptr);

static int  gin25_init_d(int devid, void *devptr,
                         int businfocount, int businfo[],
                         const char *auxinfo)
{
  privrec_t *me = devptr;

    DoDriverLog(devid, 0 | DRIVERLOG_C_ENTRYPOINT, "%s([%d]:\"%s\")",
                __FUNCTION__, businfocount, auxinfo);

    /* Initialize interface */
    me->lvmt   = GetLayerVMT(devid);
    me->devid  = devid;
    me->handle = me->lvmt->add(devid, devptr,
                               businfocount, businfo,
                               DEVTYPE_GIN25,
                               gin25_ff, gin25_in,
                               GIN25_NUMCHANS*2/*!!!*/,
                               CANKOZ_LYR_OPTION_NONE);
    if (me->handle < 0) return me->handle;

    sl_enq_tout_after(devid, devptr, ALIVE_USECS, gin25_alv, NULL);

    SetChanReturnType(devid, GIN25_CHAN_U_MES_n_base, GIN25_NUM_LINES,        IS_AUTOUPDATED_YES);
    SetChanReturnType(devid, GIN25_CHAN_HW_VER,       1,                      IS_AUTOUPDATED_TRUSTED);
    SetChanReturnType(devid, GIN25_CHAN_SW_VER,       1,                      IS_AUTOUPDATED_TRUSTED);
    SetChanReturnType(devid, GIN25_CHAN_ERROR_first,  GIN25_CHAN_ERROR_count, IS_AUTOUPDATED_TRUSTED);

    return DEVSTATE_OPERATING;
}

static void gin25_ff (int devid, void *devptr, int is_a_reset)
{
  privrec_t *me = devptr;

    me->lvmt->get_dev_ver(me->handle, &(me->hw_ver), &(me->sw_ver), NULL);
    ReturnInt32Datum(me->devid, GIN25_CHAN_HW_VER, me->hw_ver, 0);
    ReturnInt32Datum(me->devid, GIN25_CHAN_SW_VER, me->sw_ver, 0);

    me->lvmt->q_enqueue_v(me->handle, SQ_ALWAYS,
                          SQ_TRIES_INF, 0,
                          NULL, NULL,
                          0, 2,
                          DESC_SET_AUTOREPORT, 0xFF);
    SendPERIODIC(me);
}

static void gin25_alv(int devid, void *devptr,
                      sl_tid_t tid  __attribute__((unused)),
                      void *privptr __attribute__((unused)))
{
  privrec_t  *me     = (privrec_t *) devptr;

    sl_enq_tout_after(devid, devptr, ALIVE_USECS, gin25_alv, NULL);
    
    if (me->rd_rcvd == 0)
        me->lvmt->q_enq_v(me->handle, SQ_IF_ABSENT,
                          1, CANKOZ_DESC_GETDEVATTRS);
    
    me->rd_rcvd = 0;
}

static void gin25_in (int devid, void *devptr,
                      int desc,  size_t dlc, uint8 *data)
{
  privrec_t  *me       = (privrec_t *) devptr;
  int         nl;      // Line #
  int32       val;

    switch (desc)
    {
        case DESC_REQ_U_MES_n_base ... DESC_REQ_U_MES_n_base + GIN25_NUM_LINES-1:
            if (dlc < 4) return;
            me->rd_rcvd = 1;
            nl  = desc - DESC_REQ_U_MES_n_base;
            val = (uint32)(data[1]) | ((uint32)(data[2]) << 8) | ((uint32)(data[3]) << 16);
            ReturnInt32Datum(devid, GIN25_CHAN_U_MES_n_base + nl, val, 0);
            break;

        case DESC_SET_U_SET_n_base ... DESC_SET_U_SET_n_base + GIN25_NUM_LINES-1:
        case DESC_GET_U_SET_n_base ... DESC_GET_U_SET_n_base + GIN25_NUM_LINES-1:
            if (dlc < 4) return;
            me->lvmt->q_erase_and_send_next_v(me->handle, -1, desc);
            nl  = (desc & 0x0F) - 1; /*!!! a dirty hack...; Note: no reason to check resulting nl for [0..2] because of "case" ranges above */
            val = data[1] | (data[2] << 8) | (data[3] << 16);
            ReturnInt32Datum(devid, GIN25_CHAN_U_SET_n_base + nl, val, 0);
            break;

        case DESC_CHARGE_CTL_n_base ... DESC_CHARGE_CTL_n_base + GIN25_NUM_LINES-1:
            if (dlc < 2) return;
            me->lvmt->q_erase_and_send_next_v(me->handle, -1, desc);
            nl = desc - DESC_CHARGE_CTL_n_base;
            ReturnInt32Datum(devid, GIN25_CHAN_CHARGE_CTL_n_base + nl, data[1], 0);
            break;

        case DESC_GET_STATUS_n_base ... DESC_GET_STATUS_n_base + GIN25_NUM_LINES-1:
            if (dlc < 4) return;
            me->lvmt->q_erase_and_send_next_v(me->handle, -1, desc);
            nl = desc - DESC_GET_STATUS_n_base;
            ReturnInt32Datum(devid, GIN25_CHAN_CHARGE_CTL_n_base + nl, data[1], 0);
            ReturnInt32Datum(devid, GIN25_CHAN_STATUS_n_base     + nl, data[2], 0);
            ReturnErrorBits (me,                                   nl, data[3]);
            break;

        case DESC_RST_ERR_n_base ... DESC_RST_ERR_n_base + GIN25_NUM_LINES-1:
            if (dlc < 2) return;
            me->lvmt->q_erase_and_send_next_v(me->handle, -1, desc);
            nl = desc - DESC_RST_ERR_n_base;
            ReturnErrorBits(me, nl, data[1]);
            break;

        case DESC_SET_AUTOREPORT:
            if (dlc < 2) return;
            me->lvmt->q_erase_and_send_next_v(me->handle, -1, desc);
            break;
    }
}

static void gin25_rw_p(int devid, void *devptr,
                       int action,
                       int count, int *addrs,
                       cxdtype_t *dtypes, int *nelems, void **values)
{
  privrec_t *me = (privrec_t *)devptr;

  int          n;     // channel N in addrs[]/.../values[] (loop ctl var)
  int          chn;   // channel
  int          nl;    // Line #
  int32        val;   // Value

    for (n = 0;  n < count;  n++)
    {
        chn = addrs[n];
        if (action == DRVA_WRITE)
        {
            if (nelems[n] != 1  ||
                (dtypes[n] != CXDTYPE_INT32  &&  dtypes[n] != CXDTYPE_UINT32))
                goto NEXT_CHANNEL;
            val = *((int32*)(values[n]));
            ////fprintf(stderr, " write #%d:=%d\n", chn, val);
        }
        else
            val = 0xFFFFFFFF; // That's just to make GCC happy

        if      (chn >= GIN25_CHAN_U_SET_n_base  &&
                 chn <  GIN25_CHAN_U_SET_n_base + me->DEV_LINES)
        {
            nl = chn - GIN25_CHAN_U_SET_n_base;
            if (action == DRVA_WRITE)
                me->lvmt->q_enq_v(me->handle, SQ_IF_NONEORFIRST, 4,
                                  DESC_SET_U_SET_n_base + nl,
                                  ( val        & 0xFF),
                                  ((val >> 8)  & 0xFF),
                                  ((val >> 16) & 0xFF));
            else
                me->lvmt->q_enq_v(me->handle, SQ_IF_NONEORFIRST, 1,
                                  DESC_GET_U_SET_n_base + nl);
                                  
        }
        else if (chn >= GIN25_CHAN_CHARGE_CTL_n_base  &&
                 chn <  GIN25_CHAN_CHARGE_CTL_n_base + me->DEV_LINES)
        {
            nl = chn - GIN25_CHAN_CHARGE_CTL_n_base;
            if (action == DRVA_WRITE)
                me->lvmt->q_enq_v(me->handle, SQ_IF_NONEORFIRST, 2,
                                  DESC_CHARGE_CTL_n_base + nl,
                                  val & 1);
            else
                me->lvmt->q_enq_v(me->handle, SQ_IF_NONEORFIRST, 1,
                                  DESC_GET_STATUS_n_base + nl);
        }
        else if (chn >= GIN25_CHAN_RST_ERR_n_base  &&
                 chn <  GIN25_CHAN_RST_ERR_n_base + me->DEV_LINES)
        {
            nl = chn - GIN25_CHAN_RST_ERR_n_base;
            if (action == DRVA_WRITE  &&  val == CX_VALUE_COMMAND)
                me->lvmt->q_enq_v(me->handle, SQ_IF_NONEORFIRST, 2,
                                  DESC_RST_ERR_n_base + nl,
                                  me->err_reg[nl]);
            ReturnInt32Datum(devid, chn, 0, 0);
        }
        else if (chn >= GIN25_CHAN_STATUS_n_base  &&
                 chn <  GIN25_CHAN_STATUS_n_base + me->DEV_LINES)
        {
            nl = chn - GIN25_CHAN_STATUS_n_base;
            me->lvmt->q_enq_v(me->handle, SQ_IF_NONEORFIRST, 1,
                              DESC_GET_STATUS_n_base + nl);
        }
        else if (chn == GIN25_CHAN_TIMECODE)
        {
            if (action == DRVA_WRITE)
            {
                if (val < MIN_TIMECODE) val = MIN_TIMECODE;
                if (val > MAX_TIMECODE) val = MAX_TIMECODE;
                me->timecode = val;
                SendPERIODIC(me);
            }
            ReturnInt32Datum(devid, chn, me->timecode, 0);
        }
        else if (chn >= GIN25_CHAN_U_MES_n_base  &&
                 chn <  GIN25_CHAN_U_MES_n_base + me->DEV_LINES)
        {/* Those are returned upon measurement */}
        else if (chn == GIN25_CHAN_HW_VER  ||  chn == GIN25_CHAN_SW_VER)
        {/* Do-nothing -- returned from _ff() */}
        else if (chn >= GIN25_CHAN_ERROR_first  &&
                 chn <  GIN25_CHAN_ERROR_first + GIN25_CHAN_ERROR_count)
        {/* Do-nothing -- returned from _in() upon DESC_GET_STATUS_* */}
        else
            ReturnInt32Datum(devid, chn, 0, CXRF_UNSUPPORTED);

 NEXT_CHANNEL:;
    }
}

DEFINE_CXSD_DRIVER(gin25, "GIN25-via-CAN-bus driver",
                   NULL, NULL,
                   sizeof(privrec_t), gin25_params,
                   2, 2,
                   CANKOZ_LYR_API_NAME, CANKOZ_LYR_API_VERSION,
                   NULL,
                   -1, NULL, NULL,
                   gin25_init_d, NULL, gin25_rw_p);
