#include <stdio.h>

#include "cxsd_driver.h"
#include "vme_lyr.h"

#include "drv_i/rfmeas_l_timer_drv_i.h"


enum {ADDRESS_MODIFIER = 0x29};



typedef struct
{
    vme_vmt_t *lvmt;
    int        handle;
} rfmeas_l_timer_privrec_t;


static int rfmeas_l_timer_init_d(int devid, void *devptr,
                                 int businfocount, int *businfo,
                                 const char *auxinfo)
{
  rfmeas_l_timer_privrec_t *me = (rfmeas_l_timer_privrec_t*)devptr;

  int         bus_major;
  int         bus_minor;
  int         jumpers;

    if (businfocount < 3) return -CXRF_CFG_PROBL;
    bus_major  = businfo[0];
    bus_minor  = businfo[1];
    jumpers    = businfo[2] & 0xFFF;

    me->lvmt   = GetLayerVMT(devid);
    me->handle = me->lvmt->add(devid, devptr,
                               bus_major, bus_minor,
                               jumpers << 4, 2,
                               16, ADDRESS_MODIFIER,
                               0, 0, NULL,
                               NULL, VME_LYR_OPTION_NONE);
    if (me->handle < 0) return me->handle;

    //SetChanRDs  (devid, VDAC20_CHAN_ADC_n_base, VDAC20_CHAN_ADC_n_count, 1000000.0, 0.0);
    //SetChanRDs  (devid, VDAC20_CHAN_OUT_n_base, VDAC20_CHAN_OUT_n_count, 1000000.0, 0.0);
    //SetChanQuant(devid, VDAC20_CHAN_OUT_n_base, VDAC20_CHAN_OUT_n_count, (CxAnyVal_t){.i32=THE_QUANT}, CXDTYPE_INT32);

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
            default:
                ReturnInt32Datum(devid, chn, 0, CXRF_UNSUPPORTED);
        }
 NEXT_CHANNEL:;
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

////////////////////////////////////////////////////////////////////////////////////////
#ifndef __RFMEAS_L_TIMER_DRV_I_H
#define __RFMEAS_L_TIMER_DRV_I_H


enum // w50i,r50i
{
    RFMEAS_L_TIMER_NUMCHANS = 100
};


#endif /* __RFMEAS_L_TIMER_DRV_I_H */
////////////////////////////////////////////////////////////////////////////////////////
#if 0
devtype rfmeas_l_timer w50i,r50i {
}
#endif
