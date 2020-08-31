#include "fastadc_knobplugin.h"
#include "MotifKnobsP.h"

#include "adc333_gui.h"
#include "adc333_knobplugin.h"


static int ADC333_Create_m(DataKnob k, CxWidget parent)
{
    return FastadcKnobpluginDoCreate(k, parent,
                                     NULL,
                                     adc333_get_gui_dscr(),
                                     NULL, NULL);
}

dataknob_pzfr_vmt_t adc333_pzfr_vmt =
{
    {DATAKNOB_PZFR, "adc333",
        sizeof(fastadc_knobplugin_t), NULL,
        0,
        ADC333_Create_m,  NULL/*Destroy*/,
        MotifKnobs_NoColorize_m, PzframeKnobpluginHandleCmd_m}
};
