#include <X11/Intrinsic.h>
#include <Xm/Xm.h>

#include <Xm/Form.h>
#include <Xm/Label.h>
#include <Xm/Text.h>
#include <Xm/ToggleB.h>
#include "IncDecB.h"

#include "Xh.h"
#include "Xh_globals.h"
#include "datatreeP.h"
#include "Knobs.h"
#include "KnobsP.h"
#include "MotifKnobsP.h"

#include "MotifKnobs_inttext_knob.h"


//////////////////////////////////////////////////////////////////////
#define XK_MISCELLANY
#define XK_LATIN1
#include <X11/keysymdef.h>

#include "misc_macros.h"
#include "misclib.h"

static void NEW_TextChangeCB(Widget     w,
                         XtPointer  closure,
                         XtPointer  call_data)
{
  DataKnob             k    = (DataKnob)              closure;
  XmAnyCallbackStruct *info = (XmAnyCallbackStruct *) call_data;
  int                  iv;
  
    /* Do a CANCEL if losing focus */
    if (info->reason != XmCR_ACTIVATE)
    {
        cancel_knob_editing(k);
        return;
    }

    if (MotifKnobs_ExtractIntValue(w, &iv))
    {
        store_knob_undo_value (k);
        MotifKnobs_SetControlValue(k, iv, 1);
    }
}

static void NEW_TextUserCB(Widget     w __attribute__((unused)),
                       XtPointer  closure,
                       XtPointer  call_data)
{
  DataKnob                    k    = (DataKnob)                     closure;
  XmTextVerifyCallbackStruct *info = (XmTextVerifyCallbackStruct *) call_data;
    
    if (info->reason == XmCR_MOVING_INSERT_CURSOR ||
        info->event  != NULL)
        user_began_knob_editing(k);
}

static void NEW_HandleTextUpDown(Widget        w,
                             DataKnob      k,
                             unsigned int  state,
                             Boolean       up)
{
  int           iv;
  int           incdec_step;

    if (!MotifKnobs_ExtractIntValue(w, &iv)) return;

    if (k->u.k.num_params > DATAKNOB_PARAM_STEP)
        incdec_step = k->u.k.params[DATAKNOB_PARAM_STEP].value;
    else
        incdec_step = 1;

    if (state & MOD_BIGSTEPS_MASK)  incdec_step *= 10;

    if (up)
        iv += incdec_step;
    else
        iv -= incdec_step;

    store_knob_undo_value(k);
    MotifKnobs_SetControlValue(k, iv, 1);
}

static void NEW_TextKbdHandler(Widget     w,
                           XtPointer  closure,
                           XEvent    *event,
                           Boolean   *continue_to_dispatch)
{
  DataKnob      k  = (DataKnob)    closure;
  XKeyEvent    *ev = (XKeyEvent *) event;

  KeySym        ks;
  Modifiers     mr;

  Boolean       up;

    if (event->type != KeyPress) return;
    XtTranslateKeycode(XtDisplay(w), ev->keycode, ev->state, &mr, &ks);

    /* [Cancel] */
    if ((ks == XK_Escape  ||  ks == osfXK_Cancel)  &&
        (ev->state & (ShiftMask | ControlMask | Mod1Mask)) == 0
        &&  k->usertime != 0)
    {
        cancel_knob_editing(k);
        *continue_to_dispatch = False;
        return;
    }

    /* [Undo] */
    if (
        ((ks == XK_Z  ||  ks == XK_z)  &&  (ev->state & ControlMask) != 0)
        ||
        ((ks == XK_BackSpace  ||  ks == osfXK_BackSpace)  &&  (ev->state & Mod1Mask) != 0)
       )
    {
        perform_knob_undo(k);
        *continue_to_dispatch = False;
        return;
    }
    
    /* [Up] or [Down] */
    if      (ks == XK_Up    ||  ks == XK_KP_Up    ||  ks == osfXK_Up)   up = True;
    else if (ks == XK_Down  ||  ks == XK_KP_Down  ||  ks == osfXK_Down) up = False;
    else return;

    *continue_to_dispatch = False;

    NEW_HandleTextUpDown(w, k, ev->state, up);
}

