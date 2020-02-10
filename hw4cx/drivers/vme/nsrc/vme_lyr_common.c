#include <stdarg.h>

#include "misc_macros.h"

#include "cxsd_driver.h"
#include "vme_lyr.h"


#ifndef VME_HAL_FILE_H
  #error The "VME_HAL_FILE_H" macro is undefined
#else
  #include VME_HAL_FILE_H
#endif /* VME_HAL_FILE_H */

#ifndef VME_LYR_NAME
  #error The "VME_LYR_NAME" macro is undefined
#endif /* VME_LYR_NAME */


enum
{
    VME_HANDLE_ABSENT = 0,
    VME_HANDLE_FIRST  = 1,
};

enum
{
    MAXDEVS     = 100, // Arbitrary limit, VME crates are usually much smaller
    NUMIRQS     = 8,   // # of IRQs in VME; in fact, IRQ0 is probably non-functional
    VECTSPERIRQ = 256, // Vector is 1 byte
};

typedef struct
{
    int is_open;
    int vect2handle[VECTSPERIRQ];
} irqinfo_t;

typedef struct
{
    int        in_use;

    int        bus_major;
    int        bus_minor;

    int        bus_handle;  // Value from vme_hal_open_bus()

    irqinfo_t  irqs[NUMIRQS];
} vmebusinfo_t;

typedef struct
{
    int               is_used;

    int               bus_idx;

    int               devid;
    void             *devptr;

    int               bus_major;
    int               bus_minor;
    uint32            base_addr;
    uint32            space_size; // As of 15.09.2011 -- unused; for future checks in layer
    int               addr_size;
    int               am;

    int               irq_n;
    int               irq_vect;
    vme_irq_proc      irq_proc;
} vmedevinfo_t;


static int    my_lyrid;

/* Note:
       As opposed to a3818_hal.h, here the [0] element IS usable,
       because the whole busi[] is used inside layer only and
       "bus_idx" indexes aren't exposed outside.
       Thus, it is [VME_HAL_BUS_MAXCOUNT], not [1+VME_HAL_BUS_MAXCOUNT]. */
vmebusinfo_t  busi[VME_HAL_BUS_MAXCOUNT];
vmedevinfo_t  devs[MAXDEVS];

#if VME_BUS_COMPONENTS_SENSIBLE == 0
static int the_bus_handle = 0;
#endif

/*##################################################################*/
/*##################################################################*/

static int   vme_init_lyr  (int lyrid)
{
  int  r;

    DoDriverLog(my_lyrid, 0, "%s(%d)!", __FUNCTION__, lyrid);
    my_lyrid = lyrid;
    bzero(busi, sizeof(busi));
    bzero(devs, sizeof(devs));

    r = vme_hal_init(lyrid);
    if (r < 0)
    {
        DoDriverLog(my_lyrid, 0, "%s(): vme_hal_init()=%d", __FUNCTION__, r);
        return -CXRF_DRV_PROBL;
    }

    return 0;
}

static void  vme_term_lyr  (void)
{
  int  bus_idx;
  int  irq_n;

    for (bus_idx = 0;  bus_idx < countof(busi);  bus_idx++)
        if (busi[bus_idx].in_use)
        {
            for (irq_n = 0;  irq_n < countof(busi[bus_idx].irqs);  irq_n++)
            {
                if (busi[bus_idx].irqs[irq_n].is_open)
                    vme_hal_close_irq(busi[bus_idx].bus_handle, irq_n);
                busi[bus_idx].irqs[irq_n].is_open = 0;
            }
            vme_hal_close_bus(busi[bus_idx].bus_handle);
        }

    vme_hal_term();
}

