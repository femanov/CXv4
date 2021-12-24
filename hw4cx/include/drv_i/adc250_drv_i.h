#ifndef __ADC250_DRV_I_H
#define __ADC250_DRV_I_H


// r1h3132620,r4h783155,r5i,w30i,r100i
enum
{
    /* 0-9: data */
    ADC250_CHAN_DATA          = 0,
    ADC250_CHAN_LINE0         = 1,
    ADC250_CHAN_LINE1         = 2,
    ADC250_CHAN_LINE2         = 3,
    ADC250_CHAN_LINE3         = 4,

    ADC250_CHAN_MARKER        = 9,

    /* 10-39: controls */
    ADC250_CHAN_SHOT          = 10, 
    ADC250_CHAN_STOP          = 11,
    ADC250_CHAN_ISTART        = 12,
    ADC250_CHAN_WAITTIME      = 13, 
    ADC250_CHAN_CALIBRATE     = 14,
    // absent in ADC250: ADC250_CHAN_FGT_CLB       = 15,
    // absent in ADC250: ADC250_CHAN_VISIBLE_CLB   = 16,
    ADC250_CHAN_CALC_STATS    = 17,

    ADC250_CHAN_RUN_MODE      = 18,
    ADC250_CHAN_RUN           = 19,

    ADC250_CHAN_PTSOFS        = 20,
    ADC250_CHAN_NUMPTS        = 21,
    ADC250_CHAN_TIMING        = 22,
    ADC250_CHAN_FRQDIV        = 23,
    ADC250_CHAN_RANGE0        = 24,
    ADC250_CHAN_RANGE1        = 25,
    ADC250_CHAN_RANGE2        = 26,
    ADC250_CHAN_RANGE3        = 27,
    ADC250_CHAN_TRIG_TYPE     = 28,
    ADC250_CHAN_TRIG_INPUT    = 29,
// !!! Future "TRIG_LEVEL"?
// !!! Future "TRIG_MODE" (single/multi/recorder) and TRIG_MULTI_NUM?

    ADC250_CHAN_PLL_PRESET    = 39,

    /* 40-139: status */
    ADC250_CHAN_DEVICE_ID     = 40,  ADC250_CHAN_HWINFO_first = ADC250_CHAN_DEVICE_ID,
    ADC250_CHAN_BASE_SW_VER   = 41,
    ADC250_CHAN_PGA_SW_VER    = 42,
    ADC250_CHAN_BASE_HW_VER   = 43,
    ADC250_CHAN_PGA_HW_VER    = 44,
    ADC250_CHAN_PGA_VAR       = 45,
    ADC250_CHAN_BASE_UNIQ_ID  = 46,
    ADC250_CHAN_PGA_UNIQ_ID   = 47,  ADC250_CHAN_HWINFO_last  = ADC250_CHAN_PGA_UNIQ_ID,
                                     ADC250_CHAN_HWINFO_count = ADC250_CHAN_HWINFO_last  -
                                                                ADC250_CHAN_HWINFO_first + 1,

    ADC250_CHAN_PLL_LOCKED    = 48,

    // unused 49

    ADC250_CHAN_ELAPSED       = 50,
    // absent in ADC250: ADC250_CHAN_CLB_STATE     = 51,
    ADC250_CHAN_XS_PER_POINT  = 52,
    ADC250_CHAN_XS_DIVISOR    = 53,
    ADC250_CHAN_XS_FACTOR     = 54,
    ADC250_CHAN_CUR_PTSOFS    = 55,
    ADC250_CHAN_CUR_NUMPTS    = 56,
    ADC250_CHAN_CUR_TIMING    = 57,
    ADC250_CHAN_CUR_FRQDIV    = 58,
    // unused 59
    ADC250_CHAN_CUR_RANGE0    = 60,
    ADC250_CHAN_CUR_RANGE1    = 61,
    ADC250_CHAN_CUR_RANGE2    = 62,
    ADC250_CHAN_CUR_RANGE3    = 63,
    ADC250_CHAN_OVERFLOW0     = 64,
    ADC250_CHAN_OVERFLOW1     = 65,
    ADC250_CHAN_OVERFLOW2     = 66,
    ADC250_CHAN_OVERFLOW3     = 67,
    // unused 68,69