static void NEW_TextWheelHandler(Widget     w,
                             XtPointer  closure,
                             XEvent    *event,
                             Boolean   *continue_to_dispatch)
{
  DataKnob             k  = (DataKnob)              closure;
  XButtonPressedEvent *ev = (XButtonPressedEvent *) event;
  Boolean              up;

    if      (ev->button == Button4) up = True;
    else if (ev->button == Button5) up = False;
    else return;

    *continue_to_dispatch = False;

    NEW_HandleTextUpDown(w, k, ev->state, up);
}
//////////////////////////////////////////////////////////////////////
static
Widget NEW_MotifKnobs_CreateIntTextValue(DataKnob k, Widget parent)
{
  Widget  w;
  int     columns = GetTextColumns(k->u.k.dpyfmt, k->u.k.units);

    w = MotifKnobs_MakeTextWidget(parent, False, columns);
    
    return w;
}

static
Widget NEW_MotifKnobs_CreateIntTextInput(DataKnob k, Widget parent)
{
  Widget  w;
  int     columns = GetTextColumns(k->u.k.dpyfmt, NULL) + 1;

    w = MotifKnobs_MakeTextWidget(parent, True,  columns);

    XtAddCallback(w, XmNactivateCallback,     NEW_TextChangeCB, (XtPointer)k);
    XtAddCallback(w, XmNlosingFocusCallback,  NEW_TextChangeCB, (XtPointer)k);
    
    XtAddCallback(w, XmNmodifyVerifyCallback, NEW_TextUserCB,   (XtPointer)k);
    XtAddCallback(w, XmNmotionVerifyCallback, NEW_TextUserCB,   (XtPointer)k);
    
    MotifKnobs_SetTextCursorCallback(w);
    
    XtAddEventHandler(w, KeyPressMask,    False, NEW_TextKbdHandler,   (XtPointer)k);
    XtAddEventHandler(w, ButtonPressMask, False, NEW_TextWheelHandler, (XtPointer)k);

    return w;
}

static
void   NEW_MotifKnobs_SetIntTextString  (DataKnob k, Widget w, double v, const char *int_dpyfmt)
{
  char      buf[1000];
  int       len;
  int       frsize;
  char     *p;
  Boolean   ed = XmTextGetEditable(w);

    len = snprintf(buf, sizeof(buf), int_dpyfmt, (int)v);
    if (len < 0  ||  (size_t)len > sizeof(buf)-1) buf[len = sizeof(buf)-1] = '\0';
    p = buf + len;
    while (p > buf  &&  *(p - 1) == ' ') *--p = '\0'; /* Trim trailing spaces */
    if (!ed  &&  k->u.k.units != NULL)
    {
        frsize = sizeof(buf)-1 - len;
        if (frsize > 0) snprintf(buf + len, frsize, "%s", k->u.k.units);
    }
    
    p = buf;
    if (ed)
        while (*p == ' ') p++; /* Skip spaces */
    XmTextSetString(w, p);
}
//////////////////////////////////////////////////////////////////////
typedef struct
{
    int     has_label;
    int     is_gpbl;
    int     no_incdec;
    int     no_units;
    int     is_compact;
    
    Widget  label;
    Widget  grpmrk;
    Widget  text;
    Widget  incdec;
    Widget  units;

    int     defcols_obtained;
    Pixel   label_deffg;
    Pixel   label_defbg;
    Pixel   grpmrk_deffg;
    Pixel   grpmrk_defbg;
    Pixel   text_deffg;
    Pixel   text_defbg;
    Pixel   incdec_deffg;
    Pixel   incdec_defbg;
    Pixel   units_deffg;
    Pixel   units_defbg;

    char    int_dpyfmt[16];
} inttext_knob_privrec_t;

