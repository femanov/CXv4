#include <X11/Intrinsic.h>
#include <Xm/Xm.h>
#include <Xm/Form.h>
#include <Xm/Label.h>
#include <Xm/Separator.h>


#include "vcamimg_data.h"
#include "vcamimg_gui.h"
#include "pzframe_cpanel_decor.h"

#include "img878_data.h"
#include "img878_gui.h"

#include "drv_i/img878_drv_i.h"


static Widget img878_mkctls(pzframe_gui_t           *gui,
                            vcamimg_type_dscr_t     *atd,
                            Widget                   parent,
                            pzframe_gui_mkstdctl_t   mkstdctl,
                            pzframe_gui_mkparknob_t  mkparknob)
{
  vcamimg_gui_t *vcamimg_gui = pzframe2vcamimg_gui(gui);

  Widget  cform;    // Controls form
  Widget  line1;

  Widget  w1;
  Widget  w2;

    /* 0. General layout */
    /* A container form */
    cform = XtVaCreateManagedWidget("form", xmFormWidgetClass, parent,
                                    XmNshadowThickness, 0,
                                    NULL);
    /* A single lines */
    line1 = XtVaCreateManagedWidget("form", xmFormWidgetClass, cform,
                                    XmNshadowThickness, 0,
                                    NULL);

    /* 1. Line 1: standard controls */
    /* A "commons" */
    w1 = mkstdctl(gui, line1, VCAMIMG_GUI_CTL_COMMONS, 0, 0);

    w2 = mkstdctl(gui, line1, VCAMIMG_GUI_CTL_DPYMODE, 0, 0);
    attachleft(w2, w1, MOTIFKNOBS_INTERKNOB_H_SPACING);
    w1 = w2;

    w2 = mkstdctl(gui, line1, VCAMIMG_GUI_CTL_NORMALIZE, 0, 0);
    attachleft(w2, w1, MOTIFKNOBS_INTERKNOB_H_SPACING);
    w1 = w2;

    w2 = mkstdctl(gui, line1, VCAMIMG_GUI_CTL_MAX_RED, 0, 0);
    attachleft(w2, w1, MOTIFKNOBS_INTERKNOB_H_SPACING);
    w1 = w2;

    w2 = mkstdctl(gui, line1, VCAMIMG_GUI_CTL_0_VIOLET, 0, 0);
    attachleft(w2, w1, MOTIFKNOBS_INTERKNOB_H_SPACING);
    w1 = w2;

    return cform;
}

pzframe_gui_dscr_t *img878_get_gui_dscr(void)
{
  static vcamimg_gui_dscr_t  dscr;
  static int                 dscr_inited;

    if (!dscr_inited)
    {
        VcamimgGuiFillStdDscr(&dscr,
                              pzframe2vcamimg_type_dscr(img878_get_type_dscr()));

        dscr.cpanel_loc = 0/*!!!BOTTOM!!!*/;
        dscr.mkctls     = img878_mkctls;

        dscr_inited = 1;
    }
    return &(dscr.gkd);
}
