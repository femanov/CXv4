#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>
#include <netdb.h>
#include <errno.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/utsname.h>
#include <sys/un.h>
#include <netinet/in.h>
#include "fix_arpa_inet.h"
#include <net/if.h>  // "struct ifreq" & Co.

#include "cx_sysdeps.h"
#include "misc_macros.h"
#include "misclib.h"
#include "cxscheduler.h"
#include "fdiolib.h"

#include "cx.h"
#include "cxlib.h"
#include "cx_proto_v4.h"

#include "cxlib_wait_procs.h"


/*####################################################################
#                                                                    #
#  Connection management                                             #
#                                                                    #
####################################################################*/

enum
{
    MAX_UNCONNECTED_TIME = 60,
    DEF_HEARTBEAT_TIME   = 60 * 5,
};

typedef enum
{
    CT_FREE = 0,        // Isn't initialized
    CT_ANY  = 1,        // Any type -- for CheckCd() only
    CT_DATA = 2,        // Data connection
    CT_SRCH = 3         // Srch resolver
} conntype_t;

typedef enum
{
    CS_OPEN_FRESH = 0,  // Just allocated
    CS_OPEN_RESOLVE,    // Resolving server name to address (not used)
    CS_OPEN_CONNECT,    // Performing connect()
    CS_OPEN_ENDIANID,   // Have sent CXT4_ENDIANID
    CS_OPEN_LOGIN,      // Have sent CXT4_LOGIN
    CS_OPEN_FAIL,       // ECONNREFUSED, or other error on initial handshake

    CS_CLOSED,          // Closed by server, but not yet by client
    CS_USABLE,          // The connection is full-operational if >=
    CS_READY,           // Is ready for I/O

    CS_CHUNKING,        // Is cx_begin()'d and is being stuffed with chunks
} connstate_t;

typedef struct
{
    int               in_use;
    connstate_t       state;
    int               isconnected;
    int               errcode;

    int               fd;
    fdio_handle_t     fhandle;
    sl_tid_t          clp_tid;
    sl_tid_t          hbt_tid;
    
    char              progname[40];
    char              username[40];
    char              srvrspec[64];       /* Holds a copy of server-spec */
    cx_notifier_t     notifier;
    int               uniq;
    void             *privptr1;
    void             *privptr2;

    uint32            ID;                 /* Identifier */
    uint32            Seq;                /* Sent packets seq. number */
    uint32            syncSeq;            /* Seq of last synchronous pkt */
    conntype_t        type;

    CxV4Header       *sendbuf;
    size_t            sendbufsize;
} v4conn_t;

//////////////////////////////////////////////////////////////////////

enum
{
    V4_MAX_CONNS = 1000,  // An arbitrary limit
    V4_ALLOC_INC = 2,     // Must be >1 (to provide growth from 0 to 2)
};

static v4conn_t *cx4conns_list        = NULL;
static int       cx4conns_list_allocd = 0;

// GetV4connSlot()
GENERIC_SLOTARRAY_DEFINE_GROWING(static, V4conn, v4conn_t,
                                 cx4conns, in_use, 0, 1,
                                 1, V4_ALLOC_INC, V4_MAX_CONNS,
                                 , , void)

static void RlsV4connSlot(int cd)
{
  v4conn_t  *cp  = AccessV4connSlot(cd);
  int        err = errno;        // To preserve errno
  
    if (cp->in_use == 0) return; /*!!! In fact, an internal error */
    
    cp->in_use = 0;
    
    ////////////
    if (cp->fhandle >= 0) fdio_deregister(cp->fhandle);
    if (cp->fd      >= 0) close          (cp->fd);
    if (cp->clp_tid >= 0) sl_deq_tout    (cp->clp_tid);
    if (cp->hbt_tid >= 0) sl_deq_tout    (cp->hbt_tid);

    ////////////
    safe_free(cp->sendbuf);
    
    errno = err;
}

static int CdIsValid(int cd)
{
    return cd >= 0  &&  cd < cx4conns_list_allocd  &&  cx4conns_list[cd].in_use != 0;
}

static inline int cp2cd(v4conn_t *cp)
{
    return cp - cx4conns_list;
}

static int CXT42CE(uint32 Type)
{
  int  n;
  
  static struct
  {
      uint32  Type;
      int     CE;
  } trn_table[] =
  {
      {CXT4_EBADRQC,        CEBADRQC},
      {CXT4_DISCONNECT,     CEKILLED},
      {CXT4_READY,          0},
      {CXT4_ACCESS_GRANTED, 0},
      {CXT4_ACCESS_DENIED,  CEACCESS},
      {CXT4_DIFFPROTO,      CEDIFFPROTO},
      {CXT4_TIMEOUT,        CESRVSTIMEOUT},
      {CXT4_MANY_CLIENTS,   CEMANYCLIENTS},
      {CXT4_EINVAL,         CEINVAL},
      {CXT4_ENOMEM,         CESRVNOMEM},
      {CXT4_EPKT2BIG,       CEPKT2BIGSRV},
      {CXT4_EINTERNAL,      CESRVINTERNAL},
      {0, 0},
  };

    
    for (n = 0;  ;  n++)
    {
        if (trn_table[n].Type == Type) return trn_table[n].CE;
        if (trn_table[n].Type == 0)    return CEUNKNOWN;
    }
}

/*
 *  CheckCd
 *      Checks if a connection is of given type and in a given state
 *
 *  Args:
 *      cd     connection descriptor to check
 *      type   required type; if ==CT_ANY, type check isn't performed
 *      state  required state; if cp->state == CS_CLOSED, 'State' is ignored
 *
 *  Result:
 *      0   connection is ok
 *      -1  something is wrong, 'Errno' contains a CExxx code
 */

static inline int CheckCd(int cd, conntype_t type, connstate_t state)
{
  v4conn_t *cp = AccessV4connSlot(cd);
    
    if (!CdIsValid(cd))
    {
        errno = CEBADC;
        return -1;
    }
    if (type != CT_ANY  &&  cp->type != type)
    {
        errno = CEINVCONN;
        return -1;
    }
    if (cp->state == CS_CLOSED)
    {
        errno = CECLOSED;
        return -1;
    }
    if (cp->state != state)
    {
        errno = CEBUSY;
        return -1;
    }
    return 0;
}

//////////////////////////////////////////////////////////////////////

/*
 *  StartNewPacket
 *      Initializes a packet
 *
 *  Args:
 *      cp    connection pointer
 *      Type  one of CXT4_xxx constants to assign a packet type to
 */

static inline void StartNewPacket(v4conn_t *cp, uint32 Type)
{
    bzero(cp->sendbuf, sizeof(CxV4Header));
    cp->sendbuf->Type = Type;

    /* Must also assign values to Seq and ID here */
    cp->sendbuf->ID  = cp->ID;
    cp->sendbuf->Seq = ++(cp->Seq); /* Increment first, than assign,
                                       so that cp->sendbuf->Seq == cp->Seq */
}

/*
 *  GrowSendBuf
 *      Calls a GrowBuf() to grow the cp->sendbuf up to
 *      sizeof(CxV4Header)+datasize.
 */

static inline int GrowSendBuf(v4conn_t *cp, size_t datasize)
{
    return GrowBuf((void *) &(cp->sendbuf),
                   &(cp->sendbufsize),
                   sizeof(CxV4Header) + datasize);
}

/*
 *  SendDataRequest
 *      Sends a (DATA_IO) request prepared in [cd]->sendbuf to server.
 *
 *  Args:
 *      cp  connection pointer
 *
 *  Result:
 *      0   success
 *      -1  failure; `errno' contains error code
 */

static int SendDataRequest(v4conn_t *cp)
{
  int             r;

    r = fdio_send(cp->fhandle, cp->sendbuf,
                  sizeof(CxV4Header) + cp->sendbuf->DataSize);
    if (r < 0)  return -1;

    return 0;
}

//////////////////////////////////////////////////////////////////////

static char cxlib_progname[40] = "";

static cxlib_report_logger_t cxlib_report_logger_hook = NULL;
cxlib_report_logger_t cxlib_set_report_logger(cxlib_report_logger_t logger_hook)
{
  cxlib_report_logger_t ret = cxlib_report_logger_hook;

    cxlib_report_logger_hook = logger_hook;

    return ret;
}
static void cxlib_report(v4conn_t *cp, const char *format, ...)
    __attribute__ ((format (printf, 2, 3)));
static void cxlib_report(v4conn_t *cp, const char *format, ...)
{
  va_list     ap;
  const char *pn;
  const char *sr;
  int         cd;
  
    va_start(ap, format);
#if 1
    pn = cxlib_progname;
    if (cp != NULL  &&  cp->progname[0] != '\0') pn = cp->progname;
    sr = NULL;
    if (cp != NULL  &&  cp->srvrspec[0] != '\0') sr = cp->srvrspec;
    cd = (cp == NULL)? -1 : cp2cd(cp);
    if (cxlib_report_logger_hook != NULL)
    {
        cxlib_report_logger_hook(cp == NULL? 0 : cp->uniq, cd, format, ap);
    }
    else
    {
        fprintf (stderr, "%s %s%scxlib",
                 strcurtime(), pn, pn[0] != '\0'? ": " : "");
        if (cp != NULL)
            fprintf(stderr, "[%d%s%s%s]", cd,
                    sr != NULL? ":\"" : "",
                    sr != NULL? sr    : "",
                    sr != NULL? "\""  : "");
        fprintf (stderr, ": ");
        vfprintf(stderr, format, ap);
        fprintf (stderr, "\n");
    }
#endif
    va_end(ap);
}

//////////////////////////////////////////////////////////////////////

static void async_CS_OPEN_CONNECT (v4conn_t *cp);
static void async_CS_OPEN_ENDIANID(v4conn_t *cp, CxV4Header *rcvd, size_t rcvdsize);
static void async_CS_OPEN_LOGIN   (v4conn_t *cp, CxV4Header *rcvd, size_t rcvdsize);

static void async_CXT4_DATA_IO    (v4conn_t *cp, CxV4Header *rcvd, size_t rcvdsize);


static inline void CallNotifier(v4conn_t *cp, int reason, void *info)
{
    if (cp->notifier != NULL)
        cp->notifier(cp->uniq, cp->privptr1,
                     cp2cd(cp), reason, info,
                     cp->privptr2);
}

