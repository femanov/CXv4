#include"KnobsP.h"
#include"liuclient_knobset.h"

#include "two812ch_knobplugin.h"
#include "adc200me_knobplugin.h"
#include "adc812me_knobplugin.h"

static knobs_knobset_t liuclient_knobset =
{
    (dataknob_unif_vmt_t *[]){
        (dataknob_unif_vmt_t *)&two812ch_pzfr_vmt,
        (dataknob_unif_vmt_t *)&adc200me_pzfr_vmt,
        (dataknob_unif_vmt_t *)&adc812me_pzfr_vmt,
        NULL
    },
    NULL
};

void RegisterLiuclientKnobset(void)
{
    RegisterKnobset(&liuclient_knobset);
}
