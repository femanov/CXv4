#include <stdio.h>

#include "vme_lyr.h"
#include "remsrv.h"


#ifndef VMESERVER_ISSUE
  #define VMESERVER_ISSUE "CX vmeserver"
#endif


remcxsd_dev_t  devinfo[1 + 100]; // Arbitrary limit, VME crates are usually much smaller

remcxsd_dev_t *remcxsd_devices = devinfo;
int            remcxsd_maxdevs = countof(devinfo);

static remsrv_conscmd_descr_t vmeserver_commands[] =
{
    {NULL, NULL, NULL}
};

static const char * vmeserver_clninfo(int devid)
{
  vme_vmt_t    *vmt;
  int           bus_major;
  int           bus_minor;
  uint32        base_addr;
  uint32        space_size;
  int           addr_size;
  int           am;
  int           irq_n;
  int           irq_vect;

  char          devinfo_buf[200];
  static char   buf2return [100];

    if ((vmt = GetLayerVMT(devid)) != NULL  &&
        vmt->get_dev_info(devid,
                          &bus_major, &bus_minor,
                          &base_addr, &space_size,
                          &addr_size, &am,
                          &irq_n, &irq_vect) >= 0)
        sprintf(devinfo_buf, "(@%d/%d:%d:0x%02x:0x%08x/0x%08x %c:0x%02X)",
                bus_major, bus_minor, addr_size, am, base_addr, space_size,
                (irq_n >= 0  &&  irq_n <= 7)? '0'+irq_n : '-', irq_vect);
    else
        sprintf(devinfo_buf, "-");

    sprintf(buf2return, "%s", devinfo_buf);

    return buf2return;
}


int main(int argc, char *argv[])
{
    return remsrv_main(argc, argv,
                       "vmeserver", VMESERVER_ISSUE,
                       vmeserver_commands, vmeserver_clninfo);
}
