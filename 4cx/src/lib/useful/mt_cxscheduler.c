#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
              
#include <pthread.h>

#include "cx_sysdeps.h"

#include "misc_macros.h"
#include "misclib.h"

#include "mt_cxscheduler.h"


enum
{   /* this enumeration according to "man 3 pipe" */
    PIPE_RD_SIDE = 0,
    PIPE_WR_SIDE = 1,
};

static int              mt_sl_initialized  = 0;
static int              mt_sl_wake_pipe[2] = {-1, -1};
static sl_fdh_t         mt_sl_wake_fdh;
static pthread_t        mt_sl_threadid;
static pthread_mutex_t  mt_sl_mutex;


#define DEBUG_LOCKS 0
#if DEBUG_LOCKS
  #define _GNU_SOURCE         /* See feature_test_macros(7) */
  #include <unistd.h>
  #include <sys/syscall.h>   /* For SYS_xxx definitions */
static int call_gettid(void)
{
    return syscall(SYS_gettid);
}
#if OPTION_HAS_PROGRAM_INVOCATION_NAME
  #define ARGV0_REF program_invocation_name
#else
  #define ARGV0_REF "mt_cxscheduler"
#endif /* OPTION_HAS_PROGRAM_INVOCATION_NAME */
#endif /* DEBUG_LOCKS */

static void do_lock  (void)
{
#if DEBUG_LOCKS
    if (pthread_mutex_trylock(&mt_sl_mutex) == 0) return;
    fprintf(stderr, "%s %s[%d] trylock fail in %d\n", strcurtime_msc(), ARGV0_REF, getpid(), call_gettid());
#endif
    pthread_mutex_lock  (&mt_sl_mutex);
}

static void do_unlock(void)
{
    pthread_mutex_unlock(&mt_sl_mutex);
}

static void pipe_proc(int uniq __attribute__((unused)), void *privptr1 __attribute__((unused)),
                      sl_fdh_t fdh __attribute__((unused)), int fd __attribute__((unused)), int mask __attribute__((unused)), void *privptr2 __attribute__((unused)))
{
  int   repcount;
  char  some_bytes[16];

    for (repcount = 30;  repcount > 0;  repcount--)
        if (read(mt_sl_wake_pipe[PIPE_RD_SIDE], some_bytes, sizeof(some_bytes)) <= 0) break;  
}
static void *mt_sl_thread_proc(void *arg __attribute__((unused)))
{
  static char  a_byte = 0;

#if DEBUG_LOCKS
    fprintf(stderr, "%s %s[%d] %s tid=%d\n", strcurtime_msc(), ARGV0_REF, getpid(), __FUNCTION__, call_gettid());
#endif

    // 1. Lock the mutex
    pthread_mutex_lock(&mt_sl_mutex);

    // 2. Inform starter thread that initialization is finished
    write(mt_sl_wake_pipe[PIPE_WR_SIDE], &a_byte, sizeof(a_byte));

    while (1) sl_main_loop();

    return NULL;
}
int  mt_sl_start (void)
{
  int     saved_errno;

  int     r;
  fd_set  fds;
  char    some_byte;

    if (mt_sl_initialized) return 0;

    // 1. Prepare a mutex
    if ((saved_errno = pthread_mutex_init(&mt_sl_mutex, NULL)) != 0)
    {
        errno = saved_errno;
        return -1;
    }

    // 2. Prepare wake-on-fdset-change pipe
    if (pipe(mt_sl_wake_pipe) < 0) return -1;
    set_fd_flags(mt_sl_wake_pipe[PIPE_RD_SIDE], O_NONBLOCK, 1);
    set_fd_flags(mt_sl_wake_pipe[PIPE_WR_SIDE], O_NONBLOCK, 1);
    if ((mt_sl_wake_fdh = sl_add_fd(0, NULL, mt_sl_wake_pipe[PIPE_RD_SIDE], SL_RD, pipe_proc, NULL)) < 0)
    {
        saved_errno = errno;
        close(mt_sl_wake_pipe[PIPE_RD_SIDE]);  mt_sl_wake_pipe[PIPE_RD_SIDE] = -1;
        close(mt_sl_wake_pipe[PIPE_WR_SIDE]);  mt_sl_wake_pipe[PIPE_WR_SIDE] = -1;
        errno = saved_errno;
        return -1;
    }

    // 3. Finally, start a thread for sl_main_loop()
#if DEBUG_LOCKS
    fprintf(stderr, "%s %s[%d] %s tid=%d\n", strcurtime_msc(), ARGV0_REF, getpid(), __FUNCTION__, call_gettid());
#endif
    if ((saved_errno = pthread_create(&mt_sl_threadid, NULL, mt_sl_thread_proc, NULL)) != 0)
    {
        sl_del_fd(mt_sl_wake_fdh);             mt_sl_wake_fdh                = -1;
        close(mt_sl_wake_pipe[PIPE_RD_SIDE]);  mt_sl_wake_pipe[PIPE_RD_SIDE] = -1;
        close(mt_sl_wake_pipe[PIPE_WR_SIDE]);  mt_sl_wake_pipe[PIPE_WR_SIDE] = -1;
        errno = saved_errno;
        return -1;
    }

    // 4. ...and wait for that thread to finish initialization
    while (1)
    {
        FD_ZERO(&fds);
        FD_SET(mt_sl_wake_pipe[PIPE_RD_SIDE], &fds);
        r = select(mt_sl_wake_pipe[PIPE_RD_SIDE] + 1, &fds, NULL, NULL, NULL);
        if (r < 0)
        {
            if (SHOULD_RESTART_SYSCALL()) continue;
            saved_errno = errno;
            sl_del_fd(mt_sl_wake_fdh);             mt_sl_wake_fdh                = -1;
            close(mt_sl_wake_pipe[PIPE_RD_SIDE]);  mt_sl_wake_pipe[PIPE_RD_SIDE] = -1;
            close(mt_sl_wake_pipe[PIPE_WR_SIDE]);  mt_sl_wake_pipe[PIPE_WR_SIDE] = -1;
            errno = saved_errno;
            return -1;
        }
        read(mt_sl_wake_pipe[PIPE_RD_SIDE], &some_byte, 1);
        break;
    }

    mt_sl_initialized = 1;

    return 0;
}

static void hook_before_select(void)
{
    do_unlock();
}

static void hook_after_select (void)
{
    do_lock();
}

static void hook_fdset_change(void)
{
  static char  a_byte = 0;

    write(mt_sl_wake_pipe[PIPE_WR_SIDE], &a_byte, sizeof(a_byte));
}

void mt_sl_lock  (void)
{
    if (!pthread_equal(pthread_self(), mt_sl_threadid)) do_lock();
}

void mt_sl_unlock(void)
{
    if (!pthread_equal(pthread_self(), mt_sl_threadid)) do_unlock();
}


#define CXSCHEDULER_HOOK_BEFORE_SELECT hook_before_select
#define CXSCHEDULER_HOOK_AFTER_SELECT  hook_after_select
#define CXSCHEDULER_HOOK_FDSET_CHANGE  hook_fdset_change


#include "cxscheduler.c"
