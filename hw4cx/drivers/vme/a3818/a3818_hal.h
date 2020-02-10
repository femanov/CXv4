/*********************************************************************
  a3818_hal.h
      This file implements VME Hardware Abstraction Layer
      via CAEN's CAENVMElib API for A3818 adapter(s),
      and is intended to be included by *vme_lyr*
*********************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>

#include "vme_hal.h"

#define LINUX // For CAENVMEtypes.h, which uses "#ifdef LINUX" instead of "#ifdef WIN32"
#include "CAENVMElib.h"


enum
{
    VME_HAL_BUS_MAXCOUNT = 10,  // An arbitrary limit
};


enum {A3818_BUS_HANDLE_FIRST = 1};
typedef struct
{
    int      in_use;

    int      bus_major;
    int      bus_minor;

    int32_t  CAENVMElib_handle; // The same type as in CAENVMElib.h

    int      irq_mask;
} a3818_hal_bus_info_t;
VME_HAL_STORAGE_CLASS a3818_hal_bus_info_t a3818_hal_bus_info[A3818_BUS_HANDLE_FIRST + VME_HAL_BUS_MAXCOUNT];

//////////////////////////////////////////////////////////////////////
enum {A3818_HEARTBEAT_USECS = 100*1000*0 + 5000000*1}; // Use 5s for debugging
static sl_tid_t  a3818_hbt_tid = -1;

static void a3818_heartbeat_proc(int uniq, void *unsdptr,
                                 sl_tid_t tid, void *privptr)
{
  int           idx;
  CVErrorCodes  err;
  CAEN_BYTE     mask;
  int           level;
  uint32        vect32; // We use 32-bit with cvD8 just "na vsyakij sluchaj"

    a3818_hbt_tid = sl_enq_tout_after(uniq, NULL,
                                      A3818_HEARTBEAT_USECS, a3818_heartbeat_proc, NULL);

    for (idx = A3818_BUS_HANDLE_FIRST;
         idx < countof(a3818_hal_bus_info);
         idx++)
        if (a3818_hal_bus_info[idx].in_use  &&  a3818_hal_bus_info[idx].irq_mask != 0)
        {
            err = CAENVME_IRQCheck(a3818_hal_bus_info[idx].CAENVMElib_handle, &mask);
            if (err < 0  ||  (mask & (a3818_hal_bus_info[idx].irq_mask >> 1)) == 0) goto NEXT_BUS;

            for (level = 1;  level <= 7;  level++)
                if ((mask & (1 << (level - 1))) != 0)
                {
                    err = CAENVME_IACKCycle(a3818_hal_bus_info[idx].CAENVMElib_handle,
                                            level, &vect32, cvD8);
                    if (err < 0) goto NEXT_LEVEL;

            NEXT_LEVEL:;
                }

    NEXT_BUS:;
        }
}
//////////////////////////////////////////////////////////////////////


static const char * vme_hal_strerror(int errcode)
{
    return CAENVME_DecodeError(errcode);
}

static int  vme_hal_init     (int uniq)
{
    bzero(a3818_hal_bus_info, sizeof(a3818_hal_bus_info));

    a3818_hbt_tid = sl_enq_tout_after(uniq, NULL,
                                      A3818_HEARTBEAT_USECS, a3818_heartbeat_proc, NULL);

    return 0;
}

static void vme_hal_term     (void)
{
    if (a3818_hbt_tid >= 0)
    {
        sl_deq_tout(a3818_hbt_tid); a3818_hbt_tid = -1;
    }
}

static int  vme_hal_open_bus (int bus_major, int bus_minor)
{
  int           idx;
  int           first_free_idx;
  int           bus_handle;

  CVBoardTypes  BdType;
  short         Link;
  short         BdNum;
  CVErrorCodes  err;

    if (bus_major < 0  ||  bus_minor < 0)
    {
        errno = EINVAL;
        return -1;
    }

    for (idx = A3818_BUS_HANDLE_FIRST, first_free_idx = -1;
         idx < countof(a3818_hal_bus_info);
         idx++)
    {
        if (a3818_hal_bus_info[idx].in_use)
        {
            if (a3818_hal_bus_info[idx].bus_major == bus_major  &&
                a3818_hal_bus_info[idx].bus_minor == bus_minor)
            {
                errno = EBUSY;
                return -1;
            }
        }
        else if (first_free_idx < 0) first_free_idx = idx;
    }

    if (first_free_idx < 0)
    {
        errno = EOVERFLOW;
        return -1;
    }
    bus_handle = first_free_idx;

    bzero(a3818_hal_bus_info + bus_handle, sizeof(a3818_hal_bus_info[bus_handle]));
    a3818_hal_bus_info[bus_handle].in_use = 1;
    a3818_hal_bus_info[bus_handle].bus_major = bus_major;
    a3818_hal_bus_info[bus_handle].bus_minor = bus_minor;

    BdType = cvV2718;
    Link   = bus_major;
    BdNum  = bus_minor;
    if (bus_major >= 10000)
    {
        BdType = (bus_major - 10000) / 1000;
        Link   = (bus_major - 10000) % 1000;
    }
////    fprintf(stderr, "@%d/%d: %d/%d,%d\n", bus_major, bus_minor, BdType, Link, BdNum);

    err = CAENVME_Init(BdType, Link, BdNum, &(a3818_hal_bus_info[bus_handle].CAENVMElib_handle));
    if (err != cvSuccess)
    {
        errno = 0; // Signifies that vme_hal_strerror(ret) should be used instead of strerror(errno)
        a3818_hal_bus_info[bus_handle].in_use = 0;
        return err;
    }

    return bus_handle;
}

static int  vme_hal_close_bus(int bus_handle)
{
    if (bus_handle < A3818_BUS_HANDLE_FIRST  ||
        bus_handle >= countof(a3818_hal_bus_info))
    {
        errno = EINVAL;
        return -1;
    }

    if (a3818_hal_bus_info[bus_handle].in_use == 0)
    {
        errno = EBADF;
        return -1;
    }

    CAENVME_End(a3818_hal_bus_info[bus_handle].CAENVMElib_handle);
    a3818_hal_bus_info[bus_handle].in_use = 0;

    return 0;
}

static int  vme_hal_open_irq (int bus_handle, void *privptr,
                              int irq_n, vme_hal_irq_cb_t cb, int uniq)
{
    return -1;
}

static int  vme_hal_close_irq(int bus_handle __attribute__((unused)),
                              int irq_n)
{
    return -1;
}


#define VME_HAL_DEFINE_IO(AS, name, TS)                                          \
VME_HAL_STORAGE_CLASS int vme_hal_a##AS##wr##TS   (int bus_handle,               \
                                                   int am,                       \
                                                   uint32 addr, uint##TS  value) \
{                                                                                \
    return CAENVME_WriteCycle   (a3818_hal_bus_info[bus_handle].CAENVMElib_handle, \
                                 addr, &value,                                   \
                                 am, __CX_CONCATENATE(cvD,TS));                  \
}                                                                                \
                                                                                 \
VME_HAL_STORAGE_CLASS int vme_hal_a##AS##rd##TS   (int bus_handle,               \
                                                   int am,                       \
                                                   uint32 addr, uint##TS *val_p) \
{                                                                                \
    return CAENVME_ReadCycle    (a3818_hal_bus_info[bus_handle].CAENVMElib_handle, \
                                 addr, val_p,                                    \
                                 am, __CX_CONCATENATE(cvD,TS));                  \
}                                                                                \
VME_HAL_STORAGE_CLASS int vme_hal_a##AS##wr##TS##v(int bus_handle,               \
                                                   int am,                       \
                                                   uint32 addr, uint##TS *data, int count) \
{                                                                                \
  int  unused_count_bytes_transferred;                                           \
                                                                                 \
    return CAENVME_BLTWriteCycle(a3818_hal_bus_info[bus_handle].CAENVMElib_handle, \
                                 addr, data,                                     \
                                 count * sizeof(uint##TS),                       \
                                 am, __CX_CONCATENATE(cvD,TS),                   \
                                 &unused_count_bytes_transferred);               \
}                                                                                \
                                                                                 \
VME_HAL_STORAGE_CLASS int vme_hal_a##AS##rd##TS##v(int bus_handle,               \
                                                   int am,                       \
                                                   uint32 addr, uint##TS *data, int count) \
{                                                                                \
  int  unused_count_bytes_transferred;                                           \
                                                                                 \
    return CAENVME_BLTReadCycle (a3818_hal_bus_info[bus_handle].CAENVMElib_handle, \
                                 addr, data,                                     \
                                 count * sizeof(uint##TS),                       \
                                 am, __CX_CONCATENATE(cvD,TS),                   \
                                 &unused_count_bytes_transferred);               \
}

VME_HAL_DEFINE_IO(16, byte,  8)
VME_HAL_DEFINE_IO(16, word,  16)
VME_HAL_DEFINE_IO(16, dword, 32)

VME_HAL_DEFINE_IO(24, byte,  8)
VME_HAL_DEFINE_IO(24, word,  16)
VME_HAL_DEFINE_IO(24, dword, 32)

VME_HAL_DEFINE_IO(32, byte,  8)
VME_HAL_DEFINE_IO(32, word,  16)
VME_HAL_DEFINE_IO(32, dword, 32)