static psp_paramdescr_t text2inttextknobopts[] =
{
    PSP_P_FLAG("withlabel", inttext_knob_privrec_t, has_label,  1, 0),
    PSP_P_FLAG("nolabel",   inttext_knob_privrec_t, has_label,  0, 1),
    
    PSP_P_FLAG("noincdec",  inttext_knob_privrec_t, no_incdec,  1, 0),
    PSP_P_FLAG("withunits", inttext_knob_privrec_t, no_units,   0, 1),
    PSP_P_FLAG("nounits",   inttext_knob_privrec_t, no_units,   1, 0),
    
    PSP_P_FLAG("groupable", inttext_knob_privrec_t, is_gpbl,    1, 0),
    
    PSP_P_FLAG("compact",   inttext_knob_privrec_t, is_compact, 1, 0),

    PSP_P_END()
};

static void MarkChangeCB(Widget     w  __attribute__((unused)),
                         XtPointer  closure,
                         XtPointer  call_data)
{
  DataKnob                      k    = (DataKnob)                       closure;
  XmToggleButtonCallbackStruct *info = (XmToggleButtonCallbackStruct *) call_data;

    k->u.k.is_ingroup = info->set;
}

static void BlockButton2Handler(Widget     w        __attribute__((unused)),
                                XtPointer  closure  __attribute__((unused)),
                                XEvent    *event,
                                Boolean   *continue_to_dispatch)
{
  XButtonEvent *ev  = (XButtonEvent *)event;
  
    if (ev->type == ButtonPress  &&  ev->button == Button2)
        *continue_to_dispatch = False;
}