static void  vme_disconnect(int devid)
{
  int           handle;
  int           other_handle;
  int           bus_idx;
  int           irq_n;
  int           irq_vect;
  vmedevinfo_t *dp;
  int           was_found = 0;

fprintf(stderr, "%s[%d]\n", __FUNCTION__, devid);
    for (handle = VME_HANDLE_FIRST;  handle < countof(devs);  handle++)
        if (devs[handle].is_used  &&  devs[handle].devid == devid)
        {
            was_found = 1;
            dp = devs + handle;

            bus_idx  = dp->bus_idx;
            irq_n    = dp->irq_n;
            irq_vect = dp->irq_vect;
            bzero(dp, sizeof(&dp));

            // If IRQ was used, then release associated resources
            if (irq_n >= 0)
            {
                /* a. Mark this vector as free */
                busi[bus_idx].irqs[irq_n].vect2handle[irq_vect] = VME_HANDLE_ABSENT;
                /* b. Check if some other devices still use this IRQ */
                for (other_handle = VME_HANDLE_FIRST;  other_handle < countof(devs);  other_handle++)
                    if (devs[other_handle].is_used             &&
                        devs[other_handle].bus_idx == bus_idx  &&
                        devs[other_handle].irq_n   == irq_n)
                        goto BREAK_IRQ_CHECK;
                /* ...or was last "client" of this IRQ? */
                /* Then release the IRQ! */
                vme_hal_close_irq(busi[bus_idx].bus_handle, irq_n);
                busi[bus_idx].irqs[irq_n].is_open = 0;
            BREAK_IRQ_CHECK:;
            }

            /* Check if it was the last device on a bus */
            for (other_handle = VME_HANDLE_FIRST;  other_handle < countof(devs);  other_handle++)
                if (devs[other_handle].is_used             &&
                    devs[other_handle].bus_idx == bus_idx)
                    goto BREAK_BUS_CHECK;
            /* Then close/release this bus */
fprintf(stderr, "\twas the last device on bus idx=%d/handle=%d\n", bus_idx, busi[bus_idx].bus_handle);
            vme_hal_close_bus(busi[bus_idx].bus_handle);
            busi[bus_idx].in_use = 0;
        BREAK_BUS_CHECK:;
        }

    if (!was_found)
        DoDriverLog(devid, 0, "%s: request to disconnect unknown devid=%d",
                    __FUNCTION__, devid);
}


