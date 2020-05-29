#include "fastadc_knobplugin.h"
#include "MotifKnobsP.h"

#include "f4226_gui.h"
#include "f4226_knobplugin.h"


static int F4226_Create_m(DataKnob k, CxWidget parent)
{
    return FastadcKnobpluginDoCreate(k, parent,
                                     NULL,
                                     f4226_get_gui_dscr(),
                                     NULL, NULL);
}

dataknob_pzfr_vmt_t f4226_pzfr_vmt =
{
    {DATAKNOB_PZFR, "f4226",
        sizeof(fastadc_knobplugin_t), NULL,
        0,
        F4226_Create_m,  NULL/*Destroy*/,
        MotifKnobs_NoColorize_m, PzframeKnobpluginHandleCmd_m}
};