static int CreateIntTextKnob(DataKnob k, CxWidget parent)
{
  inttext_knob_privrec_t *me = (inttext_knob_privrec_t *)k->privptr;

  char                *ls;

  Widget               top;
  Widget               left;
  XmString             s;
  
  Dimension            isize;
  Dimension            mt;
  Dimension            mb;
  Dimension            lh;
  XmFontList           lfl;

  int     flags;
  int     width;
  int     precision;
  char    conv_c;

    /**/
    ParseDoubleFormat(k->u.k.dpyfmt, &flags, &width, &precision, &conv_c);
    if      (conv_c == 'a') conv_c = 'x';
    else if (conv_c == 'A') conv_c = 'X';
    else if (conv_c == 'e') conv_c = 'o';
    else if (conv_c == 'E') conv_c = 'o';
    else                    conv_c = 'd';

    CreateIntFormat(me->int_dpyfmt, sizeof(me->int_dpyfmt), 20, 10,
                    flags, width, precision, conv_c);

    /* Manage flags */
    if (!k->is_rw)
    {
        me->is_gpbl   = 0;
        me->no_incdec = 1;
        me->no_units  = 1;
    }

    if (me->is_gpbl) k->behaviour |= DATAKNOB_B_IS_GROUPABLE;
    if ((k->behaviour & DATAKNOB_B_IS_GROUPABLE) != 0  &&  k->is_rw)
        me->is_gpbl   = 1;

    if (k->u.k.units == NULL  ||  k->u.k.units[0] == '\0')
        me->no_units = 1;

    /* Container form... */
    k->w =
        ABSTRZE(XtVaCreateManagedWidget("textForm",
                                        xmFormWidgetClass,
                                        CNCRTZE(parent),
                                        XmNshadowThickness, 0,
                                        NULL));
    MotifKnobs_HookButton3(k, CNCRTZE(k->w));
    
    top = left = NULL;

    /* A label */
    if (me->has_label  &&  get_knob_label(k, &ls))
    {
        me->label = XtVaCreateManagedWidget("rowlabel",
                                            xmLabelWidgetClass,
                                            CNCRTZE(k->w),
                                            XmNlabelString,  s = XmStringCreateLtoR(ls, xh_charset),
                                            XmNalignment,    XmALIGNMENT_BEGINNING,
                                            NULL);
        XmStringFree(s);
        MotifKnobs_HookButton3(k, me->label);

        if (me->is_compact) top  = me->label;
        else                left = me->label;
    }

    /* "Group" mark */
    if (me->is_gpbl)
    {
        k->behaviour |= DATAKNOB_B_IS_GROUPABLE;
        
        /* Create a non-traversable checkbox */
        me->grpmrk = XtVaCreateManagedWidget("onoff_i",
                                             xmToggleButtonWidgetClass,
                                             CNCRTZE(k->w),
                                             XmNlabelString, s = XmStringCreateLtoR(" ", xh_charset),
                                             XmNtraversalOn, False,
                                             XmNvalue,       k->u.k.is_ingroup,
                                             XmNselectColor, XhGetColor(XH_COLOR_INGROUP),
                                             XmNdetailShadowThickness, 1,
                                             NULL);
        XmStringFree(s);
        if (top  != NULL) attachtop (me->grpmrk, top,  0);
        if (left != NULL) attachleft(me->grpmrk, left, 0);
        XtAddCallback(me->grpmrk, XmNvalueChangedCallback,
                      MarkChangeCB, (XtPointer)k);
        XtAddEventHandler(me->grpmrk, ButtonPressMask, False, BlockButton2Handler, NULL); // Temporary countermeasure against Motif Bug#1117

        /* Do all the voodoism with its height */
        XtVaGetValues(me->grpmrk,
                      XmNindicatorSize, &isize,
                      XmNmarginTop,     &mt,
                      XmNmarginBottom,  &mb,
                      XmNfontList,      &lfl,
                      NULL);
        
        s = XmStringCreate(" ", xh_charset);
        lh = XmStringHeight(lfl, s);
        XmStringFree(s);
        mt += lh / 2;
        mb += lh - lh / 2;
        
        XtVaSetValues(me->grpmrk, XmNindicatorSize, 1, NULL);
        XtVaSetValues(me->grpmrk,
                      XmNlabelString,   s = XmStringCreate("", xh_charset),
                      XmNindicatorSize, isize,
                      XmNmarginTop,     mt,
                      XmNmarginBottom,  mb,
                      XmNspacing,       0,
                      NULL);
        XmStringFree(s);

        MotifKnobs_HookButton3(k, me->grpmrk);

        left = me->grpmrk;
    }
    
    /* Text... */
    me->text = (k->is_rw? NEW_MotifKnobs_CreateIntTextInput
                        : NEW_MotifKnobs_CreateIntTextValue)(k, CNCRTZE(k->w));
    if (top  != NULL) attachtop (me->text, top,  0);
    if (left != NULL) attachleft(me->text, left, 0);
    left = me->text;

    MotifKnobs_HookButton3(k, me->text);
    
    /* Increment/decrement arrows */
    if (!(me->no_incdec))
    {
        me->incdec = XtVaCreateManagedWidget("textIncDec",
                                             xmIncDecButtonWidgetClass,
                                             CNCRTZE(k->w),
                                             XmNclientWidget,     me->text,
                                             XmNleftAttachment,   XmATTACH_WIDGET,
                                             XmNleftOffset,       0,
                                             XmNleftWidget,       me->text,
                                             XmNtopAttachment,    XmATTACH_OPPOSITE_WIDGET,
                                             XmNtopWidget,        me->text,
                                             XmNtopOffset,        0,
                                             NULL);

        left = me->incdec;
    }
    
    /* Optional "units" post-label for rw knobs */
    if (!(me->no_units))
    {
        me->units = XtVaCreateManagedWidget("rowlabel", xmLabelWidgetClass,
                                            CNCRTZE(k->w),
                                            XmNlabelString,  s = XmStringCreateLtoR(k->u.k.units, xh_charset),
                                            XmNalignment,    XmALIGNMENT_BEGINNING,
                                            XmNleftAttachment,   XmATTACH_WIDGET,
                                            XmNleftOffset,       0,
                                            XmNleftWidget,       left,
                                            XmNtopAttachment,    XmATTACH_OPPOSITE_WIDGET,
                                            XmNtopWidget,        left,
                                            XmNtopOffset,        0,
                                            NULL);
        XmStringFree(s);

        MotifKnobs_HookButton3(k, me->units);

        left = me->units;
    }

    MotifKnobs_TuneTextKnob(k, me->text, 0);
    MotifKnobs_DecorateKnob(k);

    return 0;
}

static void IntTextSetValue_m(DataKnob k, double v)
{
  inttext_knob_privrec_t *me = (inttext_knob_privrec_t *)k->privptr;

    NEW_MotifKnobs_SetIntTextString(k, me->text, v, me->int_dpyfmt);
}

