#include "Xh_includes.h"


enum {
    BUTTONS_SPACING = 4,
    BUTTONS_PADDING = 2,

    SEPARATOR_SIZE = 8,

    MAX_BUTTONS = 100
};

static void toolCB(Widget     w,
                   XtPointer  closure,
                   XtPointer  call_data  __attribute__((unused)))
{
  XhWindow       window = XhWindowOf(ABSTRZE(w));
  int            b      = ptr2lint(closure);
  int            info_int;

    if (window->tbarButtons[b].type == XhACT_COMMAND)
        info_int = 0;
    else
    {
        info_int = window->tbarButtons[b].pressed = !(window->tbarButtons[b].pressed);
        XhInvertButton(window->tbarButtons[b].btn);
    }

    if (window->commandproc != NULL)
        window->commandproc(window, window->tbarButtons[b].cmd, info_int);
}

void XhCreateToolbar(XhWindow window, xh_actdescr_t  buttons[], int use_mini)
{
  Dimension       fst;
  Widget          toolBar;
  Widget          container;
  Widget          tools[MAX_BUTTONS];
  Widget          toolSepr;
  Widget          filler;

  XmString        s;

  char          **pixmap;
  int             padding;

  int             num_buttons;

  int             b;
  int             count;
  int             firstflushed;
  int             leftlimit;

    if (buttons == NULL) return;

    padding = !use_mini? BUTTONS_PADDING : 0;

    /* First, create a container */
    XtVaGetValues(window->mainForm, XmNshadowThickness, &fst, NULL);
  
    toolBar = XtVaCreateManagedWidget("toolBar", xmFormWidgetClass, window->mainForm,
                                      XmNleftAttachment,  XmATTACH_FORM,
                                      XmNrightAttachment, XmATTACH_FORM,
                                      XmNtopAttachment,   XmATTACH_FORM,
                                      XmNleftOffset,      fst,
                                      XmNrightOffset,     fst,
                                      XmNtopOffset,       fst,
                                      NULL);
    window->toolBar = toolBar;

    container = XtVaCreateManagedWidget("zzz", xmFormWidgetClass, toolBar,
                                        XmNshadowThickness, 0,
                                        XmNleftAttachment,  XmATTACH_FORM,
                                        XmNleftOffset,      padding,
                                        XmNrightAttachment, XmATTACH_FORM,
                                        XmNrightOffset,     padding,
                                        XmNtopAttachment,   XmATTACH_FORM,
                                        XmNtopOffset,       padding,
                                        NULL);
    window->toolHolder = container;

    /* Create a working-copy of toolbar description */
    /* Obtain number of buttons[] */
    for (num_buttons = 0;  buttons[num_buttons].type != XhACT_END;  num_buttons++);
    num_buttons++;  // For XhACT_END
    window->tbarButtons = XtMalloc(sizeof(XhToolBtnInfo) * num_buttons);
    bzero(window->tbarButtons, sizeof(XhToolBtnInfo) * num_buttons);
    for (b = 0;  b < num_buttons;  b++)
    {
        window->tbarButtons[b].type = buttons[b].type;
        window->tbarButtons[b].cmd  = buttons[b].cmd;
    }

    for (b = 0, firstflushed = -1, count = 0;
         b < MAX_BUTTONS  &&  buttons[b].type != XhACT_END;
         b++)
    {
        if (buttons[b].type == XhACT_FILLER)
        {
            firstflushed = count;
            goto NEXT_BUTTON;
        }

        switch (buttons[b].type)
        {
            case XhACT_SEPARATOR:
#if 1
                // Case 1: a vertical line
                tools[count] = XtVaCreateManagedWidget("toolSepr", xmSeparatorWidgetClass, container,
                                                       XmNorientation,      XmVERTICAL,
                                                       XmNtopAttachment,    XmATTACH_FORM,
                                                       XmNbottomAttachment, XmATTACH_FORM,
                                                       NULL);
#else
                // Case 2: just a spacer
                tools[count] = XtVaCreateManagedWidget("toolSepr", widgetClass, container,
                                                       XmNborderWidth,     0,
                                                       XmNwidth,           4,
                                                       XmNheight,          1,
                                                       NULL);
#endif
                break;

            case XhACT_LEDS:
                tools[count] = XtVaCreateManagedWidget("alarmLeds", xmFormWidgetClass, container,
                                                       XmNtraversalOn,     False,
                                                       XmNshadowThickness, 0,
                                                       NULL);
                attachtop   (tools[count], NULL, 0);
                attachbottom(tools[count], NULL, 0);

                window->alarmLeds = tools[count];
                break;

            case XhACT_LABEL:
                tools[count] = XtVaCreateManagedWidget(!use_mini?"toolLabel":"miniToolLabel", xmLabelWidgetClass, container,
                                                       XmNlabelString, s = XmStringCreateLtoR(buttons[b].label, xh_charset),
                                                       NULL);
                XmStringFree(s);
                attachtop   (tools[count], NULL, 0);
                attachbottom(tools[count], NULL, 0);
                break;

            case XhACT_BANNER:
                if ((int)(buttons[b].cmd) == 0)
                {
                    count--; // To undo following "count++"
                    break;
                }
                tools[count] = XtVaCreateManagedWidget(!use_mini?"toolBanner":"miniToolBanner", xmTextWidgetClass, container,
                                                       XmNvalue,                 "",
                                                       XmNcolumns,               (int)(buttons[b].cmd),
                                                       XmNtraversalOn,           False,
                                                       XmNcursorPositionVisible, False,
                                                       XmNshadowThickness,       0,
                                                       NULL);
                attachtop   (tools[count], NULL, 0);
                attachbottom(tools[count], NULL, 0);

                window->tbarBanner = tools[count];
                break;

            case XhACT_LOGGIA:
                tools[count] = XtVaCreateManagedWidget("toolLoggia", xmFormWidgetClass, container,
                                                       XmNshadowThickness, 0,
                                                       NULL);
                window->tbarLoggia = tools[count];
                break;

            case XhACT_NOP:
                count--; // To undo following "count++"
                break;
                
            default:
                if (!use_mini)
                {
                    pixmap = buttons[b].pixmap;
                    if (pixmap == NULL)
                        pixmap = buttons[b].mini_pixmap;
                }
                else
                {
                    pixmap = buttons[b].mini_pixmap;
                    if (pixmap == NULL)
                        pixmap = buttons[b].pixmap;
                }

                tools[count] = XtVaCreateManagedWidget(!use_mini?"toolButton":"miniToolButton", xmPushButtonWidgetClass, container,
                                                       XmNtraversalOn, False,
                                                       XmNlabelType,   XmPIXMAP,
                                                       NULL);
                XhAssignPixmap(ABSTRZE(tools[count]), pixmap);
                XhSetBalloon  (ABSTRZE(tools[count]), buttons[b].tip);
                window->tbarButtons[b].btn = tools[count];

                if      (buttons[b].type == XhACT_COMMAND  ||
                         buttons[b].type == XhACT_CHECKBOX)
                    XtAddCallback(tools[count], XmNactivateCallback,
                                  toolCB,  lint2ptr(b));
        }

        count++;
        
 NEXT_BUTTON:;
    }

    leftlimit = count;
    if (firstflushed >= 0  &&  firstflushed < count)
    {
        for (b = count - 1;  b >= firstflushed;  b--)
        {
            if (b == count - 1)
                attachright(tools[b], NULL, 0*padding);
            else
                attachright(tools[b], tools[b + 1], BUTTONS_SPACING);
        }
        
        leftlimit = firstflushed;

        if (firstflushed > 0)
        {
            filler = XtVaCreateManagedWidget("", widgetClass, container,
                                             XmNborderWidth,     0,
                                             NULL);
            attachleft (filler, tools[firstflushed - 1], 0);
            attachright(filler, tools[firstflushed],     0);
        }
    }
    for (b = 0;  b < leftlimit;  b++)
    {
        //attachtop(tools[b], NULL, BUTTONS_PADDING);

        if (b == 0)
            attachleft(tools[b], NULL, 0*padding);
        else
            attachleft(tools[b], tools[b - 1], BUTTONS_SPACING);
    }

    toolSepr = XtVaCreateManagedWidget("toolLine", xmSeparatorWidgetClass, toolBar,
                                       XmNorientation,     XmHORIZONTAL,
                                       XmNleftAttachment,  XmATTACH_FORM,
                                       XmNleftOffset,      0,
                                       XmNrightAttachment, XmATTACH_FORM,
                                       XmNrightOffset,     0,
                                       XmNtopAttachment,   XmATTACH_WIDGET,
                                       XmNtopWidget,       container,
                                       XmNtopOffset,       padding,
                                       NULL);
}

