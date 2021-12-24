/*********************************************************************
  mvme3100_hal.h
      This file implements VME Hardware Abstraction Layer
      via MVME3100 "VME user" API, see
      https://github.com/torvalds/linux/blob/master/drivers/staging/vme/devices/vme_user.c
      with IRQ handling via vmeli.c API,
      and is intended to be included by *vme_lyr*
*********************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <string.h>

#include "paramstr_parser.h"

#include "vme_hal.h"

#include <linux/types.h>
#include "stripped_vme.h" // That's supposed to be include/linux/vme.h from Linux kernel source, but since that requires wa-a-ay to many kernel's definitions, we use our own stripped version containinc constants only
#define __packed // This symbol is [illegitimately] used in vme_user.h (thst's a kernel's internal definition which shouldn't be present in userspace-includable files)
#include "vme_user.h"


#ifndef MVME3100_HAL_MULTIPLE_WINDOWS
  #define MVME3100_HAL_MULTIPLE_WINDOWS 1
#endif


enum
{
    VME_HAL_BUS_MAXCOUNT = 1,  // A single bus
};


#if MVME3100_HAL_MULTIPLE_WINDOWS
    enum {MVME3100_HAL_NUM_WINDOWS = 4};
#else
    enum {MVME3100_HAL_NUM_WINDOWS = 1};
#endif
typedef struct
{
    int     mN;
    int     fd;

    int     rqd_am;  // AM required in bus_config. Note: we use 2 separate fields because cur_am is forced to -1 by default and upon close

    int     cur_am;  // Address Modifier
    int     cur_as;  // Address Size: 16, 24, 32
    int     cur_dw;  // Data Width: 8, 16, 32
    uint32  cur_base;
    uint32  cur_size;
} mvme3100_hal_win_info_t;
VME_HAL_STORAGE_CLASS mvme3100_hal_win_info_t mvme3100_hal_win_info[MVME3100_HAL_NUM_WINDOWS] =
    {[0 ... MVME3100_HAL_NUM_WINDOWS - 1] = {.mN = 0, .fd = -1, .rqd_am = -1, .cur_am = -1}};

#if MVME3100_HAL_MULTIPLE_WINDOWS
  #define VME_HAL_DEFAULT_BUS_CONFIG_FOR_VMESERVER "win0=? win1=? win2=? win3=?"
  #define VME_HAL_DEFAULT_BUS_CONFIG_FOR_VMETEST   "win0=? win1=? win2=? win3=?"
  #define VME_HAL_DO_PARSE_BUS_CONFIG(spec) mvme3100_do_parse_bus_config(spec)

static int mvme3100hal_WinspecPluginParser(const char *str, const char **endptr,
                                           void *rec, size_t recsize __attribute__((unused)),
                                           const char *separators __attribute__((unused)), const char *terminators __attribute__((unused)),
                                           void *privptr, char **errstr)
{
  mvme3100_hal_win_info_t *wp = ((mvme3100_hal_win_info_t *)rec) + ptr2lint(privptr);
  const char              *p  = str;
  char                    *errp;

    if (str == NULL) return PSP_R_OK; // Initialization: (NULL->value)

    /* Is it a "read current" specification? */
    if (*p == '?'  ||  *p == 'q')  // 'q' is a backup way, added because '?' is interpreted by shell and requires quoting
    {
        wp->mN          = 0;

        *endptr = p + 1;

        return PSP_R_OK;
    }

    /* Or a "disable" specification? */
    if (*p == '-')
    {
        wp->mN          = -1;
        wp->rqd_am      = -1;

        *endptr = p + 1;

        return PSP_R_OK;
    }

    /* None of the above -- just a straightforward "AnnDmm:ADDRESS_MODIFIER:BASE_ADDRESS/WINDOW_SIZE" spec */
    if (tolower(*p) == 'a') p++; // Optional prefix, for "A32" to be valid alongside "32"
    wp->cur_as          = strtol(p, &errp, 0);
    if (errp == p  ||
        (*errp != ':'  &&
         *errp != '/'  &&  
         tolower(*errp) != 'd'))
    {
        *errstr = "syntax error in ADDRESS_SIZE";
        return PSP_R_USRERR;
    }
    p = errp + 1;
    if (wp->cur_as != 16  &&
        wp->cur_as != 24  &&
        wp->cur_as != 32) // For VME64: also 64 and 40 (but TSI148 does NOT support A40 :D)
    {
        *errstr = "bad ADDRESS_SIZE (should be one of 16,24,32)";
        return PSP_R_USRERR;
    }
    if (*p == '/') p++;
    if (*p == 'd') p++;
    if (*p != ':')
    {
        wp->cur_dw      = strtol(p, &errp, 0);
        if (errp == p  ||  *errp != ':')
        {
            *errstr = "syntax error in DATA_WIDTH";
            return PSP_R_USRERR;
        }
        p = errp + 1;
        if (wp->cur_dw !=  8  &&
            wp->cur_dw != 16  &&
            wp->cur_dw != 32)
        {
            *errstr = "bad DATA_SIZE (should be one of 8,16,32)";
            return PSP_R_USRERR;
        }
    }
    else
        wp->cur_dw      = 32;

    wp->rqd_am          = strtol(p, &errp, 0);
    if (errp == p  ||  *errp != ':')
    {
        *errstr = "syntax error in ADDRESS_MODIFIER";
        return PSP_R_USRERR;
    }
    p = errp + 1;

    wp->cur_base        = strtoul(p, &errp, 0);
    if (errp == p  ||  *errp != '/')
    {
        *errstr = "syntax error in BASE_ADDRESS";
        return PSP_R_USRERR;
    }
    p = errp + 1;

    wp->cur_size        = strtoul(p, &errp, 0);
    if (errp == p)
    {
        *errstr = "syntax error in WINDOW_SIZE";
        return PSP_R_USRERR;
    }

    *endptr = errp;

    return PSP_R_OK;
}

