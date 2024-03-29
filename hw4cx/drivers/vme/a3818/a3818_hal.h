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

#include "misclib.h"

#include "vme_hal.h"

#define LINUX // For CAENVMEtypes.h, which uses "#ifdef LINUX" instead of "#ifdef WIN32"
#include "CAENVMElib.h"


// Note: IRQ polling in a separate thread does NOT work, because 2nd CAENVME_Init() fails with cvGenericError=-3 "Unspecified error"
#ifndef USE_THREAD_FOR_IRQ
  #define USE_THREAD_FOR_IRQ 0
#endif

#if USE_THREAD_FOR_IRQ
  #include <pthread.h>
#endif


#ifndef USE_SELECT_FOR_IRQ
  #ifdef CAENVMELIB_SUPPORTS_POLL
    #define USE_SELECT_FOR_IRQ 1
  #else
    #define USE_SELECT_FOR_IRQ 0
  #endif
#endif


enum
{
    VME_HAL_BUS_MAXCOUNT = 10,  // An arbitrary limit
};


#if USE_THREAD_FOR_IRQ
enum
{   /* this enumeration according to man(3) pipe */
    PIPE_RD_SIDE = 0,
    PIPE_WR_SIDE = 1,
};
#endif /* USE_THREAD_FOR_IRQ */

enum {A3818_BUS_HANDLE_FIRST = 1};
typedef struct
{
    vme_hal_irq_cb_t  cb;
    void             *privptr;
} a3818_hal_irq_info_t;
typedef struct
{
    int                   in_use;

    int                   bus_major;
    int                   bus_minor;

    int32_t               CAENVMElib_handle; // The same type as in CAENVMElib.h
#if USE_THREAD_FOR_IRQ
    int32_t               CAENVMElib_IRQ_handle;
    int                   irq_pipe[2];
    sl_fdh_t              irq_rfdh;
    pthread_t             irq_thread;
#elif USE_SELECT_FOR_IRQ
    int                   handle_fd;
    sl_fdh_t              handle_fdhandle;
#endif

    int                   irq_mask;
    a3818_hal_irq_info_t  irq_info[8];
} a3818_hal_bus_info_t;
VME_HAL_STORAGE_CLASS a3818_hal_bus_info_t a3818_hal_bus_info[A3818_BUS_HANDLE_FIRST + VME_HAL_BUS_MAXCOUNT];

//////////////////////////////////////////////////////////////////////
#if USE_THREAD_FOR_IRQ
static void *irq_wait_proc(void *arg)
{
  a3818_hal_bus_info_t *me = arg;

  CVErrorCodes          err;
  CAEN_BYTE             mask;
  int                   level;
  int                   repcount;
  uint32                vect32; // We use 32-bit with cvD8 just "na vsyakij sluchaj"

  uint16                irq_data;

  static CVIRQLevels level2cvIRQx[8] =
  {0, cvIRQ1, cvIRQ2, cvIRQ3, cvIRQ4, cvIRQ5, cvIRQ6, cvIRQ7};

    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,       NULL);
    pthread_setcanceltype (PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

    while (1)
    {
        err = CAENVME_IRQWait(me->CAENVMElib_IRQ_handle,
                              0x7F, 3600 * 1000); // 1 hour timeout
        if (err == cvSuccess)
        {
            err = CAENVME_IRQCheck(me->CAENVMElib_IRQ_handle, &mask);
            /* Note: there's a probable race condition in the following
                     check of mask against irq_mask, but we ignore it
                     because:
                     a. Extra mask has no consequences
                        (it will be filtered out at IRQ-dispatch stage).
                     b. Missing mask is safe
                        (it will be processed just a bil later) */
            if (err < 0  ||  (mask & (me->irq_mask >> 1)) == 0) goto NEXT_WAIT;

            for (level = 1;  level <= 7;  level++)
                if ((mask & (1 << (level - 1))) != 0)
                {
                    for (repcount = 100;  repcount > 0;  repcount--)
                    {
                        err = CAENVME_IACKCycle(me->CAENVMElib_IRQ_handle,
                                                level2cvIRQx[level], &vect32, cvD8);
////fprintf(stderr, "\tIACKCycle(%d, %d): err=%d vect32=%d\n", a3818_hal_bus_info[idx].CAENVMElib_handle, level, err, vect32);
                        if (err < 0) goto NEXT_LEVEL;

                        irq_data = (vect32 & 0xFF) | (level << 8);
                        write(me->irq_pipe[PIPE_WR_SIDE], &irq_data, sizeof(irq_data));
                    }

            NEXT_LEVEL:;
                }
        }
    NEXT_WAIT:;
    }

    return NULL;
}