void XhSetCommandOnOff  (XhWindow window, const char *cmd, int is_pushed)
{
  int            b;
  void          *userData;
  int            info_int;

    if (window->tbarButtons == NULL) return;

    is_pushed = (is_pushed != 0);

    for (b = 0;  window->tbarButtons[b].type != XhACT_END;  b++)
        if (window->tbarButtons[b].type == XhACT_CHECKBOX     &&
            strcasecmp(window->tbarButtons[b].cmd, cmd) == 0  &&
            window->tbarButtons[b].pressed != is_pushed)
        {
            window->tbarButtons[b].pressed = is_pushed;
            XhInvertButton(ABSTRZE(window->tbarButtons[b].btn));
        }
}

void XhSetCommandEnabled(XhWindow window, const char *cmd, int is_enabled)
{
  int            b;
  void          *userData;
  int            info_int;

    if (window->tbarButtons == NULL) return;

    is_enabled = (is_enabled != 0);

    for (b = 0;  window->tbarButtons[b].type != XhACT_END;  b++)
        if (window->tbarButtons[b].cmd != NULL                &&
            strcasecmp(window->tbarButtons[b].cmd, cmd) == 0  &&
            window->tbarButtons[b].btn != NULL                &&
            XtIsSensitive(window->tbarButtons[b].btn) != is_enabled)
        {
            XtSetSensitive(window->tbarButtons[b].btn, is_enabled);
        }
}

void XhSetToolbarBanner (XhWindow window, const char *text)
{
    if (window->tbarBanner != NULL)
    XmTextSetString(window->tbarBanner, text);
}

CxWidget    XhGetToolbarLoggia (XhWindow window)
{
    if (window->tbarLoggiaAsked) return NULL;
    window->tbarLoggiaAsked = 1;
    return window->tbarLoggia;
}
