#ifndef __SERIAL_HAL_H
#define __SERIAL_HAL_H


typedef void (*serial_hal_errreport_t)(void *opaqueptr,
                                       const char *format, ...);

static int serial_hal_opendev (int line, int baudrate,
                               serial_hal_errreport_t errreport,
                               void *opaqueptr);


#endif /* __SERIAL_HAL_H */