static psp_paramdescr_t mvme3100_params[] =
{
    PSP_P_PLUGIN("win0", mvme3100_hal_win_info_t, mN, mvme3100hal_WinspecPluginParser, (void*)(0)),
    PSP_P_PLUGIN("win1", mvme3100_hal_win_info_t, mN, mvme3100hal_WinspecPluginParser, (void*)(1)),
    PSP_P_PLUGIN("win2", mvme3100_hal_win_info_t, mN, mvme3100hal_WinspecPluginParser, (void*)(2)),
    PSP_P_PLUGIN("win3", mvme3100_hal_win_info_t, mN, mvme3100hal_WinspecPluginParser, (void*)(3)),
    PSP_P_END()
};

static int mvme3100_do_parse_bus_config(const char *bus_config)
{
    return psp_parse_as(bus_config, NULL, &(mvme3100_hal_win_info[0]), '=', " \t,", "", mvme3100_params, PSP_MF_NOINIT);
}
#else
  #define VME_HAL_DEFAULT_BUS_CONFIG_FOR_VMESERVER "win=0"
  #define VME_HAL_DEFAULT_BUS_CONFIG_FOR_VMETEST   "win=3"
  #define VME_HAL_DO_PARSE_BUS_CONFIG(spec) mvme3100_do_parse_bus_config(spec)

static psp_paramdescr_t mvme3100_params[] =
{
    PSP_P_INT("win", mvme3100_hal_win_info_t, mN, 0, 0, 3),
    PSP_P_END()
};

static int mvme3100_do_parse_bus_config(const char *bus_config)
{
    return psp_parse_as(bus_config, NULL, &(mvme3100_hal_win_info[0]), '=', " \t,", "", mvme3100_params, PSP_MF_NOINIT);
}
#endif

typedef struct
{
    int               fd;
    sl_fdh_t          fd_fdh;
    vme_hal_irq_cb_t  cb;
    void             *privptr;
} mvme3100_hal_irq_info_t;
VME_HAL_STORAGE_CLASS mvme3100_hal_irq_info_t mvme3100_hal_irq_info[8] = {[0 ... 7] = {.fd = -1, .fd_fdh = -1}};

