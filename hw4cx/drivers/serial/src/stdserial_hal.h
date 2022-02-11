#ifndef SERIAL_HAL_PFX
  #error The "SERIAL_HAL_PFX" macro is undefined
#endif

//#ifdef SPECIFIC_SERIAL_SETUP_FILE_H
//  #include SPECIFIC_SERIAL_SETUP_FILE_H
//#endif /* SPECIFIC_SERIAL_SETUP_FILE_H */
#ifdef MOXA_KSHD485
  #include <sys/ioctl.h>
  #include <moxadevice.h>
#endif
#ifdef MXUPCIE_KSHD485
  #include <sys/ioctl.h>
#if 1
  #define MX_IOCTL_CODE           0x400
  #define MX_SET_INTERFACE        (MX_IOCTL_CODE+79)
  #define MX_RS485_2W             0x0f
#else
  #define VERSION_CODE(ver,rel,seq)       ((ver << 16) | (rel << 8) | seq) // Moronic authors of mxpcie.* placed VERSION_CODE() definition in mxpcie.c, despite the fact that it is used in mxpcie.h
  #include "mxpcie.h"
  /* Note: the mxpcie.h file is a mxser/driver/kernel5.x/mxpcie.h from 
     https://cdn-cms.azureedge.net/getmedia/9fa12c30-9bad-4d77-ad43-7810f74b8410/moxa-linux-kernel-5.x.x-driver-v5.1.tgz
     which is available at
     https://www.moxa.com/en/products/industrial-edge-connectivity/multiport-serial-boards/pcie-upci-pci-serial-cards/cp-132el-series#resources
     The setup code itself is from utility/conf/conftool.c at the same .tgz
   */
  #define MX_SET_INTERFACE SMARTIO_PUART_SET_INTERFACE // That's because names in conftool.c differ from ones in mxpcie.h
#endif
#endif

#include "serial_hal.h"


static int serial_hal_opendev (int line, int baudrate,
                               serial_hal_errreport_t errreport,
                               void * opaqueptr)
{
  int             fd;
  char            devpath[PATH_MAX];
  int             r;
  struct termios  newtio;

    sprintf(devpath, "/dev/tty%s%d", SERIAL_HAL_PFX, line);

    /* Open a descriptor to COM port... */
    fd = open(devpath, O_RDWR | O_NOCTTY);
    if (fd < 0)
    {
        errreport(opaqueptr, "open(\"%s\")", devpath);
        return -1;
    }

    /* ...setup... */
//#ifdef SPECIFIC_SERIAL_SETUP_CODE_1
//    SPECIFIC_SERIAL_SETUP_CODE_1();
//#endif
    bzero(&newtio, sizeof(newtio));
    newtio.c_cflag     = baudrate | CRTSCTS*1 | CS8 | CLOCAL | CREAD;
    newtio.c_iflag     = IGNPAR;
    newtio.c_oflag     = 0;
    newtio.c_lflag     = 0;
    newtio.c_cc[VTIME] = 0;
    newtio.c_cc[VMIN]  = 1;
    tcflush(fd, TCIFLUSH);
    errno = 0;
    r = tcsetattr(fd,TCSANOW,&newtio);
    if (r < 0)
    {
        errreport(opaqueptr, "tcsetattr()=%d", r);
        return -1;
    }
#ifdef MOXA_KSHD485
    {
      int  mode = RS485_2WIRE_MODE;

        r = ioctl(fd, MOXA_SET_OP_MODE, &mode);
        if (r < 0)
        {
            errreport(opaqueptr, "ioctl(MOXA_SET_OP_MODE, RS485_2WIRE_MODE)=%d", r);
            return -1;
        }
    }
#endif
#ifdef MXUPCIE_KSHD485
    {
      char  mode = MX_RS485_2W;

        r = ioctl(fd, MX_SET_INTERFACE, mode);
        if (r < 0)
        {
            errreport(opaqueptr, "ioctl(MX_SET_INTERFACE, MX_RS485_2W)=%d", r);
            return -1;
        }
    }
#endif
//#ifdef SPECIFIC_SERIAL_SETUP_CODE_2
//    SPECIFIC_SERIAL_SETUP_CODE_2();
//#endif

    /* ...and turn it to non-blocking mode */
    if (set_fd_flags(fd, O_NONBLOCK, 1) != 0)
    {
        errreport(opaqueptr, "set_fd_flags()");
        close(fd);
        return -1;
    }

    return fd;
}
