#include <stdio.h>

#include "cxsd_driver.h"
#include "vme_lyr.h"

#include "drv_i/rfmeas_l_timer_drv_i.h"


enum
{
    RFMEAS_L_TIMER_SPACE_SIZE       = 0xFFFFFF, /*!!! Shouldn't it be 0x1000000? */
    RFMEAS_L_TIMER_ADDRESS_SIZE     = 32,
    RFMEAS_L_TIMER_ADDRESS_MODIFIER = 0x09
};

enum
{
    R_DEVICE_ID       = 0x000000,
    R_VERSION         = 0x000004,
    R_UNIQ_ID         = 0x000008,
    R_BASEADDR        = 0x000020,
    R_ADDRSPACE       = 0x000024,
    R_CAPABILITIES    = 0x000028,

    R_TIMER_CSR       = 0x400010,
    R_CLK_STATUS      = 0x400014,

    R_EXT_EVT_CODE    = 0x400070,
    R_EXT_SETT        = 0x400072,
    R_EXT_CSR         = 0x400074,

    R_EXT_TRG_SRC     = 0x400204,
    R_TRG_PHASE_VAL   = 0x400208,
    R_PHASE_CNT_LIMIT = 0x40020C,

    R_LOG_WR_POSITION = 0xC02000,
};

enum
{
    RFMEAS_L_TIMER_HBT_FREQ  = 20,  // 20Hz :-(
    RFMEAS_L_TIMER_HBT_USECS = 1000000 / RFMEAS_L_TIMER_HBT_FREQ
};


typedef struct
{
    vme_vmt_t *lvmt;
    int        handle;

    int        special_start_rqd_ticks;

    int32      last_clks[RFMEAS_L_TIMER_CHAN_CLKS_n_count];
    int32      last_C_PllWasUnlocked;
    int32      last_C_ExtWasUnlocked;

    uint16     last_LOG_WR_POSITION;

    uint32     wr_addr;
    uint32     rd_addr;
    uint16     wr_val;
} rfmeas_l_timer_privrec_t;


static rflags_t read_uint32(rfmeas_l_timer_privrec_t *me, uint32 ofs, uint32 *val_p)
{
  rflags_t  rflags;
  uint16    hi16, lo16;

    rflags  = (me->lvmt->a32rd16(me->handle, ofs,     &hi16) < 0? CXRF_CAMAC_NO_X : 0);
    rflags |= (me->lvmt->a32rd16(me->handle, ofs + 2, &lo16) < 0? CXRF_CAMAC_NO_X : 0);

fprintf(stderr, "%s(0x%06x) hi=0x%04x lo=0x%04x\n", __FUNCTION__, ofs, hi16, lo16);
    *val_p = (((uint32)hi16) << 16) | lo16;

    return rflags;
}


static void rfmeas_l_timer_hbt  (int devid, void *devptr,
                                 sl_tid_t tid,
                                 void *privptr);