static void irq2pipe_proc(int uniq, void *privptr1,
                          sl_fdh_t fdhandle, int fd, int mask,
                          void *privptr2)
{
  a3818_hal_bus_info_t *me = privptr2;

  uint16                irq_data;
  int                   r;
  int                   level;
  int                   vect;
  a3818_hal_irq_info_t *ii;

    r = read(fd, &irq_data, sizeof(irq_data));
    if (r != sizeof(irq_data))
    {
        /*!!!???*/
        return;
    }

    level = (irq_data >> 8) & 0xFF;
    vect  =  irq_data       & 0xFF;
    ii = me->irq_info + level;
    if (ii->cb != NULL)
        ii->cb(me - a3818_hal_bus_info /*!!! info2idx() !!! */,
               ii->privptr, level, vect);
}
#elif USE_SELECT_FOR_IRQ
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>

#include "timeval_utils.h"


void localSleepBySelect(int usecs)
{
  struct timeval  deadline;
  struct timeval  now;
  struct timeval  timeout;
    
    gettimeofday(&deadline, NULL);
    timeval_add_usecs(&deadline, &deadline, usecs);

    while (1)
    {
        gettimeofday(&now, NULL);
        if (timeval_subtract(&timeout, &deadline, &now) != 0) return;
        if (select(0, NULL, NULL, NULL, &timeout) == 0)       return;
    }
}
static void irq_fd_p(int uniq, void *privptr1,
                     sl_fdh_t fdhandle, int fd, int mask,
                     void *privptr2)
{
  a3818_hal_bus_info_t *me = privptr2;

  int                   r;
  CVErrorCodes          err;
 uint32_t              event_mask;
  CAEN_BYTE             lit_mask;
  int                   level;
  int                   repcount;
  uint32                vect32; // We use 32-bit with cvD8 just "na vsyakij sluchaj"
  a3818_hal_irq_info_t *ii;

  static CVIRQLevels level2cvIRQx[8] =
  {0, cvIRQ1, cvIRQ2, cvIRQ3, cvIRQ4, cvIRQ5, cvIRQ6, cvIRQ7};


////    CAENVME_IRQEnable(me->CAENVMElib_handle, 0x7F);

    r = CAENVME_ProcessEvent(me->CAENVMElib_handle, &event_mask, 0);
    if (r < 0  ||  event_mask == 0) return;

    if ((event_mask & cvConnectionLost)      != 0)
    {
        fprintf(stderr, "%s %s(%d,%d) A3818_CONNECTION_LOST\n",      strcurtime_msc(), __FUNCTION__, me->bus_major, me->bus_minor);
    }
    if ((event_mask & cvConnectionRecovered) != 0)
    {
        fprintf(stderr, "%s %s(%d,%d) A3818_CONNECTION_RECOVERED\n", strcurtime_msc(), __FUNCTION__, me->bus_major, me->bus_minor);
//localSleepBySelect(10*1000000);
////        err = CAENVME_DeviceReset(me->CAENVMElib_handle);
////        fprintf(stderr, "%s\treset err=%d\n", strcurtime_msc(), err);
        vect32=0xdeadbaba;
        err = CAENVME_IRQCheck(me->CAENVMElib_handle, &lit_mask);
        fprintf(stderr, "\t\t\t\tcheck: err=%d lit_mask=0x%08x\n", err, lit_mask);
        err = CAENVME_IRQEnable(me->CAENVMElib_handle, 0x7F);
        fprintf(stderr, "\t\t\t\tenable: err=%d\n", err);
#if 1
        err = CAENVME_WriteRegister(me->CAENVMElib_handle, cvVMEIRQEnaReg, 0xFF);
#endif
#if 0
        err = CAENVME_ReadCycle(me->CAENVMElib_handle, 0xd6000000, &vect32, 0x09, cvD32);
        fprintf(stderr, "%s\t\tread: err=%d vect32=0x%08x\n", strcurtime_msc(), err, vect32);
#endif
#if 0 // Re-open the device
    {
      CVBoardTypes          BdType;
      short                 Link;
      short                 BdNum;

        CAENVME_End(me->CAENVMElib_handle); me->CAENVMElib_handle = -1;
        sl_del_fd(me->handle_fdhandle);     me->handle_fdhandle   = -1;
    
        BdType = cvV2718;
        Link   = me->bus_major;
        BdNum  = me->bus_minor;
        if (me->bus_major >= 10000)
        {
            BdType = (me->bus_major - 10000) / 1000;
            Link   = (me->bus_major - 10000) % 1000;
        }
        fprintf(stderr, "\t\t\t\t...CAENVME_Init(BdType=%d, Link=%d, BdNum=%d) start\n", BdType, Link, BdNum);
        err = CAENVME_Init(BdType, Link, BdNum, &(me->CAENVMElib_handle));
        fprintf(stderr, "\t\t\t\t...CAENVME_Init() finished err=%d handle=%d\n", err, me->CAENVMElib_handle);
        err = CAENVME_IRQEnable(me->CAENVMElib_handle, 0x7F);

        me->handle_fd       = CAENVME_GetFiledes(me->CAENVMElib_handle);
        fprintf(stderr, "\t\t\t\tCAENVME_GetFiledes(%d)=%d\n", me->CAENVMElib_handle, me->handle_fd);
        set_fd_flags(me->handle_fd, O_NONBLOCK, 1);
        me->handle_fdhandle = sl_add_fd(0/*!!!uniq!!!*/, NULL,
                                        me->handle_fd, SL_EX, irq_fd_p, me);
        fprintf(stderr, "\t\t\t\tsl_add_fd(%d)=%d\n", me->handle_fd, me->handle_fdhandle);
    }
#endif
    }

#if 1
    lit_mask = event_mask & 0xFF;
    if ((lit_mask & (me->irq_mask >> 1)) == 0) return;
#else
    err = CAENVME_IRQCheck(me->CAENVMElib_handle, &lit_mask);
    if (err < 0  ||  (lit_mask & (me->irq_mask >> 1)) == 0) return;
#endif

#if 1
    for (level = 1;  level <= 7;  level++)
        if ((lit_mask & (1 << (level - 1))) != 0)
        {
          int vect_buf[100*1+1*0]; // "max value for repcount"
          int vect_buf_used;
          int vect_buf_idx;

            // 1. Get a bunch of vectors
            for (vect_buf_used = 0;
                 vect_buf_used < countof(vect_buf);
                 vect_buf_used++)
            {
                err = CAENVME_IACKCycle(me->CAENVMElib_handle,
                                        level2cvIRQx[level], &vect32, cvD8);
////fprintf(stderr, "\tIACKCycle(%d, %d): err=%d vect32=%d\n", me->CAENVMElib_handle, level, err, vect32);
                if (err < 0) goto LAST_VECT; // This goto is just "break"
                vect_buf[vect_buf_used] = (int)(vect32 & 0xFF);
            }
        LAST_VECT:;

            // 2. Process those vectors
            for (vect_buf_idx = 0;  vect_buf_idx < vect_buf_used;  vect_buf_idx++)
            {
                ii = me->irq_info + level;
                if (ii->cb != NULL)
                    ii->cb(me - a3818_hal_bus_info /*!!! info2idx() !!! */,
                           ii->privptr, level, vect_buf[vect_buf_idx]);
            }

    NEXT_LEVEL:;
        }
#else
    for (level = 1;  level <= 7;  level++)
        if ((lit_mask & (1 << (level - 1))) != 0)
        {
            for (repcount = 100*1+1*0;  repcount > 0;  repcount--)
            {
                err = CAENVME_IACKCycle(me->CAENVMElib_handle,
                                        level2cvIRQx[level], &vect32, cvD8);
////fprintf(stderr, "\tIACKCycle(%d, %d): err=%d vect32=%d\n", me->CAENVMElib_handle, level, err, vect32);
                if (err < 0) goto NEXT_LEVEL;

                ii = me->irq_info + level;
                if (ii->cb != NULL)
                    ii->cb(me - a3818_hal_bus_info /*!!! info2idx() !!! */,
                           ii->privptr, level, (int)(vect32 & 0xFF));
            }

    NEXT_LEVEL:;
        }
#endif

    // Re-enable interrupts /*!!! Shouldn't we use event_mask instead of 0x7F? *.
    CAENVME_IRQEnable(me->CAENVMElib_handle, 0x7F);
}
#else
enum {A3818_HEARTBEAT_USECS = 50*1000*1 + 5000000*0}; // Use 5s for debugging; 50ms<=>20Hz is enough for VEPP5's 10Hz
static sl_tid_t  a3818_hbt_tid = -1;

