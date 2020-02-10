#ifndef __VME_HAL_H
#define __VME_HAL_H


#ifndef VME_HAL_STORAGE_CLASS
  #define VME_HAL_STORAGE_CLASS static
#endif


#include "misc_types.h"


typedef void (*vme_hal_irq_cb_t)(int bus_handle, void *privptr,
                                 int irq_n, int irq_vect);

static const char * vme_hal_strerror(int errcode);

static int  vme_hal_init     (int uniq);
static void vme_hal_term     (void);
static int  vme_hal_open_bus (int bus_major, int bus_minor);
static int  vme_hal_close_bus(int bus_handle);
static int  vme_hal_open_irq (int bus_handle, void *privptr,
                              int irq_n, vme_hal_irq_cb_t cb, int uniq);
static int  vme_hal_close_irq(int bus_handle,
                              int irq_n);

// AS - Address Size, TS - dataType Size
#define VME_HAL_DECLARE_IO(AS, TS)                                                \
VME_HAL_STORAGE_CLASS int vme_hal_a##AS##wr##TS   (int    bus_handle,             \
                                                   int    am,                     \
                                                   uint32 addr, uint##TS  value)  \
                         __attribute__((unused));                                 \
VME_HAL_STORAGE_CLASS int vme_hal_a##AS##rd##TS   (int    bus_handle,             \
                                                   int    am,                     \
                                                   uint32 addr, uint##TS *val_p)  \
                         __attribute__((unused));                                 \
VME_HAL_STORAGE_CLASS int vme_hal_a##AS##wr##TS##v(int    bus_handle,             \
                                                   int    am,                     \
                                                   uint32 addr, uint##TS *data, int count) \
                         __attribute__((unused));                                 \
VME_HAL_STORAGE_CLASS int vme_hal_a##AS##rd##TS##v(int    bus_handle,             \
                                                   int    am,                     \
                                                   uint32 addr, uint##TS *data, int count) \
                         __attribute__((unused));


VME_HAL_DECLARE_IO(16, 8)
VME_HAL_DECLARE_IO(16, 16)
VME_HAL_DECLARE_IO(16, 32)

VME_HAL_DECLARE_IO(24, 8)
VME_HAL_DECLARE_IO(24, 16)
VME_HAL_DECLARE_IO(24, 32)

VME_HAL_DECLARE_IO(32, 8)
VME_HAL_DECLARE_IO(32, 16)
VME_HAL_DECLARE_IO(32, 32)


#endif /* __VME_HAL_H */