static int rfmeas_l_timer_init_d(int devid, void *devptr,
                                 int businfocount, int *businfo,
                                 const char *auxinfo)
{
  rfmeas_l_timer_privrec_t *me = (rfmeas_l_timer_privrec_t*)devptr;

  int         bus_major;
  int         bus_minor;
  int         jumpers;

  int         nb;

  cx_time_t   et_fresh_age;

  rflags_t    rflags;
  uint32      w32;

  uint16      w16;

    if (businfocount < 3) return -CXRF_CFG_PROBL;
    bus_major  = businfo[0];
    bus_minor  = businfo[1];
    jumpers    = businfo[2] & 0xFFF;

    me->lvmt   = GetLayerVMT(devid);
    me->handle = me->lvmt->add(devid, devptr,
                               bus_major, bus_minor,
                               jumpers << 24, RFMEAS_L_TIMER_SPACE_SIZE,
                               RFMEAS_L_TIMER_ADDRESS_SIZE, RFMEAS_L_TIMER_ADDRESS_MODIFIER,
                               0, 0, NULL,
                               NULL, VME_LYR_OPTION_NONE);
    if (me->handle < 0) return me->handle;

    et_fresh_age.sec   = 0x7FFFFFFF; // 68 years -- almost eternity
    et_fresh_age.nsec  = 0;

    SetChanReturnType(devid, RFMEAS_L_TIMER_CHAN_EVENT,        1, IS_AUTOUPDATED_YES);
    SetChanReturnType(devid, RFMEAS_L_TIMER_CHAN_PHASE,        1, IS_AUTOUPDATED_YES);
    SetChanReturnType(devid, RFMEAS_L_TIMER_CHAN_GATESTAT,     1, IS_AUTOUPDATED_YES);
    SetChanReturnType(devid, RFMEAS_L_TIMER_CHAN_SPECIAL_START_RESULT, 1, IS_AUTOUPDATED_YES);
    SetChanFreshAge  (devid, RFMEAS_L_TIMER_CHAN_SPECIAL_START_RESULT, 1, et_fresh_age);
    SetChanReturnType(devid, RFMEAS_L_TIMER_CHAN_DEBUG_RD_VAL, 1, IS_AUTOUPDATED_TRUSTED);
    SetChanReturnType(devid, RFMEAS_L_TIMER_CHAN_CLKS_n_base,  RFMEAS_L_TIMER_CHAN_CLKS_n_count, IS_AUTOUPDATED_TRUSTED);
    SetChanReturnType(devid, RFMEAS_L_TIMER_CHAN_C_CLKS_first, RFMEAS_L_TIMER_CHAN_C_CLKS_count, IS_AUTOUPDATED_TRUSTED);
    SetChanReturnType(devid, RFMEAS_L_TIMER_CHAN_HWINFO_first, RFMEAS_L_TIMER_CHAN_HWINFO_count, IS_AUTOUPDATED_TRUSTED);

    // Set last-known values of CLK_STAT bits to "unknown"
    for (nb = 0;  nb < RFMEAS_L_TIMER_CHAN_CLKS_n_count;  nb++) me->last_clks[nb] = -1;
    // Initial zero values of accumulated clock statuses
    ReturnInt32Datum(devid, RFMEAS_L_TIMER_CHAN_C_CLKS_PllWasUnlocked, me->last_C_PllWasUnlocked = 0, 0);
    ReturnInt32Datum(devid, RFMEAS_L_TIMER_CHAN_C_CLKS_ExtWasUnlocked, me->last_C_ExtWasUnlocked = 0, 0);

    // Read device HWINFO
    rflags = read_uint32(me, R_DEVICE_ID,     &w32); ReturnInt32Datum(devid, RFMEAS_L_TIMER_CHAN_DEVICE_ID, w32, rflags);
    rflags = read_uint32(me, R_VERSION,       &w32);
    ReturnInt32Datum(devid, RFMEAS_L_TIMER_CHAN_HW_VERSION,      (w32 >> 16) & 0x3FFF, rflags);
    ReturnInt32Datum(devid, RFMEAS_L_TIMER_CHAN_SW_VERSION,       w32        & 0xFFFF, rflags);
    ReturnInt32Datum(devid, RFMEAS_L_TIMER_CHAN_VERSION_B_30_31, (w32 >> 30) & 0x0003, rflags);
    rflags = read_uint32(me, R_UNIQ_ID,       &w32); ReturnInt32Datum(devid, RFMEAS_L_TIMER_CHAN_UNIQ_ID,      w32, rflags);
    rflags = read_uint32(me, R_BASEADDR,      &w32); ReturnInt32Datum(devid, RFMEAS_L_TIMER_CHAN_BASEADDR,     w32, rflags);
    rflags = read_uint32(me, R_ADDRSPACE,     &w32); ReturnInt32Datum(devid, RFMEAS_L_TIMER_CHAN_ADDRSPACE,    w32, rflags);
    rflags = read_uint32(me, R_CAPABILITIES,  &w32); ReturnInt32Datum(devid, RFMEAS_L_TIMER_CHAN_CAPABILITIES, w32, rflags);

    /* Initialize everything */
    // 3. Switch clock source to external
    me->lvmt->a32rd16(me->handle, R_CLK_STATUS, &w16);
    if ((w16 & (1 << 7)) == 0)  // #7:ExtActiveClk
    {
        me->lvmt->a32wr16(me->handle, R_TIMER_CSR, 1 << 8); // 0x400010.ClkSel1:=1
    }
    // 4. Set TTL0 events source to ==1
    me->lvmt->a32wr16(me->handle, 0xA02000, 1);
    me->lvmt->a32wr16(me->handle, 0xA02800+0, 0); // Delay Hi-word
    me->lvmt->a32wr16(me->handle, 0xA02800+2, 0); // Delay Lo-word
    // 5. Set oneshot mode with event code =1
    me->lvmt->a32wr16(me->handle, R_EXT_EVT_CODE,    0x01); // 0x400070:=1 (Code=1)
    me->lvmt->a32wr16(me->handle, R_EXT_SETT,        0x20); // 0x400072:=0x20 Oneshot (bit5:EnaSingleShoot)
    // 6. Schetchik kratnosti
    me->lvmt->a32wr16(me->handle, R_PHASE_CNT_LIMIT, 80);   // PHASE_CNT_LIMIT 0x40020C:=80 (DECimal!)

    // Remember current position for future comparison in rfmeas_l_timer_hbt()
    me->lvmt->a32rd16(me->handle, R_LOG_WR_POSITION, &(me->last_LOG_WR_POSITION));

    /* Start heartbeat */
    sl_enq_tout_after(devid, devptr, RFMEAS_L_TIMER_HBT_USECS, rfmeas_l_timer_hbt, NULL);

    return DEVSTATE_OPERATING;
}

