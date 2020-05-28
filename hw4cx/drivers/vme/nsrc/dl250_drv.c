#include "timeval_utils.h"
#include "cxsd_driver.h"
#include "vme_lyr.h"

#include "drv_i/dl250_drv_i.h"


enum
{
    DL250_SPACE_SIZE       = 0xFFFFFF,
    DL250_ADDRESS_SIZE     = 32,
    DL250_ADDRESS_MODIFIER = 0x09
};

enum
{
    /* General-purpose registers */
    DL250_R_DEVICE_ID          = 0x000000,
    DL250_R_VERSION            = 0x000004,
    DL250_R_UNIQ_ID            = 0x000008,
    DL250_R_INT_ENA            = 0x000010,
    DL250_R_INT_STATUS         = 0x000014,
    DL250_R_INT_VECTOR         = 0x000018,
    DL250_R_INT_LINE           = 0x00001c,
    DL250_R_BASEADDR           = 0x000020,
    DL250_R_ADDRSPACE          = 0x000024,
    DL250_R_CAPABILITIES       = 0x000028,

    /* Service registers */
    DL250_R_REGS_UNLOCK        = 0x00002c,
    DL250_R_EPCS_FIRST         = 0x0000c0,

    /* Specific registers */
    DL250_R_CTRL               = 0x000100,
    DL250_R_CLK_SELECT         = 0x000104,
    // No 0x000108?
    DL250_R_READY              = 0x00010c,
    DL250_R_ENABLE             = 0x000110,
    DL250_R_AUTODISABLE        = 0x000114,
    DL250_R_BLOCK_MASK         = 0x000118,
    DL250_R_BLOCKED            = 0x00011c,
    DL250_R_BSOURCE_base       = 0x000120, // Up to 0x000134
      DL250_R_BSOURCE_incr       = 4,
    DL250_R_START_SOURCE_base  = 0x000140, // Up to 0x000154
      DL250_R_START_SOURCE_incr  = 4,
    DL250_R_DELAY_base         = 0x000180, // Up to 0x0001dc
      DL250_R_DELAY_incr         = 4,
};

enum
{
    DL250_CLK_SELECT_CHANGE_shift = 0,
     DL250_CLK_SELECT_CHANGE_mask = 1 << DL250_CLK_SELECT_CHANGE_shift,

    DL250_CLK_SELECT_PLLOK_shift  = 1,
     DL250_CLK_SELECT_PLLOK_mask  = 1 << DL250_CLK_SELECT_PLLOK_shift,

    DL250_CLK_SELECT_INTBAD_shift = 2,
     DL250_CLK_SELECT_INTBAD_mask = 1 << DL250_CLK_SELECT_INTBAD_shift,

    DL250_CLK_SELECT_EXTBAD_shift = 3,
     DL250_CLK_SELECT_EXTBAD_mask = 1 << DL250_CLK_SELECT_EXTBAD_shift,

    DL250_CLK_SELECT_SOURCE_shift = 4,
     DL250_CLK_SELECT_SOURCE_mask = 1 << DL250_CLK_SELECT_SOURCE_shift,
};

enum
{
    DL250_START_SOURCE_SOURCE_shift = 0,
     DL250_START_SOURCE_SOURCE_bits = 0x7,

    DL250_START_SOURCE_TTL_shift    = 4,
     DL250_START_SOURCE_TTL_bits    = 0x7,
};


enum {DL250_HBT_USECS = 100000}; // 100ms is enough to support one-second-granularity autoshot periods


typedef struct
{
    vme_vmt_t      *lvmt;
    int             devid;
    int             handle;
    int             irq_n;
    int             irq_vect;

    int32           auto_shot;
    int32           auto_msec;
    struct timeval  last_auto_shot;

    struct timeval  last_shot;

    uint32          last_enable;
    uint32          last_ready;
    uint32          last_ailk;
} dl250_privrec_t;

static psp_paramdescr_t dl250_params[] =
{
    PSP_P_END()
};


