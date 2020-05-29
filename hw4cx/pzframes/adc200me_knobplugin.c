#include "fastadc_knobplugin.h"
#include "MotifKnobsP.h"

#include "adc200me_gui.h"
#include "adc200me_knobplugin.h"


static int ADC200ME_Create_m(DataKnob k, CxWidget parent)
{
    return FastadcKnobpluginDoCreate(k, parent,
                                     NULL,
                                     adc200me_get_gui_dscr(),
                                     NULL, NULL);
}

dataknob_pzfr_vmt_t adc200me_pzfr_vmt =
{
    {DATAKNOB_PZFR, "adc200me",
        sizeof(fastadc_knobplugin_t), NULL,
        0,
        ADC200ME_Create_m,  NULL/*Destroy*/,
        MotifKnobs_NoColorize_m, PzframeKnobpluginHandleCmd_m}
};
