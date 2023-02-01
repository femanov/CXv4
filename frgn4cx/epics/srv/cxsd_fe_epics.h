#ifndef __CXSD_FE_EPICS_H
#define __CXSD_FE_EPICS_H


#include "cx.h"


#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


enum
{
    // Not an error but a status code
    CXSD_FE_EPICS_ALREADY  = +1,

    CXSD_FE_EPICS_SUCCESS  = 0,

    // Errors
    CXSD_FE_EPICS_ERROR    = -1,
    CXSD_FE_EPICS_noMemory = -2,
};


int  cxsd_fe_epics_pvExistTest(const char *name);
int  cxsd_fe_epics_pvAttach   (const char *name, int *mid_p);

int  cxsd_fe_epics_set_PV_ptr (int mid, void  *PV_ptr);
int  cxsd_fe_epics_get_PV_ptr (int mid, void **PV_ptr_p);

const char *
     cxsd_fe_epics_get_name   (int mid);

int  cxsd_fe_epics_get_info   (int mid,
                               cxdtype_t *dtype_p, int *max_nelems_p, int *is_rw_p);

int  cxsd_fe_epics_get_data   (int mid,
                               cxdtype_t *dtype_p, int *nelems_p,       void **data_p,
                               rflags_t *rflags_p, cx_time_t *timestamp_p);
int  cxsd_fe_epics_do_write   (int mid, 
                               cxdtype_t  dtype,   int  nelems,   const void  *data);


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* __CXSD_FE_EPICS_H */