    ADC250_CHAN_LINE0ON       = 70,
    ADC250_CHAN_LINE1ON       = 71,
    ADC250_CHAN_LINE2ON       = 72,
    ADC250_CHAN_LINE3ON       = 73,
    ADC250_CHAN_LINE0RANGEMIN = 74,
    ADC250_CHAN_LINE1RANGEMIN = 75,
    ADC250_CHAN_LINE2RANGEMIN = 76,
    ADC250_CHAN_LINE3RANGEMIN = 77,
    ADC250_CHAN_LINE0RANGEMAX = 78,
    ADC250_CHAN_LINE1RANGEMAX = 79,
    ADC250_CHAN_LINE2RANGEMAX = 80,
    ADC250_CHAN_LINE3RANGEMAX = 81,

    ADC250_CHAN_CLB_FAIL0     = 82,
    ADC250_CHAN_CLB_FAIL1     = 83,
    ADC250_CHAN_CLB_FAIL2     = 84,
    ADC250_CHAN_CLB_FAIL3     = 85,
    ADC250_CHAN_CLB_DYN0      = 86,
    ADC250_CHAN_CLB_DYN1      = 87,
    ADC250_CHAN_CLB_DYN2      = 88,
    ADC250_CHAN_CLB_DYN3      = 89,
    ADC250_CHAN_CLB_ZERO0     = 90,
    ADC250_CHAN_CLB_ZERO1     = 91,
    ADC250_CHAN_CLB_ZERO2     = 92,
    ADC250_CHAN_CLB_ZERO3     = 93,
    ADC250_CHAN_CLB_GAIN0     = 94,
    ADC250_CHAN_CLB_GAIN1     = 95,
    ADC250_CHAN_CLB_GAIN2     = 96,
    ADC250_CHAN_CLB_GAIN3     = 97,
    // unused 98,99

    ADC250_CHAN_CUR_PLL1_CTRL = 98,
    ADC250_CHAN_CUR_PLL2_CTRL = 99,

    ADC250_CHAN_MIN0          = 100, ADC250_CHAN_STATS_first = ADC250_CHAN_MIN0,
    ADC250_CHAN_MIN1          = 101,
    ADC250_CHAN_MIN2          = 102,
    ADC250_CHAN_MIN3          = 103,
    ADC250_CHAN_MAX0          = 104,
    ADC250_CHAN_MAX1          = 105,
    ADC250_CHAN_MAX2          = 106,
    ADC250_CHAN_MAX3          = 107,
    ADC250_CHAN_AVG0          = 108,
    ADC250_CHAN_AVG1          = 109,
    ADC250_CHAN_AVG2          = 110,
    ADC250_CHAN_AVG3          = 111,
    ADC250_CHAN_INT0          = 112,
    ADC250_CHAN_INT1          = 113,
    ADC250_CHAN_INT2          = 114,
    ADC250_CHAN_INT3          = 115, ADC250_CHAN_STATS_last = ADC250_CHAN_INT3,

    ADC250_CHAN_LINE0TOTALMIN = 116,
    ADC250_CHAN_LINE1TOTALMIN = 117,
    ADC250_CHAN_LINE2TOTALMIN = 118,
    ADC250_CHAN_LINE3TOTALMIN = 119,
    ADC250_CHAN_LINE0TOTALMAX = 120,
    ADC250_CHAN_LINE1TOTALMAX = 121,
    ADC250_CHAN_LINE2TOTALMAX = 122,
    ADC250_CHAN_LINE3TOTALMAX = 123,
    ADC250_CHAN_NUM_LINES     = 124,

    ADC250_NUMCHANS = 140
};

enum
{
    ADC250_PGA_VAR_1CH = 0, // 1ch/1000MSPS
    ADC250_PGA_VAR_2CH = 1, // 2ch/500MSPS
    ADC250_PGA_VAR_4CH = 2, // 4ch/250MSPS
    ADC250_PGA_VAR_ERR = 3, // Check contact between baseboard and amplifier
};

/* General device specs */
enum
{
    ADC250_NUM_ADCS   = 4, // ==NUM_LINES in ADC250, but still ==4 in ADC1000, where NUM_LINES=1
    ADC250_NUM_LINES  = 4,
    ADC250_MAX_NUMPTS = 783155, // =783173-18
};
enum
{
    ADC250_MIN_VALUE = -32767, /*!!! Should check!!! */
    ADC250_MAX_VALUE = +32767,
};


#endif /* __ADC250_DRV_I_H */