static void MarkAsClosed(v4conn_t *cp, int errcode)
{
  int  reason;
    
    cp->state   = cp->isconnected? CS_CLOSED : CS_OPEN_FAIL;
    cp->errcode = errcode;

    if      (errcode == CEACCESS) reason = CAR_EACCESS;  // A separate check BEFORE !isconnected, for CXT4_ACCESS_DENIED to be treated specially and NOT lead to reconnect
    else if (!cp->isconnected)    reason = CAR_CONNFAIL;
    else if (errcode == CEKILLED) reason = CAR_KILLED;
    else                          reason = CAR_ERRCLOSE;

    if (cp->fhandle >= 0) fdio_deregister(cp->fhandle);  cp->fhandle = -1;
    if (cp->fd      >= 0) close          (cp->fd);       cp->fd      = -1;
    if (cp->clp_tid >= 0) sl_deq_tout    (cp->clp_tid);  cp->clp_tid = -1;
    if (cp->hbt_tid >= 0) sl_deq_tout    (cp->hbt_tid);  cp->hbt_tid = -1;

    errno = errcode;
    CallNotifier(cp, reason, NULL);
    _cxlib_break_wait();
}

static void ProcessFdioEvent(int uniq, void *unsdptr,
                             fdio_handle_t handle, int reason,
                             void *inpkt, size_t inpktsize,
                             void *privptr)
{
  int         cd  = ptr2lint(privptr);
  v4conn_t   *cp  = AccessV4connSlot(cd);
  CxV4Header *hdr = inpkt;
  
    switch (reason)
    {
        case FDIO_R_DATA:
            switch (hdr->Type)
            {
                case CXT4_EBADRQC:
                case CXT4_DISCONNECT:
                case CXT4_ACCESS_DENIED:
                case CXT4_DIFFPROTO:
                case CXT4_TIMEOUT:
                case CXT4_MANY_CLIENTS:
                case CXT4_EINVAL:
                case CXT4_ENOMEM:
                case CXT4_EPKT2BIG:
                case CXT4_EINTERNAL:
                    MarkAsClosed(cp, CXT42CE(hdr->Type));
                    break;

                case CXT4_PING:
                    /* Now requires nothing */;
                    break;
                case CXT4_PONG:
                    /* requires no reaction */;
                    /*!!! In fact, never used */
                    break;

                case CXT4_MUSTER:
                    CallNotifier(cp, CAR_MUSTER, NULL);
                    break;

                case CXT4_END_OF_CYCLE:
                    //cxlib_report(cp, "EOC");
                    CallNotifier(cp, CAR_CYCLE, NULL);
                    break;

                case CXT4_DATA_IO:
                case CXT4_DATA_MONITOR:
                    ////fprintf(stderr, "DATA_IO nc=%d\n", hdr->NumChunks);
                    async_CXT4_DATA_IO(cp, hdr, inpktsize);
                    break;

                default:
                    switch (cp->state)
                    {
                        case CS_OPEN_ENDIANID: async_CS_OPEN_ENDIANID(cp, hdr, inpktsize); break;
                        case CS_OPEN_LOGIN:    async_CS_OPEN_LOGIN   (cp, hdr, inpktsize); break;
                        
                        default:
                            cxlib_report(cp, "%s: unexpected packet of type %d arrived when in state %d",
                                         __FUNCTION__, hdr->Type, cp->state);
                    }
            }
            break;
      
        case FDIO_R_CONNECTED:
            async_CS_OPEN_CONNECT(cp);
            break;

        case FDIO_R_CONNERR:
            MarkAsClosed(cp, fdio_lasterr(handle));
            break;

        case FDIO_R_CLOSED:
            MarkAsClosed(cp, CESOCKCLOSED);
            break;

        case FDIO_R_IOERR:
            MarkAsClosed(cp, fdio_lasterr(handle));
            break;

        case FDIO_R_PROTOERR:
            /* In fact, as of 10.09.2009, FDIO_R_PROTOERR is unused by fdiolib */
            MarkAsClosed(cp, fdio_lasterr(handle)); /*!!! We should introduce CEPROTOERR */
            break;

        case FDIO_R_INPKT2BIG:
            MarkAsClosed(cp, CERCV2BIG);
            break;

        case FDIO_R_ENOMEM:
            MarkAsClosed(cp, ENOMEM);
            break;

        default:
            cxlib_report(cp, "%s: unknown fdiolib reason %d",
                         __FUNCTION__, reason);
            MarkAsClosed(cp, CEINTERNAL);
    }
}


//////////////////////////////////////////////////////////////////////
static int create_a_cd(int            uniq,     void *privptr1,
                       const char    *spec,     int flags,
                       const char    *argv0,    const char *username,
                       cx_notifier_t  notifier, void *privptr2,
                       conntype_t     type,     size_t INIT_SENDBUFSIZE)
{
  int                 cd;
  v4conn_t           *cp;

  const char         *shrtname;

    /* Sanitize supplied credentials */
    /* User name... */
    if (username == NULL  ||  username[0] == '\0')
        username = getenv("LOGNAME");
    if (username == NULL)
        username = "";
    /* ...and program name */
    shrtname = "???";
    if (argv0 != NULL  &&  argv0[0] != '\0')
    {
        shrtname = strrchr(argv0, '/');
        if (shrtname == NULL) shrtname = argv0;
    }
    /* With GNU libc+ld we can determine the true argv[0] */
#if OPTION_HAS_PROGRAM_INVOCATION_NAME
    shrtname = program_invocation_short_name;
#endif /* OPTION_HAS_PROGRAM_INVOCATION_NAME */
    if (cxlib_progname[0] == '\0')
        strzcpy(cxlib_progname, shrtname, sizeof(cxlib_progname));
    
    /* Allocate a free slot... */
    if ((cd = GetV4connSlot()) < 0) return cd;

    /* ...and fill it with data */
    cp = AccessV4connSlot(cd);
    cp->state    = CS_OPEN_FRESH;
    cp->fd       = -1;
    cp->fhandle  = -1;
    cp->clp_tid  = -1;
    cp->hbt_tid  = -1;
    strzcpy(cp->progname, shrtname, sizeof(cp->progname));
    strzcpy(cp->username, username, sizeof(cp->username));
    cp->notifier = notifier;
    cp->uniq     = uniq;
    cp->privptr1 = privptr1;
    cp->privptr2 = privptr2;
    cp->type     = type;

    /* Store server-spec for future debug use */
    if (spec != NULL) strzcpy(cp->srvrspec, spec, sizeof(cp->srvrspec));

    /* Alloc buffer(s) */
    cp->sendbufsize = INIT_SENDBUFSIZE;
    cp->sendbuf     = malloc(cp->sendbufsize);
    if (cp->sendbuf == NULL)
    {
        RlsV4connSlot(cd);
        return -1;
    }

    return cd;
}
//////////////////////////////////////////////////////////////////////

static int GetSrvSpecParts(const char *spec,
                           char       *hostbuf, size_t hostbufsize,
                           int        *number)
{
  const char *colonp;
  char       *endptr;
  size_t      len;

    /* No host specified?  Try CX_SERVER variable */
    if (spec == NULL  ||  *spec == '\0') spec = getenv("CX_SERVER");
    /* Treat missing spec as localhost */
    if (spec == NULL  ||  *spec == '\0') spec = "localhost";

    colonp = strchr(spec, ':');
    if (colonp != NULL)
    {
        *number = (int)(strtol(colonp + 1, &endptr, 10));
        if (endptr == colonp + 1  ||  *endptr != '\0')
        {
            cxlib_report(NULL, "invalid server specification \"%s\"", spec);
            errno = CEINVAL;
            return -1;
        }
        if (*number > CX_MAX_SERVER)
        {
            cxlib_report(NULL, "server # in specification \"%s\" is out of range [0..%d]",
                         spec, CX_MAX_SERVER);
            errno = CEINVAL;
            return -1;
        }
        len = colonp - spec;
    }
    else
    {
        *number = 0;
        len = strlen(spec);
    }

    if (len > hostbufsize - 1) len = hostbufsize - 1;
    memcpy(hostbuf, spec, len);
    hostbuf[len] = '\0';

    if (hostbuf[0] == '\0') /* Local ":N" spec? */
        strzcpy(hostbuf, "localhost", hostbufsize);

    return 0;
}

