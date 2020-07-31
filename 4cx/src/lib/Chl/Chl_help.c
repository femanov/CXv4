#include "Chl_includes.h"

#include <Xm/MwmUtil.h>

#include <Xm/DialogS.h>
#include <Xm/Frame.h>
#include <Xm/MessageB.h>


typedef help_rec_t dlgrec_t;


typedef struct
{
    int         fgcol, bgcol;
    const char *label;
    const char *comment;
} colhelpline_t;

static colhelpline_t colhelp_contents[] =
{
#if !MAY_USE_RUSSIAN_KOI8
    {-1, XH_COLOR_BG_NORM_YELLOW, "Yellow",        "Value is out of normal range"},
    {-1, XH_COLOR_BG_NORM_RED,    "Red",           "Value is out of safe range"},
    {XH_COLOR_FG_WEIRD,
         XH_COLOR_BG_WEIRD,       "Weird",         "Value is weird"},
    {-1, XH_COLOR_BG_DEFUNCT,     "Goose skin",    "Value has not been updated for a long time"},
    {-1, XH_COLOR_BG_HWERR,       "Maroon",        "Hardware problem (right click on a value - details)"},
    {-1, XH_COLOR_BG_SFERR,       "Swamp",         "Software problem (right click on a value - details)"},
    {-1, XH_COLOR_BG_OTHEROP,     "Orange",        "Value is changed by another person"},
    {-1, XH_COLOR_BG_JUSTCREATED, "Pale blue",     "Data was never read"},
    {-1, XH_COLOR_JUST_GREEN,     "Green",         "An alarm light had just gone out"},
    {-1, XH_COLOR_BG_PRGLYCHG,    "Pale green",    "Value was changed programmatically"},
    {-1, XH_COLOR_BG_NOTFOUND,    "Dark grey",     "Channel not found"},
#else
    {-1, XH_COLOR_BG_NORM_YELLOW, "������",        "�������� ��� ����������� ���������"},
    {-1, XH_COLOR_BG_NORM_RED,    "�������",       "�������� ��� ����������� ���������"},
    {XH_COLOR_FG_WEIRD,
         XH_COLOR_BG_WEIRD,       "��������",      "�������� �������"},
    {-1, XH_COLOR_BG_DEFUNCT,     "������� ����",  "�������� ����� �� �����������"},
    {-1, XH_COLOR_BG_HWERR,       "��������",      "���������� �������� (������ ������ ������� ���� �� �������� - ������)"},
    {-1, XH_COLOR_BG_SFERR,       "��������",      "����������� �������� (������ ������ ������� ���� �� �������� - ������)"},
    {-1, XH_COLOR_BG_OTHEROP,     "���������",     "�������� �������� ������ ����������"},
    {-1, XH_COLOR_BG_JUSTCREATED, "�������������", "������ ��� �� ���� �� ���� ���������"},
    {-1, XH_COLOR_JUST_GREEN,     "�������",       "������ ��� ������� �������� ������������"},
    {-1, XH_COLOR_BG_PRGLYCHG,    "�������������", "�������� �������� ����������"},
    {-1, XH_COLOR_BG_NOTFOUND,    "����������",    "����� �� ������"},
#endif
};

typedef struct
{
    const char *keys;
    const char *comment;
} keyhelpline_t;

static keyhelpline_t keyhelp_contents[] =
{
#if !MAY_USE_RUSSIAN_KOI8
    {"<Tab>/Shift+<Tab>",
        "Move between channels"},
    {"Up/Down arrows\n(in text fields)",
        "Increase/decrease value by a step\n"
            "Holding down Ctrl decreases the step by 10 times\n"
            "Holding down Alt increases the step by 10 times"},
    {"<Enter>",
        "Finish input"},
    {"<Esc>",
        "Cancel input (BEFORE pressing <Enter>!)"},
    {"", NULL},
    {"<Right mouse click>\nAlt+<Enter>\nShift+<F10>",
        "Get a \"Knob properties\" window"},
    {"Ctrl+<Right mouse click>\nCtrl+<Space>",
        "Display a value in large font"},
    {"Shift+<Right mouse click>\nShift+<Space>",
        "Put the channel to a history plot"},
#else
    {"<Tab>/Shift+<Tab>",
        "����������� �� �������"},
    {"������� �����/����\n(� ��������� ����)",
        "����������/���������� �������� �� ���\n"
            "��������� ��� ���� Ctrl ��������� ��� � 10 ���\n"
            "��������� Alt ����������� ��� � 10 ���"},
    {"<Enter>",
        "���������� �����"},
    {"<Esc>",
        "������ ����� (�� ������� <Enter>!)"},
    {"", NULL},
    {"<������ ������ ����>\nAlt+<Enter>\nShift+<F10>",
        "����� ���� \xab�������� ������\xbb"},
    {"Ctrl+<������ ������ ����>\nCtrl+<������>",
        "����� �������� ������� �������"},
    {"Shift+<������ ������ ����>\nShift+<������>",
        "�������� ������ �� ������-���������"},
#endif
};

