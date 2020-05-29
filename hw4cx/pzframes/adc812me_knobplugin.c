#include "fastadc_knobplugin.h"
#include "MotifKnobsP.h"

#include "adc812me_gui.h"
#include "adc812me_knobplugin.h"


static int ADC812ME_Create_m(DataKnob k, CxWidget parent)
{
    return FastadcKnobpluginDoCreate(k, parent,
                                     NULL,
                                     adc812me_get_gui_dscr(),
                                     NULL, NULL);
}

dataknob_pzfr_vmt_t adc812me_pzfr_vmt =
{
    {DATAKNOB_PZFR, "adc812me",
        sizeof(fastadc_knobplugin_t), NULL,
        0,
        ADC812ME_Create_m,  NULL/*Destroy*/,
        MotifKnobs_NoColorize_m, PzframeKnobpluginHandleCmd_m}
};
