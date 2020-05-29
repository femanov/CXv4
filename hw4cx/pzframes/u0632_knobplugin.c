#include "wirebpm_knobplugin.h"
#include "MotifKnobsP.h"

#include "u0632_gui.h"
#include "u0632_knobplugin.h"


static int U0632_Create_m(DataKnob k, CxWidget parent)
{
    return WirebpmKnobpluginDoCreate(k, parent,
                                     NULL,
                                     u0632_get_gui_dscr(),
                                     NULL, NULL);
}

dataknob_pzfr_vmt_t u0632_pzfr_vmt =
{
    {DATAKNOB_PZFR, "u0632",
        sizeof(wirebpm_knobplugin_t), NULL,
        0,
        U0632_Create_m,  NULL/*Destroy*/,
        MotifKnobs_NoColorize_m, PzframeKnobpluginHandleCmd_m}
};