static void IntTextKnobColorize_m(DataKnob k, knobstate_t newstate)
{
  inttext_knob_privrec_t *me = (inttext_knob_privrec_t *)k->privptr;
  Pixel                fg;
  Pixel                bg;
  
    if (!me->defcols_obtained)
    {
        if (me->label != NULL)
            XtVaGetValues(me->label,
                          XmNforeground, &(me->label_deffg),
                          XmNbackground, &(me->label_defbg),
                          NULL);
        if (me->grpmrk != NULL)
            XtVaGetValues(me->grpmrk,
                          XmNforeground, &(me->grpmrk_deffg),
                          XmNbackground, &(me->grpmrk_defbg),
                          NULL);
        XtVaGetValues(me->text,
                      XmNforeground, &(me->text_deffg),
                      XmNbackground, &(me->text_defbg),
                      NULL);
        if (me->incdec != NULL)
            XtVaGetValues(me->incdec,
                          XmNforeground, &(me->incdec_deffg),
                          XmNbackground, &(me->incdec_defbg),
                          NULL);
        if (me->units != NULL)
            XtVaGetValues(me->units,
                          XmNforeground, &(me->units_deffg),
                          XmNbackground, &(me->units_defbg),
                          NULL);
        
        me->defcols_obtained = 1;
    }

    MotifKnobs_ChooseColors(k->colz_style, newstate,
                            k->deffg, k->defbg, &fg, &bg);
    XtVaSetValues(CNCRTZE(k->w),
                  XmNforeground, fg,
                  XmNbackground, bg,
                  NULL);
    if (me->label != NULL)
    {
        MotifKnobs_ChooseColors(k->colz_style, newstate,
                                me->label_deffg, me->label_defbg, &fg, &bg);
        XtVaSetValues(me->label,
                      XmNforeground, fg,
                      XmNbackground, bg,
                      NULL);
    }
    if (me->grpmrk != NULL)
    {
        MotifKnobs_ChooseColors(k->colz_style, newstate,
                                me->grpmrk_deffg, me->grpmrk_defbg, &fg, &bg);
        XtVaSetValues(me->grpmrk,
                      XmNforeground, fg,
                      XmNbackground, bg,
                      NULL);
    }
    MotifKnobs_ChooseColors(k->colz_style, newstate,
                            me->text_deffg, me->text_defbg, &fg, &bg);
    XtVaSetValues(me->text,
                  XmNforeground, fg,
                  XmNbackground, bg,
                  NULL);
    if (me->incdec != NULL)
    {
        MotifKnobs_ChooseColors(k->colz_style, newstate,
                                me->incdec_deffg, me->incdec_defbg, &fg, &bg);
        XtVaSetValues(me->incdec,
                      XmNforeground, fg,
                      XmNbackground, bg,
                      NULL);
    }
    if (me->units != NULL)
    {
        MotifKnobs_ChooseColors(k->colz_style, newstate,
                                me->units_deffg, me->units_defbg, &fg, &bg);
        XtVaSetValues(me->units,
                      XmNforeground, fg,
                      XmNbackground, bg,
                      NULL);
    }
}

static void IntTextPropsChg_m(DataKnob k, DataKnob old_k  __attribute__((unused)))
{
  inttext_knob_privrec_t *me = (inttext_knob_privrec_t *)k->privptr;

    if (k->behaviour & DATAKNOB_B_IS_GROUPABLE  &&
        me->grpmrk != NULL                      &&
        XmToggleButtonGetState(me->grpmrk) != (k->u.k.is_ingroup != 0))
        XmToggleButtonSetState(me->grpmrk,     k->u.k.is_ingroup != 0, False);
}


dataknob_knob_vmt_t motifknobs_inttext_knob_vmt =
{
    {DATAKNOB_KNOB, "inttext",
        sizeof(inttext_knob_privrec_t), text2inttextknobopts,
        0,
        CreateIntTextKnob, MotifKnobs_CommonDestroy_m,
        IntTextKnobColorize_m, NULL},
    IntTextSetValue_m, IntTextPropsChg_m
};
