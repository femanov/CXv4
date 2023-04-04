#include "cxscheduler.h"

#include "cxsd_hw.h"
#include "cxsd_hwP.h"
#include "cxsd_frontend.h"
#include "cxsd_modmgr.h"

#include "cxsd_sfi.h"
#include "cxsd_fe_epics_cas.h"

static sl_tid_t  fdManager_process_polling_tid = -1;
static void call_fdManager_process(int uniq, void *unsdptr, sl_tid_t tid, void *privptr)
{
  int   repcount;
//fprintf(stderr, "%s ", strcurtime_msc());
#if 1
    for (repcount = 30;  repcount > 0;  repcount--)
#endif
    cxsd_fe_epics_cas_do_poll();
    fdManager_process_polling_tid = sl_enq_tout_after(0, NULL, 100000/*100ms<=>10Hz*/, call_fdManager_process, NULL);
}

static void test_fd7(int uniq, void *unsdptr, sl_tid_t tid, void *privptr)
{
  int  r;
    errno = 0;
    r = check_fd_state(7, 0);
    fprintf(stderr, "%s r=%d errno=%d/%s\n", __FUNCTION__, r, errno, strerror(errno));
    sl_enq_tout_after(0, NULL, 1000000, test_fd7, NULL);
}
static int  epics_init_f (int server_instance)
{
  int r;

//    test_fd7(0, NULL, 0, NULL);

    r = cxsd_fe_epics_cas_init_f(server_instance, cxsd_hw_numchans);
    if (r == 0) fdManager_process_polling_tid = sl_enq_tout_after(0, NULL, 1/*Schedule first poll right after start*/, call_fdManager_process, NULL);
    return r;
}

static void epics_term_f (void)
{
    if (fdManager_process_polling_tid >= 0) sl_deq_tout(fdManager_process_polling_tid); fdManager_process_polling_tid = -1;
    cxsd_fe_epics_cas_term_f();
    cxsd_sfi_term_f();
}

static void epics_begin_c(void)
{
    cxsd_sfi_request_subscription();
}

static void epics_end_c  (void)
{
}


static int  epics_init_m(void);
static void epics_term_m(void);

//////////////////////////////////////////////////////////////////////

DEFINE_CXSD_FRONTEND(zpics, "EPICS/CA (Channel Access) frontend for CXv4",
                     epics_init_m, epics_term_m,
                     epics_init_f, epics_term_f,
                     epics_begin_c, epics_end_c);

static int  epics_init_m(void)
{
    cxsd_sfi_set_mon_update(cxsd_fe_epics_cas_update);
    return CxsdRegisterFrontend(&(CXSD_FRONTEND_MODREC_NAME(zpics)));
}

static void epics_term_m(void)
{
    cxsd_sfi_set_mon_update(NULL);
    CxsdDeregisterFrontend     (&(CXSD_FRONTEND_MODREC_NAME(zpics)));
}