static void vme_lyr_irq_cb(int bus_handle, void *privptr,
                           int irq_n, int irq_vect)
{
  int           bus_idx = ptr2lint(privptr);
  vmedevinfo_t *dp;
  int           handle;

    handle = busi[bus_idx].irqs[irq_n].vect2handle[irq_vect];
////fprintf(stderr, "\t%s %d,%d ->%d\n", __FUNCTION__, irq_n, irq_vect, handle);
    if (handle != VME_HANDLE_ABSENT)
    {
        dp = devs + handle;
        if (dp->irq_proc != NULL)
            dp->irq_proc(dp->devid, dp->devptr, irq_n, irq_vect);
    }
}
static int vme_add(int devid, void *devptr,
                   int    bus_major, int    bus_minor,
                   uint32 base_addr, uint32 space_size,
                   int    addr_size, int    am,
                   int irq_n, int irq_vect, vme_irq_proc irq_proc)
{
  int           idx;
  int           first_free_idx;
  int           bus_idx;

  vmedevinfo_t *dp;
  int           handle;
  int           other_devid;
  int           irq_open_r;

    if (devid == DEVID_NOT_IN_DRIVER)
    {
        DoDriverLog(my_lyrid, 0, "%s: devid==DEVID_NOT_IN_DRIVER", __FUNCTION__);
        return -CXRF_DRV_PROBL;
    }

    if (addr_size != 16  &&
        addr_size != 24  &&
        addr_size != 32) // For VME64: also 40 and 64
    {
        DoDriverLog(devid, 0, "%s: bad addr_size=%d (should be one of 16,24,32)", __FUNCTION__, addr_size);
        return -CXRF_DRV_PROBL;
    }

    if (irq_n >= NUMIRQS)
    {
        DoDriverLog(devid, 0, "%s: irq_n=%d, out_of[0,%d]", __FUNCTION__, irq_n, NUMIRQS-1);
        return -CXRF_CFG_PROBL;
    }

    /* Try to find a bus cell:
       - either an already used with appropriate {major,minor}
       - or a free one */
    for (idx = 0, first_free_idx = -1, bus_idx = -1;
         idx < countof(busi)     &&    bus_idx < 0;
         idx++)
    {
        if (busi[idx].in_use)
        {
            if (busi[idx].bus_major == bus_major  &&
                busi[idx].bus_minor == bus_minor)
                bus_idx = idx;
        }
        else
            if (first_free_idx < 0) first_free_idx = idx;
    }

    /* Wasn't found? */
    if (bus_idx < 0)
    {
        /* Okay, was a FREE cell found? */
        if (first_free_idx < 0)
        {
            DoDriverLog(devid, 0, "%s: bus table overflow, no room for bus(%d,%d)",
                        __FUNCTION__, bus_major, bus_minor);
            return -CXRF_DRV_PROBL;
        }

        /* Yes! */
        bus_idx = first_free_idx;
        busi[bus_idx].in_use = 1;
        busi[bus_idx].bus_handle = vme_hal_open_bus(bus_major, bus_minor);
        if (busi[bus_idx].bus_handle < 0)
        {
            DoDriverLog(devid, 0, "%s: vme_hal_open_bus(%d,%d) failed: %s",
                        __FUNCTION__, bus_major, bus_minor,
                        errno != 0? strerror(errno) : vme_hal_strerror(busi[bus_idx].bus_handle));
            busi[bus_idx].in_use = 0;
            return -CXRF_DRV_PROBL;
        }
    }

    /* Check if requested {irq_n,irq_vect} tuple isn't already used */
    irq_vect &= 0xFF;
    if (irq_n > 0  &&
        busi[bus_idx].irqs[irq_n].vect2handle[irq_vect] != VME_HANDLE_ABSENT)
    {
        other_devid = (busi[bus_idx].irqs[irq_n].vect2handle[irq_vect] > 0  &&
                       busi[bus_idx].irqs[irq_n].vect2handle[irq_vect] < countof(devs))?
            devs[busi[bus_idx].irqs[irq_n].vect2handle[irq_vect]].devid : DEVID_NOT_IN_DRIVER;
        DoDriverLog(devid, 0, "%s: (irq_n=%d,irq_vect=%d) is already in use (handle=%d, devid=%d)",
                    __FUNCTION__, irq_n, irq_vect,
                    busi[bus_idx].irqs[irq_n].vect2handle[irq_vect],
                    other_devid);
        if (other_devid != DEVID_NOT_IN_DRIVER)
            ReturnDataSet(other_devid, RETURNDATA_COUNT_PONG,
                          NULL, NULL, NULL, NULL, NULL, NULL);
        return -CXRF_CFG_PROBL;
    }

    /* Find a free cell... */
    for (handle = VME_HANDLE_FIRST;  handle < countof(devs);  handle++)
        if (devs[handle].is_used == 0) break;
    if (handle >= countof(devs))
    {
        DoDriverLog(devid, 0, "%s: devs[] overflow (too many devices requested)", __FUNCTION__);
        return -CXRF_DRV_PROBL;
    }
    /* ...and mark it as used */
    dp = devs + handle;
    dp->is_used = 1;

    /* Remember info */
    dp->bus_idx    = bus_idx;
    dp->devid      = devid;
    dp->devptr     = devptr;
    dp->bus_major  = bus_major;
    dp->bus_minor  = bus_minor;
    dp->base_addr  = base_addr;
    dp->space_size = space_size;
    dp->addr_size  = addr_size;
    dp->am         = am;
    dp->irq_n      = irq_n;
    dp->irq_vect   = irq_vect;
    dp->irq_proc   = irq_proc;

    if (irq_n > 0)
    {
        if (!(busi[bus_idx].irqs[irq_n].is_open))
        {
            irq_open_r = vme_hal_open_irq(busi[bus_idx].bus_handle, lint2ptr(bus_idx),
                                          irq_n, vme_lyr_irq_cb, my_lyrid);
            if (irq_open_r < 0)
            {
                DoDriverLog(devid, 0, "vme_hal_open_irq(IRQ%d): %s",
                            irq_n,
                            errno != 0? strerror(errno) : vme_hal_strerror(irq_open_r));
                bzero(dp, sizeof(*dp));
                return -CXRF_DRV_PROBL;
            }
            busi[bus_idx].irqs[irq_n].is_open = 1;
        }
        busi[bus_idx].irqs[irq_n].vect2handle[irq_vect] = handle;
    }

    return handle;
}

static int vme_get_dev_info(int devid,
                            int    *bus_major_p, int    *bus_minor_p,
                            uint32 *base_addr_p, uint32 *space_size_p,
                            int    *addr_size_p, int    *am_p,
                            int    *irq_n_p,     int    *irq_vect_p)
{
  int           handle;
  vmedevinfo_t *dp;

    for (handle = VME_HANDLE_FIRST;  handle < countof(devs);  handle++)
        if (devs[handle].is_used  &&  devs[handle].devid == devid)
        {
            dp = devs + handle;
            if (bus_major_p  != NULL) *bus_major_p  = dp->bus_major;
            if (bus_minor_p  != NULL) *bus_minor_p  = dp->bus_minor;
            if (base_addr_p  != NULL) *base_addr_p  = dp->base_addr;
            if (space_size_p != NULL) *space_size_p = dp->space_size;
            if (addr_size_p  != NULL) *addr_size_p  = dp->addr_size;
            if (am_p         != NULL) *am_p         = dp->am;
            if (irq_n_p      != NULL) *irq_n_p      = dp->irq_n;
            if (irq_vect_p   != NULL) *irq_vect_p   = dp->irq_vect;

            return 0;
        }


    return -1;
}


