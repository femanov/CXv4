#include <X11/Intrinsic.h>
#include <Xm/Xm.h>
#include <Xm/Form.h>
#include <Xm/Label.h>
#include <Xm/Separator.h>


#include "fastadc_data.h"
#include "fastadc_gui.h"
#include "pzframe_cpanel_decor.h"

#include "onei32l_data.h"
#include "onei32l_gui.h"

#include "onei32l_drv_i.h"


static Widget onei32l_mkctls(pzframe_gui_t           *gui,
                             fastadc_type_dscr_t     *atd,
                             Widget                   parent,
                             pzframe_gui_mkstdctl_t   mkstdctl,
                             pzframe_gui_mkparknob_t  mkparknob)
{
  Widget  cform;    // Controls form
  Widget  dwform;   // Device-Wide-parameters form
  Widget  rpgrid;   // Repers grid
  Widget  stgrdc;
  Widget  icform;   // Intra-cell form
  Widget  w1;
  Widget  w2;
  Widget  left;

  int     nr;

  char    spec[1000];

    /* 1. General layout */
    /* A container form */
    cform = XtVaCreateManagedWidget("form", xmFormWidgetClass, parent,
                                    XmNshadowThickness, 0,
                                    NULL);
    /* A form for device-wide parameters */
    dwform = XtVaCreateManagedWidget("form", xmFormWidgetClass, cform,
                                     XmNshadowThickness, 0,
                                     NULL);
    /* A grid for repers parameters */
    rpgrid = XhCreateGrid(cform, "grid");
    XhGridSetGrid   (rpgrid, 0, 0);
    XhGridSetSpacing(rpgrid, MOTIFKNOBS_INTERKNOB_H_SPACING, MOTIFKNOBS_INTERKNOB_V_SPACING);
    XhGridSetPadding(rpgrid, 0, 0);
    /* Statistics */
    stgrdc = XhCreateGrid(cform, "grid");
    XhGridSetGrid   (stgrdc, 0, 0);
    XhGridSetSpacing(stgrdc, MOTIFKNOBS_INTERKNOB_H_SPACING, MOTIFKNOBS_INTERKNOB_V_SPACING);
    XhGridSetPadding(stgrdc, 0, 0);
    /* Perform attachment */
    attachtop (rpgrid, dwform, MOTIFKNOBS_INTERKNOB_V_SPACING);
    attachtop (stgrdc, dwform, MOTIFKNOBS_INTERKNOB_V_SPACING);
    attachleft(stgrdc, rpgrid, MOTIFKNOBS_INTERKNOB_H_SPACING);

    /* A "commons" */
    w1 = mkstdctl(gui, dwform, FASTADC_GUI_CTL_COMMONS, 0, 0);
    attachleft  (w1, NULL,   0); left = w1;

    /* 2. Device-wide parameters */
    /* NUMPTS@PTSOFS */
    snprintf(spec, sizeof(spec),
             " look=text rw label='Numpoints' step=100"
             " options='withlabel'"
             " minnorm=%d maxnorm=%d mindisp=%d maxdisp=%d dpyfmt='%%4.0f'",
             1, ONEI32L_MAX_NUMPTS, 1, ONEI32L_MAX_NUMPTS);
    w1 = mkparknob(gui, dwform, spec, ONEI32L_CHAN_NUMPTS);
    attachleft(w1, left, MOTIFKNOBS_INTERKNOB_H_SPACING); left = w1;

    /* Cmpr, magn */
    w1 = mkstdctl(gui, dwform, FASTADC_GUI_CTL_X_CMPR,    0, 0);
    attachleft(w1, left, MOTIFKNOBS_INTERKNOB_H_SPACING); left = w1;
    w1 = mkstdctl(gui, dwform, FASTADC_GUI_CTL_LINE_MAGN, 0, 0);
    attachleft(w1, left, MOTIFKNOBS_INTERKNOB_H_SPACING); left = w1;

    /* Repers themselves */
    for (nr = 0;
         nr < XH_PLOT_NUM_REPERS;
         nr++)
    {
        w1 = mkstdctl(gui, dwform, FASTADC_GUI_CTL_REPER_ONOFF, 0, nr);
        attachleft(w1, left, MOTIFKNOBS_INTERKNOB_H_SPACING); left = w1;
        w1 = mkstdctl(gui, dwform, FASTADC_GUI_CTL_REPER_TIME,  0, nr);
        attachleft(w1, left, 0);                              left = w1;
        w1 = mkstdctl(gui, dwform, FASTADC_GUI_CTL_REPER_VALUE, 0, nr);
        attachleft(w1, left, 0);                              left = w1;
    }

    return cform;
}

pzframe_gui_dscr_t *onei32l_get_gui_dscr(void)
{
  static fastadc_gui_dscr_t  dscr;
  static int                 dscr_inited;

    if (!dscr_inited)
    {
        FastadcGuiFillStdDscr(&dscr,
                              pzframe2fastadc_type_dscr(onei32l_get_type_dscr()));

        dscr.def_plot_w = 1000;
        dscr.min_plot_w = 100;
        dscr.max_plot_w = 3000;
        dscr.def_plot_h = 300;
        dscr.min_plot_h = 100;
        dscr.max_plot_h = 3000;

        dscr.cpanel_loc = XH_PLOT_CPANEL_AT_TOP;
        dscr.mkctls     = onei32l_mkctls;

        dscr_inited = 1;
    }
    return &(dscr.gkd);
}
