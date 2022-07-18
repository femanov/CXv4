#include "fastadc_data.h"
#include "onei32l_data.h"
#include "onei32l_drv_i.h"


enum { ONEI32L_DTYPE = CXDTYPE_INT32 };
typedef int32 ONEI32L_DATUM_T;

static void onei32l_info2mes(fastadc_data_t      *adc,
                             pzframe_chan_data_t *info,
                             fastadc_mes_t       *mes_p)
{
  ONEI32L_DATUM_T *p;
  ONEI32L_DATUM_T  minval, maxval;
  int64            sumval;
  int              rest;

    return;
    for (rest = adc->mes.cur_numpts - 1, p = adc->mes.plots[0].x_buf, sumval = minval = maxval = *p++;
         rest > 0;
         rest--,                         p++)
    {
        sumval += *p;
        if (minval > *p) minval = *p;
        if (maxval < *p) maxval = *p;
    }
}

static pzframe_chan_dscr_t onei32l_chan_dscrs[] =
{
    [ONEI32L_CHAN_DATA]         = {"data", 0, PZFRAME_CHAN_IS_FRAME | PZFRAME_CHAN_MARKER_MASK,
                                   ONEI32L_DTYPE,
                                   ONEI32L_MAX_NUMPTS},

    [ONEI32L_CHAN_NUMPTS]       = {"numpts"},

    [ONEI32L_CHAN_CUR_NUMPTS]   = {"cur_numpts"},

    [ONEI32L_CHAN_RANGEMIN]     = {"rangemin"},
    [ONEI32L_CHAN_RANGEMAX]     = {"rangemax"},

    [ONEI32L_NUMCHANS] = {"_devstate", 0, PZFRAME_CHAN__DEVSTATE_MASK},
};

static fastadc_line_dscr_t onei32l_line_dscrs[] =
{
    {"DATA", "V", NULL,
     ONEI32L_CHAN_DATA,
     FASTADC_DATA_CN_MISSING, FASTADC_DATA_IS_ON_ALWAYS,
     1000000.0,
     {.int_r={ONEI32L_MIN_VALUE, ONEI32L_MAX_VALUE}},
     ONEI32L_CHAN_RANGEMIN, ONEI32L_CHAN_RANGEMAX}
};

pzframe_type_dscr_t *onei32l_get_type_dscr(void)
{
  static fastadc_type_dscr_t  dscr;
  static int                  dscr_inited;

    if (!dscr_inited)
    {
        FastadcDataFillStdDscr(&dscr, "onei32l",
                               0,
                               onei32l_chan_dscrs, countof(onei32l_chan_dscrs),
                               /* ADC characteristics */
                               ONEI32L_MAX_NUMPTS, ONEI32L_NUM_LINES,
                               ONEI32L_CHAN_CUR_NUMPTS*0+FASTADC_DATA_CN_MISSING,
                               FASTADC_DATA_CN_MISSING,
                               FASTADC_DATA_CN_MISSING,
                               FASTADC_DATA_CN_MISSING,
                               FASTADC_DATA_CN_MISSING,
                               onei32l_info2mes,
                               onei32l_line_dscrs,
                               /* Data specifics */
                               "Ch",
                               "% 6.3f",
                               NULL/*no x2xs()*/, "xs");
        dscr_inited = 1;
    }
    return &(dscr.ftd);
}
