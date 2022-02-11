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

#define SPECIFIC_SERIAL_SETUP_CODE_1()                                          \
    do                                                                          \
    {                                                                           \
      char  mode = MX_RS485_2W;                                                 \
                                                                                \
        r = ioctl(fd, MX_SET_INTERFACE, mode);                                  \
        if (r < 0)                                                              \
        {                                                                       \
            errreport(opaqueptr, "ioctl(MX_SET_INTERFACE, MX_RS485_2W)=%d", r); \
            return -1;                                                          \
        }                                                                       \
    } while (0)
