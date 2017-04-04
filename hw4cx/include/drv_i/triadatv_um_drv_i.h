#ifndef __TRIADATV_UM_DRV_I_H
#define __TRIADATV_UM_DRV_I_H


// w50i,r50i
enum
{
    TRIADATV_UM_CHAN_WR_base         = 0,
      TRIADATV_UM_CHAN_WR_count      = 50,

    TRIADATV_UM_CHAN_RD_base         = 50,
      TRIADATV_UM_CHAN_RD_count      = 50,
    TRIADATV_UM_CHAN_PROTO_VER       = TRIADATV_UM_CHAN_RD_base + 0,
    TRIADATV_UM_CHAN_UM_TYPE         = TRIADATV_UM_CHAN_RD_base + 1,
    TRIADATV_UM_CHAN_UM_NUMBER       = TRIADATV_UM_CHAN_RD_base + 2,
    TRIADATV_UM_CHAN_SERIAL          = TRIADATV_UM_CHAN_RD_base + 3,
    TRIADATV_UM_CHAN_P_OUT           = TRIADATV_UM_CHAN_RD_base + 4,
    TRIADATV_UM_CHAN_P_IN            = TRIADATV_UM_CHAN_RD_base + 5,
    TRIADATV_UM_CHAN_P_REFL          = TRIADATV_UM_CHAN_RD_base + 6,
    TRIADATV_UM_CHAN_T1              = TRIADATV_UM_CHAN_RD_base + 7,
    TRIADATV_UM_CHAN_T2              = TRIADATV_UM_CHAN_RD_base + 8,
    TRIADATV_UM_CHAN_KP              = TRIADATV_UM_CHAN_RD_base + 9,
    TRIADATV_UM_CHAN_PHASE           = TRIADATV_UM_CHAN_RD_base + 10,

    TRIADATV_UM_CHAN_FAULT_PN        = TRIADATV_UM_CHAN_RD_base + 11,
    TRIADATV_UM_CHAN_IS_FAULT        = TRIADATV_UM_CHAN_RD_base + 12,
    TRIADATV_UM_CHAN_DISP_PROC       = TRIADATV_UM_CHAN_RD_base + 13,
    TRIADATV_UM_CHAN_DISP_STATE      = TRIADATV_UM_CHAN_RD_base + 14,

    TRIADATV_UM_CHAN_STATUS12_n_base = TRIADATV_UM_CHAN_RD_base + 15,
      TRIADATV_UM_CHAN_STATUS12_n_count = 8,
    TRIADATV_UM_CHAN_S_FAN1          = TRIADATV_UM_CHAN_STATUS12_n_base + 0,
    TRIADATV_UM_CHAN_S_FAN2          = TRIADATV_UM_CHAN_STATUS12_n_base + 1,
    TRIADATV_UM_CHAN_S_FAN3          = TRIADATV_UM_CHAN_STATUS12_n_base + 2,
    TRIADATV_UM_CHAN_S_POWER_SUPPLY  = TRIADATV_UM_CHAN_STATUS12_n_base + 3,
    TRIADATV_UM_CHAN_S_FAST_PROT     = TRIADATV_UM_CHAN_STATUS12_n_base + 4,
    TRIADATV_UM_CHAN_S_INPUT_SIGNAL  = TRIADATV_UM_CHAN_STATUS12_n_base + 5,
    TRIADATV_UM_CHAN_S_CMDS_02_13    = TRIADATV_UM_CHAN_STATUS12_n_base + 6,
    TRIADATV_UM_CHAN_S_RESERVED7     = TRIADATV_UM_CHAN_STATUS12_n_base + 7,

    TRIADATV_UM_CHAN_NUM_CURRENTS    = TRIADATV_UM_CHAN_RD_base + 23,
    TRIADATV_UM_CHAN_I_n_base        = TRIADATV_UM_CHAN_RD_base + 24,
      TRIADATV_UM_CHAN_I_n_count       = 19,
//    TRIADATV_UM_CHAN_     = TRIADATV_UM_CHAN_RD_base + 43,

    TRIADATV_UM_CHAN_STATS_first = TRIADATV_UM_CHAN_PROTO_VER,
    TRIADATV_UM_CHAN_STATS_last  = TRIADATV_UM_CHAN_I_n_base +
                                   TRIADATV_UM_CHAN_I_n_count-1,

    TRIADATV_UM_NUMCHANS = 100
};


#endif /* __TRIADATV_UM_DRV_I_H */