static void a3818_heartbeat_proc(int uniq, void *unsdptr,
                                 sl_tid_t tid, void *privptr)
{
  int                   idx;
  CVErrorCodes          err;
  CAEN_BYTE             mask;
  int                   level;
  int                   repcount;
  uint32                vect32; // We use 32-bit with cvD8 just "na vsyakij sluchaj"
  a3818_hal_irq_info_t *ii;

  static CVIRQLevels level2cvIRQx[8] =
  {0, cvIRQ1, cvIRQ2, cvIRQ3, cvIRQ4, cvIRQ5, cvIRQ6, cvIRQ7};

    a3818_hbt_tid = sl_enq_tout_after(uniq, NULL,
                                      A3818_HEARTBEAT_USECS, a3818_heartbeat_proc, NULL);

    for (idx = A3818_BUS_HANDLE_FIRST;
         idx < countof(a3818_hal_bus_info);
         idx++)
        if (a3818_hal_bus_info[idx].in_use  &&  a3818_hal_bus_info[idx].irq_mask != 0)
        {
            err = CAENVME_IRQCheck(a3818_hal_bus_info[idx].CAENVMElib_handle, &mask);
////fprintf(stderr, "IRQCheck: err=%d mask=%d\n", err, mask);
            if (err < 0  ||  (mask & (a3818_hal_bus_info[idx].irq_mask >> 1)) == 0) goto NEXT_BUS;

            for (level = 1;  level <= 7;  level++)
                if ((mask & (1 << (level - 1))) != 0)
                {
                    for (repcount = 100;  repcount > 0;  repcount--)
                    {
                        err = CAENVME_IACKCycle(a3818_hal_bus_info[idx].CAENVMElib_handle,
                                                level2cvIRQx[level], &vect32, cvD8);
////fprintf(stderr, "\tIACKCycle(%d, %d): err=%d vect32=%d\n", a3818_hal_bus_info[idx].CAENVMElib_handle, level, err, vect32);
                        if (err < 0) goto NEXT_LEVEL;

                        ii = a3818_hal_bus_info[idx].irq_info + level;
                        if (ii->cb != NULL)
                            ii->cb(idx, ii->privptr, level, (int)(vect32 & 0xFF));
                    }

            NEXT_LEVEL:;
                }

    NEXT_BUS:;
        }
}
#endif /* USE_THREAD_FOR_IRQ */
//////////////////////////////////////////////////////////////////////


