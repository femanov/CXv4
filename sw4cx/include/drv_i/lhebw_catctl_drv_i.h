#ifndef __LHEBW_CATCTL_DRV_I_H
#define __LHEBW_CATCTL_DRV_I_H

// w50i,r50i
enum
{
    LHEBW_CATCTL_CHAN_WR_n_base = 0,

    LHEBW_CATCTL_CHAN_ISET = LHEBW_CATCTL_CHAN_WR_n_base + 0,
    LHEBW_CATCTL_CHAN_ILIM = LHEBW_CATCTL_CHAN_WR_n_base + 1,

    LHEBW_CATCTL_CHAN_ENABLE = LHEBW_CATCTL_CHAN_WR_n_base + 40,

    LHEBW_CATCTL_CHAN_RD_n_base = 50,

    LHEBW_CATCTL_CHAN_ISET_CTL = LHEBW_CATCTL_CHAN_RD_n_base + 0,  LHEBW_CATCTL_CHAN_ADC_first = LHEBW_CATCTL_CHAN_ISET_CTL,
    LHEBW_CATCTL_CHAN_ILIM_CTL = LHEBW_CATCTL_CHAN_RD_n_base + 1,
    LHEBW_CATCTL_CHAN_IVIP_CTL = LHEBW_CATCTL_CHAN_RD_n_base + 2,  LHEBW_CATCTL_CHAN_ADC_last  = LHEBW_CATCTL_CHAN_IVIP_CTL,
                                                                   LHEBW_CATCTL_CHAN_ADC_count = LHEBW_CATCTL_CHAN_ADC_last  -
                                                                                                 LHEBW_CATCTL_CHAN_ADC_first + 1,

    LHEBW_CATCTL_CHAN_VDEV_STATE   = 99,
    
    LHEBW_CATCTL_NUMCHANS       = 100,
        
}

#endif /* __LHEBW_CATCTL_DRV_I_H */
