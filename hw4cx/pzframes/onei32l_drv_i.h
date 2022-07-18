#ifndef __ONEI32L_DRV_I_H
#define __ONEI32L_DRV_I_H


/* This is a "fake device driver interface" file,
   for "onei32l" knobplugin sources to employ
   the same techniques as others */


enum
{
    ONEI32L_CHAN_DATA       = 0,
    ONEI32L_CHAN_NUMPTS     = 1,
    ONEI32L_CHAN_CUR_NUMPTS = 2,
    ONEI32L_CHAN_RANGEMIN   = 3,
    ONEI32L_CHAN_RANGEMAX   = 4,

    ONEI32L_NUMCHANS = 5
};

/* General device specs */
enum
{
    ONEI32L_NUM_LINES  = 1,
    ONEI32L_MAX_NUMPTS = 10000,
};
enum
{
    ONEI32L_MIN_VALUE = -10230000*0-32767, // ???
    ONEI32L_MAX_VALUE = +10230000*0+32767, // ???
};


#endif /* __ONEI32L_DRV_I_H */
