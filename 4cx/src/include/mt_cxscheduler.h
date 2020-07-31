#ifndef __MT_CXSCHEDULER_H
#define __MT_CXSCHEDULER_H


#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


#include "cxscheduler.h"

int  mt_sl_start (void);

void mt_sl_lock  (void);
void mt_sl_unlock(void);


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* __MT_CXSCHEDULER_H */
