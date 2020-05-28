#ifndef __IMG878_H
#define __IMG878_H


/*
    Note: this is a "fake" _drv_i, for pzframe/vcamimg only;
    there is NO omg878 driver, but the "device" is composed of
    noop- and const-driven channels.
 */

// w1+b`'eval(580*450),r9i
enum
{
    IMG878_CHAN_DATA = 0,
    IMG878_CHAN_W    = 1,
    IMG878_CHAN_H    = 2,

    IMG878_NUMCHANS  = 10
};


/* General device specs */
enum
{
    IMG878_MAX_W     = 580,
    IMG878_MAX_H     = 450,

    IMG878_SRCMAXVAL = 255,
    IMG878_BPP       = 8,
};


#endif /* __IMG878_H */
