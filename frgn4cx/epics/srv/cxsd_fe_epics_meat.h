#ifndef __CXSD_FE_EPICS_MEAT_H
#define __CXSD_FE_EPICS_MEAT_H


#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


int  cxsd_fe_epics_meat_init_f (int server_instance, int cxsd_hw_numchans_val);
void cxsd_fe_epics_meat_term_f (void);

void cxsd_fe_epics_meat_do_poll(void);

void cxsd_fe_epics_meat_update (int mid, void *PV_ptr);


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* __CXSD_FE_EPICS_MEAT_H */