int  cx_open  (int            uniq,     void *privptr1,
               const char    *spec,     int flags,
               const char    *argv0,    const char *username,
               cx_notifier_t  notifier, void *privptr2)
{
  int                 cd;
  v4conn_t           *cp;
  int                 s;
  int                 r;

  char                hostname[256];
  int                 srv_n;
  int                 srv_port;

  int                 fastest;
  struct utsname      uname_buf;
  struct hostent     *hp;

  const char         *shrtname;
  
  in_addr_t           thishost;
  in_addr_t           loclhost;
  in_addr_t           spechost;

  struct sockaddr_un  udst;
  struct sockaddr_in  idst;
  struct sockaddr    *serv_addr;
  int                 addrlen;

  int                 on    = 1;
  size_t              bsize = CX_V4_MAX_PKTSIZE;

  enum {INIT_SENDBUFSIZE = sizeof(CxV4Header) + 1024 /* arbitrary value */};

    /* Parse & check spec first */
    if (GetSrvSpecParts(spec, hostname, sizeof(hostname), &srv_n) != 0)
        return -1;
    if (srv_n >= 0)
    {
        srv_port = CX_V4_INET_PORT + srv_n;
        fastest  = 1;
    }
    else
    {
        srv_port = -srv_n;
        fastest  = 0;
    }

#if 1
    if ((cd = create_a_cd(uniq,     privptr1,
                          spec,     flags,
                          argv0,    username,
                          notifier, privptr2,
                          CT_DATA,  INIT_SENDBUFSIZE)) < 0) return cd;
    cp = AccessV4connSlot(cd);
#else
    /* Sanitize supplied credentials */
    /* User name... */
    if (username == NULL  ||  username[0] == '\0')
        username = getenv("LOGNAME");
    /* ...and program name */
    shrtname = "???";
    if (argv0 != NULL  &&  argv0[0] != '\0')
    {
        shrtname = strrchr(argv0, '/');
        if (shrtname == NULL) shrtname = argv0;
    }
    /* With GNU libc+ld we can determine the true argv[0] */
#if OPTION_HAS_PROGRAM_INVOCATION_NAME
    shrtname = program_invocation_short_name;
#endif /* OPTION_HAS_PROGRAM_INVOCATION_NAME */
    if (cxlib_progname[0] == '\0')
        strzcpy(cxlib_progname, shrtname, sizeof(cxlib_progname));
    
    /* Allocate a free slot... */
    if ((cd = GetV4connSlot()) < 0) return cd;

    /* ...and fill it with data */
    cp = AccessV4connSlot(cd);
    cp->state    = CS_OPEN_FRESH;
    cp->fd       = -1;
    cp->fhandle  = -1;
    cp->clp_tid  = -1;
    cp->hbt_tid  = -1;
    strzcpy(cp->progname, shrtname, sizeof(cp->progname));
    strzcpy(cp->username, username, sizeof(cp->username));
    cp->notifier = notifier;
    cp->uniq     = uniq;
    cp->privptr1 = privptr1;
    cp->privptr2 = privptr2;

    /* Store server-spec for future debug use */
    if (spec != NULL) strzcpy(cp->srvrspec, spec, sizeof(cp->srvrspec));

    /* Alloc buffer(s) */
    cp->sendbufsize = INIT_SENDBUFSIZE;
    cp->sendbuf     = malloc(cp->sendbufsize);
    if (cp->sendbuf == NULL)
        goto CLEANUP_ON_ERROR;
#endif

    /* Resolve host:N */
    cp->state    = CS_OPEN_RESOLVE;
    /* Note:
     All the code from this point and up to switch to CS_OPEN_CONNECT
     could be moved to asynchronous processing if there was a way to perform
     resolving asynchronously
     */

    /* Find out IP of the specified host */
    /* First, is it in dot notation (aaa.bbb.ccc.ddd)? */
    spechost = inet_addr(hostname);
    /* No, should do a hostname lookup */
    if (spechost == INADDR_NONE)
    {
        hp = gethostbyname(hostname);
        /* No such host?! */
        if (hp == NULL)
        {
            errno = CENOHOST;
            goto CLEANUP_ON_ERROR;
        }

        memcpy(&spechost, hp->h_addr, hp->h_length);
    }

    /* Find out IP of this host (if required) */
    thishost = INADDR_NONE;
    loclhost = inet_addr("127.0.0.1");
    if (fastest  &&  spechost != loclhost)
    {
        uname(&uname_buf);
        hp = gethostbyname(uname_buf.nodename);
        if (hp != NULL)
            memcpy(&thishost, hp->h_addr, hp->h_length);
    }

    /* Should connect via UNIX if possible or via INET otherwise */
    if (fastest  &&               /* May use AF_UNIX? */
        (
         spechost == loclhost  || /* Is it a localhost (127.0.0.1)? */
         spechost == thishost     /* Is it the same host? */
        )
       )
    /* Use AF_UNIX */
    {
        /* Create a socket */
        s = cp->fd = socket(AF_UNIX, SOCK_STREAM, 0);
        if (s < 0)  goto CLEANUP_ON_ERROR;

        /* Set addrlen::servaddr to the path */
        udst.sun_family = AF_UNIX;
        sprintf(udst.sun_path, CX_V4_UNIX_ADDR_FMT, srv_n);
        serv_addr = (struct sockaddr*) &udst;
        addrlen   = offsetof(struct sockaddr_un, sun_path) + strlen(udst.sun_path);
    }
    else
    /* Use AF_INET */
    {
        /* Create a socket */
        s = cp->fd = socket(AF_INET, SOCK_STREAM, 0);
        if (s < 0)  goto CLEANUP_ON_ERROR;

        /* Set addrlen::servaddr to host:port */
        idst.sin_family = AF_INET;
        idst.sin_port   = htons(srv_port);
        memcpy(&idst.sin_addr, &spechost, sizeof(spechost));
        serv_addr = (struct sockaddr *) &idst;
        addrlen   = sizeof(idst);

        /* For TCP, turn on keepalives */
        r = setsockopt(s, SOL_SOCKET, SO_KEEPALIVE, &on, sizeof(on));
    }

    /* Set Close-on-exec:=1 */
    fcntl(s, F_SETFD, 1);

    /* Set buffers */
    setsockopt(s, SOL_SOCKET, SO_SNDBUF, (char *) &bsize, sizeof(bsize));
    setsockopt(s, SOL_SOCKET, SO_RCVBUF, (char *) &bsize, sizeof(bsize));

    /* Initiate connect() */
    cp->state = CS_OPEN_CONNECT;

    set_fd_flags(s, O_NONBLOCK, 1);
    r = connect(s, serv_addr, addrlen);
    if (r < 0  &&  errno != EINPROGRESS) goto CLEANUP_ON_ERROR;
    
    /* Register with fdiolib */
    cp->fhandle = fdio_register_fd(uniq, NULL,
                                   s, FDIO_CONNECTING,
                                   ProcessFdioEvent, lint2ptr(cd),
                                   CX_V4_MAX_PKTSIZE,
                                   sizeof(CxV4Header),
                                   FDIO_OFFSET_OF(CxV4Header, DataSize),
                                   FDIO_SIZEOF   (CxV4Header, DataSize),
                                   BYTE_ORDER == LITTLE_ENDIAN? FDIO_LEN_LITTLE_ENDIAN
                                                              : FDIO_LEN_BIG_ENDIAN,
                                   1, sizeof(CxV4Header));
    if (cp->fhandle < 0) goto CLEANUP_ON_ERROR;

    /* Block for request completion, if sync mode */
    _cxlib_begin_wait();

    /* Is operation still in progress? */
    if (cp->state != CS_OPEN_FAIL  &&  cp->state != CS_CLOSED) return cd;
    
    /* No, obtain error code and fall through to cleanup */
    errno = cp->errcode;
    
 CLEANUP_ON_ERROR:
    if (errno == ENOENT) errno = ECONNREFUSED;
    RlsV4connSlot(cd);
    return -1;
}

int  cx_close (int  cd)
{
  v4conn_t *cp = AccessV4connSlot(cd);

  struct
  {
      CxV4Header    hdr;
  } pkt;
  
    if (!CdIsValid(cd))
    {
        errno = CEBADC;
        return -1;
    }

    /* Try to send a CXT_LOGOUT packet, if it has sense... */
    if (cp->isconnected  &&  cp->fhandle >= 0)
    {
        bzero(&pkt, sizeof(pkt));
        pkt.hdr.DataSize = sizeof(pkt) - sizeof(pkt.hdr);
        pkt.hdr.Type     = CXT4_LOGOUT;
        fdio_send(cp->fhandle, &pkt, sizeof(pkt));
        ////SleepBySelect(10000000);
        
        /* Do "preventive" closes */
        fdio_deregister_flush(cp->fhandle, 10/*!!!Should make an enum-constant*/);
        cp->fhandle = -1;
        cp->fd      = -1;
    }
    
    RlsV4connSlot(cd);

    return 0;
}


static int cleanup_checker(v4conn_t *cp, void *privptr)
{
  int  uniq = *((int *)privptr);

    if (cp->uniq == uniq)
        cx_close(cp2cd(cp));

    return 0;
}

void cx_do_cleanup(int uniq)
{
    ForeachV4connSlot(cleanup_checker, &uniq);
}

//////////////////////////////////////////////////////////////////////

static void DoHeartbeatPing(int uniq, void *unsdptr, sl_tid_t tid, void *privptr)
{
  int         cd  = ptr2lint(privptr);
  v4conn_t   *cp  = AccessV4connSlot(cd);
  CxV4Header  hdr;

    cp->hbt_tid = sl_enq_tout_after(cp->uniq, NULL,
                                    DEF_HEARTBEAT_TIME * 1000000,
                                    DoHeartbeatPing, lint2ptr(cp2cd(cp)));

    bzero(&hdr, sizeof(hdr));
    hdr.Type = CXT4_PING;
    hdr.ID   = cp->ID;

    if (fdio_send(cp->fhandle, &hdr, sizeof(hdr)) < 0)
        MarkAsClosed(cp, errno);
}

static void SetReady(v4conn_t *cp, uint32 ID)
{
    cp->state       = CS_READY;
    cp->isconnected = 1;
    cp->errcode     = 0;
    cp->ID          = ID;
    sl_deq_tout(cp->clp_tid);
    cp->clp_tid     = -1;

    cp->hbt_tid = sl_enq_tout_after(cp->uniq, NULL,
                                    DEF_HEARTBEAT_TIME * 1000000,
                                    DoHeartbeatPing, lint2ptr(cp2cd(cp)));

    CallNotifier(cp, CAR_CONNECT, NULL);
    _cxlib_break_wait();
}

static void WipeUnconnectedTimed(int uniq, void *unsdptr, sl_tid_t tid, void *privptr)
{
  int         cd  = ptr2lint(privptr);
  v4conn_t   *cp  = AccessV4connSlot(cd);

    cp->clp_tid = -1;
    MarkAsClosed(cp, CETIMEOUT);
}

static void async_CS_OPEN_CONNECT (v4conn_t *cp)
{
  struct
  {
      CxV4Header    hdr;
  } pkt;
  
    ////SleepBySelect(1000000);
    ////fprintf(stderr, "\nready_w=%d\n", check_fd_state(cp->fd, O_RDWR));
    bzero(&pkt, sizeof(pkt));
    pkt.hdr.DataSize = sizeof(pkt) - sizeof(pkt.hdr);
    pkt.hdr.Type     = CXT4_ENDIANID;
    pkt.hdr.var1     = CXV4_VAR1_ENDIANNESS_SIG;
    pkt.hdr.var2     = CX_V4_PROTO_VERSION;

    if (fdio_send(cp->fhandle, &pkt, sizeof(pkt)) < 0)
        MarkAsClosed(cp, errno);
    else
        cp->state = CS_OPEN_ENDIANID;

    cp->clp_tid = sl_enq_tout_after(cp->uniq, NULL,
                                    MAX_UNCONNECTED_TIME * 1000000,
                                    WipeUnconnectedTimed, lint2ptr(cp2cd(cp)));
}

static void async_CS_OPEN_ENDIANID(v4conn_t *cp, CxV4Header *rcvd, size_t rcvdsize)
{
  struct
  {
      CxV4Header      hdr;
      CxV4LoginChunk  lr;
  } pkt;
  
    if      (rcvd->Type != CXT4_LOGIN  &&  rcvd->Type != CXT4_ACCESS_GRANTED)
    {
        MarkAsClosed(cp, CXT42CE(rcvd->Type));
        return;
    }

    /* Note: the versions are "reversed" in check, since server's version
       must be same-or-higher than ours */
    if (!CX_VERSION_IS_COMPATIBLE(CX_V4_PROTO_VERSION, rcvd->var2))
    {
        MarkAsClosed(cp, CEDIFFPROTO);
        return;
    }

    if (rcvd->Type == CXT4_ACCESS_GRANTED)
    {
        SetReady(cp, rcvd->ID);
        return;
    }
    
    bzero(&pkt, sizeof(pkt));
    pkt.hdr.DataSize  = sizeof(pkt) - sizeof(pkt.hdr);
    pkt.hdr.Type      = CXT4_LOGIN;
    pkt.hdr.NumChunks = 1;

    pkt.lr.ck.OpCode   = 0; /*!!!*/
    pkt.lr.ck.ByteSize = sizeof(pkt.lr);
    pkt.lr.ck.rs1      = getpid ();
    pkt.lr.ck.rs2      = getppid();
    pkt.lr.ck.rs3      = getuid ();
    strzcpy(pkt.lr.progname, cp->progname, sizeof(pkt.lr.progname));
    strzcpy(pkt.lr.username, cp->username, sizeof(pkt.lr.username));

    if (fdio_send(cp->fhandle, &pkt, sizeof(pkt)) < 0)
        MarkAsClosed(cp, errno);
    else
        cp->state = CS_OPEN_LOGIN;
}