static const char * vme_hal_strerror(int errcode)
{
    return CAENVME_DecodeError(errcode);
}

static int  vme_hal_init     (int uniq)
{
    bzero(a3818_hal_bus_info, sizeof(a3818_hal_bus_info));

#if   USE_THREAD_FOR_IRQ
#elif USE_SELECT_FOR_IRQ
#else
    a3818_hbt_tid = sl_enq_tout_after(uniq, NULL,
                                      A3818_HEARTBEAT_USECS, a3818_heartbeat_proc, NULL);
#endif /* USE_THREAD_FOR_IRQ */

    return 0;
}

static void vme_hal_term     (void)
{
#if   USE_THREAD_FOR_IRQ
#elif USE_SELECT_FOR_IRQ
#else
    if (a3818_hbt_tid >= 0)
    {
        sl_deq_tout(a3818_hbt_tid); a3818_hbt_tid = -1;
    }
#endif /* USE_THREAD_FOR_IRQ */
}

static int  vme_hal_open_bus (int bus_major, int bus_minor)
{
  int                   idx;
  int                   first_free_idx;
  int                   bus_handle;

  a3818_hal_bus_info_t *me;

  CVBoardTypes          BdType;
  short                 Link;
  short                 BdNum;
  CVErrorCodes          err;

#if   USE_THREAD_FOR_IRQ
  int                   saved_errno;
#elif USE_SELECT_FOR_IRQ
  int                   saved_errno;
#endif /* USE_THREAD_FOR_IRQ */

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

    me = a3818_hal_bus_info + bus_handle;
    bzero(me, sizeof(*me));
    me->in_use = 1;
    me->bus_major = bus_major;
    me->bus_minor = bus_minor;

    BdType = cvV2718;
    Link   = bus_major;
    BdNum  = bus_minor;
    if (bus_major >= 10000)
    {
        BdType = (bus_major - 10000) / 1000;
        Link   = (bus_major - 10000) % 1000;
    }
////    fprintf(stderr, "@%d/%d: %d/%d,%d\n", bus_major, bus_minor, BdType, Link, BdNum);

fprintf(stderr, "...CAENVME_Init(BdType=%d, Link=%d, BdNum=%d) start\n", BdType, Link, BdNum);
    err = CAENVME_Init(BdType, Link, BdNum, &(me->CAENVMElib_handle));
fprintf(stderr, "...CAENVME_Init() finished err=%d handle=%d\n", err, me->CAENVMElib_handle);
    if (err != cvSuccess)
    {
        errno = 0; // Signifies that vme_hal_strerror(ret) should be used instead of strerror(errno)
        me->in_use = 0;
        return err;
    }
////fprintf(stderr, "a3818_hal_bus_info[bus_handle].CAENVMElib_handle=%d\n", a3818_hal_bus_info[bus_handle].CAENVMElib_handle);
#if USE_SELECT_FOR_IRQ
    err = CAENVME_IRQEnable(me->CAENVMElib_handle, 0x7F);
    ////fprintf(stderr, "\t####CAENVME_IRQEnable()=%d\n", err);
#endif
#if   USE_THREAD_FOR_IRQ
    // Initialize values for cleanup
    me->irq_rfdh               = -1;
    me->irq_pipe[PIPE_RD_SIDE] = -1;
    me->irq_pipe[PIPE_WR_SIDE] = -1;
    // Get a separate handle for IRQs
    err = CAENVME_Init(BdType, Link, BdNum, &(me->CAENVMElib_IRQ_handle));
fprintf(stderr, "Init(IRQ_handle)=%d\n", err);
    if (err != cvSuccess)
    {
        CAENVME_End(me->CAENVMElib_handle);
        errno = 0; // Signifies that vme_hal_strerror(ret) should be used instead of strerror(errno)
        me->in_use = 0;
        return err;
    }
    // Create a messaging pipe...
    if (pipe(me->irq_pipe) < 0)
        goto ERREXIT;
    /* Note: we mark only the RD side of the pipe as non-blockable,
             explicitly putting the WR side to a blocking mode,
             thus throttling the IRQ polling rate to what is processable. */
    set_fd_flags(me->irq_pipe[PIPE_RD_SIDE], O_NONBLOCK, 1);
    set_fd_flags(me->irq_pipe[PIPE_WR_SIDE], O_NONBLOCK, 0);
    // ...and register it with cxscheduler
    if ((me->irq_rfdh = sl_add_fd(0/*!!!uniq!!!*/, NULL,
                                  me->irq_pipe[PIPE_RD_SIDE], SL_RD, irq2pipe_proc, me)) < 0)
        goto ERREXIT;
    // Finally, create an IRQ-waiting-thread
    if (pthread_create(&(me->irq_thread), NULL, irq_wait_proc, me) != 0)
        goto ERREXIT;
#elif USE_SELECT_FOR_IRQ
    me->handle_fdhandle = -1;  // For ERREXIT cleanup only
    me->handle_fd       = CAENVME_GetFiledes(me->CAENVMElib_handle);
    if (me->handle_fd < 0)
    {
        saved_errno = errno;
        fprintf(stderr, "%s::%s(%d,%d): CAENVME_GetFiledes()=%d\n", __FILE__, __FUNCTION__, bus_major, bus_minor, me->handle_fd);
        errno = saved_errno;
        goto ERREXIT;
    }
    set_fd_flags(me->handle_fd, O_NONBLOCK, 1);
    if ((me->handle_fdhandle = sl_add_fd(0/*!!!uniq!!!*/, NULL,
                                         me->handle_fd, SL_EX, irq_fd_p, me)) < 0)
    {
        saved_errno = errno;
        fprintf(stderr, "%s::%s(%d,%d): sl_add_fd()=%d errno=%d/\"%s\"\n", __FILE__, __FUNCTION__, bus_major, bus_minor, me->handle_fd, errno, strerror(errno));
        errno = saved_errno;
        goto ERREXIT;
    }
#endif /* USE_THREAD_FOR_IRQ */

    return bus_handle;

#if USE_THREAD_FOR_IRQ
 ERREXIT:
    saved_errno = errno;
    CAENVME_End(me->CAENVMElib_IRQ_handle);
    CAENVME_End(me->CAENVMElib_handle);
    if (me->irq_rfdh >= 0)               {sl_del_fd(me->irq_rfdh);           me->irq_rfdh               = -1;}
    if (me->irq_pipe[PIPE_RD_SIDE] >= 0) {close(me->irq_pipe[PIPE_RD_SIDE]); me->irq_pipe[PIPE_RD_SIDE] = -1;}
    if (me->irq_pipe[PIPE_WR_SIDE] >= 0) {close(me->irq_pipe[PIPE_WR_SIDE]); me->irq_pipe[PIPE_WR_SIDE] = -1;}
    me->in_use = 0;
    errno = saved_errno;
    return -1;
#endif /* USE_THREAD_FOR_IRQ */
#if USE_SELECT_FOR_IRQ
 ERREXIT:
    saved_errno = errno;
    CAENVME_End(me->CAENVMElib_handle);
    if (me->handle_fdhandle >= 0) {sl_del_fd(me->handle_fdhandle); me->handle_fdhandle = -1;}
    me->in_use = 0;
    errno = saved_errno;
    return -1;
#endif /* USE_SELECT_FOR_IRQ */
}

