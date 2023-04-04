#ifndef __FROLOV_TIME24K4_DRV_I_H
#define __FROLOV_TIME24K4_DRV_I_H


// w50i,r40i,r4+b2048,r6i
enum
{
    FROLOV_TIME24K4_CHAN_WR_base     = 0,
      FROLOV_TIME24K4_CHAN_WR_count  = 50,

    FROLOV_TIME24K4_CHAN_MASK_n_base = FROLOV_TIME24K4_CHAN_WR_base + 0,

    FROLOV_TIME24K4_CHAN_CLK_MODE    = FROLOV_TIME24K4_CHAN_WR_base + 10,

    FROLOV_TIME24K4_CHAN_RD_base     = 50,
      FROLOV_TIME24K4_CHAN_RD_count  = 50,

    FROLOV_TIME24K4_CHAN_TIME_n_base = FROLOV_TIME24K4_CHAN_RD_base + 0,

    FROLOV_TIME24K4_CHAN_STAT_n_base = FROLOV_TIME24K4_CHAN_RD_base + 40,

    FROLOV_TIME24K4_CHAN_ADCSIM_base = FROLOV_TIME24K4_CHAN_RD_base + 44,
    FROLOV_TIME24K4_CHAN_NUMPTS      = FROLOV_TIME24K4_CHAN_ADCSIM_base + 0,  // :=2048
    FROLOV_TIME24K4_CHAN_RANGEMIN    = FROLOV_TIME24K4_CHAN_ADCSIM_base + 1,  // :=0
    FROLOV_TIME24K4_CHAN_RANGEMAX    = FROLOV_TIME24K4_CHAN_ADCSIM_base + 2,  // :=255

    FROLOV_TIME24K4_NUMCHANS = 100,
};

enum
{
    FROLOV_TIME24K4_CLK_MODE_INT = 0,
    FROLOV_TIME24K4_CLK_MODE_EXT = 1,
};

enum
{
    FROLOV_TIME24K4_NUM_LINES  = 4,
    FROLOV_TIME24K4_MAX_NUMPTS = 2048,
};


#endif /* __FROLOV_TIME24K4_DRV_I_H */
