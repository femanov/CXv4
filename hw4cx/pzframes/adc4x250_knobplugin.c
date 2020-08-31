#include "fastadc_knobplugin.h"
#include "MotifKnobsP.h"

#include "adc4x250_gui.h"
#include "adc4x250_knobplugin.h"


static int ADC4X250_Create_m(DataKnob k, CxWidget parent)
{
    return FastadcKnobpluginDoCreate(k, parent,
                                     NULL,
                                     adc4x250_get_gui_dscr(),
                                     NULL, NULL);
}

dataknob_pzfr_vmt_t adc4x250_pzfr_vmt =
{
    {DATAKNOB_PZFR, "adc4x250",
        sizeof(fastadc_knobplugin_t), NULL,
        0,
        ADC4X250_Create_m,  NULL/*Destroy*/,
        MotifKnobs_NoColorize_m, PzframeKnobpluginHandleCmd_m}
};