static void rfmeas_l_timer_term_d(int devid, void *devptr)
{
  rfmeas_l_timer_privrec_t  *me = (rfmeas_l_timer_privrec_t*)devptr;

    if (me->handle < 0) return; // For non-initialized devices

    // Disable interrupts
}


static void rfmeas_l_timer_rdwr_p(int devid, void *devptr,
                                  int action,
                                  int count, int *addrs,
                                  cxdtype_t *dtypes, int *nelems, void **values)
{
  rfmeas_l_timer_privrec_t *me = (rfmeas_l_timer_privrec_t*)devptr;

  int         n;
  int         chn;
  int         l;

  int32       code;
  int32       val;
  rflags_t    rflags;

  uint16      w16;
  uint16      cur_LOG_WR_POSITION;

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

        switch (chn)
        {
            case RFMEAS_L_TIMER_CHAN_SHOT:
fprintf(stderr, "%s\tSHOT=%d\n", strcurtime_msc(), val);
                if (action == DRVA_WRITE  &&  val == CX_VALUE_COMMAND  &&
                    me->special_start_rqd_ticks == 0 /* Don't do while SPECIAL_START'ing */)
                {
                    me->lvmt->a32rd16(me->handle, R_EXT_CSR, &w16);        // Reading 0x400074 returns "Gate Status"
                    me->lvmt->a32wr16(me->handle, R_EXT_CSR, 1);           // 0x400074.OpenGate:=1
                    ReturnInt32Datum(devid, RFMEAS_L_TIMER_CHAN_GATESTAT, w16, 0);
                }
                ReturnInt32Datum(devid, chn, 0, 0);
                break;

            case RFMEAS_L_TIMER_CHAN_SPECIAL_START:
fprintf(stderr, "%s\tSPECIAL_START=%d\n", strcurtime_msc(), val);
                if (action == DRVA_WRITE  &&  val == CX_VALUE_COMMAND  &&
                    me->special_start_rqd_ticks == 0 /* Ignore command if SPECIAL_START sequence is already initiated */)
                {
                    me->lvmt->a32wr16(me->handle, R_EXT_CSR, 2);           // 0x400074.CloseGate:=1
                    // Enforce the "last known LOG_WR_POSITION" to a current-in-device value,
                    //     to eliminate a race condition.
                    // Note: it is done immediately after CloseGate:=1.
                    me->lvmt->a32rd16(me->handle, R_LOG_WR_POSITION, &cur_LOG_WR_POSITION);
                    if (cur_LOG_WR_POSITION != me->last_LOG_WR_POSITION)
                    {
                        me->last_LOG_WR_POSITION = cur_LOG_WR_POSITION;
                        // Notify upper-level somehow about this rare situation?
                    }

                    me->lvmt->a32rd16(me->handle, R_EXT_CSR, &w16);        // Reading 0x400074 returns "Gate Status"
                    me->lvmt->a32wr16(me->handle, R_EXT_TRG_SRC, 0x0001);  // 0x400204.TRG_SRC:=1 (external signal)
                    me->lvmt->a32wr16(me->handle, R_EXT_CSR, 1);           // 0x400074.OpenGate:=1
                    me->special_start_rqd_ticks = RFMEAS_L_TIMER_HBT_FREQ; // Initiate a 1s counter, which finally results in R_EXT_TRG_SRC/0x400204.TRG_SRC:=0
                    ReturnInt32Datum(devid, RFMEAS_L_TIMER_CHAN_GATESTAT, w16, 0);
                }
                ReturnInt32Datum(devid, chn, 0, 0);
                break;

            case RFMEAS_L_TIMER_CHAN_RESET_C_ILKS:
                if (action == DRVA_WRITE  &&  val == CX_VALUE_COMMAND)
                {
                    ReturnInt32Datum(devid, RFMEAS_L_TIMER_CHAN_C_CLKS_PllWasUnlocked, me->last_C_PllWasUnlocked = 0, 0);
                    ReturnInt32Datum(devid, RFMEAS_L_TIMER_CHAN_C_CLKS_ExtWasUnlocked, me->last_C_ExtWasUnlocked = 0, 0);
                }
                ReturnInt32Datum(devid, chn, 0, 0);
                break;

            case RFMEAS_L_TIMER_CHAN_OPERATION_MODE:
                if (action == DRVA_WRITE)
                {
                    if ((val & 1) == 0)
                    {
                        // 0: switch to continuous mode
                        me->lvmt->a32wr16(me->handle, R_EXT_SETT, 0x00); // 0x400072:=0x00 drop Oneshot (bit5:EnaSingleShoot)
                        me->lvmt->a32wr16(me->handle, R_EXT_CSR, 1);     // 0x400074.OpenGate:=1
                    }
                    else
                    {
                        // 1: switch to one-shot mode
                        me->lvmt->a32wr16(me->handle, R_EXT_CSR,  2);    // 0x400074.CloseGate:=1
                        me->lvmt->a32wr16(me->handle, R_EXT_SETT, 0x20); // 0x400072:=0x20 Oneshot (bit5:EnaSingleShoot)
                    }
                }
                me->lvmt->a32rd16(me->handle, R_EXT_SETT, &w16);
                ReturnInt32Datum(devid, chn, (w16 & 0x20) == 0? 0 : 1, 0);
                break;

            case RFMEAS_L_TIMER_CHAN_DEBUG_WR_ADDR:
                if (action == DRVA_WRITE)
                {
                    if (val < 0)                             val = 0;
                    if (val > RFMEAS_L_TIMER_SPACE_SIZE - 1) val = RFMEAS_L_TIMER_SPACE_SIZE - 1; /*!!! Shouldn't it be not "-1" but "-2"/-sizeof(uint16)? */
                    me->wr_addr = val;
                }
                ReturnInt32Datum(devid, chn, me->wr_addr, 0);
                break;

            case RFMEAS_L_TIMER_CHAN_DEBUG_WR_VAL:
                if (action == DRVA_WRITE)
                {
                    me->wr_val = val & 0xFFFF;
                    me->lvmt->a32wr16(me->handle, me->wr_addr, me->wr_val);
                }
                ReturnInt32Datum(devid, chn, me->wr_val, 0);
                break;

            case RFMEAS_L_TIMER_CHAN_DEBUG_RD_ADDR:
                if (action == DRVA_WRITE)
                {
                    if (val < 0)                             val = 0;
                    if (val > RFMEAS_L_TIMER_SPACE_SIZE - 1) val = RFMEAS_L_TIMER_SPACE_SIZE - 1; /*!!! Shouldn't it be not "-1" but "-2"/-sizeof(uint16)? */
                    me->rd_addr = val;
                    me->lvmt->a32rd16(me->handle, me->rd_addr, &w16);
                    // Note: there is a separate Return of rd_addr for it to be returned BEFORE the value read
                    ReturnInt32Datum(devid, chn, me->rd_addr, 0);
                    ReturnInt32Datum(devid, RFMEAS_L_TIMER_CHAN_DEBUG_RD_VAL, w16, 0);
                }
                else // This 'else' is to avoid duplicate Return upon WRITE (where a seoarate Return is present)
                    ReturnInt32Datum(devid, chn, me->rd_addr, 0);
                break;

            case RFMEAS_L_TIMER_CHAN_EVENT:
            case RFMEAS_L_TIMER_CHAN_PHASE:
            case RFMEAS_L_TIMER_CHAN_GATESTAT:
            case RFMEAS_L_TIMER_CHAN_DEBUG_RD_VAL:
            case RFMEAS_L_TIMER_CHAN_CLKS_n_base  ... RFMEAS_L_TIMER_CHAN_CLKS_n_base + RFMEAS_L_TIMER_CHAN_CLKS_n_count - 1:
            case RFMEAS_L_TIMER_CHAN_C_CLKS_first ... RFMEAS_L_TIMER_CHAN_C_CLKS_last:
            case RFMEAS_L_TIMER_CHAN_HWINFO_first ... RFMEAS_L_TIMER_CHAN_HWINFO_last:
                /* Those are returned upon update, so do-nothing */
                break;

            default:
////fprintf(stderr, "\t\t[%d] UNSUPPORTED %d #%d\n", devid, action, chn);
                ReturnInt32Datum(devid, chn, 0, CXRF_UNSUPPORTED);
        }
 NEXT_CHANNEL:;
    }
}

