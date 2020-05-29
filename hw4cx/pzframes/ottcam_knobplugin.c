#include "vcamimg_knobplugin.h"
#include "MotifKnobsP.h"

#include "ottcam_gui.h"
#include "ottcam_knobplugin.h"


static int OTTCAM_Create_m(DataKnob k, CxWidget parent)
{
    return VcamimgKnobpluginDoCreate(k, parent,
                                     NULL,
                                     ottcam_get_gui_dscr(),
                                     NULL, NULL);
}

dataknob_pzfr_vmt_t ottcam_pzfr_vmt =
{
    {DATAKNOB_PZFR, "ottcam",
        sizeof(vcamimg_knobplugin_t), NULL,
        0,
        OTTCAM_Create_m,  NULL/*Destroy*/,
        MotifKnobs_NoColorize_m, PzframeKnobpluginHandleCmd_m}
};
