#include "fastadc_knobplugin.h"
#include "MotifKnobsP.h"

#include "adc502_gui.h"
#include "adc502_knobplugin.h"


static int ADC502_Create_m(DataKnob k, CxWidget parent)
{
    return FastadcKnobpluginDoCreate(k, parent,
                                     NULL,
                                     adc502_get_gui_dscr(),
                                     NULL, NULL);
}

dataknob_pzfr_vmt_t adc502_pzfr_vmt =
{
    {DATAKNOB_PZFR, "adc502",
        sizeof(fastadc_knobplugin_t), NULL,
        0,
        ADC502_Create_m,  NULL/*Destroy*/,
        MotifKnobs_NoColorize_m, PzframeKnobpluginHandleCmd_m}
};
