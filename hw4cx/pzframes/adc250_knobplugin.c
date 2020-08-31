#include "fastadc_knobplugin.h"
#include "MotifKnobsP.h"

#include "adc250_gui.h"
#include "adc250_knobplugin.h"


static int ADC250_Create_m(DataKnob k, CxWidget parent)
{
    return FastadcKnobpluginDoCreate(k, parent,
                                     NULL,
                                     adc250_get_gui_dscr(),
                                     NULL, NULL);
}

dataknob_pzfr_vmt_t adc250_pzfr_vmt =
{
    {DATAKNOB_PZFR, "adc250",
        sizeof(fastadc_knobplugin_t), NULL,
        0,
        ADC250_Create_m,  NULL/*Destroy*/,
        MotifKnobs_NoColorize_m, PzframeKnobpluginHandleCmd_m}
};