static int AM2cycle[64] = {
    [0x09] = VME_USER  | VME_DATA,
    [0x0D] = VME_SUPER | VME_DATA,
    [0x29] = VME_USER  | VME_DATA,
    [0x2D] = VME_SUPER | VME_DATA,
};
static int mvme3100_hal_configure_window(int win, int AM, int AS, int TS, uint32 addr, uint32 size)
{
  int                r;
  struct vme_master  master;
  uint32             vme_base;
  uint32             vme_size;
  uint32             max_size;
  int                aspace;
  int                cycle;
  int                dwidth;

    // 1. Convert "reasonable" numeric parameters to corresponding codes
    // Magic numbers are taken from https://gitlab.collabora.com/vme/vme-utils/-/blob/master/vme_master.c
    // ...which, in turn, match corresponding ones from include/linux/vme.h in Linux kernel source

    // Address Size
    if      (AS == 16) {aspace = VME_A16; max_size = 0x00010000;}
    else if (AS == 24) {aspace = VME_A24; max_size = 0x01000000;}
    else if (AS == 32) {aspace = VME_A32; max_size = 0x01000000 * 21;}
  /*else if (AS == 64) {aspace = VME_A64; max_size = 0x01000000 * 21;}*/
    else
    {
        errno = EINVAL;
        return -1;
    }

    // "cycle" is a transformation of Address Modifier
    cycle = AM2cycle[AM];
    if (cycle == 0)
    {
        errno = EINVAL;
        return -1;
    }

    // Data Width -- that's in fact just size in bytes
    dwidth = TS / 8;

    // 2. Suppose some BASE and SIZE...
    vme_base = addr & 0xFFFF0000;  // See vme_tsi148.c::tsi148_master_set(): the  "if (vme_base & 0xFFFF)" check implies the base address must be 64KB-aligned
    vme_size = max_size;

    // 3. Set
    // See example at https://linux-vme.org/kernel-drivers.html
    master.enable   = 1;
    master.vme_addr = vme_base;
    master.size     = vme_size;
    master.aspace   = aspace;
    master.cycle    = cycle;
    master.dwidth   = dwidth;
    r = ioctl(mvme3100_hal_win_info[win].fd, VME_SET_MASTER, &master);
    if (r < 0) return r;

    // 4. Store settings as current
    mvme3100_hal_win_info[win].cur_am   = AM;
    mvme3100_hal_win_info[win].cur_as   = AS;
    mvme3100_hal_win_info[win].cur_dw   = TS;
    mvme3100_hal_win_info[win].cur_base = vme_base;
    mvme3100_hal_win_info[win].cur_size = vme_size;

    return 0;
}

static const char * vme_hal_strerror(int errcode)
{
    /* Note: 
       if we are here (so that caller decided NOT to use errno)
       than we have no chance to describe the error anyway. */
    return errcode == 0? "NO_ERROR" : "JUST_AN_ERROR";
}

