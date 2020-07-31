#include <stdlib.h>

#include <Xm/Xm.h>
#include <Xm/ToggleB.h>

#include "misclib.h"

#include "Xh.h"

#include "MotifKnobs_cda_leds.h"


static void ledCB(Widget     w,
                  XtPointer  closure,
                  XtPointer  call_data  __attribute__((unused)))
{
  MotifKnobs_leds_t *parent_leds = (void *)closure;
  int                n;
  int                nth;

    XtVaSetValues(w, XmNset, False, NULL);

    for (nth = -1, n = 0;  n < parent_leds->count;  n++)
        if (w == parent_leds->leds[n].w) nth = n;
            cda_reconnect_srv(parent_leds->cid, nth);
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

int MotifKnobs_oneled_create    (MotifKnobs_oneled_t *led,
                                 MotifKnobs_leds_t *parent_leds,
                                 Widget  parent, int size,
                                 const char *srvname)
{
  CxWidget     w;
  XmString     s;
  int          no_decor = 0;

    if (size < 0)
    {
        size = -size;
        no_decor = 1;
    }
    if (size <= 1) size = 15;

    w = XtVaCreateManagedWidget("LED",
                                xmToggleButtonWidgetClass,
                                parent,
                                XmNtraversalOn,    False,
                                XmNlabelString,    s = XmStringCreateSimple(""),
                                XmNindicatorSize,  abs(size),
                                XmNspacing,        0,
                                XmNindicatorOn,    XmINDICATOR_CROSS,
                                XmNindicatorType,  XmONE_OF_MANY_ROUND,
                                XmNset,            False,
                                NULL);
    XmStringFree(s);
    if (no_decor)
        XtVaSetValues(w,
                      XmNmarginWidth,  0,
                      XmNmarginHeight, 0,
                      NULL);

    XhSetBalloon     (w, srvname);
    XtAddCallback    (w, XmNvalueChangedCallback, ledCB, parent_leds);
    XtAddEventHandler(w, ButtonPressMask, False, BlockButton2Handler, NULL); // Temporary countermeasure against Motif Bug#1117

    led->w      = w;
    led->status = CDA_SERVERSTATUS_ERROR;

    return 0;
}

int MotifKnobs_oneled_set_status(MotifKnobs_oneled_t *led, int status)
{
  int  idx;
    
    idx                                              = XH_COLOR_JUST_GREEN;
    if (status == CDA_SERVERSTATUS_FROZEN)       idx = XH_COLOR_BG_DEFUNCT;
    if (status == CDA_SERVERSTATUS_ALMOSTREADY)  idx = XH_COLOR_JUST_YELLOW;
    if (status == CDA_SERVERSTATUS_DISCONNECTED) idx = XH_COLOR_JUST_RED;
    
    XtVaSetValues(led->w, XmNunselectColor, XhGetColor(idx), NULL);
    
    led->status = status;

    return 0;
}

static void leds_context_evproc(int            uniq,
                                void          *privptr1,
                                cda_context_t  cid,
                                int            reason,
                                int            info_int,
                                void          *privptr2)
{
  MotifKnobs_leds_t *leds = privptr2;

    if      (reason == CDA_CTX_R_SRVSTAT  &&
             (info_int >= 0  &&  info_int < leds->count))
        MotifKnobs_oneled_set_status(leds->leds + info_int, cda_status_of_srv(leds->cid, info_int));
    else if (reason == CDA_CTX_R_NEWSRV)
        MotifKnobs_leds_grow(leds);
}

int MotifKnobs_leds_create      (MotifKnobs_leds_t *leds,
                                 Widget  parent, int size,
                                 cda_context_t cid, int parent_kind)
{
    bzero(leds, sizeof(*leds));
    leds->cid         = cid;
    leds->parent      = parent;
    leds->parent_kind = parent_kind;
    leds->size        = size;

    cda_add_context_evproc(leds->cid, CDA_CTX_EVMASK_SRVSTAT | CDA_CTX_EVMASK_NEWSRV, 
                           leds_context_evproc, leds);

    return MotifKnobs_leds_grow(leds);
}

int MotifKnobs_leds_destroy     (MotifKnobs_leds_t *leds)
{
    cda_del_context_evproc(leds->cid, CDA_CTX_EVMASK_SRVSTAT | CDA_CTX_EVMASK_NEWSRV, 
                           leds_context_evproc, leds);
    safe_free(leds->leds);
    bzero(leds, sizeof(*leds));

    return 0;
}

int MotifKnobs_leds_grow        (MotifKnobs_leds_t *leds)
{
  int  old_count = leds->count;
  int  new_count = cda_status_srvs_count(leds->cid);
  int  n;

    if (new_count < 0) return -1;
    if (new_count <= leds->count) return 0;
    if (GrowUnitsBuf(&(leds->leds),
                     &(leds->count), new_count, sizeof(leds->leds[0])) < 0)
        return -1;

    if (leds->parent_kind == MOTIFKNOBS_LEDS_PARENT_GRID)
        XhGridSetSize(leds->parent, new_count, 1);
    for (n = old_count;  n < new_count;  n++)
    {
        MotifKnobs_oneled_create    (leds->leds + n,
                                     leds,
                                     leds->parent, leds->size,
                                     cda_status_srv_name(leds->cid, n));
        MotifKnobs_oneled_set_status(leds->leds + n,
                                     cda_status_of_srv(leds->cid, n));
        if (leds->parent_kind == MOTIFKNOBS_LEDS_PARENT_GRID)
            XhGridSetChildAlignment(leds->leds[n].w, 0, 0);
    }

    return 0;
}

int MotifKnobs_leds_update      (MotifKnobs_leds_t *leds)
{
  int  n;

    for (n = 0;  n < leds->count;  n++)
        MotifKnobs_oneled_set_status(leds->leds + n,
                                     cda_status_of_srv(leds->cid, n));

    return 0;
}
