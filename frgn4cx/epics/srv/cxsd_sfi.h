#ifndef __CXSD_SFI_H
#define __CXSD_SFI_H


#include "cx.h"


#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


enum
{
    // Not an error but a status code
    CXSD_SFI_ALREADY  = +1,

    CXSD_SFI_SUCCESS  = 0,

    // Errors
    CXSD_SFI_ERROR    = -1,
    CXSD_SFI_noMemory = -2,
};


typedef void (*cxsd_sfi_mon_update_cb_t) (int mid, void *PV_ptr);
void cxsd_sfi_set_mon_update(cxsd_sfi_mon_update_cb_t cb);


int  cxsd_sfi_pvExistTest(const char *name);
int  cxsd_sfi_pvAttach   (const char *name, int *mid_p);

int  cxsd_sfi_set_PV_ptr (int mid, void  *PV_ptr);
int  cxsd_sfi_get_PV_ptr (int mid, void **PV_ptr_p);

const char *
     cxsd_sfi_get_name   (int mid);

int  cxsd_sfi_get_info   (int mid,
                          cxdtype_t *dtype_p, int *max_nelems_p, int *is_rw_p);

int  cxsd_sfi_get_data   (int mid,
                          cxdtype_t *dtype_p, int *nelems_p,       void **data_p,
                          rflags_t *rflags_p, cx_time_t *timestamp_p);
int  cxsd_sfi_do_write   (int mid, 
                          cxdtype_t  dtype,   int  nelems,   const void  *data);

void cxsd_sfi_term_f     (void);
int  cxsd_sfi_request_subscription(void);


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* __CXSD_SFI_H */