static void ReportInitialState(dl250_privrec_t *me,
                               int              reg_ofs,
                               uint32          *last_p,
                               int              CHAN_n_base)
{
  uint32  w;
  int     nl;

    me->lvmt->a32rd32(me->handle, reg_ofs, &w);

    for (nl = 0;  nl < DL250_NUMOUTPUTS;  nl++)
        ReturnInt32Datum(me->devid, CHAN_n_base + nl, (w >> nl) & 1, 0);
    ReturnInt32Datum(me->devid, CHAN_n_base + DL250_NUMOUTPUTS, w & 0x00FFFFFF, 0);

    *last_p = w;
}

static void ReportChangedState(dl250_privrec_t *me,
                               int              reg_ofs,
                               uint32          *last_p,
                               int              CHAN_n_base)
{
  uint32  w;
  int     nl;

    me->lvmt->a32rd32(me->handle, reg_ofs, &w);
    if (w == *last_p) return;

    for (nl = 0;  nl < DL250_NUMOUTPUTS;  nl++)
        if (((w ^ *last_p) >> nl) != 0)
            ReturnInt32Datum(me->devid, CHAN_n_base + nl, (w >> nl) & 1, 0);
    ReturnInt32Datum(me->devid, CHAN_n_base + DL250_NUMOUTPUTS, w & 0x00FFFFFF, 0);

    *last_p = w;
}


static void dl250_irq_p(int devid, void *devptr,
                        int irq_n, int irq_vect);
static void dl250_hbt  (int devid, void *devptr,
                        sl_tid_t tid,
                        void *privptr);

static int dl250_init_d(int devid, void *devptr,
                        int businfocount, int businfo[],
                        const char *auxinfo)
{
  dl250_privrec_t  *me = (dl250_privrec_t*)devptr;

  int               bus_major;
  int               bus_minor;
  int               jumpers;

  int               r;
  int               nl;

  uint32            w;

  uint32            v_devid;
  uint32            v_ver;
  uint32            v_uniq;

    bus_major    = businfo[0];
    bus_minor    = businfo[1];
    jumpers      = businfo[2]; /*!!!*/
    me->irq_n    = businfo[3] &  0x7;
    me->irq_vect = businfo[4] & 0xFF;

    me->lvmt   = GetLayerVMT(devid);
    me->devid  = devid;
    me->handle = me->lvmt->add(devid, devptr,
                               bus_major, bus_minor,
                               jumpers << 24, DL250_SPACE_SIZE,
                               DL250_ADDRESS_SIZE, DL250_ADDRESS_MODIFIER,
                               me->irq_n, me->irq_vect, dl250_irq_p,
                               NULL, VME_LYR_OPTION_NONE);
    if (me->handle < 0) return me->handle;

    // Set channels' properties
    SetChanRDs(devid, DL250_CHAN_AUTO_MSEC, 1, 1000.0, 0.0);
    SetChanRDs(devid, DL250_CHAN_AUTO_LEFT, 1, 1000.0, 0.0);
    SetChanRDs(devid, DL250_CHAN_T_n_base,  DL250_NUMOUTPUTS, 
                                               1.0/4,  0.0);
    SetChanReturnType(devid, DL250_CHAN_MISC_STAT_first, DL250_CHAN_MISC_STAT_count, IS_AUTOUPDATED_TRUSTED);
    SetChanReturnType(devid, DL250_CHAN_HWINFO_first,    DL250_CHAN_HWINFO_count,    IS_AUTOUPDATED_TRUSTED);

    // First of the first: check if device is present
    // ...simultaneously dropping RORA-style interrupt
    r = me->lvmt->a32rd32(me->handle, DL250_R_INT_STATUS, &w);
    if (r < 0)
    {
        DoDriverLog(devid, 0, "a32rd32(DL250_R_INT_STATUS): %d; probably a missing/unconfigured device, terminating", r);
        return -CXRF_CAMAC_NO_X;
    }

    ReportInitialState(me, DL250_R_ENABLE,  &(me->last_enable), DL250_CHAN_ENABLE_n_base);
    ReportInitialState(me, DL250_R_READY,   &(me->last_ready),  DL250_CHAN_READY_n_base);
    ReportInitialState(me, DL250_R_BLOCKED, &(me->last_ailk),   DL250_CHAN_AILK_n_base);

    //
    me->lvmt->a32rd32(me->handle, DL250_R_DEVICE_ID, &v_devid);
    me->lvmt->a32rd32(me->handle, DL250_R_VERSION,   &v_ver);
    me->lvmt->a32rd32(me->handle, DL250_R_UNIQ_ID,   &v_uniq);

    ReturnInt32Datum(devid, DL250_CHAN_DEVICE_ID,      v_devid,               0);

    ReturnInt32Datum(devid, DL250_CHAN_HW_VERSION,    (v_ver >> 18) & 0x3FFF, 0);
    ReturnInt32Datum(devid, DL250_CHAN_RIO_PRESENT,   (v_ver >> 17) & 1,      0);
    ReturnInt32Datum(devid, DL250_CHAN_FRONT_PRESENT, (v_ver >> 16) & 1,      0);
    ReturnInt32Datum(devid, DL250_CHAN_SW_VERSION,     v_ver        & 0xFFFF, 0);

    ReturnInt32Datum(devid, DL250_CHAN_UNIQ_ID,        v_uniq,                0);

    me->lvmt->a32rd32(me->handle, DL250_R_CAPABILITIES, &w);
    ReturnInt32Datum(devid, DL250_CHAN_CAPABILITIES,   w,                     0);
    me->lvmt->a32rd32(me->handle, DL250_R_BASEADDR,     &w);
    ReturnInt32Datum(devid, DL250_CHAN_BASEADDR,       w,                     0);
    me->lvmt->a32rd32(me->handle, DL250_R_ADDRSPACE,    &w);
    ReturnInt32Datum(devid, DL250_CHAN_ADDRSPACE,      w,                     0);

    /* Start heartbeat */
    sl_enq_tout_after(devid, devptr, DL250_HBT_USECS, dl250_hbt, NULL);

    /* Program IRQ */
    me->lvmt->a32wr32(me->handle, DL250_R_INT_LINE,   me->irq_n);
    me->lvmt->a32wr32(me->handle, DL250_R_INT_VECTOR, me->irq_vect);
    me->lvmt->a32wr32(me->handle, DL250_R_INT_ENA,    1);

    return DEVSTATE_OPERATING;
}