#if MVME3100_HAL_MULTIPLE_WINDOWS
static int  vme_hal_init     (int uniq)
{
  int                win;
  char               dev_bus_vme_mN[100];
  int                any_open;
  int                r;
  int                saved_errno;
  struct vme_master  master;

  int                mode;
  int                AM;

  // These are bits for conversion from (aspace,cycle) to Address Modifier via table
  enum
  {
      // Bit 0: data/code
      TBL_DATA = 0 << 0,
      TBL_CODE = 1 << 0,
      // Bit 1: unprivileged/privileged
      TBL_USER = 0 << 1,
      TBL_PRIV = 1 << 1,
      // Bits 2-4: address size
      TBL_A16  = 0 << 2,
      TBL_A24  = 1 << 2,
      TBL_A32  = 2 << 2,
      TBL_A40  = 3 << 2,
      TBL_A64  = 4 << 2,
  };

  static int mode2AM[] =
  {
      [TBL_A16 | TBL_PRIV | TBL_DATA] = 0x2D, // A16 supervisory access
      [TBL_A16 | TBL_USER | TBL_DATA] = 0x29, // A16 non-privileged

      [TBL_A24 | TBL_PRIV | TBL_CODE] = 0x3E, // A24 supervisory program access
      [TBL_A24 | TBL_PRIV | TBL_DATA] = 0x3D, // A24 supervisory data access
      [TBL_A24 | TBL_USER | TBL_CODE] = 0x3A, // A24 non-privileged program access
      [TBL_A24 | TBL_USER | TBL_DATA] = 0x39, // A24 non-privileged data access

      [TBL_A32 | TBL_PRIV | TBL_CODE] = 0x0E, // A32 supervisory program access
      [TBL_A32 | TBL_PRIV | TBL_DATA] = 0x0D, // A32 supervisory data access
      [TBL_A32 | TBL_USER | TBL_CODE] = 0x0A, // A32 non-privileged program access
      [TBL_A32 | TBL_USER | TBL_DATA] = 0x09, // A32 non-privileged data access      
  };

    any_open = 0;
    for (win = 0;  win < countof(mvme3100_hal_win_info);  win++)
    {
        if      (mvme3100_hal_win_info[win].mN == 0)  // Was AUTOconfiguration required?
        {
            sprintf(dev_bus_vme_mN, "/dev/bus/vme/m%d", win);
            mvme3100_hal_win_info[win].fd = open(dev_bus_vme_mN, O_RDWR);
            if (mvme3100_hal_win_info[win].fd < 0) goto ERREXIT;
            // Get current configuration from the device and reflect it in mvme3100_hal_win_info[win].cur_*
            if ((r = ioctl(mvme3100_hal_win_info[win].fd, VME_GET_MASTER, &master)) < 0) goto ERREXIT;

            if (master.enable == 0) goto DISABLE_WIN;

            // Address space
            if      (master.aspace == VME_A16) mvme3100_hal_win_info[win].cur_as = 16;
            else if (master.aspace == VME_A24) mvme3100_hal_win_info[win].cur_as = 24;
            else if (master.aspace == VME_A32) mvme3100_hal_win_info[win].cur_as = 32;
          /*else if (master.aspace == VME_A64) mvme3100_hal_win_info[win].cur_as = 64;*/
          /*else if (master.aspace == VME_CRCSR) {???}*/
            else goto DISABLE_WIN;

            // Translate damned (aspace,cycle) duplet into damned Address Modifier
            // a. Address space
            if      (master.aspace == VME_A16) mode = TBL_A16;
            else if (master.aspace == VME_A16) mode = TBL_A24;
            else if (master.aspace == VME_A32) mode = TBL_A32;
          /*else if (master.aspace == VME_A64) mode = TBL_A64;*/
          /*else if (master.aspace == VME_CRCSR) {???}*/
            else goto DISABLE_WIN; // In fact, this guard is extroneous, because of an identical stack of conditions above, but it is here for safety and to keep compiler from "'mode' may be used uninitialized" warning
            // b. Access mode
            mode |= ((master.cycle & VME_SUPER)? TBL_PRIV : TBL_USER);
            mode |= ((master.cycle & VME_SUPER)? TBL_CODE : TBL_DATA);
            if (mode >= countof(mode2AM)  ||  mode2AM[mode] == 0) goto DISABLE_WIN;
            mvme3100_hal_win_info[win].cur_am = mode2AM[mode];

            // Simple parameters
            mvme3100_hal_win_info[win].cur_dw   = master.dwidth * 8;  // dwidth is in fact just size in bytes
            mvme3100_hal_win_info[win].cur_base = master.vme_addr;
            mvme3100_hal_win_info[win].cur_size = master.size;

            goto NEXT_WIN;

 DISABLE_WIN:
            if (mvme3100_hal_win_info[win].fd >= 0)
            {
                close(mvme3100_hal_win_info[win].fd);
                mvme3100_hal_win_info[win].fd = -1;
            }
            mvme3100_hal_win_info[win].cur_am = -1;
        }
        else if (mvme3100_hal_win_info[win].rqd_am >= 0)  // Is this window configured somehow?
        {
            sprintf(dev_bus_vme_mN, "/dev/bus/vme/m%d", win);
            mvme3100_hal_win_info[0].fd = open(dev_bus_vme_mN, O_RDWR);
            if (mvme3100_hal_win_info[0].fd < 0) goto ERREXIT;
            r = mvme3100_hal_configure_window(win, 
                                              mvme3100_hal_win_info[win].rqd_am,
                                              mvme3100_hal_win_info[win].cur_as,
                                              mvme3100_hal_win_info[win].cur_dw, 
                                              mvme3100_hal_win_info[win].cur_base,
                                              mvme3100_hal_win_info[win].cur_size);
            if (r < 0) goto ERREXIT;
        }
 NEXT_WIN:;
    }
    if (!any_open)
    {
        /*!!!???*/
        return -1;
    }

    return 0;

 ERREXIT:
    saved_errno = errno;
    vme_hal_term();
    errno = saved_errno;
    return -1;
}
#else
static int  vme_hal_init     (int uniq)
{
  char  dev_bus_vme_mN[100];

    if (mvme3100_hal_win_info[0].fd >= 0)
    {
        errno = EBUSY;
        return -1;
    }

    if (mvme3100_hal_win_info[0].mN < 0  ||  mvme3100_hal_win_info[0].mN > 3)
    {
        errno = EINVAL;
        return -1;
    }
    sprintf(dev_bus_vme_mN, "/dev/bus/vme/m%d", mvme3100_hal_win_info[0].mN);
    mvme3100_hal_win_info[0].fd = open(dev_bus_vme_mN, O_RDWR);
    if (mvme3100_hal_win_info[0].fd < 0) return -1;

    mvme3100_hal_win_info[0].cur_am = -1;  // For fist I/O call to configure the window

    return 0;
}
#endif

