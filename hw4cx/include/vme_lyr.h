#ifndef __VME_LYR_H
#define __VME_LYR_H


#include "cx_version.h"

#include "cxsd_driver.h"


#define VME_LYR_API_NAME "vme"
enum
{
    VME_LYR_API_VERSION_MAJOR = 1,
    VME_LYR_API_VERSION_MINOR = 0,
    VME_LYR_API_VERSION = CX_ENCODE_VERSION(VME_LYR_API_VERSION_MAJOR,
                                            VME_LYR_API_VERSION_MINOR)
};


/* Driver-provided callbacks */

typedef void (*vme_irq_proc)(int devid, void *devptr,
                             int irq_n, int irq_vect);


/* Layer API for drivers */

typedef int  (*VmeAddDevice) (int devid, void *devptr,
                              int    bus_major, int    bus_minor,
                              uint32 base_addr, uint32 space_size,
                              int    addr_size, int    am,
                              int irq_n, int irq_vect, vme_irq_proc irq_proc);

typedef int  (*VmeGetDevInfo)(int devid,
                              int    *bus_major_p, int    *bus_minor_p,
                              uint32 *base_addr_p, uint32 *space_size_p,
                              int    *addr_size_p, int    *am_p,
                              int    *irq_n_p,     int    *irq_vect_p);

typedef int  (*VmeHalIoCtl)  (int handle, const char *hal_api_name,
                              int request, ...);

typedef int  (*VmeAxxWr8)    (int handle, uint32 ofs, uint8    value);
typedef int  (*VmeAxxRd8)    (int handle, uint32 ofs, uint8   *val_p);
typedef int  (*VmeAxxWr16)   (int handle, uint32 ofs, uint16   value);
typedef int  (*VmeAxxRd16)   (int handle, uint32 ofs, uint16  *val_p);
typedef int  (*VmeAxxWr32)   (int handle, uint32 ofs, uint32   value);
typedef int  (*VmeAxxRd32)   (int handle, uint32 ofs, uint32  *val_p);

typedef int  (*VmeAxxWr8v)   (int handle, uint32 ofs, uint8   *data, int count);
typedef int  (*VmeAxxRd8v)   (int handle, uint32 ofs, uint8   *data, int count);
typedef int  (*VmeAxxWr16v)  (int handle, uint32 ofs, uint16  *data, int count);
typedef int  (*VmeAxxRd16v)  (int handle, uint32 ofs, uint16  *data, int count);
typedef int  (*VmeAxxWr32v)  (int handle, uint32 ofs, uint32  *data, int count);
typedef int  (*VmeAxxRd32v)  (int handle, uint32 ofs, uint32  *data, int count);


typedef struct
{
    VmeAddDevice  add;

    VmeGetDevInfo get_dev_info;

    VmeHalIoCtl   hal_ioctl;

    /* I/O */

    // a. Scalar

    // A16
    VmeAxxWr8     a16wr8;
    VmeAxxRd8     a16rd8;
    VmeAxxWr16    a16wr16;
    VmeAxxRd16    a16rd16;
    VmeAxxWr32    a16wr32;
    VmeAxxRd32    a16rd32;

    // A24
    VmeAxxWr8     a24wr8;
    VmeAxxRd8     a24rd8;
    VmeAxxWr16    a24wr16;
    VmeAxxRd16    a24rd16;
    VmeAxxWr32    a24wr32;
    VmeAxxRd32    a24rd32;

    // A32
    VmeAxxWr8     a32wr8;
    VmeAxxRd8     a32rd8;
    VmeAxxWr16    a32wr16;
    VmeAxxRd16    a32rd16;
    VmeAxxWr32    a32wr32;
    VmeAxxRd32    a32rd32;

    // b. Vector

    // A16
    VmeAxxWr8v    a16wr8v;
    VmeAxxRd8v    a16rd8v;
    VmeAxxWr16v   a16wr16v;
    VmeAxxRd16v   a16rd16v;
    VmeAxxWr32v   a16wr32v;
    VmeAxxRd32v   a16rd32v;

    // A24
    VmeAxxWr8v    a24wr8v;
    VmeAxxRd8v    a24rd8v;
    VmeAxxWr16v   a24wr16v;
    VmeAxxRd16v   a24rd16v;
    VmeAxxWr32v   a24wr32v;
    VmeAxxRd32v   a24rd32v;

    // A32
    VmeAxxWr8v    a32wr8v;
    VmeAxxRd8v    a32rd8v;
    VmeAxxWr16v   a32wr16v;
    VmeAxxRd16v   a32rd16v;
    VmeAxxWr32v   a32wr32v;
    VmeAxxRd32v   a32rd32v;
} vme_vmt_t;


#endif /* __VME_LYR_H */