static void async_CS_OPEN_LOGIN   (v4conn_t *cp, CxV4Header *rcvd, size_t rcvdsize)
{
    if (rcvd->Type == CXT4_ACCESS_GRANTED)
    {
        SetReady(cp, rcvd->ID);
        return;
    }
    else
    {
        MarkAsClosed(cp, CXT42CE(rcvd->Type));
        return;
    }
}

//////////////////////////////////////////////////////////////////////

static void HandleSrchReply(int uniq, void *unsdptr,
                            fdio_handle_t handle, int reason,
                            void *inpkt, size_t inpktsize,
                            void *privptr)
{
  int         cd  = ptr2lint(privptr);
  v4conn_t   *cp; /* Note: NO initialization! Immediately before use only! */
  CxV4Header *hdr = inpkt;

  int        numcns;          // # of chunks in reply
  uint8     *ptr;
  CxV4Chunk *rpy;
  int        rpycn;           // RePlY chunk #
  size_t     rpycsize;        // RePlY chunk size in bytes
  size_t     rpylen;          // RePlY *data* LENgth in bytes
  size_t     rpy_total;       // TOTAL size of previous chunks in bytes

  struct sockaddr  addr;
  socklen_t        addrlen;

  char            srv_addr_buf[100];
  cx_srch_info_t  sri;

    if (reason != FDIO_R_DATA) return;

    /* Check packet Type, size, ...  */
    /* 1. Packet type */
    if (hdr->Type != CXT4_SEARCH_RESULT) return;
    /* 2. Packet size */
    /*    Note: no reason to check "inpktsize <= CX_V4_MAX_UDP_PKTSIZE",
                because that is not crucial */
    if (hdr->DataSize + sizeof(*hdr) != inpktsize)
    {
        return;
    }

    _cxlib_break_wait();

    addrlen = sizeof(addr);
    fdio_last_src_addr(handle, &addr, &addrlen);
    strzcpy(srv_addr_buf,
            inet_ntoa(((struct sockaddr_in *)(&addr))->sin_addr),
            sizeof(srv_addr_buf));

    numcns = hdr->NumChunks;

    /* Walk through the request */
    for (rpycn = 0, ptr = hdr->data, rpy_total = sizeof(CxV4Header);
         rpycn < numcns;
         rpycn++,   ptr += rpycsize, rpy_total += rpycsize)
    {
        /* Check if we MAY access chunk */
        if (rpy_total + sizeof(CxV4Chunk) > inpktsize)
        {
            return;
        }

        rpy = (void *)ptr;
        rpycsize = rpy->ByteSize;
        rpylen   = rpycsize - sizeof(CxV4Chunk);

        /* Check: */
        /* I. Size validity */
        /* 1. Is rpycsize sane at all? */
        if (rpycsize < sizeof(*rpy) + 1)
        {
            return;
        }
        /* 2. Does this chunk have appropriate alignment? */
        if (rpycsize != CXV4_CHUNK_CEIL(rpycsize))
        {
            return;
        }
        /* 3. Okay, does this chunk still fit into packet? */
        if (rpy_total + rpycsize > inpktsize)
        {
            return;
        }
        /* II. Name */
        /* 1. Is there a meaningful string
              ("2" means non-empty, for at least 1 char besides NUL) */
        if (rpylen < 2) continue;
        /* 2. Is the name NUL-terminated? */
        rpy->data[rpylen-1] = '\0';

        if (rpy->OpCode == CXC_CVT_TO_RPY(CXC_SEARCH))
        {
            sri.param1   = rpy->param1;
            sri.param2   = rpy->param2;
            sri.name     = rpy->data;
            sri.srv_addr = srv_addr_buf;
            sri.srv_n    = rpy->rs4;
            cp = AccessV4connSlot(cd);
            CallNotifier(cp, CAR_SRCH_RESULT, &sri);
        }
    }
}
int  cx_seeker(int            uniq,     void *privptr1,
               const char    *argv0,    const char *username,
               cx_notifier_t  notifier, void *privptr2)
{
  int                 cd;
  v4conn_t           *cp;
  int                 on = 1; /* "1" for BROADCAST */

    if ((cd = create_a_cd(uniq,     privptr1,
                          NULL,     0,
                          argv0,    username,
                          notifier, privptr2,
                          CT_SRCH,  CX_V4_MAX_UDP_PKTSIZE)) < 0) return cd;

    cp = AccessV4connSlot(cd);

    /* Create a socket */
    cp->fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (cp->fd < 0) goto CLEANUP_ON_ERROR;
    /* Set Close-on-exec:=1 */
    fcntl(cp->fd, F_SETFD, 1);
    /* Mark it as non-blocking */
    set_fd_flags(cp->fd, O_NONBLOCK, 1);
    /* ...and broadcasting */
    if (setsockopt(cp->fd, SOL_SOCKET, SO_BROADCAST, &on, sizeof(on)) < 0)
    {
        ////fprintf(stderr, "setsockopt(udp_socket, , SO_BROADCAST): %s\n", strerror(errno));
        goto CLEANUP_ON_ERROR;
    }

    /* Register with fdiolib */
    cp->fhandle = fdio_register_fd(uniq, NULL,
                                   cp->fd,          FDIO_DGRAM,
                                   HandleSrchReply, lint2ptr(cd),
                                   CX_V4_MAX_UDP_PKTSIZE,
                                   sizeof(CxV4Header), 0, 0,
                                   FDIO_LEN_LITTLE_ENDIAN, 0, 0);
    if (cp->fhandle < 0) goto CLEANUP_ON_ERROR;

    /* Everything is okay */
    cp->state       = CS_READY;
    cp->isconnected = 1;

    return cd;

 CLEANUP_ON_ERROR:
    RlsV4connSlot(cd);
    return -1;
}

int  cx_srch  (int cd, const char *name,      int  param1,  int  param2)
{
  v4conn_t       *cp = AccessV4connSlot(cd);
  int             r;
  size_t          bytesize;
  CxV4Chunk      *req;

    if ((r = CheckCd(cd, CT_SRCH, CS_CHUNKING)) != 0) return r;

    bytesize = CXV4_CHUNK_CEIL(sizeof(*req) + strlen(name) + 1);
    if (cp->sendbuf->DataSize + bytesize > CX_V4_MAX_UDP_DATASIZE) return +1;
    if (GrowSendBuf(cp, cp->sendbuf->DataSize + bytesize) != 0) return -1;

    req  = cp->sendbuf->data + cp->sendbuf->DataSize;
    bzero(req,  bytesize);

    cp->sendbuf->DataSize += bytesize;
    cp->sendbuf->NumChunks++;

    req->OpCode   = CXC_SEARCH;
    req->ByteSize = bytesize;
    req->param1   = param1;
    req->param2   = param2;
    strcpy(req->data, name);

    return 0;
}

//--------------------------------------------------------------------
enum {BCAST_MAX_ADDRS = 100};
static uint32_t  bcast_list[BCAST_MAX_ADDRS];
static uint16_t  bcast_port[BCAST_MAX_ADDRS];
static int       bcast_list_count   = 0;
static int       bcast_list_present = 0;
static int       bcast_list_fixed   = 0;
static uint8    *IFCONF_buf         = NULL;  // Note: this is NOT free()'d because it is valid till exit()

static void set_bcast_list_INADDR_BROADCAST(void)
{
    bcast_list[0]      = htonl(INADDR_BROADCAST);
    bcast_port[0]      = htons(CX_V4_INET_RESOLVER);
    bcast_list_count   = 1;
    bcast_list_present = 1;
}

static int is_a_guru_list_separator(int ch)
{
    return ch == ','  ||  ch == ';'  ||  isspace(ch);
}

/* Note:
       fill_bcast_list() CAN and probably SHOULD be separated into
       2 separate functions:
           1) CX_GURU_LIST parsing;
           2) SIOCGIFCONF & Co. readout
       But as it is now is more compact and comprehensible
 */
