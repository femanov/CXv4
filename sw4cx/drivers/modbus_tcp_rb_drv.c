#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include <limits.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fix_arpa_inet.h>

#include "cx_sysdeps.h"
#include "timeval_utils.h"

#include "cxsd_driver.h"

#include "drv_i/modbus_tcp_rb_drv_i.h"

//// Device specifications ///////////////////////////////////////////

enum {MODBUS_TCP_PORT = 502};

//// Driver definitions //////////////////////////////////////////////

enum
{
    RECONNECT_DELAY  =  10*1000*1000, /* 10 seconds */
    HEARTBEAT_PERIOD =   1*1000*1000/5, /* 1 second, 1 Hz */
};

typedef struct
{
    int                  devid;
    int                  fd;
    fdio_handle_t        iohandle;
    sl_tid_t             rcn_tid;

    int                  is_ready;
    int                  is_reconnecting;
    int                  was_success;

    unsigned long        hostaddr;
} privrec_t;

//// ... /////////////////////////////////////////////////////////////

static void CleanupFDIO(int devid, privrec_t *me)
{
    if (me->iohandle >= 0)
    {
        fdio_deregister(me->iohandle);
        me->iohandle = -1;
    }
    if (me->fd     >= 0)
    {
        close(me->fd);
        me->fd       = -1;
    }
}


static void CommitSuicide(int devid, privrec_t *me,
                          const char *function, const char *reason)
{
    DoDriverLog(devid, DRIVERLOG_CRIT, "%s: FATAL: %s", function, reason);
    SetDevState(devid, DEVSTATE_OFFLINE, CXRF_DRV_PROBL, reason);
}

static void ReportConnfail(int devid, privrec_t *me, const char *function)
{
    if (!me->is_reconnecting)
        DoDriverLog(devid, DRIVERLOG_NOTICE | DRIVERLOG_ERRNO,
                    "%s: connect()", function);
}

static void ReportConnect (int devid, privrec_t *me)
{
    if (me->is_reconnecting)
        DoDriverLog(devid, DRIVERLOG_NOTICE,
                    "Successfully %sconnected",
                    me->was_success? "re-" : "");
}

//////////////////////////////////////////////////////////////////////
static int  InitiateStart     (int devid, privrec_t *me);
static void InitializeRemote  (int devid, privrec_t *me);
static void ProcessInData     (int devid, privrec_t *me,
                               uint8 *pkt, size_t inpktsize);
//////////////////////////////////////////////////////////////////////

static void TryToReconnect(int devid, void *devptr,
                           sl_tid_t tid,
                           void *privptr)
{
  privrec_t  *me  = (privrec_t *)devptr;
  int         r;
  
    me->rcn_tid = -1;
    r = InitiateStart(devid, me);
    if (r < 0)
        CommitSuicide(devid, me, NULL, NULL);
}
static void ScheduleReconnect(int devid, privrec_t *me, const char *reason)
{
    /* Perform cleanup and notify server of our problem */
    CleanupFDIO(devid, me);
    SetDevState(devid, DEVSTATE_NOTREADY, CXRF_REM_C_PROBL, reason);

    /* And do schedule reconnect */
    me->is_ready        = 0;
    me->is_reconnecting = 1;
    if (me->rcn_tid >= 0) sl_deq_tout(me->rcn_tid);
    me->rcn_tid         = sl_enq_tout_after(devid, me, RECONNECT_DELAY,
                                            TryToReconnect, NULL);
}


static void ProcessIO(int devid, void *devptr,
                      fdio_handle_t iohandle __attribute__((unused)), int reason,
                      void *inpkt, size_t inpktsize,
                      void *privptr __attribute__((unused)))
{
  privrec_t           *me  = (privrec_t *)devptr;

////fprintf(stderr, "%s reason=%d inpktsize=%zd\n", __FUNCTION__, reason, inpktsize);
    switch (reason)
    {
        case FDIO_R_DATA:
            ProcessInData     (devid, me, inpkt, inpktsize);
            break;
            
        case FDIO_R_CONNECTED:
            DoDriverLog(devid, DRIVERLOG_INFO | DRIVERLOG_C_PKTINFO,
                        "FDIO_R_CONNECTED");
            InitializeRemote(devid, me);
            break;

        case FDIO_R_CONNERR:
            ReportConnfail(devid, me, __FUNCTION__);
            ScheduleReconnect(devid, me, "connection failure");
            break;

        case FDIO_R_CLOSED:
        case FDIO_R_IOERR:
            DoDriverLog(devid, DRIVERLOG_NOTICE | DRIVERLOG_ERRNO,
                        "%s: ERROR", __FUNCTION__);
            ScheduleReconnect(devid, me,
                              reason == FDIO_R_CLOSED? "connection closed"
                                                     : "I/O error");
            break;

        case FDIO_R_PROTOERR:
            CommitSuicide(devid, me, __FUNCTION__, "protocol error");
            break;

        case FDIO_R_INPKT2BIG:
            CommitSuicide(devid, me, __FUNCTION__, "received packet is too big");
            break;

        case FDIO_R_ENOMEM:
            CommitSuicide(devid, me, __FUNCTION__, "out of memory");
            break;

        default:
            DoDriverLog(devid, DRIVERLOG_CRIT,
                        "%s: unknown fdiolib reason %d",
                         __FUNCTION__, reason);
            CommitSuicide(devid, me, __FUNCTION__, "unknown fdiolib reason");
    }
}


