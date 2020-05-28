#include <string.h>
#include <errno.h>

#include "cxsd_driver.h"
#include "vme_lyr.h"

#include "drv_i/vsdc4_drv_i.h"


typedef struct
{
    vme_vmt_t         *lvmt;
    int                devid;
    int                handles  [VSDC4_NUM_INPUTS];
    int                irq_n;
    int                irq_vects[VSDC4_NUM_INPUTS];

} vsdc4_privrec_t;


static void vsdc4_irq_p(int devid, void *devptr,
                        int irq_n, int irq_vect)
{
  vsdc4_privrec_t *me = (privrec_t*)devptr;

    if      (irq_vect == me->irq_vects[0]) nl = 0;
    else if (irq_vect == me->irq_vects[1]) nl = 1;
    else if (irq_vect == me->irq_vects[2]) nl = 2;
    else if (irq_vect == me->irq_vects[3]) nl = 3;
    else
    {
        return;
    }
}

static int vsdc4_init_d(int devid, void *devptr,
                        int businfocount, int businfo[],
                        const char *auxinfo)
{
  vsdc4_privrec_t *me = (privrec_t*)devptr;

  int              bus_major;
  int              bus_minor;
  int              jumpers;

  int              nl;

    bus_major        = businfo[0];
    bus_minor        = businfo[1];
    jumpers          = businfo[2]; /*!!!*/
    me->irq_n        = businfo[3] &  0x7;
    me->irq_vects[0] = businfo[4] & 0xFF;
    me->irq_vects[1] = businfo[5] & 0xFF;
    me->irq_vects[2] = businfo[6] & 0xFF;
    me->irq_vects[3] = businfo[7] & 0xFF;

    me->lvmt   = GetLayerVMT(devid);
    me->devid  = devid;

    for (nl = 0;  nl < VSDC4_NUM_INPUTS;  nl++)
        me->handles[nl] = -1;
    for (nl = 0;  nl < VSDC4_NUM_INPUTS;  nl++)
    {
        me->handles[nl] = me->lvmt->add(devid, devptr,
                                        bus_major, bus_minor,
                                        jumpers << 25, FASTADC_SPACE_SIZE,
                                        me->irq_n, me->irq_vects[nl], vsdc4_irq_p,
                                        NULL, VME_LYR_OPTION_NONE));
        if (me->handles[nl] < 0)
            return me->handles[nl];

    return DEVSTATE_OPERATING;
}

static void vsdc4_term_d(int devid, void *devptr)
{
  vsdc4_privrec_t *me = (vsdc4_privrec_t*)devptr;

//    me->lvmt->a16wr16(me->handle, KREG_IRQ, 0);
//    me->lvmt->a16wr16(me->handle, KREG_IO,  KCMD(KCMD_STOP, 0));
}

static void rdwr_p(int devid, void *devptr,
                   int action,
                   int count, int *addrs,
                   cxdtype_t *dtypes, int *nelems, void **values)
{
  vsdc4_privrec_t *me = (vsdc4_privrec_t*)devptr;

  int              n;
  int              chn;
  int              l;

  int32            val;

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


DEFINE_CXSD_DRIVER(vsdc4, "VsDC VME integrator driver",
                   NULL, NULL,
                   sizeof(vsdc4_privrec_t), vsdc4_params,
                   8, 8,
                   VME_LYR_API_NAME, VME_LYR_API_VERSION,
                   NULL,
                   -1, NULL, NULL,
                   vsdc4_init_d, vsdc4_term_d, vsdc4_rdwr_p);
