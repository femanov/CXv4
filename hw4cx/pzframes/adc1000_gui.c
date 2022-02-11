#include <X11/Intrinsic.h>
#include <Xm/Xm.h>
#include <Xm/Form.h>
#include <Xm/Label.h>
#include <Xm/Separator.h>


#include "fastadc_data.h"
#include "fastadc_gui.h"
#include "pzframe_cpanel_decor.h"

#include "adc1000_data.h"
#include "adc1000_gui.h"

#include "drv_i/adc1000_drv_i.h"


static Widget adc1000_mkctls(pzframe_gui_t           *gui,
                             fastadc_type_dscr_t     *atd,
                             Widget                   parent,
                             pzframe_gui_mkstdctl_t   mkstdctl,
                             pzframe_gui_mkparknob_t  mkparknob)
{
  fastadc_gui_t *fastadc_gui = pzframe2fastadc_gui(gui);

  Widget  cform;    // Controls form
  Widget  commons;  // 
  Widget  dwgrid;   // Device-Wide-parameters grid
  Widget  vline;    // Separator line
  Widget  pcgrid;   // Per-Channel-parameters grid
  Widget  rpgrid;   // Repers grid
  Widget  ptsform;  // NUMPTS@PTSOFS form
  Widget  icform;   // Intra-cell form
  Widget  ovflform; // overflow[4] form
  Widget  ctlsubw;
  Widget  ctlform;
  Widget  verform;
  Widget  vergrid;
  Widget  ctlsep;
  Widget  pllform;
  Widget  pllgrid;
  Widget  clbform;
  Widget  clbgrid;

  Widget  w1;
  Widget  w2;

  int     x;
  int     y;
  int     nl;
  int     nr;

  char    spec[1000];

    /* 1. General layout */
    /* A container form */
    cform = XtVaCreateManagedWidget("form", xmFormWidgetClass, parent,
                                    XmNshadowThickness, 0,
                                    NULL);
    /* A "commons" */
    commons = mkstdctl(gui, cform, FASTADC_GUI_CTL_COMMONS, 0, 0);
    /* A grid for device-wide parameters */
    dwgrid = XhCreateGrid(cform, "grid");
    XhGridSetGrid   (dwgrid, 0, 0);
    XhGridSetSpacing(dwgrid, MOTIFKNOBS_INTERKNOB_H_SPACING, MOTIFKNOBS_INTERKNOB_V_SPACING);
    XhGridSetPadding(dwgrid, 0, 0);
    /* A separator line */
    vline = XtVaCreateManagedWidget("vline",
                                    xmSeparatorWidgetClass,
                                    cform,
                                    XmNorientation,     XmVERTICAL,
                                    NULL);
    /* A form for PTS@OFS */
    ptsform = XtVaCreateManagedWidget("form", xmFormWidgetClass, cform,
                                      XmNshadowThickness, 0,
                                      NULL);
    /* A commons<->ptsform "connector" */
    w1 = XtVaCreateManagedWidget("", widgetClass, cform,
                                 XmNwidth,       1,
                                 XmNheight,      1,
                                 XmNborderWidth, 0,
                                 NULL);
    attachleft (w1, commons, 0);
    attachright(w1, ptsform, 0);
    /* A grid for per-channel parameters */
    pcgrid = XhCreateGrid(cform, "grid");
    XhGridSetGrid   (pcgrid, 0, 0);
    XhGridSetSpacing(pcgrid, MOTIFKNOBS_INTERKNOB_H_SPACING, MOTIFKNOBS_INTERKNOB_V_SPACING);
    XhGridSetPadding(pcgrid, 0, 0);
    /* A grid for repers parameters */
    rpgrid = XhCreateGrid(cform, "grid");
    XhGridSetGrid   (rpgrid, 0, 0);
    XhGridSetSpacing(rpgrid, MOTIFKNOBS_INTERKNOB_H_SPACING, MOTIFKNOBS_INTERKNOB_V_SPACING);
    XhGridSetPadding(rpgrid, 0, 0);
    /**/
    /* Perform attachment */
    attachright (rpgrid,  NULL,    0);
    attachright (ptsform, rpgrid,  MOTIFKNOBS_INTERKNOB_H_SPACING);
    attachright (pcgrid,  rpgrid,  MOTIFKNOBS_INTERKNOB_H_SPACING);
    attachtop   (pcgrid,  ptsform, MOTIFKNOBS_INTERKNOB_V_SPACING);
    attachright (vline,   pcgrid,  MOTIFKNOBS_INTERKNOB_H_SPACING);
    attachtop   (vline,   ptsform, 0);
    attachbottom(vline,   NULL,    0);
    attachright (dwgrid,  vline,   MOTIFKNOBS_INTERKNOB_H_SPACING);
    attachtop   (dwgrid,  ptsform, MOTIFKNOBS_INTERKNOB_V_SPACING);
    attachleft  (commons, NULL,    0);
    // ovflform is attached below individually
    ovflform = XtVaCreateManagedWidget("form", xmFormWidgetClass, cform,
                                       XmNshadowThickness, 0,
                                       NULL);

    /* 2. Device-wide parameters */
    y = 0;

    /* Cmpr */
    MakeStdCtl(dwgrid, 1, y++, gui, mkstdctl, FASTADC_GUI_CTL_X_CMPR, 0, 0);

    /* Timing */
    MakeALabel (dwgrid, 0, y, "Timing");
    MakeParKnob(dwgrid, 1, y++,
                " look=selector rw label=''"
                " options=compact"
                " items='#TInt 50MHz\tFrontPanel\tBackplane'",
                gui, mkparknob, ADC1000_CHAN_TIMING);
    
    /* IStart */
    MakeALabel (dwgrid, 0, y, "Int. Start");
    MakeParKnob(dwgrid, 1, y++,
                " look=led rw label='Int. Start' options='panel'",
                gui, mkparknob, ADC1000_CHAN_ISTART);
    
    /* Wait time */
    MakeALabel (dwgrid, 0, y, "Wait time");
    MakeParKnob(dwgrid, 1, y++,
                " look=text rw"
                " options=''"
                " units=ms minnorm=0 maxnorm=1000000000 dpyfmt=%7.0f step=1000",
                gui, mkparknob, ADC1000_CHAN_WAITTIME);
    
    XhGridSetSize(dwgrid, 2, y);

    /* Misc. controls */
    ctlsubw = MakeSubwin(dwgrid, 0, y,
                         "Ctrl...", "Misc controls");

    /* Shot, Stop */
    icform = MakeAForm(dwgrid, 1, y++);
    XhGridSetChildFilling(icform, 1, 0);
    w1 = mkparknob(gui, icform,
                   " look=button rw label='Stop'",
                   ADC1000_CHAN_STOP);
    attachright(w1, NULL, 0);
    w2 = mkparknob(gui, icform,
                   " look=button rw label='Shot!'",
                   ADC1000_CHAN_SHOT);
    attachright(w2, w1, MOTIFKNOBS_INTERKNOB_H_SPACING);

    /* NUMPTS@PTSOFS */
    snprintf(spec, sizeof(spec),
             " look=text rw label='' step=100"
             " options=''"
             " minnorm=%d maxnorm=%d mindisp=%d maxdisp=%d dpyfmt='%%6.0f'",
             1, ADC1000_MAX_NUMPTS, 1, ADC1000_MAX_NUMPTS);
    w1 = mkparknob(gui, ptsform, spec, ADC1000_CHAN_NUMPTS);
    snprintf(spec, sizeof(spec),
             " look=text rw label='@' step=100"
             " options='withlabel'"
             " minnorm=%d maxnorm=%d mindisp=%d maxdisp=%d dpyfmt='%%6.0f'",
             0, ADC1000_MAX_NUMPTS-1, 0, ADC1000_MAX_NUMPTS-1);
    w2 = mkparknob(gui, ptsform, spec, ADC1000_CHAN_PTSOFS);
    attachleft(w2, w1, 0);

    /* 3. Per-channel parameters and repers */
    nl = 0;
    w1 = MakeStdCtl(pcgrid, 0, nl, gui, mkstdctl, FASTADC_GUI_CTL_LINE_LABEL, nl, 1);
    XhGridSetChildFilling  (w1, 1,      0);
    MakeStdCtl     (pcgrid, 1, nl, gui, mkstdctl, FASTADC_GUI_CTL_LINE_MAGN,  nl, 1);
    MakeParKnob    (pcgrid, 2, nl,
                    " look=selector rw label=''"
                    " options=align=right"
                    " items='#T0.5V\t1V\t2V\t4V'",
                    gui, mkparknob, ADC1000_CHAN_RANGE);

    for (nr = 0;
         nr < XH_PLOT_NUM_REPERS;
         nr++)
    {
        if (nl == 0)
        {
            icform = MakeAForm(rpgrid, nr, 0);
            w1 = mkstdctl(gui, icform, FASTADC_GUI_CTL_REPER_ONOFF, 0, nr);
            w2 = mkstdctl(gui, icform, FASTADC_GUI_CTL_REPER_TIME,  0, nr);
            attachleft(w2, w1, NULL);
        }
        MakeStdCtl(rpgrid, nr, nl + 1, gui, mkstdctl, FASTADC_GUI_CTL_REPER_VALUE, nl, nr);
    }
    nl = 1;
    XhGridSetSize(pcgrid, 4,                      nl);
    XhGridSetSize(rpgrid, XH_PLOT_NUM_REPERS, nl + 1);

    w1 = mkparknob(gui, cform,
                   " look=selector rw label='Trigger'"
                   " items='#TDisable\tInternal\tFront\tBack\tBack+Sync'",
                   ADC1000_CHAN_TRIG_TYPE);
    w2 = mkparknob(gui, cform,
                   " look=selector rw label='BackN'"
                   " items='#T0\t1\t2\t3\t4\t5\t6\t7'",
                   ADC1000_CHAN_TRIG_INPUT);
    attachleft(w1, vline,  MOTIFKNOBS_INTERKNOB_H_SPACING);
    attachleft(w2, w1,     MOTIFKNOBS_INTERKNOB_H_SPACING);
    attachtop (w1, pcgrid, MOTIFKNOBS_INTERKNOB_V_SPACING);
    attachtop (w2, pcgrid, MOTIFKNOBS_INTERKNOB_V_SPACING);

    attachleft(ovflform, vline, MOTIFKNOBS_INTERKNOB_H_SPACING);
    attachtop (ovflform, w1,    MOTIFKNOBS_INTERKNOB_V_SPACING);
    for (nl = 0, w1 = NULL;  nl < ADC1000_NUM_ADCS;  nl++)
    {
        snprintf(spec, sizeof(spec),
                 " look=led ro label='%d' options='color=red' tip='ADC%d overflow'",
                 nl, nl);
        w2 = mkparknob(gui, ovflform,
                       spec,
                       ADC1000_CHAN_OVERFLOW0 + nl);
        attachleft(w2, w1, w1 == NULL? 0 : MOTIFKNOBS_INTERKNOB_H_SPACING);
        w1 = w2;
    }

    /* 4. Controls subwindow */
    ctlform = XtVaCreateManagedWidget("form", xmFormWidgetClass, ctlsubw,
                                      XmNshadowThickness, 0,
                                      NULL);
    verform = XtVaCreateManagedWidget("form", xmFormWidgetClass, ctlform,
                                      XmNshadowThickness, 0,
                                      NULL);
    vergrid = MakeFreeGrid(verform);
    ctlsep  = XtVaCreateManagedWidget("vline",
                                      xmSeparatorWidgetClass,
                                      ctlform,
                                      XmNorientation,     XmVERTICAL,
                                      NULL);
    pllform = XtVaCreateManagedWidget("form", xmFormWidgetClass, ctlform,
                                      XmNshadowThickness, 0,
                                      NULL);
    pllgrid = MakeFreeGrid(pllform);
    clbform = XtVaCreateManagedWidget("form", xmFormWidgetClass, ctlform,
                                      XmNshadowThickness, 0,
                                      NULL);
    clbgrid = MakeFreeGrid(clbform);

    attachleft  (verform, NULL,    0);
    attachleft  (ctlsep,  verform, MOTIFKNOBS_INTERKNOB_H_SPACING);
    attachleft  (pllform, ctlsep,  MOTIFKNOBS_INTERKNOB_H_SPACING);
    attachtop   (pllform, NULL,    0);
    attachleft  (clbform, ctlsep,  MOTIFKNOBS_INTERKNOB_H_SPACING);
    attachtop   (clbform, pllform, MOTIFKNOBS_INTERKNOB_V_SPACING);
    attachtop   (ctlsep,  NULL,    0);
    attachbottom(ctlsep,  NULL,    0);

    // Version info
    w1 = mkparknob(gui, verform,
                   "look=inttext  ro label='Dev ID' options='withlabel' dpyfmt=%#10a",
                   ADC1000_CHAN_DEVICE_ID);
    w2 = mkparknob(gui, verform,
                   "look=selector ro label='Var' items='#T0:1ch/1000MSPS\t1:2ch/500MSPS\t2:4ch/250MSPS\t3:UNKNOWN/err'",
                   ADC1000_CHAN_PGA_VAR);
    attachtop(w1,      NULL, 0);
    attachtop(w2,      w1,   MOTIFKNOBS_INTERKNOB_V_SPACING);
    attachtop(vergrid, w2,   MOTIFKNOBS_INTERKNOB_V_SPACING);
    y = 0;
    MakeALabel (vergrid, 1, y, "Base");
    MakeALabel (vergrid, 2, y, "PGA");
    y++;
    MakeALabel (vergrid, 0, y, "SW ver");
    MakeParKnob(vergrid, 1, y, "look=text ro dpyfmt=%-5.0f", gui, mkparknob, ADC1000_CHAN_BASE_SW_VER);
    MakeParKnob(vergrid, 2, y, "look=text ro dpyfmt=%-5.0f", gui, mkparknob, ADC1000_CHAN_PGA_SW_VER);
    y++;
    MakeALabel (vergrid, 0, y, "HW ver");
    MakeParKnob(vergrid, 1, y, "look=text ro dpyfmt=%-5.0f", gui, mkparknob, ADC1000_CHAN_BASE_HW_VER);
    MakeParKnob(vergrid, 2, y, "look=text ro dpyfmt=%-5.0f", gui, mkparknob, ADC1000_CHAN_PGA_HW_VER);
    y++;
    MakeALabel (vergrid, 0, y, "Uniq ID");
    MakeParKnob(vergrid, 1, y, "look=text ro dpyfmt=%-5.0f", gui, mkparknob, ADC1000_CHAN_BASE_UNIQ_ID);
    MakeParKnob(vergrid, 2, y, "look=text ro dpyfmt=%-5.0f", gui, mkparknob, ADC1000_CHAN_PGA_UNIQ_ID);
    y++;
    XhGridSetSize(vergrid, 3, y);

    // PLL-related stuff
    x = 0;
    MakeParKnob(pllgrid, x++, 0, "look=selector ro label='PLL' items='fail\b\blit=red\tlocked\b\blit=green'",
                gui, mkparknob, ADC1000_CHAN_PLL_LOCKED);
    MakeParKnob(pllgrid, x++, 0, "look=text     rw label=' Prs#' options='withlabel,noincdec' dpyfmt=%2.0f",
                gui, mkparknob, ADC1000_CHAN_PLL_PRESET);
    MakeParKnob(pllgrid, x++, 0, "look=inttext  ro dpyfmt=%#10a",
                gui, mkparknob, ADC1000_CHAN_CUR_PLL1_CTRL);
    MakeParKnob(pllgrid, x++, 0, "look=inttext  ro dpyfmt=%#10a",
                gui, mkparknob, ADC1000_CHAN_CUR_PLL2_CTRL);
    XhGridSetSize(pllgrid, x, 1);

    // Calibrations
    MakeParKnob(clbgrid, 0, 0, " look=button rw label='Calibrate'",
                gui, mkparknob, ADC1000_CHAN_CALIBRATE);

    for (nl = 0;  nl < ADC1000_NUM_ADCS;  nl++)
    {
        /* Note:
               ADC250 can use lines' descr,
               but ADC1000 and ADC500 must use just "ADCn" (n=0...3) */
        snprintf(spec, sizeof(spec),
                 "ADC%d", nl);
        MakeALabel(clbgrid, 0, nl + 1, spec);

        if (nl == 0)
            MakeALabel(clbgrid, 1, 0, "!");
        MakeParKnob   (clbgrid, 1, nl + 1,
                       " look=led ro label='' options='color=red'",
                       gui, mkparknob, ADC1000_CHAN_CLB_FAIL0 + nl);

        if (nl == 0)
            MakeALabel(clbgrid, 2, 0, "DYN");
        MakeParKnob   (clbgrid, 2, nl + 1,
                       " look=text ro dpyfmt=%10.0f",
                       gui, mkparknob, ADC1000_CHAN_CLB_DYN0  + nl);

        if (nl == 0)
            MakeALabel(clbgrid, 3, 0, "ZERO");
        MakeParKnob   (clbgrid, 3, nl + 1,
                       " look=text ro dpyfmt=%10.0f",
                       gui, mkparknob, ADC1000_CHAN_CLB_ZERO0 + nl);

        if (nl == 0)
            MakeALabel(clbgrid, 4, 0, "GAIN");
        MakeParKnob   (clbgrid, 4, nl + 1,
                       " look=text ro dpyfmt=%10.0f",
                       gui, mkparknob, ADC1000_CHAN_CLB_GAIN0 + nl);
    }
    XhGridSetSize(clbgrid, 5, nl + 1);

    return cform;
}

pzframe_gui_dscr_t *adc1000_get_gui_dscr(void)
{
  static fastadc_gui_dscr_t  dscr;
  static int                 dscr_inited;

    if (!dscr_inited)
    {
        FastadcGuiFillStdDscr(&dscr,
                              pzframe2fastadc_type_dscr(adc1000_get_type_dscr()));

        dscr.def_plot_w = 700;
        dscr.min_plot_w = 100;
        dscr.max_plot_w = 3000;
        dscr.def_plot_h = 256;
        dscr.min_plot_h = 100;
        dscr.max_plot_h = 3000;

        dscr.cpanel_loc = XH_PLOT_CPANEL_AT_BOTTOM;
        dscr.mkctls     = adc1000_mkctls;

        dscr_inited = 1;
    }
    return &(dscr.gkd);
}