static void rfmeas_l_timer_hbt  (int devid, void *devptr,
                                 sl_tid_t tid,
                                 void *privptr)
{
  rfmeas_l_timer_privrec_t *me = (rfmeas_l_timer_privrec_t*)devptr;

  int     nb;

  uint16  cur_LOG_WR_POSITION;
  uint16  phase_val;

  int32   v32;
  uint16  v_clks;

  enum                {BUNCH = 100}; // 2(PHASE,EVENT)+
  int                  count, n;
  int32                vals  [BUNCH];
  int                  addrs [BUNCH];
  cxdtype_t            dtypes[BUNCH];
  int                  nelems[BUNCH];
  void                *val_ps[BUNCH];
  rflags_t             rflags[BUNCH];

    sl_enq_tout_after(devid, devptr, RFMEAS_L_TIMER_HBT_USECS, rfmeas_l_timer_hbt, NULL);

    count = 0;

    // I. Detect the "was event" condition (thus emulating IRQ)
    me->lvmt->a32rd16(me->handle, R_LOG_WR_POSITION, &cur_LOG_WR_POSITION);
    if (cur_LOG_WR_POSITION != me->last_LOG_WR_POSITION)
    {
fprintf(stderr, "%s %s cur_LOG_WR_POSITION=%d != last_LOG_WR_POSITION=%d\n", strcurtime_msc(), __FUNCTION__, cur_LOG_WR_POSITION, me->last_LOG_WR_POSITION);
        // 1. Store the new value as a last known one
        me->last_LOG_WR_POSITION = cur_LOG_WR_POSITION;
        // 2. Was it in fact a SPECIAL start?
        if (me->special_start_rqd_ticks > 0)
        {
            // Finish the sequence
            me->lvmt->a32wr16(me->handle, R_EXT_TRG_SRC, 0x0000);  // 0x400204.TRG_SRC:=0 (EVT signal)
            // Notify about the "special start successfully finished" event
            addrs[count] = RFMEAS_L_TIMER_CHAN_SPECIAL_START_RESULT; vals[count] = 1; count++;
            // Mark the action as "finished"
            me->special_start_rqd_ticks = 0;
        }
        // 3. Notify about the event
        me->lvmt->a32rd16(me->handle, R_TRG_PHASE_VAL, &phase_val);
        //ReturnInt32Datum(devid, RFMEAS_L_TIMER_CHAN_PHASE, phase_val, 0);
        //ReturnInt32Datum(devid, RFMEAS_L_TIMER_CHAN_EVENT, 0,         0);
        addrs[count] = RFMEAS_L_TIMER_CHAN_PHASE; vals[count] = phase_val; count++;
        addrs[count] = RFMEAS_L_TIMER_CHAN_EVENT; vals[count] = 0;         count++;
    }

    // II. special_start_rqd_ticks
    if (me->special_start_rqd_ticks > 0)
    {
        me->special_start_rqd_ticks--;
        if (me->special_start_rqd_ticks == 0)
        {
            // a. Force stop of the sequence
            me->lvmt->a32wr16(me->handle, R_EXT_TRG_SRC, 0x0000);  // 0x400204.TRG_SRC:=0 (EVT signal)
            // b. Notify of a timeout
            addrs[count] = RFMEAS_L_TIMER_CHAN_SPECIAL_START_RESULT; vals[count] = 0; count++;
        }
    }

    // III. Handle the CLK_STATUS register
    me->lvmt->a32rd16(me->handle, R_CLK_STATUS, &v_clks);
    for (nb = 0;  nb < RFMEAS_L_TIMER_CHAN_CLKS_n_count;  nb++, v_clks >>= 1)
    {
        v32 = v_clks & 1;
        if (me->last_clks[nb] != v32)
        {
            vals [count] = me->last_clks[nb] = v32;
            addrs[count] = RFMEAS_L_TIMER_CHAN_CLKS_n_base + nb;
            count++;
        }

        if (RFMEAS_L_TIMER_CHAN_CLKS_n_base + nb == RFMEAS_L_TIMER_CHAN_CLKS_PllWasUnlocked  &&
            v32 > me->last_C_PllWasUnlocked)
        {
            vals [count] = (me->last_C_PllWasUnlocked |= v32);
            addrs[count] = RFMEAS_L_TIMER_CHAN_C_CLKS_PllWasUnlocked;
            count++;
        }
        if (RFMEAS_L_TIMER_CHAN_CLKS_n_base + nb == RFMEAS_L_TIMER_CHAN_CLKS_ExtWasUnlocked  &&
            v32 > me->last_C_ExtWasUnlocked)
        {
            vals [count] = (me->last_C_ExtWasUnlocked |= v32);
            addrs[count] = RFMEAS_L_TIMER_CHAN_C_CLKS_ExtWasUnlocked;
            count++;
        }
    }

    if (count > 0)
    {
        for (n = 0;  n < count;  n++)
        {
            dtypes[n] = CXDTYPE_INT32;
            nelems[n] = 1;
            val_ps[n] = vals + n;
            rflags[n] = 0;
        }
        ReturnDataSet(devid,
                      count,
                      addrs,  dtypes, nelems,
                      val_ps, rflags, NULL);
    }
}


DEFINE_CXSD_DRIVER(rfmeas_l_timer, "RFMEAS-in-klystronka L_TIMER driver",
                   NULL, NULL,
                   sizeof(rfmeas_l_timer_privrec_t), NULL,
                   3, 3,
                   VME_LYR_API_NAME, VME_LYR_API_VERSION,
                   NULL,
                   -1, NULL, NULL,
                   rfmeas_l_timer_init_d, rfmeas_l_timer_term_d, rfmeas_l_timer_rdwr_p);