static void fill_bcast_list(v4conn_t *cp)
{
  int            r;
  int            buf_offs;

  uint32_t       loopback_addr = 0;

  struct ifconf  IFCONF;
  struct ifreq   IFINFO; // Note: this is used for ALL per-interface requests; as "ifr_name" is a separate field, we rely on it being unchanged after calls; plus, data from previous ioctl() is always fetched only BEFORE next ioctl()

  enum {IFCONF_BUF_SIZE = BCAST_MAX_ADDRS * sizeof(struct ifreq)};

  const char    *cx_guru_list;
  const char    *h_beg;
  const char    *p;
  char          *endptr;
  char           hostname[256];
  size_t         hostnamelen;
  uint16_t       port;
  struct hostent*hp;
  in_addr_t      spechost;

    /* A. Use $CX_GURU_LIST if specified */
    cx_guru_list = getenv("CX_GURU_LIST");
    if (cx_guru_list != NULL)
    {
        /* Note: this flag is intentionally set at the beginning,
           so that even if parsing fails (e.g. due to "host not found"),
           it wouldn't repeat again. */
        bcast_list_fixed = 1;

        /* Parse the value, treating ','/';'/whitespace as separators */
        for (p = cx_guru_list, bcast_list_count = 0;
                               bcast_list_count < BCAST_MAX_ADDRS;
             /* No "increment" statement */)
        {
            while (*p != '\0'  &&  is_a_guru_list_separator(*p)) p++;
            if (*p == '\0') goto END_PARSE_CX_GURU_LIST;

            // Consume legal-for-hostname characters,
            // ...allowing only NON-leading '-' and '.'
            for (h_beg = p;
                 isalnum(*p)  ||  (p != h_beg  &&  (*p == '-'  ||  *p == '.'));
                 p++);
            hostnamelen = p - h_beg;
            if (hostnamelen == 0)
            {
                cxlib_report(cp, "%s(): garbage in $CX_GURU_LIST at position %zd", __FUNCTION__, p - cx_guru_list);
                goto END_PARSE_CX_GURU_LIST;
            }
            if (hostnamelen > sizeof(hostname) - 1)
                hostnamelen = sizeof(hostname) - 1;
            memcpy(hostname, h_beg, hostnamelen); hostname[hostnamelen] = '\0';

            // Is ":PORT" specified?
            if (*p == ':')
            {
                p++;
                if (*p == '\0')
                {
                    cxlib_report(cp, "%s(): unexpected end of $CX_GURU_LIST after ':'", __FUNCTION__);
                    goto END_PARSE_CX_GURU_LIST;
                }
                port = (int)(strtol(p, &endptr, 10));
                if (endptr == p  ||
                    (*endptr != '\0'  &&  !is_a_guru_list_separator(*endptr)))
                {
                    cxlib_report(cp, "%s(): port number expected in $CX_GURU_LIST after \"%s:\"", __FUNCTION__, hostname);
                    goto END_PARSE_CX_GURU_LIST;
                }
                p = endptr;
            }
            else
                port = CX_V4_INET_RESOLVER;

            // Resolve
            /* Find out IP of the specified host */
            /* First, is it in dot notation (aaa.bbb.ccc.ddd)? */
            spechost = inet_addr(hostname);
            /* No, should do a hostname lookup */
            if (spechost == INADDR_NONE)
            {

#ifndef MAY_USE_GETHOSTBYNAME_FOR_CX_GURU_LIST
#define MAY_USE_GETHOSTBYNAME_FOR_CX_GURU_LIST 1
#endif
#if MAY_USE_GETHOSTBYNAME_FOR_CX_GURU_LIST
                hp = gethostbyname(hostname);
                /* No such host?! */
                if (hp == NULL)
                {
                    cxlib_report(cp, "%s(): error parsing $CX_GURU_LIST: gethostbyname(\"%s\"): %s",
                                 __FUNCTION__, hostname, hstrerror(h_errno));
                    goto NEXT_CX_GURU_LIST_ITEM;
                }

                memcpy(&spechost, hp->h_addr, hp->h_length);
#else
                cxlib_report(cp, "%s(): hostname-resolving in $CX_GURU_LIST is turned off at compile-time, so can't resolve \"%s\"", __FUNCTION__, hostname);
                goto NEXT_CX_GURU_LIST_ITEM;
#endif
            }

            // Store data
            bcast_list[bcast_list_count] = spechost;
            bcast_port[bcast_list_count] = htons(port);
            bcast_list_count++;

 NEXT_CX_GURU_LIST_ITEM:;
        }

 END_PARSE_CX_GURU_LIST:;
        /* Did we get anything? */
        if (bcast_list_count > 0)
        {
            bcast_list_present = 1;
            return;
        }

        cxlib_report(cp, "%s(): $CX_GURU_LIST produced empty list, falling back to SIOCGIFCONF", __FUNCTION__);
    }

    /* B. Obtain list of addresses from the kernel */

    /* Allocate a buffer */
    if (IFCONF_buf == NULL)
    {
        IFCONF_buf = malloc(IFCONF_BUF_SIZE);
        if (IFCONF_buf == NULL)
        {
            cxlib_report(cp, "%s(): malloc(IFCONF_BUF_SIZE) failed: errno=%d; using 255.255.255.255", __FUNCTION__, errno);
            goto FALLBACK_TO_INADDR_BROADCAST;
        }
        bzero(IFCONF_buf, IFCONF_BUF_SIZE);
    }

    /* Obtain list of interfaces */
    IFCONF.ifc_len = IFCONF_BUF_SIZE;
    IFCONF.ifc_req = (void*)IFCONF_buf;
    r = ioctl(cp->fd, SIOCGIFCONF, &IFCONF);
    if (r < 0  ||  IFCONF.ifc_len == 0)
    {
        cxlib_report(cp, "%s(): ioctl(,SIOCGIFCONF,)=%d errno=%d IFCONF.ifc_len=%d; using 255.255.255.255", __FUNCTION__, r, errno, IFCONF.ifc_len);
        goto FALLBACK_TO_INADDR_BROADCAST;
    }

    for (buf_offs = 0,                  bcast_list_count = 0;
         buf_offs < IFCONF.ifc_len  &&  bcast_list_count < BCAST_MAX_ADDRS;
         /*!!!*/buf_offs += sizeof(struct ifreq) /*!!! OSX: "IFNAMSIZ + ifreq->ifr_addr.sa_len" */)
    {
        IFINFO = *((struct ifreq *)(IFCONF_buf + buf_offs));
        /* Ignore non-IPv4 interfaces */
        if (IFINFO.ifr_addr.sa_family != AF_INET) continue;

        /* Get flags and act accordingly */
        r = ioctl(cp->fd, SIOCGIFFLAGS, &IFINFO);
        if (r < 0)
        {
            cxlib_report(cp, "%s(): \"%s\": ioctl(,SIOCGIFFLAGS,)=%d errno=%d", __FUNCTION__, IFINFO.ifr_name, r, errno);
            goto NEXT_IFACE;
        }
        /* Ignore non-up interfaces*/
        if ((IFINFO.ifr_flags & IFF_UP) == 0) goto NEXT_IFACE;

        /* Okay, what type of interface it is and which of its addresses to use? */
        // A broadcast-capable interface: use its broadcast address
        if      (IFINFO.ifr_flags & IFF_BROADCAST)
        {
            r = ioctl(cp->fd, SIOCGIFBRDADDR, &IFINFO);
            if (r < 0)
            {
                cxlib_report(cp, "%s(): \"%s\": ioctl(,SIOCGIFBRDADDR,)=%d errno=%d", __FUNCTION__, IFINFO.ifr_name, r, errno);
                goto NEXT_IFACE;
            }
            bcast_list[bcast_list_count] = ((struct sockaddr_in *)(&(IFINFO.ifr_broadaddr)))->sin_addr.s_addr;
            bcast_port[bcast_list_count] = htons(CX_V4_INET_RESOLVER);
            bcast_list_count++;
        }
        // A loopback: use its direct address...
        else if (IFINFO.ifr_flags & IFF_LOOPBACK)
        {
            r = ioctl(cp->fd, SIOCGIFADDR,    &IFINFO);
            if (r < 0)
            {
                cxlib_report(cp, "%s(): \"%s\": ioctl(,SIOCGIFADDR,)=%d errno=%d", __FUNCTION__, IFINFO.ifr_name, r, errno);
                goto NEXT_IFACE;
            }
            // Remember this address for future possible case "no other addresses available"
            loopback_addr = ((struct sockaddr_in *)(&(IFINFO.ifr_addr)))->sin_addr.s_addr;
        }
        // Point-to-point: use its "external" address
        else if (IFINFO.ifr_flags & IFF_POINTOPOINT)
        {
            r = ioctl(cp->fd, SIOCGIFDSTADDR, &IFINFO);
            if (r < 0)
            {
                cxlib_report(cp, "%s(): \"%s\": ioctl(,SIOCGIFDSTADDR,)=%d errno=%d", __FUNCTION__, IFINFO.ifr_name, r, errno);
                goto NEXT_IFACE;
            }
            bcast_list[bcast_list_count] = ((struct sockaddr_in *)(&(IFINFO.ifr_dstaddr)))->sin_addr.s_addr;
            bcast_port[bcast_list_count] = htons(CX_V4_INET_RESOLVER);
            bcast_list_count++;
        }

 NEXT_IFACE:;
    }

    // If no "foreign" addresses available then try to use loopback (useful for networkless standalone hosts, running both server and client)
    if (bcast_list_count == 0  &&  loopback_addr != 0)
    {
        bcast_list[bcast_list_count] = loopback_addr;
        bcast_port[bcast_list_count] = htons(CX_V4_INET_RESOLVER);
        bcast_list_count++;
    }

    if (bcast_list_count > 0)
    {
        bcast_list_present = 1;
        return;
    }

    cxlib_report(cp, "%s(): empty list of broadcast addresses; using 255.255.255.255", __FUNCTION__);
    /* Fallback to INADDR_BROADCAST*/

 FALLBACK_TO_INADDR_BROADCAST:
    set_bcast_list_INADDR_BROADCAST();
    bcast_list_present = 1;
}
//--------------------------------------------------------------------
/*
 *  SendSrchRequest
 *      Sends a SEARCH request prepared in [cd]->sendbuf to server.
 *
 *  Args:
 *      cp  connection pointer
 *
 *  Result:
 *      0   success
 *      -1  failure; `errno' contains error code
 */

static int SendSrchRequest(v4conn_t *cp)
{
  int             r;
  struct sockaddr_in  b_addr;
  int             n;

    /* Required additions for connectionless handshake/identification */
    cp->sendbuf->var1 = CXV4_VAR1_ENDIANNESS_SIG;
    cp->sendbuf->var2 = CX_V4_PROTO_VERSION;

#if 1
    if (!bcast_list_present) fill_bcast_list(cp);
    for (n = 0;  n < bcast_list_count;  n++)
    {
        /* Prepare an address... */
        b_addr.sin_family      = AF_INET;
        b_addr.sin_addr.s_addr = bcast_list[n];
        b_addr.sin_port        = bcast_port[n];

        /* ...and send */
        r = fdio_send_to(cp->fhandle, cp->sendbuf,
                         sizeof(CxV4Header) + cp->sendbuf->DataSize,
                         (struct sockaddr *)&b_addr, sizeof(b_addr));
        if (r < 0)  return -1;
    }
#else
    /* Prepare an address... */
    b_addr.sin_family      = AF_INET;
    b_addr.sin_addr.s_addr = htonl(INADDR_BROADCAST); /*!!! Note: maybe should prepare that address beforehand, to be able to check for any environment specs and even perform gethostbyname(). What about MULTIPLE addresses? */
    b_addr.sin_port        = htons(CX_V4_INET_RESOLVER);

    /* ...and send */
    r = fdio_send_to(cp->fhandle, cp->sendbuf,
                     sizeof(CxV4Header) + cp->sendbuf->DataSize,
                     (struct sockaddr *)&b_addr, sizeof(b_addr));
    if (r < 0)  return -1;
#endif

    return 0;
}

//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
/*####################################################################
#                                                                    #
#  Transport                                                         #
#                                                                    #
####################################################################*/



/*####################################################################
#                                                                    #
#  Brains                                                            #
#                                                                    #
####################################################################*/

static int RequestResult(v4conn_t *cp)
{
    cp->errcode = CERUNNING;
    _cxlib_begin_wait();

    if (cp->state == CS_READY)
    {
        errno = 0;
        return 0;
    }
    else
    {
        errno = cp->errcode;
        return -1;
    }
}

