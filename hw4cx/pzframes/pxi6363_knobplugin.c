#include "fastadc_knobplugin.h"
#include "MotifKnobsP.h"

#include "pxi6363_gui.h"
#include "pxi6363_knobplugin.h"


static int PXI6363_Create_m(DataKnob k, CxWidget parent)
{
    return FastadcKnobpluginDoCreate(k, parent,
                                     NULL,
                                     pxi6363_get_gui_dscr(),
                                     NULL, NULL);
}

dataknob_pzfr_vmt_t pxi6363_pzfr_vmt =
{
    {DATAKNOB_PZFR, "pxi6363",
        sizeof(fastadc_knobplugin_t), NULL,
        0,
        PXI6363_Create_m,  NULL/*Destroy*/,
        MotifKnobs_NoColorize_m, PzframeKnobpluginHandleCmd_m}
};