static void vme_hal_term     (void)
{
  int   win;

    for (win = 0;  win < countof(mvme3100_hal_win_info);  win++)
        if (mvme3100_hal_win_info[win].fd >= 0)
        {
            close(mvme3100_hal_win_info[win].fd);
            mvme3100_hal_win_info[win].fd     = -1;
            mvme3100_hal_win_info[win].cur_am = -1;
        }
}

static int  vme_hal_open_bus (int bus_major __attribute__((unused)), int bus_minor __attribute__((unused)) )
{
    return 0;
}

static int  vme_hal_close_bus(int bus_handle __attribute__((unused)))
{
    return 0;
}

static void mvme3100_hal_irq_cb(int uniq, void *unsdptr __attribute__((unused)),
                                sl_fdh_t fdh, int fd, int mask, void *privptr)
{
  int                      irq_n = ptr2lint(privptr);
  mvme3100_hal_irq_info_t *ii    = mvme3100_hal_irq_info + irq_n;
  int                      r;
  int32                    vector;

    errno = 0;
    r = read(fd, &vector, sizeof(vector));
    if (r == sizeof(vector))
        ii->cb(0, ii->privptr, irq_n, vector);
    else
        fprintf(stderr, "%s::%s(irq_n=%d): read()=%d, !=%zd, errno=%d/%s\n",
                __FILE__, __FUNCTION__,
                irq_n, r, sizeof(vector),
                errno, strerror(errno));
                
}
static int  vme_hal_open_irq (int bus_handle __attribute__((unused)), void *privptr,
                              int irq_n, vme_hal_irq_cb_t cb, int uniq)
{
  char                     dev_vmeiN[100];
  mvme3100_hal_irq_info_t *ii = mvme3100_hal_irq_info + irq_n;

    sprintf(dev_vmeiN, "/dev/vmeli%d", irq_n);
    if ((ii->fd = open(dev_vmeiN, O_RDONLY | O_NONBLOCK)) < 0)
    {
        fprintf(stderr, "%s::%s(): open(\"%s\"): %s\n",
                __FILE__, __FUNCTION__,
                dev_vmeiN, strerror(errno));
        return -1;
    }
    if ((ii->fd_fdh = sl_add_fd(uniq, NULL,
                                ii->fd, SL_EX,
                                mvme3100_hal_irq_cb, lint2ptr(irq_n))) < 0)
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
  mvme3100_hal_irq_info_t *ii = mvme3100_hal_irq_info + irq_n;

    sl_del_fd(ii->fd_fdh); ii->fd_fdh = -1;
    close    (ii->fd);     ii->fd     = -1;

    return 0;
}