static int  vme_hal_close_bus(int bus_handle)
{
  a3818_hal_bus_info_t *me = a3818_hal_bus_info + bus_handle;

    if (bus_handle < A3818_BUS_HANDLE_FIRST  ||
        bus_handle >= countof(a3818_hal_bus_info))
    {
        errno = EINVAL;
        return -1;
    }

    if (me->in_use == 0)
    {
        errno = EBADF;
        return -1;
    }

#if   USE_THREAD_FOR_IRQ
    pthread_cancel(me->irq_thread);
    pthread_join  (me->irq_thread, NULL);
    sl_del_fd(me->irq_rfdh);
    close(me->irq_pipe[PIPE_RD_SIDE]);
    close(me->irq_pipe[PIPE_WR_SIDE]);
    CAENVME_End(me->CAENVMElib_IRQ_handle);
#elif USE_SELECT_FOR_IRQ
    sl_del_fd(me->handle_fdhandle);  me->handle_fdhandle = -1;
    // Note: should NOT do "close(me->handle_fd)", because that fd is in CAENVMElib's responsibility and is taken care of by CAENVME_End()
#endif /* USE_THREAD_FOR_IRQ */
    CAENVME_End(me->CAENVMElib_handle);
    me->in_use = 0;

    return 0;
}

static int  vme_hal_open_irq (int bus_handle, void *privptr,
                              int irq_n, vme_hal_irq_cb_t cb, int uniq)
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

    // IRQ line sanity
    if (irq_n < 0  ||  irq_n > 7)
    {
        errno = EINVAL;
        return -1;
    }

    // Is this IRQ already busy?
    if ((a3818_hal_bus_info[bus_handle].irq_mask & (1 << irq_n)) != 0)
    {
        errno = EBUSY;
        return -1;
    }

    // Okay, let's register it
    a3818_hal_bus_info[bus_handle].irq_mask |=  (1 << irq_n);
    a3818_hal_bus_info[bus_handle].irq_info[irq_n].cb      = cb;
    a3818_hal_bus_info[bus_handle].irq_info[irq_n].privptr = privptr;

    return 0;
}

