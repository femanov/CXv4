#ifndef __DL250_DRV_I_H
#define __DL250_DRV_I_H


enum
{
    DL250_NUMOUTPUTS = 24
};

enum // w24+i,w276i,r100i,w50i,r50i
{
    // 0-299: write channels, in groups of 25 (24 per-channel + 1 "all")
    DL250_CHAN_T_n_base            = 0,
    DL250_CHAN_ENABLE_n_base       = 25,   DL250_CHAN_ENABLE_24           = 49,
    DL250_CHAN_AUTODISABLE_n_base  = 50,   DL250_CHAN_AUTODISABLE_24      = 74,
    DL250_CHAN_ILK_MASK_n_base     = 75,   DL250_CHAN_ILK_MASK_24         = 99,
    DL250_CHAN_ILK_EXT_n_base      = 100,  DL250_CHAN_ILK_EXT_24          = 124,
    DL250_CHAN_ILK_DZ0_n_base      = 125,  DL250_CHAN_ILK_DZ0_24          = 149,
    DL250_CHAN_ILK_DZ1_n_base      = 150,  DL250_CHAN_ILK_DZ1_24          = 174,
    DL250_CHAN_ILK_DZ2_n_base      = 175,  DL250_CHAN_ILK_DZ2_24          = 199,
    DL250_CHAN_ILK_DZ3_n_base      = 200,  DL250_CHAN_ILK_DZ3_24          = 224,
    DL250_CHAN_START_TYPE_n_base   = 225,  DL250_CHAN_START_TYPE_ALL      = 249,
    DL250_CHAN_START_INPUT_n_base  = 250,  DL250_CHAN_START_INPUT_ALL     = 274,
    DL250_CHAN_unused_wg11_n_base  = 275,

    // 300-399: read channels, in groups of 25 (24 per-channel + 1 "all")
    DL250_CHAN_READY_n_base        = 300,  DL250_CHAN_READY_24            = 324,
    DL250_CHAN_AILK_n_base         = 325,  DL250_CHAN_AILK_24             = 349,
    DL250_CHAN_unused_rg2_n_base   = 350,
    DL250_CHAN_unused_rg3_n_base   = 375,
                                           DL250_CHAN_MISC_STAT_first = DL250_CHAN_READY_n_base,
                                           DL250_CHAN_MISC_STAT_last  = DL250_CHAN_AILK_24,
                                           DL250_CHAN_MISC_STAT_count = DL250_CHAN_MISC_STAT_last  -
                                                                        DL250_CHAN_MISC_STAT_first + 1,

    // 400-449: various control channels
    DL250_CHAN_CLK_MODE            = 400,

    DL250_CHAN_DO_SHOT             = 410,
    DL250_CHAN_AUTO_SHOT           = 411,
    DL250_CHAN_AUTO_MSEC           = 412,

    // 450-499: various status channels
    DL250_CHAN_DEVICE_ID           = 450,  DL250_CHAN_HWINFO_first = DL250_CHAN_DEVICE_ID,
    DL250_CHAN_HW_VERSION          = 451,
    DL250_CHAN_RIO_PRESENT         = 452,
    DL250_CHAN_FRONT_PRESENT       = 453,
    DL250_CHAN_SW_VERSION          = 454,
    DL250_CHAN_UNIQ_ID             = 455,  
    DL250_CHAN_CAPABILITIES        = 456,
    DL250_CHAN_BASEADDR            = 457,
    DL250_CHAN_ADDRSPACE           = 458,
    DL250_CHAN_unused_459          = 459,  DL250_CHAN_HWINFO_last  = DL250_CHAN_unused_459,
                                           DL250_CHAN_HWINFO_count = DL250_CHAN_HWINFO_last  -
                                                                     DL250_CHAN_HWINFO_first + 1,

    DL250_CHAN_CLK_PLLOK           = 461,  DL250_CHAN_CLK_STAT_first = DL250_CHAN_CLK_PLLOK,
    DL250_CHAN_CLK_INTBAD          = 462,
    DL250_CHAN_CLK_EXTBAD          = 463,
    DL250_CHAN_CLK_SOURCE          = 464,  DL250_CHAN_CLK_STAT_last  = DL250_CHAN_CLK_SOURCE,

    DL250_CHAN_WAS_SHOT            = 470,
    //
    DL250_CHAN_AUTO_LEFT           = 475,

    DL250_NUMCHANS = 500
};

enum
{
    DL250_INPUT_PROG     = 0,
    DL250_INPUT_EXT      = 1,
    DL250_INPUT_SYNC     = 2,
    DL250_INPUT_TTL      = 3,
    DL250_INPUT_TTL_SYNC = 4,
    DL250_INPUT_PROG5    = 5,
    DL250_INPUT_PROG6    = 6,
    DL250_INPUT_PROG7    = 7,
};

enum
{
    DL250_CLK_MODE_INT50MHZ = 0,
    DL250_CLK_MODE_CLK125   = 1,
};


#endif /* __DL250_DRV_I_H */