#if MVME3100_HAL_MULTIPLE_WINDOWS
static int CHECK_AND_SETUP_VME_WINDOW(int AM, int AS, int TS, uint32 addr, uint32 size)
{
  int                win;  // WindowInfo Number

    if (AM < 0  ||  AM > countof(AM2cycle))
    {
        errno = EINVAL;
        return -1;
    }

    for (win = 0;  win < countof(mvme3100_hal_win_info);  win++)
        if (AM   == mvme3100_hal_win_info[win].cur_am    ||
            AS   == mvme3100_hal_win_info[win].cur_as    ||
            TS   == mvme3100_hal_win_info[win].cur_dw    ||
            addr >= mvme3100_hal_win_info[win].cur_base  ||
            addr + size <= mvme3100_hal_win_info[win].cur_base + mvme3100_hal_win_info[win].cur_size)
            return win;    

    errno = ENOENT;
    return -1;
}
#else
static int CHECK_AND_SETUP_VME_WINDOW(int AM, int AS, int TS, uint32 addr, uint32 size)
{
  int                win;  // WindowInfo Number
  int                r;

    win = 0;

    if (AM < 0  ||  AM > countof(AM2cycle))
    {
        errno = EINVAL;
        return -1;
    }

    if (AM   != mvme3100_hal_win_info[win].cur_am    ||
        AS   != mvme3100_hal_win_info[win].cur_as    ||
        TS   != mvme3100_hal_win_info[win].cur_dw    ||
        addr <  mvme3100_hal_win_info[win].cur_base  ||
        addr + size > mvme3100_hal_win_info[win].cur_base + mvme3100_hal_win_info[win].cur_size)
    {
        r = mvme3100_hal_configure_window(win, AM, AS, TS, addr, size);
        if (r < 0) return r;
    }

    return win;
}
#endif

#define VME_HAL_DEFINE_IO(AS, name, TS)                          \
VME_HAL_STORAGE_CLASS int vme_hal_a##AS##wr##TS   (int bus_handle __attribute__((unused)), \
                                                   int am,       \
                                                   uint32 addr, uint##TS  value)  \
{                                                                \
  int  win;                                                      \
                                                                 \
    win = CHECK_AND_SETUP_VME_WINDOW(am, AS, TS, addr, sizeof(uint##TS)); \
    if (win < 0) return win;                                     \
                                                                 \
    return pwrite(mvme3100_hal_win_info[win].fd,                 \
                  &value,                                        \
                  sizeof(uint##TS),                              \
                  addr - mvme3100_hal_win_info[win].cur_base);   \
}                                                                \
                                                                 \
VME_HAL_STORAGE_CLASS int vme_hal_a##AS##rd##TS   (int bus_handle __attribute__((unused)), \
                                                   int am,       \
                                                   uint32 addr, uint##TS *val_p)  \
{                                                                \
  int  win;                                                      \
                                                                 \
    win = CHECK_AND_SETUP_VME_WINDOW(am, AS, TS, addr, sizeof(uint##TS)); \
    if (win < 0) return win;                                     \
                                                                 \
    return pread (mvme3100_hal_win_info[win].fd,                 \
                  val_p,                                         \
                  sizeof(uint##TS),                              \
                  addr - mvme3100_hal_win_info[win].cur_base);   \
}                                                                \
                                                                 \
VME_HAL_STORAGE_CLASS int vme_hal_a##AS##wr##TS##v(int bus_handle __attribute__((unused)), \
                                                   int am,       \
                                                   uint32 addr, uint##TS *data, int count) \
{                                                                \
  int  win;                                                      \
                                                                 \
    win = CHECK_AND_SETUP_VME_WINDOW(am, AS, TS, addr, sizeof(uint##TS)); \
    if (win < 0) return win;                                     \
                                                                 \
    return pwrite(mvme3100_hal_win_info[win].fd,                 \
                  data,                                          \
                  sizeof(uint##TS) * count,                      \
                  addr - mvme3100_hal_win_info[win].cur_base);   \
}                                                                \
                                                                 \
VME_HAL_STORAGE_CLASS int vme_hal_a##AS##rd##TS##v(int bus_handle __attribute__((unused)), \
                                                   int am,       \
                                                   uint32 addr, uint##TS *data, int count) \
{                                                                \
  int  win;                                                      \
                                                                 \
    win = CHECK_AND_SETUP_VME_WINDOW(am, AS, TS, addr, sizeof(uint##TS)); \
    if (win < 0) return win;                                     \
                                                                 \
    return pread (mvme3100_hal_win_info[win].fd,                 \
                  data,                                          \
                  sizeof(uint##TS) * count,                      \
                  addr - mvme3100_hal_win_info[win].cur_base);   \
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
