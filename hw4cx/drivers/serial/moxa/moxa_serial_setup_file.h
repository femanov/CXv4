#include <sys/ioctl.h>
#include <moxadevice.h>

#define SPECIFIC_SERIAL_SETUP_CODE_2()                                          \
    do                                                                          \
    {                                                                           \
      int  mode = RS485_2WIRE_MODE;                                             \
                                                                                \
        r = ioctl(fd, MOXA_SET_OP_MODE, &mode);                                 \
        if (r < 0)                                                              \
        {                                                                       \
            errreport(opaqueptr, "ioctl(MOXA_SET_OP_MODE, RS485_2WIRE_MODE)=%d", r); \
            return -1;                                                          \
        }                                                                       \
    } while (0)
