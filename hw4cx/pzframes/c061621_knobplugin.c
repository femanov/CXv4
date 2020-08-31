#include "fastadc_knobplugin.h"
#include "MotifKnobsP.h"

#include "c061621_gui.h"
#include "c061621_knobplugin.h"


static int C061621_Create_m(DataKnob k, CxWidget parent)
{
    return FastadcKnobpluginDoCreate(k, parent,
                                     NULL,
                                     c061621_get_gui_dscr(),
                                     NULL, NULL);
}

dataknob_pzfr_vmt_t c061621_pzfr_vmt =
{
    {DATAKNOB_PZFR, "c061621",
        sizeof(fastadc_knobplugin_t), NULL,
        0,
        C061621_Create_m,  NULL/*Destroy*/,
        MotifKnobs_NoColorize_m, PzframeKnobpluginHandleCmd_m}
};