int  cx_begin (int cd)
{
  v4conn_t       *cp = AccessV4connSlot(cd);
  int             r;
  uint32          Type;

    if ((r = CheckCd(cd, CT_ANY, CS_READY)) != 0) return r;

    if      (cp->type == CT_DATA)
        Type = CXT4_DATA_IO;
    else if (cp->type == CT_SRCH)
        Type = CXT4_SEARCH;
    else
    {
        errno = CEINVCONN;
        return -1;
    }

    StartNewPacket(cp, Type);
    cp->state     = CS_CHUNKING;

    return 0;
}

int  cx_run   (int cd)
{
  v4conn_t       *cp = AccessV4connSlot(cd);
  int             r;

    if ((r = CheckCd(cd, CT_ANY, CS_CHUNKING)) != 0) return r;

    /* Send the packet... */
    if      (cp->type == CT_DATA)
        r = SendDataRequest(cp);
    else if (cp->type == CT_SRCH)
        r = SendSrchRequest(cp);
    else
    {
        errno = CEINVCONN;
        return -1;
    }
    if (r < 0)
    {
        MarkAsClosed(cp, errno);
        return -1;
    }

    cp->syncSeq = cp->sendbuf->Seq;
    cp->state   = CS_READY;
////fprintf(stderr, "%s nc=%d\n", __FUNCTION__, cp->sendbuf->NumChunks);

    return RequestResult(cp);
}

int  cx_rslv  (int cd, const char *name,      int  param1,  int  param2)
{
  v4conn_t       *cp = AccessV4connSlot(cd);
  int             r;
  size_t          bytesize;
  CxV4Chunk      *req;

    if ((r = CheckCd(cd, CT_DATA, CS_CHUNKING)) != 0) return r;

    bytesize = CXV4_CHUNK_CEIL(sizeof(*req) + strlen(name) + 1);
    if (GrowSendBuf(cp, cp->sendbuf->DataSize + bytesize) != 0) return -1;

    req  = cp->sendbuf->data + cp->sendbuf->DataSize;
    bzero(req,  bytesize);

    cp->sendbuf->DataSize += bytesize;
    cp->sendbuf->NumChunks++;

    req->OpCode   = CXC_RESOLVE;
    req->ByteSize = bytesize;
    req->param1   = param1;
    req->param2   = param2;
    strcpy(req->data, name);

    return 0;
}

int  cx_setmon(int cd, int count, int *hwids, int *param1s, int *param2s,
               int on_update)
{
  v4conn_t       *cp = AccessV4connSlot(cd);
  int             r;
  size_t          addsize;
  uint8          *ptr;
  CxV4MonitorChunk *mnrq;
  int             cn;

    if ((r = CheckCd(cd, CT_DATA, CS_CHUNKING)) != 0) return r;

    if (count == 0) return 0;
    if (count <  0) return -1;
    if (count > 1000000) /* An arbitrary limit. */
    {
        errno = EINVAL;
        return -1;
    }

    addsize = CXV4_CHUNK_CEIL(sizeof(*mnrq)) * count;
    if (GrowSendBuf(cp, cp->sendbuf->DataSize + addsize) != 0) return -1;

    ptr = cp->sendbuf->data + cp->sendbuf->DataSize;
    bzero(ptr, addsize);

    cp->sendbuf->DataSize += addsize;
    cp->sendbuf->NumChunks+= count;

    for (cn = 0;  cn < count;  cn++, ptr += mnrq->ck.ByteSize)
    {
        mnrq = ptr;
        mnrq->ck.OpCode      = CXC_SETMON;
        mnrq->ck.ByteSize    = CXV4_CHUNK_CEIL(sizeof(*mnrq));
        mnrq->ck.param1      = (param1s != NULL)? param1s[cn] : 0;
        mnrq->ck.param2      = (param2s != NULL)? param2s[cn] : 0;
        mnrq->cpid           = hwids  [cn];
        mnrq->dtype_and_cond = (CXDTYPE_UNKNOWN) |
                               ((on_update? CX_MON_COND_ON_UPDATE
                                          : CX_MON_COND_ON_CYCLE) << 8);
    }

    return 0;
}

/* A carbon copy of cx_setmon() with only CXC_DELMON instead of CXC_SETMON */
int  cx_delmon(int cd, int count, int *hwids, int *param1s, int *param2s,
               int on_update)
{
  v4conn_t       *cp = AccessV4connSlot(cd);
  int             r;
  size_t          addsize;
  uint8          *ptr;
  CxV4MonitorChunk *mnrq;
  int             cn;

    if ((r = CheckCd(cd, CT_DATA, CS_CHUNKING)) != 0) return r;

    if (count == 0) return 0;
    if (count <  0) return -1;
    if (count > 1000000) /* An arbitrary limit. */
    {
        errno = EINVAL;
        return -1;
    }

    addsize = CXV4_CHUNK_CEIL(sizeof(*mnrq)) * count;
    if (GrowSendBuf(cp, cp->sendbuf->DataSize + addsize) != 0) return -1;

    ptr = cp->sendbuf->data + cp->sendbuf->DataSize;
    bzero(ptr, addsize);

    cp->sendbuf->DataSize += addsize;
    cp->sendbuf->NumChunks+= count;

    for (cn = 0;  cn < count;  cn++, ptr += mnrq->ck.ByteSize)
    {
        mnrq = ptr;
        mnrq->ck.OpCode      = CXC_DELMON;
        mnrq->ck.ByteSize    = CXV4_CHUNK_CEIL(sizeof(*mnrq));
        mnrq->ck.param1      = (param1s != NULL)? param1s[cn] : 0;
        mnrq->ck.param2      = (param2s != NULL)? param2s[cn] : 0;
        mnrq->cpid           = hwids  [cn];
        mnrq->dtype_and_cond = (CXDTYPE_UNKNOWN) |
                               ((on_update? CX_MON_COND_ON_UPDATE
                                          : CX_MON_COND_ON_CYCLE) << 8);
    }

    return 0;
}

int  cx_rq_l_o(int cd, int             hwid,  int  param1,  int  param2,
               int operation)
{
  v4conn_t       *cp = AccessV4connSlot(cd);
  int             r;
  size_t          bytesize;
  CxV4LockOpChunk *req;

    if ((r = CheckCd(cd, CT_DATA, CS_CHUNKING)) != 0) return r;

    bytesize = CXV4_CHUNK_CEIL(sizeof(*req));
    
    if (GrowSendBuf(cp, cp->sendbuf->DataSize + bytesize) != 0) return -1;

    req  = cp->sendbuf->data + cp->sendbuf->DataSize;
    bzero(req,  bytesize);

    cp->sendbuf->DataSize += bytesize;
    cp->sendbuf->NumChunks++;

    req->ck.OpCode   = CXC_LOCK_OP;
    req->ck.ByteSize = bytesize;
    req->ck.param1   = param1;
    req->ck.param2   = param2;
    req->cpid        = hwid;
    req->operation   = operation;

    return 0;
}

static int do_rq_read(int OpCode,
                      int cd, int count, int *hwids, int *param1s, int *param2s)
{
  v4conn_t       *cp = AccessV4connSlot(cd);
  int             r;
  size_t          addsize;
  uint8          *ptr;
  CxV4ReadChunk  *rdrq;
  int             cn;

    ////fprintf(stderr, "%s count=%d hwid=%d cd=%d,%d\n", __FUNCTION__, count, hwids[0], cd, cp->state);

    if ((r = CheckCd(cd, CT_DATA, CS_CHUNKING)) != 0) return r;

    if (count == 0) return 0;
    if (count <  0) return -1;
    if (count > 1000000) /* An arbitrary limit. */
    {
        errno = EINVAL;
        return -1;
    }

    addsize = CXV4_CHUNK_CEIL(sizeof(*rdrq)) * count;
    if (GrowSendBuf(cp, cp->sendbuf->DataSize + addsize) != 0) return -1;

    ptr = cp->sendbuf->data + cp->sendbuf->DataSize;
    bzero(ptr, addsize);

    cp->sendbuf->DataSize += addsize;
    cp->sendbuf->NumChunks+= count;

    for (cn = 0;  cn < count;  cn++, ptr += rdrq->ck.ByteSize)
    {
        rdrq = ptr;
        rdrq->ck.OpCode     = OpCode;
        rdrq->ck.ByteSize   = sizeof(*rdrq);
        rdrq->ck.param1     = (param1s != NULL)? param1s[cn] : 0;
        rdrq->ck.param2     = (param2s != NULL)? param2s[cn] : 0;
        rdrq->hwid          = hwids  [cn];
    }

    return 0;
}


int  cx_rd_cur(int cd, int count, int *hwids, int *param1s, int *param2s)
{
    return do_rq_read(CXC_PEEK,  cd, count, hwids, param1s, param2s);
}

int  cx_rq_rd (int cd, int count, int *hwids, int *param1s, int *param2s)
{
    return do_rq_read(CXC_RQRDC, cd, count, hwids, param1s, param2s);
}

int  cx_rq_wr (int cd, int count, int *hwids, int *param1s, int *param2s,
               cxdtype_t *dtypes, int *nelems, void **values)
{
  v4conn_t       *cp = AccessV4connSlot(cd);
  int             r;
  int             stage;
  size_t          addsize;
  uint8          *ptr;
  CxV4WriteChunk *wrrq;
  int             cn;
  size_t          reqcsize;        // REQuest chunk size in bytes
  size_t          bytesize;

    if ((r = CheckCd(cd, CT_DATA, CS_CHUNKING)) != 0) return r;

    if (count == 0) return 0;
    if (count <  0) return -1;
    if (count > 1000000) /* An arbitrary limit. */
    {
        errno = EINVAL;
        return -1;
    }

    /* Pass 0: count sizes; Pass 1: store */
    for (stage = 0, addsize = 0;  stage <= 1;  stage++)
    {
        for (cn = 0;  cn < count;  cn++)
        {
            if (nelems[cn] < 0  /*!!! nelems[cn] > 100000000 */)
            {
                errno = EINVAL;
                return -1;
            }
            
            bytesize = sizeof_cxdtype(dtypes[cn]);
            if (bytesize > 8)
            {
                errno = EINVAL;
                return -1;
            }

            reqcsize = CXV4_CHUNK_CEIL(sizeof(*wrrq) + bytesize * nelems[cn]);
            ////fprintf(stderr, "bytesize=%zd reqcsize=%zd\n", bytesize, reqcsize);

            if (stage == 0)
                addsize += reqcsize;
            else
            {
                wrrq = ptr;
                wrrq->ck.OpCode     = CXC_RQWRC;
                wrrq->ck.ByteSize   = reqcsize;
                wrrq->ck.param1     = (param1s != NULL)? param1s[cn] : 0;
                wrrq->ck.param2     = (param2s != NULL)? param2s[cn] : 0;
                wrrq->hwid          = hwids  [cn];
                wrrq->dtype         = dtypes [cn];
                wrrq->nelems        = nelems [cn];
                if (wrrq->nelems != 0)
                    memcpy(wrrq->data, values[cn], wrrq->nelems * bytesize);

                ptr     += reqcsize;
            }
        }

        if (stage == 0)
        {
            if (GrowSendBuf(cp, cp->sendbuf->DataSize + addsize) != 0) return -1;

            ptr = cp->sendbuf->data + cp->sendbuf->DataSize;
            bzero(ptr, addsize);

            cp->sendbuf->DataSize += addsize;
            cp->sendbuf->NumChunks+= count;
            ////fprintf(stderr, "%d %d\n", cp->sendbuf->DataSize, cp->sendbuf->NumChunks);
        }
    }

    return 0;
}

