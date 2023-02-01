#ifndef __EPICS2SMTH_H
#define __EPICS2SMTH_H


#include "cx.h" // For cxdtype_t+CXDTYPE_*, various bit-types definitions (int32 etc.)


#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


//////////////////////////////////////////////////////////////////////

typedef void * e2s_backend_obj_ref_t;

typedef int                   (*e2s_check_name)(const char *name);
typedef e2s_backend_obj_ref_t (*e2s_add_chan)  (const char *name, int mid);
typedef int                   (*e2s_del_chan)  (e2s_backend_obj_ref_t obj_ref);
typedef int                   (*e2s_snd_data)  (e2s_backend_obj_ref_t obj_ref,
                                                cxdtype_t dtype, int nelems, const void *data);


typedef struct
{
    int             names_case_sensitive;

    e2s_check_name  check_name;
    e2s_add_chan    add_chan;
    e2s_del_chan    del_chan;
    e2s_snd_data    snd_data;
} e2s_backend_t;


void e2s_set_found  (int mid, int is_found);
void e2s_set_props  (int mid,
                     cxdtype_t dtype, int max_nelems, int is_rw);
void e2s_update_data(int mid,
                     cxdtype_t dtype, int nelems, void *value,
                     int some_flags, cx_time_t timestamp);


void e2s_run(e2s_backend_t *backend);


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* __EPICS2SMTH_H */