static int  RegisterWithFDIO  (int devid, privrec_t *me, int fdtype)
{
    me->iohandle = fdio_register_fd(devid, me, me->fd, fdtype,
                                    ProcessIO, NULL,
                                    1024, // maxpktsize: in fact, is MUCH less (255 bytes?)
                                    6,    // hdr_size:   here is so to include the length field
                                    4,    // len_offset
                                    2,    // len_size
                                    FDIO_LEN_BIG_ENDIAN, // Sad but true: all Modbus 16-bit values are BIG endian
                                    1,    // len_units
                                    6);   // len_add:    equal to hdr_size
    if (me->iohandle < 0)
    {
        DoDriverLog(devid, DRIVERLOG_CRIT | DRIVERLOG_ERRNO,
                    "%s: FATAL: fdio_register_fd()", __FUNCTION__);
        return -CXRF_DRV_PROBL;
    }

    return 0;
}

static int  InitiateStart     (int devid, privrec_t *me)
{
  struct sockaddr_in  idst;
  socklen_t           addrlen;
  int                 r;
  int                 on    = 1;

    /* Create a socket */
    me->fd = socket(AF_INET, SOCK_STREAM, 0);
    if (me->fd < 0)
    {
        DoDriverLog(devid, DRIVERLOG_CRIT | DRIVERLOG_ERRNO,
                    "%s: FATAL: socket()", __FUNCTION__);
        return -CXRF_DRV_PROBL;
    }

    idst.sin_family = AF_INET;
    idst.sin_port   = htons(MODBUS_TCP_PORT);
    memcpy(&idst.sin_addr, &(me->hostaddr), sizeof(me->hostaddr));
    addrlen = sizeof(idst);

    /* Turn on TCP keepalives, ... */
    r = setsockopt(me->fd, SOL_SOCKET, SO_KEEPALIVE, &on, sizeof(on));
    /* ...switch to non-blocking mode... */
    r = set_fd_flags(me->fd, O_NONBLOCK, 1);
    /* ...and initiate connection */
    r = connect(me->fd, (struct sockaddr *)&idst, addrlen);
    DoDriverLog(devid, DRIVERLOG_INFO | DRIVERLOG_C_PKTINFO,
                "connect()=%d/%d", r, errno);
    if (r < 0)
    {
        if (errno == EINPROGRESS)
            ;
        else if (IS_A_TEMPORARY_CONNECT_ERROR())
        {
            ReportConnfail(devid, me, __FUNCTION__);
            ScheduleReconnect(devid, me, "connection failure");
            return 0;
        }
        else
        {
            DoDriverLog(devid, DRIVERLOG_CRIT | DRIVERLOG_ERRNO,
                        "%s: FATAL: connect()", __FUNCTION__);
            close(me->fd); me->fd = -1;
            return -CXRF_DRV_PROBL;
        }
    }

    /* Register with fdio */
    r = RegisterWithFDIO(devid, me, FDIO_CONNECTING);
    if (r < 0)
    {
        close(me->fd); me->fd = -1;
        return r;
    }

    /* Okay -- the connection will be completed later */
    return 0;
}

static void InitializeRemote  (int devid, privrec_t *me)
{
    /* Proclaim that we are ready */
    ReportConnect(devid, me);
    me->is_ready        = 1;
    me->is_reconnecting = 0;
    me->was_success     = 1;
    SetDevState(devid, DEVSTATE_OPERATING, 0, NULL);

    return;
    
 FAILURE:
    DoDriverLog(devid, DRIVERLOG_CRIT | DRIVERLOG_ERRNO, __FUNCTION__);
    ScheduleReconnect(devid, me, "remote initialization failure");
}

