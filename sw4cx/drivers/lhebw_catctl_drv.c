#include <stdio.h>

#include "cxsd_driver.h"
#include "vdev.h"

#include "drv_i/lhebw_catctl_drv_i.h"


enum
{
    SODC_ISET_NEG,
    SODC_ILIM_NEG,
    SODC_OUT2,
    SODC_OUT3,

    SODC_ISET_CTL,
    SODC_ILIM_CTL,
    SODC_IVIP_CTL,
    SODC_ADC3,

    SODC_ENABLE,

    SUBORD_NUMCHANS
};

static vdev_sodc_dsc_t hw2our_mapping[SUBORD_NUMCHANS] =
{
    [SODC_ISET_NEG] = {"out0",   VDEV_IMPR, LHEBW_CATCTL_CHAN_ISET},
    [SODC_ILIM_NEG] = {"out1",   VDEV_IMPR, LHEBW_CATCTL_CHAN_ILIM},

    [SODC_ISET_CTL] = {"adc0",   VDEV_TUBE, LHEBW_CATCTL_CHAN_ISET_CTL},
    [SODC_ILIM_CTL] = {"adc1",   VDEV_TUBE, LHEBW_CATCTL_CHAN_ILIM_CTL},
    [SODC_IVIP_CTL] = {"adc2",   VDEV_TUBE, LHEBW_CATCTL_CHAN_IVIP_CTL},

    [SODC_ENABLE]   = {"outrb0", VDEV_IMPR, LHEBW_CATCTL_CHAN_ENABLE},
};

static const char *devstate_names[] = {"_devstate"};

typedef struct {
    int                 devid;

    vdev_context_t      ctx;
    vdev_sodc_cur_t     cur[SUBORD_NUMCHANS];
    vdev_sodc_cur_t     devstate_cur[countof(devstate_names)];

} privrec_t;

static int lhebw_catctl_init_d(int devid, void *devptr,
                               int businfocount, int businfo[],
                               const char *auxinfo)
{
  privrec_t      *me = (privrec_t *)devptr;

    me->devid = devid;

    SetChanReturnType(devid, LIU_DOORILKS_CHAN_SUM_STATE,  1,
                      IS_AUTOUPDATED_YES);
    SetChanReturnType(devid, LHEBW_CATCTL_CHAN_ADC_first, LHEBW_CATCTL_CHAN_ADC_count,
                      IS_AUTOUPDATED_YES);

    me->ctx.num_sodcs      = SUBORD_NUMCHANS;
    me->ctx.map            = hw2our_mapping;
    me->ctx.cur            = me->cur;
    me->ctx.devstate_count = countof(devstate_names);
    me->ctx.devstate_names = devstate_names;
    me->ctx.devstate_cur   = me->devstate_cur;
    me->ctx.do_rw          = NULL;
    me->ctx.sodc_cb        = lhebw_catctl_sodc_cb;

    return vdev_init(&(me->ctx), devid, devptr, -1, auxinfo);
}

static void lhebw_catctl_term_d(int devid __attribute__((unused)), void *devptr)
{
  privrec_t      *me = (privrec_t *)devptr;

    vdev_fini(&(me->ctx));
}

static void lhebw_catctl_rw_p(int devid, void *devptr,
                              int action,
                              int count, int *addrs,
                              cxdtype_t *dtypes, int *nelems, void **values) 
{
  privrec_t *me = devptr;
  int             n;     // channel N in addrs[]/.../values[] (loop ctl var)
  int             chn;   // channel
  int32           val;
  int             ilk;

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

        if      ()
        {
        }
        else if (chn == )
        {
            if (action == DRVA_WRITE)
            {
                if (val < 0)              val = 0;
                if (val > countof(modes)) val = countof(modes);
                if (val != me->mode) set_ilk_mode(me, val);
            }
            ReturnInt32Datum(devid, chn, me->mode, 0);
        }
        else if (chn >= LHEBW_CATCTL_CHAN_ADC_first  &&
                 chn <= LHEBW_CATCTL_CHAN_ADC_last)
            /* Do-nothing */;
        else
            ReturnInt32Datum(devid, chn, 0, CXRF_UNSUPPORTED);

 NEXT_CHANNEL:;
    }
}

DEFINE_CXSD_DRIVER(lhebw_catctl, "Laser-Heated ElectronBeamWelding Catode Control driver",
                   NULL, NULL,
                   sizeof(privrec_t), NULL,
                   0, 0,
                   NULL, 0,
                   NULL,
                   -1, NULL, NULL,
                   lhebw_catctl_init_d,
                   lhebw_catctl_term_d,
                   lhebw_catctl_rw_p);
