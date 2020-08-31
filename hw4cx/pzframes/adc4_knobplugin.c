#include "fastadc_knobplugin.h"
#include "MotifKnobsP.h"

#include "adc4_gui.h"
#include "adc4_knobplugin.h"


static int ADC4_Create_m(DataKnob k, CxWidget parent)
{
    return FastadcKnobpluginDoCreate(k, parent,
                                     NULL,
                                     adc4_get_gui_dscr(),
                                     NULL, NULL);
}

dataknob_pzfr_vmt_t adc4_pzfr_vmt =
{
    {DATAKNOB_PZFR, "adc4",
        sizeof(fastadc_knobplugin_t), NULL,
        0,
        ADC4_Create_m,  NULL/*Destroy*/,
        MotifKnobs_NoColorize_m, PzframeKnobpluginHandleCmd_m}
};