static int  vme_hal_close_irq(int bus_handle __attribute__((unused)),
                              int irq_n)
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

    // IRQ line sanity
    if (irq_n < 0  ||  irq_n > 7)
    {
        errno = EINVAL;
        return -1;
    }

    // Is this IRQ already unused?
    if ((a3818_hal_bus_info[bus_handle].irq_mask & (1 << irq_n)) == 0)
    {
        errno = ESRCH; // An almost arbitrary code, to distinguish from EBADF above
        return -1;
    }

    // Okay, let's forget it
    a3818_hal_bus_info[bus_handle].irq_mask &=~ (1 << irq_n);

    return 0;
}


#ifndef DEBUG_A3818_HAL_IO
  #define DEBUG_A3818_HAL_IO 1
#endif
#if DEBUG_A3818_HAL_IO
  #define A3818_HAL_IO_BEG  int r = 
  #define A3818_HAL_IO_END  if (r < 0) fprintf(stderr, "%s(0x%02x:%08x)=%d\n", __FUNCTION__, am, addr, r); \
                            return r;
#else
  #define A3818_HAL_IO_BEG  return
  #define A3818_HAL_IO_END
#endif
#define VME_HAL_DEFINE_IO(AS, name, TS)                                          \
VME_HAL_STORAGE_CLASS int vme_hal_a##AS##wr##TS   (int bus_handle,               \
                                                   int am,                       \
                                                   uint32 addr, uint##TS  value) \
{                                                                                \
    A3818_HAL_IO_BEG                                                             \
           CAENVME_WriteCycle   (a3818_hal_bus_info[bus_handle].CAENVMElib_handle, \
                                 addr, &value,                                   \
                                 am, __CX_CONCATENATE(cvD,TS));                  \
    A3818_HAL_IO_END;                                                            \
}                                                                                \
                                                                                 \
