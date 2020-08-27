#ifndef __MISC_ISO8601_H
#define __MISC_ISO8601_H


#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


#include <sys/time.h>


/* ISO 8601 time presentation */
char *stroftime     (time_t when, char *dts);
char *stroftime_msc (struct timeval *when, char *dts);
char *strcurtime    (void);
char *strcurtime_msc(void);


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* __MISC_ISO8601_H */
