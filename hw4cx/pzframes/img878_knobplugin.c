#include "vcamimg_knobplugin.h"
#include "MotifKnobsP.h"

#include "img878_gui.h"
#include "img878_knobplugin.h"


static int IMG878_Create_m(DataKnob k, CxWidget parent)
{
    return VcamimgKnobpluginDoCreate(k, parent,
                                     NULL,
                                     img878_get_gui_dscr(),
                                     NULL, NULL);
}

dataknob_pzfr_vmt_t img878_pzfr_vmt =
{
    {DATAKNOB_PZFR, "img878",
        sizeof(vcamimg_knobplugin_t), NULL,
        0,
        IMG878_Create_m,  NULL/*Destroy*/,
        MotifKnobs_NoColorize_m, PzframeKnobpluginHandleCmd_m}
};