#if VME_BUS_COMPONENTS_SENSIBLE
  #define VME_LYR_CHECK_HANDLE()                                          \
      do {                                                                \
          if (handle < VME_HANDLE_FIRST  ||  handle >= countof(devs)  ||  \
              devs[handle].is_used == 0)                                  \
          {                                                               \
              errno = EBADF;                                              \
              return -1;                                                  \
          }                                                               \
      } while (0)
  #define VME_LYR_BUS_HANDLE_VALUE  busi[devs[handle].bus_idx].bus_handle
#else
  #define VME_LYR_CHECK_HANDLE()    do { } while (0) /* A no-op */
  #define VME_LYR_BUS_HANDLE_VALUE  the_bus_handle
#endif

#define VME_LYR_DEFINE_IO(AS, TS)                                          \
static int vme_a##AS##wr##TS   (int handle, uint32 ofs, uint##TS  value)   \
{                                                                          \
    VME_LYR_CHECK_HANDLE();                                                \
    return vme_hal_a##AS##wr##TS   (VME_LYR_BUS_HANDLE_VALUE,              \
                                    devs[handle].am,                       \
                                    devs[handle].base_addr + ofs, value);  \
}                                                                          \
static int vme_a##AS##rd##TS   (int handle, uint32 ofs, uint##TS *val_p)   \
{                                                                          \
    VME_LYR_CHECK_HANDLE();                                                \
    return vme_hal_a##AS##rd##TS   (VME_LYR_BUS_HANDLE_VALUE,              \
                                    devs[handle].am,                       \
                                    devs[handle].base_addr + ofs, val_p);  \
}                                                                          \
static int vme_a##AS##wr##TS##v(int handle, uint32 ofs, uint##TS *data, int count) \
{                                                                          \
    VME_LYR_CHECK_HANDLE();                                                \
    return vme_hal_a##AS##wr##TS##v(VME_LYR_BUS_HANDLE_VALUE,              \
                                    devs[handle].am,                       \
                                    devs[handle].base_addr + ofs, data, count); \
}                                                                          \
static int vme_a##AS##rd##TS##v(int handle, uint32 ofs, uint##TS *data, int count) \
{                                                                          \
    VME_LYR_CHECK_HANDLE();                                                \
    return vme_hal_a##AS##rd##TS##v(VME_LYR_BUS_HANDLE_VALUE,              \
                                    devs[handle].am,                       \
                                    devs[handle].base_addr + ofs, data, count); \
}

VME_LYR_DEFINE_IO(16, 8)
VME_LYR_DEFINE_IO(16, 16)
VME_LYR_DEFINE_IO(16, 32)

VME_LYR_DEFINE_IO(24, 8)
VME_LYR_DEFINE_IO(24, 16)
VME_LYR_DEFINE_IO(24, 32)

VME_LYR_DEFINE_IO(32, 8)
VME_LYR_DEFINE_IO(32, 16)
VME_LYR_DEFINE_IO(32, 32)


static vme_vmt_t vme_vmt =
{
    vme_add,

    vme_get_dev_info,

    NULL,

    /* I/O */

    // a. Scalar

    // A16
    vme_a16wr8,
    vme_a16rd8,
    vme_a16wr16,
    vme_a16rd16,
    vme_a16wr32,
    vme_a16rd32,

    // A24
    vme_a24wr8,
    vme_a24rd8,
    vme_a24wr16,
    vme_a24rd16,
    vme_a24wr32,
    vme_a24rd32,

    // A32
    vme_a32wr8,
    vme_a32rd8,
    vme_a32wr16,
    vme_a32rd16,
    vme_a32wr32,
    vme_a32rd32,

    // b. Vector

    // A16
    vme_a16wr8v,
    vme_a16rd8v,
    vme_a16wr16v,
    vme_a16rd16v,
    vme_a16wr32v,
    vme_a16rd32v,

    // A24
    vme_a24wr8v,
    vme_a24rd8v,
    vme_a24wr16v,
    vme_a24rd16v,
    vme_a24wr32v,
    vme_a24rd32v,

    // A32
    vme_a32wr8v,
    vme_a32rd8v,
    vme_a32wr16v,
    vme_a32rd16v,
    vme_a32wr32v,
    vme_a32rd32v,
};

DEFINE_CXSD_LAYER(VME_LYR_NAME, "VME-layer implementation via '" __CX_STRINGIZE(VME_HAL_DESCR) "' HAL",
                  NULL, NULL,
                  VME_LYR_API_NAME, VME_LYR_API_VERSION,
                  vme_init_lyr, vme_term_lyr,
                  vme_disconnect,
                  &vme_vmt);
