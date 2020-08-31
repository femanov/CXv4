#include "fastadc_knobplugin.h"
#include "MotifKnobsP.h"

#include "vsdc2_1_gui.h"
#include "vsdc2_1_knobplugin.h"


static int VSDC2_1_Create_m(DataKnob k, CxWidget parent)
{
    return FastadcKnobpluginDoCreate(k, parent,
                                     NULL,
                                     vsdc2_1_get_gui_dscr(),
                                     NULL, NULL);
}

dataknob_pzfr_vmt_t vsdc2_1_pzfr_vmt =
{
    {DATAKNOB_PZFR, "vsdc2_1",
        sizeof(fastadc_knobplugin_t), NULL,
        0,
        VSDC2_1_Create_m,  NULL/*Destroy*/,
        MotifKnobs_NoColorize_m, PzframeKnobpluginHandleCmd_m}
};
