/*********************************************************************
  bivme2_hal.h
      This file implements VME Hardware Abstraction Layer
      via BIVME2 libvmedirect API,
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

#include "libvmedirect.h"


enum
{
    VME_HAL_BUS_MAXCOUNT = 1,  // A single bus
};


static int bivme2_hal_last_used_am = -1;
static int bivme2_hal_param_var;

typedef struct
{
    int               fd;
    sl_fdh_t          fd_fdh;
    vme_hal_irq_cb_t  cb;
    void             *privptr;
} bivme2_hal_irq_info_t;
VME_HAL_STORAGE_CLASS bivme2_hal_irq_info_t bivme2_hal_irq_info[8] = {[0 ... 7] = {.fd = -1, .fd_fdh = -1}};

VME_HAL_STORAGE_CLASS int libvmedirect_inited = 0;

static const char * vme_hal_strerror(int errcode)
{
    /* Note: there ARE some different return codes in
       libvmedirect.c's libvme_init() and libvme_open(),
       but there's no much benefit in differentiating between them
       for an app; and I/O operations only return -1 on errors. */
    return errcode == 0? "NO_ERROR" : "JUST_AN_ERROR";
}

static int  vme_hal_init     (int uniq)
{
  int  r;
  int  v;

    if (!libvmedirect_inited)
    {
        r = libvme_init();
        if (r < 0)
        {
            errno = 0; // Signifies that vme_hal_strerror(ret) should be used instead of strerror(errno)
            return r;
        }

        libvmedirect_inited = 1;
    }

    v = 1;
    r = libvme_ctl(VME_SYSTEM_W, &v);

    return 0;
}

static void vme_hal_term     (void)
{
    libvme_close();
}

static int  vme_hal_open_bus (int bus_major __attribute__((unused)), int bus_minor __attribute__((unused)) )
{
    return 0;
}

static int  vme_hal_close_bus(int bus_handle __attribute__((unused)))
{
    return 0;
}

static void bivme2_hal_irq_cb(int uniq, void *unsdptr __attribute__((unused)),
                              sl_fdh_t fdh, int fd, int mask, void *privptr)
{
  int                    irq_n = ptr2lint(privptr);
  bivme2_hal_irq_info_t *ii    = bivme2_hal_irq_info + irq_n;
  int                    r;
  int32                  vector;

    errno = 0;
    r = read(fd, &vector, sizeof(vector));
    if (r == sizeof(vector))
        ii->cb(0, ii->privptr, irq_n, vector);
    else
        fprintf(stderr, "%s::%s(irq_n=%d): read()=%d, !=%d, errno=%d/%s\n",
                __FILE__, __FUNCTION__,
                irq_n, r, sizeof(vector),
                errno, strerror(errno));
                
}
static int  vme_hal_open_irq (int bus_handle __attribute__((unused)), void *privptr,
                              int irq_n, vme_hal_irq_cb_t cb, int uniq)
{
  char                   dev_vmeiN[100];
  bivme2_hal_irq_info_t *ii = bivme2_hal_irq_info + irq_n;

    sprintf(dev_vmeiN, "/dev/vmei%d", irq_n);
    if ((ii->fd = open(dev_vmeiN, O_RDONLY | O_NONBLOCK)) < 0)
    {
        fprintf(stderr, "%s::%s(): open(\"%s\"): %s\n",
                __FILE__, __FUNCTION__,
                dev_vmeiN, strerror(errno));
        return -1;
    }
    if ((ii->fd_fdh = sl_add_fd(uniq, NULL,
                                ii->fd, SL_EX,
                                bivme2_hal_irq_cb, lint2ptr(irq_n))) < 0)
    {
        fprintf(stderr, "%s::%s(): sl_add_fd(fd=%d): %s\n",
                __FILE__, __FUNCTION__,
                ii->fd,    strerror(errno));
        close(ii->fd); ii->fd     = -1;
        return -1;
    }
    ii->cb      = cb;
    ii->privptr = privptr;

    return 0;
}

static int  vme_hal_close_irq(int bus_handle __attribute__((unused)),
                              int irq_n)
{
  bivme2_hal_irq_info_t *ii = bivme2_hal_irq_info + irq_n;

    sl_del_fd(ii->fd_fdh); ii->fd_fdh = -1;
    close    (ii->fd);     ii->fd     = -1;

    return 0;
}


#define VME_HAL_DEFINE_IO(AS, name, TS)                          \
VME_HAL_STORAGE_CLASS int vme_hal_a##AS##wr##TS   (int bus_handle __attribute__((unused)), \
                                                   int am,       \
                                                   uint32 addr, uint##TS  value)  \
{                                                                \
    if (am != bivme2_hal_last_used_am)                           \
    {                                                            \
        bivme2_hal_param_var = am;                               \
        libvme_ctl(VME_AM_W, &bivme2_hal_param_var);             \
        bivme2_hal_last_used_am = am;                            \
    }                                                            \
                                                                 \
    return libvme_write_a##AS##_##name(addr, value);             \
}                                                                \
                                                                 \
VME_HAL_STORAGE_CLASS int vme_hal_a##AS##rd##TS   (int bus_handle __attribute__((unused)), \
                                                   int am,       \
                                                   uint32 addr, uint##TS *val_p)  \
{                                                                \
  uint##TS  w;                                                   \
  int       r;                                                   \
                                                                 \
    if (am != bivme2_hal_last_used_am)                           \
    {                                                            \
        bivme2_hal_param_var = am;                               \
        libvme_ctl(VME_AM_W, &bivme2_hal_param_var);             \
        bivme2_hal_last_used_am = am;                            \
    }                                                            \
                                                                 \
    r = libvme_read_a##AS##_##name(addr, &w);                    \
    *val_p = w;                                                  \
    return r;                                                    \
}                                                                \
                                                                 \
VME_HAL_STORAGE_CLASS int vme_hal_a##AS##wr##TS##v(int bus_handle __attribute__((unused)), \
                                                   int am,       \
                                                   uint32 addr, uint##TS *data, int count) \
{                                                                \
  int  ctr;                                                      \
  int  r;                                                        \
                                                                 \
    if (am != bivme2_hal_last_used_am)                           \
    {                                                            \
        bivme2_hal_param_var = am;                               \
        libvme_ctl(VME_AM_W, &bivme2_hal_param_var);             \
        bivme2_hal_last_used_am = am;                            \
    }                                                            \
                                                                 \
    for (ctr = count, r = 0;                                     \
         ctr > 0  &&  r == 0;                                    \
         ctr--, addr += sizeof(uint##TS), data += sizeof(uint##TS)) \
        r = libvme_write_a##AS##_##name(addr, *data);            \
    return r;                                                    \
}                                                                \
                                                                 \
VME_HAL_STORAGE_CLASS int vme_hal_a##AS##rd##TS##v(int bus_handle __attribute__((unused)), \
                                                   int am,       \
                                                   uint32 addr, uint##TS *data, int count) \
{                                                                \
  int       ctr;                                                 \
  int       r;                                                   \
                                                                 \
    if (am != bivme2_hal_last_used_am)                           \
    {                                                            \
        bivme2_hal_param_var = am;                               \
        libvme_ctl(VME_AM_W, &bivme2_hal_param_var);             \
        bivme2_hal_last_used_am = am;                            \
    }                                                            \
                                                                 \
    for (ctr = count, r = 0;                                     \
         ctr > 0  &&  r == 0;                                    \
         ctr--, addr += sizeof(uint##TS), data += sizeof(uint##TS)) \
        r = libvme_read_a##AS##_##name (addr,  data);            \
    return r;                                                    \
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