static void HeartBeat(int devid, void *devptr,
                      sl_tid_t tid,
                      void *privptr)
{
  privrec_t           *me = (privrec_t *) devptr;
  static const char    req[] = 
                           "\x55\xaa"   // Transaction identifier (Seq number)
                           "\x00\x00"   // Protocol identifier
                           "\x00\x06"   // Length of remaining data
                           "\x01"       // Device address (Unit identifier); that PLC requires 1 instead of standard 255
                           "\x03"       // Command: read multiple holding registers
                           "\x00\x01"   // First register address
                           "\x00\x05";//"\x00\x05";  // Number of registers

    if (me->is_ready)
    {
        DoDriverLog(devid, DRIVERLOG_INFO | DRIVERLOG_C_PKTINFO,
                    "%s() send", __FUNCTION__);
        if (fdio_send(me->iohandle, req, sizeof(req) - 1) < 0)
        {
            DoDriverLog(devid, 0 | DRIVERLOG_ERRNO,
                        "%s: fdio_send() failure; terminating and scheduling restart",
                        __FUNCTION__);
            ScheduleReconnect(devid, me, "PING failure");
            goto RE_ENQ;
        }
    }

 RE_ENQ:
    sl_enq_tout_after(devid, devptr, HEARTBEAT_PERIOD, HeartBeat, NULL);
}

static int modbus_tcp_rb_init_d(int devid, void *devptr,
                                int businfocount, int businfo[],
                                const char *auxinfo)
{
  privrec_t           *me = (privrec_t *) devptr;
  int                  r;
  struct hostent      *hp;

    me->devid    = devid;
    me->fd       = -1;
    me->iohandle = -1;
    me->rcn_tid  = -1;

    /* Find out IP of the host */
    
    /* First, is it in dot notation (aaa.bbb.ccc.ddd)? */
    me->hostaddr = inet_addr(auxinfo);
    
    /* No, should do a hostname lookup */
    if (me->hostaddr == INADDR_NONE)
    {
        hp = gethostbyname(auxinfo);
        /* No such host?! */
        if (hp == NULL)
        {
            DoDriverLog(devid, 0, "%s: unable to resolve name \"%s\"",
                        __FUNCTION__, auxinfo);
            return -CXRF_CFG_PROBL;
        }
        
        memcpy(&me->hostaddr, hp->h_addr, hp->h_length);
    }

    /* Try to start */
    r = InitiateStart(devid, me);
    if (r < 0)
        return r;

    SetChanReturnType(devid, 0, MODBUS_TCP_RB_NUMCHANS, IS_AUTOUPDATED_YES);

    HeartBeat(devid, me, -1, NULL);

    return DEVSTATE_NOTREADY;
}

static void modbus_tcp_rb_term_d(int devid, void *devptr)
{
  privrec_t  *me = (privrec_t *)devptr;

    // SendTerm(me)?
}

static void ProcessInData     (int devid, privrec_t *me,
                               uint8 *pkt, size_t inpktsize)
{
// 55 aa 00 00 00 0d 01 03  0a 00 00 84 e0 00 43 80  0c 00 df

  int        addrs [5*(1+16)];
  int32      values[5*(1+16)];
  void      *val_ps[5*(1+16)];
  cxdtype_t  dtypes[5*(1+16)];
  int        nelems[5*(1+16)];
  rflags_t   rflags[5*(1+16)];

  int        nc;
  int        nreg;
  int        nbit;
  uint16     regval;
  uint16     mask;

    DoDriverLog(devid, DRIVERLOG_INFO | DRIVERLOG_C_PKTINFO,
                "%s(%zd)", __FUNCTION__, inpktsize);

    if (inpktsize != 19) return;

    for (nc = 0;  nc < countof(val_ps);  nc++)
    {
        val_ps[nc] = values + nc;
        dtypes[nc] = CXDTYPE_INT32;
        nelems[nc] = 1;
        rflags[nc] = 0;
    }   

    nc = 0;
    for (nreg = 0;  nreg < 5;  nreg++)
    {
        regval = (pkt[9 + nreg * 2] << 8) +
                 (pkt[9 + nreg * 2 + 1]);
        addrs [nc] = MODBUS_TCP_RB_CHAN_REGS_base + nreg;
        values[nc] = regval;
        nc++;
        for (nbit = 0, mask = 1;  nbit <= 15  && 1;  nbit++, mask <<= 1, nc++)
        {
            addrs [nc] = MODBUS_TCP_RB_CHAN_BITS_base + nreg * 16 + nbit;
            values[nc] = (regval & mask) != 0;
        }
    }
    ReturnDataSet(devid, nc, addrs, dtypes, nelems, val_ps, rflags, NULL);
}


DEFINE_CXSD_DRIVER(modbus_tcp_rb, "VEPP-5 Modbus-TCP-connected Radiation-Bezopasnost driver",
                   NULL, NULL,
                   sizeof(privrec_t), NULL,
                   0, 0,
                   NULL, 0,
                   NULL,
                   -1, NULL, NULL,
                   modbus_tcp_rb_init_d, modbus_tcp_rb_term_d, NULL);