int  cx_ch_open  (int cd, const char *name, int fail_on_err, int upd_cond,
                  int  param1,  int  param2)
{
  v4conn_t       *cp = AccessV4connSlot(cd);
  int             r;
  size_t          bytesize;
  CxV4Chunk      *req;

  int             mon_cond;

    if ((r = CheckCd(cd, CT_DATA, CS_CHUNKING)) != 0) return r;

    bytesize = CXV4_CHUNK_CEIL(sizeof(*req) + strlen(name) + 1);
    if (GrowSendBuf(cp, cp->sendbuf->DataSize + bytesize) != 0) return -1;

    if      (upd_cond == CX_UPD_COND_ON_UPDATE) mon_cond = CX_MON_COND_ON_UPDATE;
    else if (upd_cond == CX_UPD_COND_ON_CYCLE)  mon_cond = CX_MON_COND_ON_CYCLE;
    else                                        mon_cond = CX_MON_COND_NEVER;

    req  = cp->sendbuf->data + cp->sendbuf->DataSize;
    bzero(req,  bytesize);

    cp->sendbuf->DataSize += bytesize;
    cp->sendbuf->NumChunks++;

    req->OpCode   = CXC_CH_OPEN;
    req->ByteSize = bytesize;
    req->param1   = param1;
    req->param2   = param2;
    req->rs1      = fail_on_err;
    req->rs2      = mon_cond;
    strcpy(req->data, name);

    return 0;
}

int  cx_ch_close (int cd, int chnd,
                  int  param1,  int  param2)
{
  v4conn_t       *cp = AccessV4connSlot(cd);
  int             r;
  size_t          bytesize;
  CxV4Chunk      *req;

    if ((r = CheckCd(cd, CT_DATA, CS_CHUNKING)) != 0) return r;

    bytesize = CXV4_CHUNK_CEIL(sizeof(*req));
    if (GrowSendBuf(cp, cp->sendbuf->DataSize + bytesize) != 0) return -1;

    req  = cp->sendbuf->data + cp->sendbuf->DataSize;
    bzero(req,  bytesize);

    cp->sendbuf->DataSize += bytesize;
    cp->sendbuf->NumChunks++;

    req->OpCode   = CXC_CH_CLOSE;
    req->ByteSize = bytesize;
    req->param1   = param1;
    req->param2   = param2;
    req->rs1      = chnd;

    return 0;
}

int  cx_ch_rq_l_o(int cd, int chnd, int operation)
{
  v4conn_t       *cp = AccessV4connSlot(cd);
  int             r;
  size_t          bytesize;
  CxV4Chunk      *req;

    if ((r = CheckCd(cd, CT_DATA, CS_CHUNKING)) != 0) return r;

    bytesize = CXV4_CHUNK_CEIL(sizeof(*req));
    if (GrowSendBuf(cp, cp->sendbuf->DataSize + bytesize) != 0) return -1;

    req  = cp->sendbuf->data + cp->sendbuf->DataSize;
    bzero(req,  bytesize);

    cp->sendbuf->DataSize += bytesize;
    cp->sendbuf->NumChunks++;

    req->OpCode   = CXC_CH_LOCK_OP;
    req->ByteSize = bytesize;
    req->rs1      = chnd;
    req->rs2      = operation;

    return 0;
}

int  cx_ch_peek  (int cd, int chnd)
{
  v4conn_t       *cp = AccessV4connSlot(cd);
  int             r;
  size_t          bytesize;
  CxV4Chunk      *req;

    if ((r = CheckCd(cd, CT_DATA, CS_CHUNKING)) != 0) return r;

    bytesize = CXV4_CHUNK_CEIL(sizeof(*req));
    if (GrowSendBuf(cp, cp->sendbuf->DataSize + bytesize) != 0) return -1;

    req  = cp->sendbuf->data + cp->sendbuf->DataSize;
    bzero(req,  bytesize);

    cp->sendbuf->DataSize += bytesize;
    cp->sendbuf->NumChunks++;

    req->OpCode   = CXC_CH_PEEK;
    req->ByteSize = bytesize;
    req->rs1      = chnd;

    return 0;
}

int  cx_ch_rq_rd (int cd, int chnd)
{
  v4conn_t       *cp = AccessV4connSlot(cd);
  int             r;
  size_t          bytesize;
  CxV4Chunk      *req;

    if ((r = CheckCd(cd, CT_DATA, CS_CHUNKING)) != 0) return r;

    bytesize = CXV4_CHUNK_CEIL(sizeof(*req));
    if (GrowSendBuf(cp, cp->sendbuf->DataSize + bytesize) != 0) return -1;

    req  = cp->sendbuf->data + cp->sendbuf->DataSize;
    bzero(req,  bytesize);

    cp->sendbuf->DataSize += bytesize;
    cp->sendbuf->NumChunks++;

    req->OpCode   = CXC_CH_RQRD;
    req->ByteSize = bytesize;
    req->rs1      = chnd;

    return 0;
}

int  cx_ch_rq_wr (int cd, int chnd,
                  cxdtype_t dtype, int nelems, void *data)
{
  v4conn_t       *cp = AccessV4connSlot(cd);
  int             r;
  size_t          unitsize;
  size_t          datasize;
  size_t          bytesize;
  CxV4Chunk      *req;

    if ((r = CheckCd(cd, CT_DATA, CS_CHUNKING)) != 0) return r;

    unitsize = sizeof_cxdtype(dtype);
    if (unitsize > 8  ||  /*!!! int128/float128 are illegal as for now */
        nelems > CX_V4_MAX_DATASIZE / unitsize /* This check with '/' also protects from unitsize*nelems overflow */)
    {
        errno = EINVAL;
        return -1;
    }
    datasize = unitsize * nelems;

    bytesize = CXV4_CHUNK_CEIL(sizeof(*req) + datasize);
    if (GrowSendBuf(cp, cp->sendbuf->DataSize + bytesize) != 0) return -1;

    req  = cp->sendbuf->data + cp->sendbuf->DataSize;
    bzero(req,  bytesize);

    cp->sendbuf->DataSize += bytesize;
    cp->sendbuf->NumChunks++;

    req->OpCode   = CXC_CH_RQWR;
    req->ByteSize = bytesize;
    req->rs1      = chnd;
    req->rs2      = dtype;
    req->rs3      = nelems;
    if (datasize != 0)
        memcpy(req->data, data, datasize);

    return 0;
}