void   ChlShowHelp      (XhWindow window, int parts)
{
  chl_privrec_t *privrec = _ChlPrivOf(window);
  dlgrec_t      *rec = &(privrec->hr);

  Widget         w;
  Widget         form;
  Widget         frame;
  Widget         grid;
  Widget         prev = NULL;
  XmString       s;
  colhelpline_t *clp;
  keyhelpline_t *klp;
  int            row;
  int            fg, bg;

#if !MAY_USE_RUSSIAN_KOI8
  static const char *help_window_title = "Short help";
  static const char *help_colors_title = "Colors";
  static const char *help_keys_title   = "Mouse and keyboard shortcuts";
#else
  static const char *help_window_title = "������� �������";
  static const char *help_colors_title = "�����";
  static const char *help_keys_title   = "������������� ���������� � ����";
#endif
  
    if (!(rec->initialized))
    {
        rec->box = XhCreateStdDlg(window, "help", "Help", NULL, help_window_title,
                                  XhStdDlgFOk);

        form = XtVaCreateManagedWidget("form", xmFormWidgetClass, rec->box,
                                       XmNshadowThickness, 0,
                                       NULL);

        /* List of colors */
        if (parts & CHL_HELP_COLORS)
        {
            frame = XtVaCreateManagedWidget("frame", xmFrameWidgetClass, form,
                                            XmNmarginWidth, 1,
                                            NULL);
            attachtop(frame, prev, 0);
            prev = frame;
            
            XtVaCreateManagedWidget("title", xmLabelWidgetClass, frame,
                                    XmNframeChildType, XmFRAME_TITLE_CHILD,
                                    XmNlabelString, s = XmStringCreateLtoR(help_colors_title, xh_charset),
                                    NULL);
            XmStringFree(s);
            
            grid = XhCreateGrid(frame, "grid");
            XhGridSetGrid   (grid, 0, 0);
            XhGridSetSpacing(grid, 0, 0);
            XhGridSetPadding(grid, 0, 0);
            
            for (row = 0, clp = colhelp_contents;
                 row < countof(colhelp_contents);
                 row++,   clp++)
            {
                fg = clp->fgcol; if (fg < 0) fg = XH_COLOR_FG_DEFAULT;
                bg = clp->bgcol; if (bg < 0) bg = XH_COLOR_BG_DEFAULT;
                
                /* A model... */
                w = XtVaCreateManagedWidget("rowlabel", xmLabelWidgetClass, grid,
                                            XmNlabelString, s = XmStringCreateLtoR(clp->label, xh_charset),
                                            XmNalignment,  XmALIGNMENT_BEGINNING,
                                            XmNforeground, XhGetColor(fg),
                                            XmNbackground, XhGetColor(bg),
                                            NULL);
                XmStringFree(s);
                
                XhGridSetChildPosition (w, 0,   row);
                XhGridSetChildFilling  (w, 1,   0);
                XhGridSetChildAlignment(w, -1, -1);
                
                /* ...and a comment */
                w = XtVaCreateManagedWidget("rowlabel", xmLabelWidgetClass, grid,
                                            XmNlabelString, s = XmStringCreateLtoR(clp->comment, xh_charset),
                                            XmNalignment,  XmALIGNMENT_BEGINNING,
                                            NULL);
                XmStringFree(s);
                
                XhGridSetChildPosition (w, 1,   row);
                XhGridSetChildFilling  (w, 0,   0);
                XhGridSetChildAlignment(w, -1, -1);
            }
        }

        /* List of key/mouse bindings */
        if (parts & CHL_HELP_KEYS)
        {
            frame = XtVaCreateManagedWidget("frame", xmFrameWidgetClass, form,
                                            XmNmarginWidth, 1,
                                            NULL);
            attachtop(frame, prev, 0);
            prev = frame;
            
            XtVaCreateManagedWidget("title", xmLabelWidgetClass, frame,
                                    XmNframeChildType, XmFRAME_TITLE_CHILD,
                                    XmNlabelString, s = XmStringCreateLtoR(help_keys_title, xh_charset),
                                    NULL);
            XmStringFree(s);
            
            grid = XhCreateGrid(frame, "grid");
            XhGridSetGrid   (grid, 0, 0);
            XhGridSetSpacing(grid, 0, 0);
            XhGridSetPadding(grid, 0, 0);
            
            for (row = 0, klp = keyhelp_contents;
                 row < countof(keyhelp_contents);
                 row++,   klp++)
            {
                /* Have we reached keys/mouse-elem separator? */
                if (klp->keys[0] == '\0')
                {
                    if (parts & CHL_HELP_MOUSE) continue;
                    else                        break;
                }

                /* A model... */
                w = XtVaCreateManagedWidget("rowlabel", xmLabelWidgetClass, grid,
                                            XmNlabelString, s = XmStringCreateLtoR(klp->keys, xh_charset),
                                            XmNalignment,  XmALIGNMENT_END,
                                            NULL);
                XmStringFree(s);
                
                XhGridSetChildPosition (w, 0,   row);
                XhGridSetChildFilling  (w, 0,   0);
                XhGridSetChildAlignment(w, +1, -1);
                
                /* ...and a comment */
                w = XtVaCreateManagedWidget("rowlabel", xmLabelWidgetClass, grid,
                                            XmNlabelString, s = XmStringCreateLtoR(klp->comment, xh_charset),
                                            XmNalignment,  XmALIGNMENT_BEGINNING,
                                            NULL);
                XmStringFree(s);
                
                XhGridSetChildPosition (w, 1,   row);
                XhGridSetChildFilling  (w, 0,   0);
                XhGridSetChildAlignment(w, -1, -1);
            }
        }
        
        rec->initialized = 1;
    }

    if (XtIsManaged(rec->box)) 
        XhStdDlgHide(rec->box);
    else
        XhStdDlgShow(rec->box);
}
