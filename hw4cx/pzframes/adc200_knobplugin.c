#include "fastadc_knobplugin.h"
#include "MotifKnobsP.h"

#include "adc200_gui.h"
#include "adc200_knobplugin.h"


static int ADC200_Create_m(DataKnob k, CxWidget parent)
{
    return FastadcKnobpluginDoCreate(k, parent,
                                     NULL,
                                     adc200_get_gui_dscr(),
                                     NULL, NULL);
}

dataknob_pzfr_vmt_t adc200_pzfr_vmt =
{
    {DATAKNOB_PZFR, "adc200",
        sizeof(fastadc_knobplugin_t), NULL,
        0,
        ADC200_Create_m,  NULL/*Destroy*/,
        MotifKnobs_NoColorize_m, PzframeKnobpluginHandleCmd_m}
};