static void dl250_term_d(int devid, void *devptr)
{
  dl250_privrec_t  *me = (dl250_privrec_t*)devptr;

    if (me->handle < 0) return; // For non-initialized devices

    // Disable interrupts
    me->lvmt->a32wr32(me->handle, DL250_R_INT_ENA,    0);
}

static
void ReturnUInt32Datum(int devid, int chan, uint32 v, rflags_t rflags)
{
  static cxdtype_t  dt_uint32   = CXDTYPE_UINT32;
  static int        nels_1      = 1;

  void             *vp          = &v;

    ReturnDataSet(devid, 1,
                  &chan, &dt_uint32, &nels_1,
                  &vp, &rflags, NULL);
}

static void CallReadOne(dl250_privrec_t *me, int chan);

static void dl250_rw_p (int devid, void *devptr,
                        int action,
                        int count, int *addrs,
                        cxdtype_t *dtypes, int *nelems, void **values)
{
  dl250_privrec_t  *me = (dl250_privrec_t*)devptr;

  int               n;    // channel N in values[] (loop ctl var)
  int               chn;  // channel
  int               nl;   // Line #
  int               nr;   // Reg #
  int               nb;   // Byte #
  int               sf;   // Shift Factor

  int32             val;
  uint32            uval; // For T channels specifically
  rflags_t          rflags;
  uint32            w;

  uint32            mask4and;
  uint32            mask4or;
  int32             rval; // Read-from-hw VALue

  struct timeval    now;
  struct timeval    msc; // timeval-representation of MilliSeConds
  struct timeval    dln; // DeadLiNe

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

        rflags = 0;
        switch (chn)
        {
            case DL250_CHAN_T_n_base ... DL250_CHAN_T_n_base + DL250_NUMOUTPUTS-1:
                nl = chn - DL250_CHAN_T_n_base;
                if (action == DRVA_WRITE)
                {
                    uval = *((int32*)(values[n]));
                    me->lvmt->a32wr32(me->handle, DL250_R_DELAY_base + DL250_R_DELAY_incr * nl, val);
                }
                me->lvmt->a32rd32(me->handle, DL250_R_DELAY_base + DL250_R_DELAY_incr * nl, &uval);
                ReturnUInt32Datum(me->devid, chn, uval, 0);
                goto NEXT_CHANNEL;
                break;

#define ON_ONEBIT_24(ch_n, r_n) \
            case DL250_CHAN_##ch_n##_n_base ... DL250_CHAN_##ch_n##_n_base + DL250_NUMOUTPUTS-1: \
                nl = chn - DL250_CHAN_##ch_n##_n_base;                    \
                if (action == DRVA_WRITE)                                 \
                {                                                         \
                    val = val != 0;                                       \
                    me->lvmt->a32rd32(me->handle, DL250_R_##r_n, &w);     \
                    w = (w &~ (1 << nl)) | (val << nl);                   \
                    me->lvmt->a32wr32(me->handle, DL250_R_##r_n,  w);     \
                    CallReadOne(me, DL250_CHAN_##ch_n##_24);              \
                }                                                         \
                me->lvmt->a32rd32(me->handle, DL250_R_##r_n, &w);         \
                val = (w >> nl) & 1;                                      \
                break;                                                    \
                                                                          \
            case DL250_CHAN_##ch_n##_24:                                  \
                if (action == DRVA_WRITE)                                 \
                {                                                         \
                    me->lvmt->a32wr32(me->handle, DL250_R_##r_n, w & 0x00FFFFFF); \
                    for (nl = 0;  nl < DL250_NUMOUTPUTS-1;  nl++)         \
                        CallReadOne(me, DL250_CHAN_##ch_n##_n_base + nl); \
                }                                                         \
                me->lvmt->a32rd32(me->handle, DL250_R_##r_n, &w);         \
                val = w & 0x00FFFFFF;                                     \
                break;

#define ON_ILKSRC_B4(s_n, ibn) /* s_n - source name, ibn - ilk bit number (0-4) */ \
            case DL250_CHAN_ILK_##s_n##_n_base ... DL250_CHAN_ILK_##s_n##_n_base + DL250_NUMOUTPUTS-1: \
                nl = chn - DL250_CHAN_ILK_##s_n##_n_base;                 \
                nr = nl / 4;                                              \
                nb = nl % 4;                                              \
                sf = nb * 8 + ibn;                                        \
                if (action == DRVA_WRITE)                                 \
                {                                                         \
                    val = val != 0;                                       \
                    me->lvmt->a32rd32(me->handle, DL250_R_BSOURCE_base + DL250_R_BSOURCE_incr * nr, &w); \
                    w = (w &~ (1 << sf)) | (val << sf);                   \
                    me->lvmt->a32wr32(me->handle, DL250_R_BSOURCE_base + DL250_R_BSOURCE_incr * nr,  w); \
                    CallReadOne(me, DL250_CHAN_ILK_##s_n##_24);           \
                }                                                         \
                me->lvmt->a32rd32(me->handle, DL250_R_BSOURCE_base + DL250_R_BSOURCE_incr * nr, &w); \
                val = (w >> sf) & 1;                                      \
                break;                                                    \
                                                                          \
            case DL250_CHAN_ILK_##s_n##_24:                               \
                if (action == DRVA_WRITE)                                 \
                {                                                         \
                    for (nr = 0;  nr < DL250_NUMOUTPUTS/4;  nr++)         \
                    {                                                     \
                        me->lvmt->a32rd32(me->handle, DL250_R_BSOURCE_base + DL250_R_BSOURCE_incr * nr, &w); \
                        for (nb = 0, sf = ibn;                            \
                             nb < 4;                                      \
                             nb++,   sf += 8)                             \
                            w = (w &~ (1 << sf)) | (((val >> (nr * 4 + nb)) & 1) << sf); \
                        me->lvmt->a32wr32(me->handle, DL250_R_BSOURCE_base + DL250_R_BSOURCE_incr * nr,  w); \
                    for (nl = 0;  nl < DL250_NUMOUTPUTS-1;  nl++)         \
                        CallReadOne(me, DL250_CHAN_ILK_##s_n##_n_base + nl); \
                    }                                                     \
                }                                                         \
                val = 0;                                                  \
                for (nr = 0;  nr < DL250_NUMOUTPUTS/4;  nr++)             \
                {                                                         \
                    me->lvmt->a32rd32(me->handle, DL250_R_BSOURCE_base + DL250_R_BSOURCE_incr * nr, &w); \
                    val |= (                                              \
                            ((w >>  ibn          ) & 1) |                 \
                            ((w >> (ibn +  (8-1))) & 2) |                 \
                            ((w >> (ibn + (16-2))) & 4) |                 \
                            ((w >> (ibn + (24-3))) & 8)                   \
                           ) << (nr * 4);                                 \
                }
                break;

#define ON_START_PRM(prm, rpn) /* prm - PaRaMeter name, rpn - Register Part Name */ \
            case DL250_CHAN_START_##prm##_n_base ... DL250_CHAN_START_##prm##_n_base + DL250_NUMOUTPUTS-1: \
                nl = chn - DL250_CHAN_START_##prm##_n_base;               \
                nr = nl / 4;                                              \
                nb = nl % 4;                                              \
                sf = nb * 8 + DL250_START_SOURCE_##rpn##_shift;           \
                if (action == DRVA_WRITE  &&                              \
                    val >= 0  &&  val <= DL250_START_SOURCE_##rpn##_bits) \
                {                                                         \
                    me->lvmt->a32rd32(me->handle, DL250_R_START_SOURCE_base + DL250_R_START_SOURCE_incr * nr, &w); \
                    w = (w &~ (DL250_START_SOURCE_##rpn##_bits << sf)) | (val << sf); \
                    me->lvmt->a32wr32(me->handle, DL250_R_START_SOURCE_base + DL250_R_START_SOURCE_incr * nr,  w); \
                    CallReadOne(me, DL250_CHAN_START_##prm##_ALL);        \
                }                                                         \
                me->lvmt->a32rd32(me->handle, DL250_R_START_SOURCE_base + DL250_R_START_SOURCE_incr * nr, &w); \
                val = (w >> sf) & DL250_START_SOURCE_##rpn##_bits;        \
                break;                                                    \
                                                                          \
            case DL250_CHAN_START_##prm##_ALL:                            \
                if (action == DRVA_WRITE  &&                              \
                    val >= 0  &&  val <= DL250_START_SOURCE_##rpn##_bits) \
                {                                                         \
                    mask4and = DL250_START_SOURCE_##rpn##_bits << DL250_START_SOURCE_##rpn##_shift; \
                    mask4and =  mask4and        |                         \
                               (mask4and <<  8) |                         \
                               (mask4and << 16) |                         \
                               (mask4and << 24);                          \
                    mask4or  = val                             << DL250_START_SOURCE_##rpn##_shift; \
                    mask4or  =  mask4or         |                         \
                               (mask4or  <<  8) |                         \
                               (mask4or  << 16) |                         \
                               (mask4or  << 24);                          \
                    for (nr = 0;  nr < DL250_NUMOUTPUTS/4;  nr++)         \
                    {                                                     \
                        me->lvmt->a32rd32(me->handle, DL250_R_START_SOURCE_base + DL250_R_START_SOURCE_incr * nr, &w); \
                        w = (w & mask4and) | mask4or;                     \
                        me->lvmt->a32wr32(me->handle, DL250_R_START_SOURCE_base + DL250_R_START_SOURCE_incr * nr,  w); \
                    }                                                     \
                    for (nl = 0;  nl < DL250_NUMOUTPUTS-1;  nl++)         \
                        CallReadOne(me, DL250_CHAN_START_##prm##_n_base + nl); \
                }                                                         \
                for (nr = 0;  nr < DL250_NUMOUTPUTS/4;  nr++)             \
                {                                                         \
                    me->lvmt->a32rd32(me->handle, DL250_R_START_SOURCE_base + DL250_R_START_SOURCE_incr * nr, &w); \
                    w >>= DL250_START_SOURCE_##rpn##_shift;               \
                    for (nb = 0;  nb < 4;  nb++, w >>= 8)                 \
                    {                                                     \
                        rval = w & DL250_START_SOURCE_##rpn##_bits;       \
                        if (nr == 0  &&  nb == 0)                         \
                            val = rval;                                   \
                        else                                              \
                            if (rval != val)                              \
                            {                                             \
                                val = DL250_START_SOURCE_##rpn##_bits + 1; \
                                goto BREAK_##prm##_CHECK;                 \
                            }                                             \
                    }                                                     \
                }                                                         \
                BREAK_##prm##_CHECK:;                                     \
                break;

            ON_ONEBIT_24(ENABLE,      ENABLE)
            ON_ONEBIT_24(AUTODISABLE, AUTODISABLE)
            ON_ONEBIT_24(ILK_MASK,    BLOCK_MASK)

            ON_ILKSRC_B4(EXT, 0)
            ON_ILKSRC_B4(DZ0, 1)
            ON_ILKSRC_B4(DZ1, 2)
            ON_ILKSRC_B4(DZ2, 3)
            ON_ILKSRC_B4(DZ3, 4)

            ON_START_PRM(TYPE,  SOURCE)
            ON_START_PRM(INPUT, TTL)

            case DL250_CHAN_CLK_STAT_first ... DL250_CHAN_CLK_STAT_last /* NO "-1"! */:
                me->lvmt->a32rd32(me->handle, DL250_R_CLK_SELECT, &w);
                val = (w >> (chn - DL250_CHAN_CLK_STAT_first + 1)) & 1;
                break;

            case DL250_CHAN_CLK_MODE:
                if (action == DRVA_WRITE)
                {
                    val = val != 0;
                    me->lvmt->a32rd32(me->handle, DL250_R_CLK_SELECT, &w);
                    if (val != ((w & DL250_CLK_SELECT_SOURCE_mask) != 0)  &&
                        (w & (DL250_CLK_SELECT_INTBAD_mask | DL250_CLK_SELECT_EXTBAD_mask)) == 0)
                    {
                        w ^= 1;
                        me->lvmt->a32wr32(me->handle, DL250_R_CLK_SELECT, w);
                    }
                }
                me->lvmt->a32rd32(me->handle, DL250_R_CLK_SELECT, &w);
                val = (w & DL250_CLK_SELECT_SOURCE_mask) != 0;
                break;

            case DL250_CHAN_DO_SHOT:
                if (action == DRVA_WRITE  &&  val == CX_VALUE_COMMAND)
                {
                    me->lvmt->a32wr32(me->handle, DL250_R_CTRL, 1);
                }
                val = 0;
                break;

            case DL250_CHAN_AUTO_SHOT:
                if (action == DRVA_WRITE)
                {
                    val = val != 0;
                    me->auto_shot = val;
                }
                val = me->auto_shot;
                break;

            case DL250_CHAN_AUTO_MSEC:
                if (action == DRVA_WRITE)
                {
                    if (val < 0)            val = 0;
                    if (val > 1000*86400*7) val = 1000*86400*7; // Max 1wk period
                    me->auto_msec = val;
                }
                val = me->auto_msec;
                break;

            case DL250_CHAN_WAS_SHOT:
                gettimeofday(&now, NULL);
                /*
                 Note: instead of checking for AFTER(now,last+1sec), which
                 would require a copy of me->last_shot, we employ
                 a mathematically equivalent AFTER(now-1sec,last).
                 */
                now.tv_sec -= 1;
                val = TV_IS_AFTER(now, me->last_shot)? 0 : 1;
                break;

            case DL250_CHAN_AUTO_LEFT:
                val = -1000; // -1s
                if (me->auto_shot  &&  me->auto_msec > 0)
                {
                    gettimeofday(&now, NULL);
                    msc.tv_sec   =  me->auto_msec / 1000;
                    msc.tv_usec  = (me->auto_msec % 1000) * 1000;
                    timeval_add(&dln, &(me->last_auto_shot), &msc);
                    if (timeval_subtract(&msc, &dln, &now) == 0)
                        val = msc.tv_sec * 1000 + msc.tv_usec / 1000;
                }
                break;

            default:
                val  = 0;
                rflags = CXRF_UNSUPPORTED;
        }
        ReturnInt32Datum(devid, chn, val, rflags);

 NEXT_CHANNEL:;
    }
}

static void  CallReadOne(dl250_privrec_t *me, int chan)
{
    dl250_rw_p(me->devid, me,
               DRVA_READ,
               1, &chan,
               NULL, NULL, NULL);
}

static void dl250_irq_p(int devid, void *devptr,
                        int irq_n, int irq_vect)
{
  dl250_privrec_t  *me = (dl250_privrec_t*)devptr;

  uint32            w;

////fprintf(stderr, "%s IRQ[%d]: n=%d vector=%d\n", strcurtime_msc(), devid, irq_n, irq_vect);
    if (irq_vect != me->irq_vect) return;

    // Drop RORA-style interrupt
    me->lvmt->a32rd32(me->handle, DL250_R_INT_STATUS, &w);
}

static void dl250_hbt  (int devid, void *devptr,
                        sl_tid_t tid,
                        void *privptr)
{
  dl250_privrec_t   *me = (dl250_privrec_t *)devptr;
  struct timeval     now;
  struct timeval     msc; // timeval-representation of MilliSeConds
  int32              on           = CX_VALUE_COMMAND;
  void              *on_p         = &on;
  int                do_shot_chan = DL250_CHAN_DO_SHOT;

  static cxdtype_t   dt_int32     = CXDTYPE_INT32;
  static int         nels_1       = 1;

    sl_enq_tout_after(devid, devptr, DL250_HBT_USECS, dl250_hbt, NULL);

    /* Is auto-shot on? */
    if (me->auto_shot  &&  me->auto_msec > 0)
    {
        gettimeofday(&now, NULL);
        msc.tv_sec   =  me->auto_msec / 1000;
        msc.tv_usec  = (me->auto_msec % 1000) * 1000;
        timeval_subtract(&now, &now, &msc);
        /* If the period elapsed? */
        if (TV_IS_AFTER(now, me->last_auto_shot))
        {
            /* Is mode suitable for auto-shot?  */
            if (
                1
               )
            {
//            fprintf(stderr, "zz %ld\n", (long int)time(NULL));
                gettimeofday(&(me->last_auto_shot), NULL);
                dl250_rw_p(devid, me,
                           DRVA_WRITE,
                           1, &do_shot_chan,
                           &dt_int32, &nels_1, &on_p);
            }
        }
    }

    /* Report ENABLE, READY, AILK channels if changed */
    ReportChangedState(me, DL250_R_ENABLE,  &(me->last_enable), DL250_CHAN_ENABLE_n_base);
    ReportChangedState(me, DL250_R_READY,   &(me->last_ready),  DL250_CHAN_READY_n_base);
    ReportChangedState(me, DL250_R_BLOCKED, &(me->last_ailk),   DL250_CHAN_AILK_n_base);
}



DEFINE_CXSD_DRIVER(dl250, "DL250 VME delay line driver",
                   NULL, NULL,
                   sizeof(dl250_privrec_t), dl250_params,
                   3, 5,
                   VME_LYR_API_NAME, VME_LYR_API_VERSION,
                   NULL,
                   -1, NULL, NULL,
                   dl250_init_d, dl250_term_d, dl250_rw_p);
