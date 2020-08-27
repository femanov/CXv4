#ifndef __MISC_SLEEPBYSELECT_H
#define __MISC_SLEEPBYSELECT_H


#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


/* Portable sleep() with microsecond precision;
   sleeps NOT LESS than specified (MAY sleep longer). */
void SleepBySelect(int usecs);


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* __MISC_SLEEPBYSELECT_H */
