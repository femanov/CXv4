#include"KnobsP.h"
#include"pzframeclient_knobset.h"

#include "adc200me_knobplugin.h"
#include "adc812me_knobplugin.h"
#include "adc200_knobplugin.h"
#include "adc502_knobplugin.h"
#include "adc333_knobplugin.h"
#include "adc4_knobplugin.h"
#include "c061621_knobplugin.h"
#include "f4226_knobplugin.h"
#include "vsdc2_1_knobplugin.h"
#include "vsdc2_2_knobplugin.h"
#include "pxi6363_knobplugin.h"
#include "adc4x250_knobplugin.h"
#include "adc250_knobplugin.h"
#include "ottcam_knobplugin.h"
#include "img878_knobplugin.h"
#include "u0632_knobplugin.h"

static knobs_knobset_t pzframeclient_knobset =
{
    (dataknob_unif_vmt_t *[]){
        (dataknob_unif_vmt_t *)&adc200me_pzfr_vmt,
        (dataknob_unif_vmt_t *)&adc812me_pzfr_vmt,
        (dataknob_unif_vmt_t *)&adc200_pzfr_vmt,
        (dataknob_unif_vmt_t *)&adc502_pzfr_vmt,
        (dataknob_unif_vmt_t *)&adc333_pzfr_vmt,
        (dataknob_unif_vmt_t *)&adc4_pzfr_vmt,
        (dataknob_unif_vmt_t *)&c061621_pzfr_vmt,
        (dataknob_unif_vmt_t *)&f4226_pzfr_vmt,
        (dataknob_unif_vmt_t *)&vsdc2_1_pzfr_vmt,
        (dataknob_unif_vmt_t *)&vsdc2_2_pzfr_vmt,
        (dataknob_unif_vmt_t *)&pxi6363_pzfr_vmt,
        (dataknob_unif_vmt_t *)&adc4x250_pzfr_vmt,
        (dataknob_unif_vmt_t *)&adc250_pzfr_vmt,
        (dataknob_unif_vmt_t *)&ottcam_pzfr_vmt,
        (dataknob_unif_vmt_t *)&img878_pzfr_vmt,
        (dataknob_unif_vmt_t *)&u0632_pzfr_vmt,
        NULL
    },
    NULL
};

void RegisterPzframeclientKnobset(void)
{
    RegisterKnobset(&pzframeclient_knobset);
}
