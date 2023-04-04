#ifndef __GIN25_DRV_I_H
#define __GIN25_DRV_I_H


enum {GIN25_NUM_LINES = 3};


// w50i,r50i
enum
{
    GIN25_CHAN_WR_base           = 0,
      GIN25_CHAN_WR_count        = 50,

    GIN25_CHAN_TIMECODE          = GIN25_CHAN_WR_base + 0,

    GIN25_CHAN_U_SET_n_base      = GIN25_CHAN_WR_base + 10,
    GIN25_CHAN_CHARGE_CTL_n_base = GIN25_CHAN_WR_base + 13,
    GIN25_CHAN_RST_ERR_n_base    = GIN25_CHAN_WR_base + 16,

    GIN25_CHAN_RD_base           = 50,
      GIN25_CHAN_RD_count        = 50,

    GIN25_CHAN_U_MES_n_base      = GIN25_CHAN_RD_base + 0,
    GIN25_CHAN_STATUS_n_base     = GIN25_CHAN_RD_base + 3,  GIN25_CHAN_STATUS_first = GIN25_CHAN_STATUS_n_base,
    GIN25_CHAN_ERR8B_n_base      = GIN25_CHAN_RD_base + 6,  GIN25_CHAN_ERROR_first  = GIN25_CHAN_ERR8B_n_base,
    GIN25_CHAN_ERRBx_n_base      = GIN25_CHAN_RD_base + 9,
      GIN25_CHAN_ERRBx_pl_count  = 8, // "pl" -- Per Line
                                                            GIN25_CHAN_STATUS_last  = GIN25_CHAN_ERRBx_n_base + GIN25_CHAN_ERRBx_pl_count * GIN25_NUM_LINES - 1,
                                                            GIN25_CHAN_STATUS_count = GIN25_CHAN_STATUS_last - GIN25_CHAN_STATUS_first + 1,
                                                            GIN25_CHAN_ERROR_last   = GIN25_CHAN_ERRBx_n_base + GIN25_CHAN_ERRBx_pl_count * GIN25_NUM_LINES - 1,
                                                            GIN25_CHAN_ERROR_count  = GIN25_CHAN_ERROR_last  - GIN25_CHAN_ERROR_first + 1,

    GIN25_CHAN_HW_VER            = 98,
    GIN25_CHAN_SW_VER            = 99,



    GIN25_NUMCHANS = 100
};


#endif /* __GIN25_DRV_I_H */