static void async_CXT4_DATA_IO    (v4conn_t *cp, CxV4Header *rcvd, size_t rcvdsize)
{
  int        numcns;          // # of chunks in reply
  uint8     *ptr;
  CxV4Chunk *rpy;
  int        rpycn;           // RePlY chunk #
  size_t     rpycsize;        // RePlY chunk size in bytes
  size_t     rpylen;          // RePlY *data* LENgth in bytes

  cxdtype_t  dtype;
  size_t     unitsize;

  CxV4ResultChunk      *rslt;
  CxV4_NT_AVALUE_Chunk *aval;
  cx_newval_info_t      nvi;

  CxV4CpointPropsChunk *prps;
  cx_rslv_info_t        rsi;
  CxV4_NT_OPEN_Chunk   *opnc;
  cx_ch_open_info_t     opi;

  CxV4FreshAgeChunk    *frsh;
  cx_fresh_age_info_t   fai;

  CxV4StrsChunk        *strd;
  CxV4_NT_STRS_Chunk   *strc;
  cx_strs_info_t        sti;
  int                   strcount;
  int                   ns;

  CxV4RDsChunk         *rdsl;
  cx_rds_info_t         rdi;
  int                   phys_count;
  enum {RDS_MAX_COUNT = 20};
  double                rds_buf[RDS_MAX_COUNT*2];

  CxV4QuantChunk       *qunt;
  cx_quant_info_t       qui;

  CxV4RangeChunk       *rnge;
  cx_range_info_t       rni;

  CxV4LockOpChunk      *lost;
  cx_lockstat_info_t    loi;

    if (rcvd->Type == CXT4_DATA_IO) _cxlib_break_wait();

    numcns = rcvd->NumChunks;

////fprintf(stderr, "Type=%08x DataSize=%d inpktsize=%zd\n", rcvd->Type, rcvd->DataSize, rcvdsize);
    /* Walk through the request */
    for (rpycn = 0, ptr = rcvd->data;
         rpycn < numcns;
         rpycn++,   ptr += rpycsize)
    {
        rpy = (void *)ptr;
        rpycsize = rpy->ByteSize;
        rpylen   = rpycsize - sizeof(CxV4Chunk);
        /*!!! Any checks? */
////fprintf(stderr, "\top=%08x sz=%zd\n", rpy->OpCode, rpycsize);
        if (rpycsize < sizeof(*rpy)) continue; /*!!!*/

        switch (rpy->OpCode)
        {
            case CXC_NEWVAL:
            case CXC_CURVAL:
////fprintf(stderr, "NEWVAL ByteSize=%zd sizeof(*rpy)=%zd\n", rpy->ByteSize, sizeof(*rpy));
                if (rpy->ByteSize <= sizeof(*rpy)) continue;
                rslt = (void*)rpy;
                nvi.hwid      = rslt->hwid;
                nvi.param1    = rpy->param1;
                nvi.param2    = rpy->param2;
                nvi.is_update = (rpy->OpCode == CXC_NEWVAL);
                nvi.dtype     = rslt->dtype;
                nvi.nelems    = rslt->nelems;
                nvi.rflags    = rslt->rflags;
                nvi.timestamp.nsec = rslt->timestamp_nsec;
                nvi.timestamp.sec  = rslt->timestamp_sec_lo32; /*!!! +_hi32<<32*/
                nvi.data      = rslt->data;
                CallNotifier(cp, CAR_NEWDATA, &nvi);
                break;

            case CXC_CVT_TO_RPY(CXC_RESOLVE):
                if      (rpy->ByteSize == sizeof(*prps))
                {
                    prps = (void*)rpy;
                    rsi.hwid   = prps->hwid*0 + prps->cpid*1;
                    //
                    rsi.rw     = prps->rw;
                    rsi.dtype  = prps->dtype;
                    rsi.nelems = prps->nelems;
                }
                else if (rpy->ByteSize == sizeof(CxV4Chunk))
                {
                    /* "NOTFOUND" */
                    rsi.hwid   = -1;
                    //
                    rsi.rw     = -1;
                    rsi.dtype  = CXDTYPE_UNKNOWN;
                    rsi.nelems = -1;
                }
                else continue;
                rsi.param1 = rpy->param1;
                rsi.param2 = rpy->param2;
                CallNotifier(cp, CAR_RSLV_RESULT, &rsi);
                break;

            case CXC_CVT_TO_RPY(CXC_FRH_AGE):
                if (rpy->ByteSize != sizeof(*frsh)) continue;
                frsh = (void*)rpy;
                fai.hwid   = frsh->hwid;
                fai.param1 = rpy->param1;
                fai.param2 = rpy->param2;
                fai.fresh_age.nsec = frsh->fresh_age_nsec;
                fai.fresh_age.sec  = frsh->fresh_age_sec_lo32; /*!!! +_hi32<<32*/
                CallNotifier(cp, CAR_FRESH_AGE, &fai);
                break;

            case CXC_CVT_TO_RPY(CXC_STRS):
                if (rpy->ByteSize < sizeof(*strd)) continue;
                strd = (void*)rpy;
                strcount = countof(strd->offsets);
                if (strcount > countof(sti.strings))
                    strcount = countof(sti.strings);
                /*!!! Note: MUST check for buffer overflow */
                bzero(&sti, sizeof(sti));
                sti.param1 = rpy->param1;
                sti.param2 = rpy->param2;
                for (ns = 0;  ns < strcount;  ns++)
                    sti.strings[ns] = strd->offsets[ns] >= 0? strd->data + strd->offsets[ns]
                                                            : NULL;
                CallNotifier(cp, CAR_STRS, &sti);
                break;

            case CXC_CVT_TO_RPY(CXC_RDS):
                if (rpy->ByteSize < sizeof(*rdsl)) continue;
                rdsl = (void*)rpy;
                phys_count = rdsl->phys_count;
                if (phys_count < 0  ||  phys_count > 1000) continue;
                if (rpy->ByteSize < sizeof(*rdsl) + sizeof(float64) * phys_count * 2) continue;

                if (phys_count > RDS_MAX_COUNT) phys_count = RDS_MAX_COUNT;
                rdi.hwid   = rdsl->hwid;
                rdi.param1 = rpy->param1;
                rdi.param2 = rpy->param2;
                rdi.phys_count = rdsl->phys_count;
                rdi.rds    = rds_buf;
                if (phys_count > 0)
                    memcpy(rds_buf, rdsl->data,
                           sizeof(rds_buf[0]) * 2 * phys_count);
                CallNotifier(cp, CAR_RDS, &rdi);
                break;

            case CXC_CVT_TO_RPY(CXC_QUANT):
                if (rpy->ByteSize != sizeof(*qunt)) continue;
                qunt = (void*)rpy;
                qui.hwid   = qunt->hwid;
                qui.param1 = rpy->param1;
                qui.param2 = rpy->param2;
                qui.q_dtype= qunt->q_dtype;
                /*!!! Note: MUST check that sizeof_cxdtype(q_dtype) is sane (<=sizeof(qunt->q_data)) */
                memcpy(&(qui.q), qunt->q_data, sizeof(qui.q));
                CallNotifier(cp, CAR_QUANT, &qui);
                break;

            case CXC_CVT_TO_RPY(CXC_RANGE):
                if (rpy->ByteSize < sizeof(*rnge)) continue;
                rnge = (void*)rpy;
                rni.hwid        = rnge->hwid;
                rni.param1      = rpy->param1;
                rni.param2      = rpy->param2;
                rni.range_dtype = rnge->range_dtype;
                /*!!! Note: MUST check that sizeof_cxdtype(range_dtype) is sane (<=sizeof(rnge->range[0])) */
                memcpy(rni.range+0, rnge->range_min, sizeof(rni.range[0]));
                memcpy(rni.range+1, rnge->range_max, sizeof(rni.range[1]));
                CallNotifier(cp, CAR_RANGE, &rni);
                break;

            case CXC_CVT_TO_RPY(CXC_LOCK_OP):
                if (rpy->ByteSize < sizeof(*lost)) continue;
                lost = (void*)rpy;
                loi.hwid     = lost->cpid;
                loi.param1   = rpy->param1;
                loi.param2   = rpy->param2;
                loi.lockstat = lost->lockstat_result;
                CallNotifier(cp, CAR_LOCKSTAT, &loi);
                break;

//--------------------------------------------------------------------

            case CXC_NT_NEWVAL:
            case CXC_NT_CURVAL:
                if (rpycsize < sizeof(*aval)) {/*fprintf(stderr, "AVAL: %zd<%zd\n", rpycsize, sizeof(*aval));*/ continue;}
                aval = (void*)rpy;
                nvi.hwid      = rpy->rs1;
                nvi.param1    = rpy->param1;
                nvi.param2    = rpy->param2;
                nvi.is_update = (rpy->OpCode == CXC_NT_NEWVAL);
                nvi.dtype     = aval->dtype;
                nvi.nelems    = aval->nelems;
                nvi.rflags    = aval->rflags;
                nvi.timestamp.nsec = aval->timestamp_nsec;
                nvi.timestamp.sec  = aval->timestamp_sec_lo32; /*!!! +_hi32<<32*/
                nvi.data      = aval->data;
                CallNotifier(cp, CAR_NEWDATA, &nvi);
                break;

            case CXC_NT_OPEN_NOTFOUND:
            case CXC_NT_OPEN_FOUND_STAGE1:
            case CXC_NT_OPEN_FOUND_STAGE2:
                if (rpycsize < sizeof(*opnc)) continue;
                opnc = (void*)rpy;
                opi.chnd   = opnc->ck.rs1;
                opi.param1 = opnc->ck.param1;
                opi.param2 = opnc->ck.param2;
                if (rpy->OpCode == CXC_NT_OPEN_NOTFOUND)
                    opi.status = -1;
                else
                    opi.status = (rpy->OpCode == CXC_NT_OPEN_FOUND_STAGE2);
                opi.hwid       = opnc->hwid;
                opi.rw         = opnc->rw;
                opi.dtype      = opnc->dtype;
                opi.max_nelems = opnc->max_nelems;
                if (rpylen < 2)
                    opi.name = NULL;
                else
                {
                    opnc->name[rpylen-1] = '\0';
                    opi.name = opnc->name;
                }
                CallNotifier(cp, CAR_CH_OPEN_RESULT, &opi);
                break;

            case CXC_NT_LOCKSTAT:
                loi.hwid     = rpy->rs1;
                loi.param1   = rpy->param1;
                loi.param2   = rpy->param2;
                loi.lockstat = rpy->rs2;
                CallNotifier(cp, CAR_LOCKSTAT, &loi);
                break;

            case CXC_NT_STRS:
                if (rpycsize < sizeof(*strc)) continue;
                strc = (void*)rpy;
                strcount = countof(strc->offsets);
                if (strcount > countof(sti.strings))
                    strcount = countof(sti.strings);
                /*!!! Note: MUST check for buffer overflow */
                bzero(&sti, sizeof(sti));
                sti.param1 = rpy->param1;
                sti.param2 = rpy->param2;
                for (ns = 0;  ns < strcount;  ns++)
                    sti.strings[ns] = strc->offsets[ns] >= 0? strc->data + strc->offsets[ns]
                                                            : NULL;
                CallNotifier(cp, CAR_STRS, &sti);
                break;

            case CXC_NT_RDS:
                dtype    = rpy->rs2;
                if (dtype != CXDTYPE_DOUBLE) continue; // ...
                phys_count = rpy->rs3;
                if (phys_count < 0  ||  phys_count > 1000)     continue;
                if (rpylen < sizeof(float64) * phys_count * 2) continue;

                if (phys_count > RDS_MAX_COUNT) phys_count = RDS_MAX_COUNT;
                rdi.hwid   = rpy->rs1;
                rdi.param1 = rpy->param1;
                rdi.param2 = rpy->param2;
                rdi.phys_count = phys_count;
                rdi.rds    = rds_buf;
                if (phys_count > 0)
                    memcpy(rds_buf, rpy->data,
                           sizeof(rds_buf[0]) * 2 * phys_count);
                CallNotifier(cp, CAR_RDS, &rdi);
                break;

            case CXC_NT_FRH_AGE:
                fai.hwid   = rpy->rs1;
                fai.param1 = rpy->param1;
                fai.param2 = rpy->param2;
                fai.fresh_age.nsec = rpy->rs2;
                fai.fresh_age.sec  = rpy->rs3; /*!!! +_rs4<<32*/
                CallNotifier(cp, CAR_FRESH_AGE, &fai);
                break;

            case CXC_NT_QUANT:
                dtype    = rpy->rs2;
                unitsize = sizeof_cxdtype(dtype);
                if (unitsize > sizeof(qui.q))        continue; // BIGTYPE
                if (rpylen < unitsize)               continue;
                qui.hwid        = rpy->rs1;
                qui.param1      = rpy->param1;
                qui.param2      = rpy->param2;
                qui.q_dtype     = dtype;
                memcpy(&(qui.q),    rpy->data,          unitsize);
                CallNotifier(cp, CAR_QUANT, &qui);
                break;

            case CXC_NT_RANGE:
                dtype    = rpy->rs2;
                unitsize = sizeof_cxdtype(dtype);
                if (unitsize > sizeof(rni.range[0])) continue; // BIGTYPE
                if (rpylen < unitsize*2)             continue;
                rni.hwid        = rpy->rs1;
                rni.param1      = rpy->param1;
                rni.param2      = rpy->param2;
                rni.range_dtype = dtype;
                memcpy(rni.range+0, rpy->data,          unitsize);
                memcpy(rni.range+1, rpy->data+unitsize, unitsize);
                CallNotifier(cp, CAR_RANGE, &rni);
                break;
        }
    }
}