VME_HAL_STORAGE_CLASS int vme_hal_a##AS##rd##TS   (int bus_handle,               \
                                                   int am,                       \
                                                   uint32 addr, uint##TS *val_p) \
{                                                                                \
    A3818_HAL_IO_BEG                                                             \
           CAENVME_ReadCycle    (a3818_hal_bus_info[bus_handle].CAENVMElib_handle, \
                                 addr, val_p,                                    \
                                 am, __CX_CONCATENATE(cvD,TS));                  \
    A3818_HAL_IO_END;                                                            \
}                                                                                \
VME_HAL_STORAGE_CLASS int vme_hal_a##AS##wr##TS##v(int bus_handle,               \
                                                   int am,                       \
                                                   uint32 addr, uint##TS *data, int count) \
{                                                                                \
  int  unused_count_bytes_transferred;                                           \
                                                                                 \
    A3818_HAL_IO_BEG                                                             \
           CAENVME_BLTWriteCycle(a3818_hal_bus_info[bus_handle].CAENVMElib_handle, \
                                 addr, data,                                     \
                                 count * sizeof(uint##TS),                       \
                          /* Note: "| 2" is valid for A16/A24/A32/A64(?) only, NOT for A40 (cvA40=0x34, cvA40_BLT=0x37) */ \
                                 am | 2, __CX_CONCATENATE(cvD,TS),               \
                                 &unused_count_bytes_transferred);               \
    A3818_HAL_IO_END;                                                            \
}                                                                                \
                                                                                 \
VME_HAL_STORAGE_CLASS int vme_hal_a##AS##rd##TS##v(int bus_handle,               \
                                                   int am,                       \
                                                   uint32 addr, uint##TS *data, int count) \
{                                                                                \
  int  unused_count_bytes_transferred;                                           \
                                                                                 \
    A3818_HAL_IO_BEG                                                             \
           CAENVME_BLTReadCycle (a3818_hal_bus_info[bus_handle].CAENVMElib_handle, \
                                 addr, data,                                     \
                                 count * sizeof(uint##TS),                       \
                          /* Note: "| 2" is valid for A16/A24/A32/A64(?) only, NOT for A40 (cvA40=0x34, cvA40_BLT=0x37) */ \
                                 am | 2, __CX_CONCATENATE(cvD,TS),               \
                                 &unused_count_bytes_transferred);               \
    A3818_HAL_IO_END;                                                            \
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
