#include "fastadc_data.h"
#include "adc1000_data.h"
#include "drv_i/adc1000_drv_i.h"


enum { ADC1000_DTYPE     = CXDTYPE_INT16 };

static void adc1000_info2mes(fastadc_data_t      *adc,
                             pzframe_chan_data_t *info,
                             fastadc_mes_t       *mes_p)
{
}

static int    adc1000_x2xs(pzframe_chan_data_t *info, int x)
{
    return x;
}

#define DSCR_L1(n,pfx,sfx,np,ns,_change_important,_chan_type,_dtype,_max_nelems,_rd_source_cn) \
    [__CX_CONCATENATE(ADC1000_CHAN_,__CX_CONCATENATE(pfx,__CX_CONCATENATE(n,sfx)))] = {np __CX_STRINGIZE(n) ns, _change_important,_chan_type,_dtype,_max_nelems,_rd_source_cn}
#define DSCR_X4(   pfx,sfx,np,ns,_change_important,_chan_type,_dtype,_max_nelems) \
    DSCR_L1(0, pfx,sfx,np,ns,_change_important,_chan_type,_dtype,_max_nelems,-1), \
    DSCR_L1(1, pfx,sfx,np,ns,_change_important,_chan_type,_dtype,_max_nelems,-1), \
    DSCR_L1(2, pfx,sfx,np,ns,_change_important,_chan_type,_dtype,_max_nelems,-1), \
    DSCR_L1(3, pfx,sfx,np,ns,_change_important,_chan_type,_dtype,_max_nelems,-1)
#define DSCR_X4RDS(pfx,sfx,np,ns,_change_important,_chan_type,_dtype,_max_nelems,_rd_source_cn) \
    DSCR_L1(0, pfx,sfx,np,ns,_change_important,_chan_type,_dtype,_max_nelems,_rd_source_cn+0), \
    DSCR_L1(1, pfx,sfx,np,ns,_change_important,_chan_type,_dtype,_max_nelems,_rd_source_cn+1), \
    DSCR_L1(2, pfx,sfx,np,ns,_change_important,_chan_type,_dtype,_max_nelems,_rd_source_cn+2), \
    DSCR_L1(3, pfx,sfx,np,ns,_change_important,_chan_type,_dtype,_max_nelems,_rd_source_cn+3)

static pzframe_chan_dscr_t adc1000_chan_dscrs[] =
{
    [ADC1000_CHAN_DATA]         = {"data", 0, PZFRAME_CHAN_IS_FRAME | PZFRAME_CHAN_MARKER_MASK,
                                   ADC1000_DTYPE,
                                   ADC1000_MAX_NUMPTS},

    [ADC1000_CHAN_SHOT]         = {"shot",         0, PZFRAME_CHAN_IS_PARAM | PZFRAME_CHAN_RW_ONLY_MASK},
    [ADC1000_CHAN_STOP]         = {"stop",         0, PZFRAME_CHAN_IS_PARAM | PZFRAME_CHAN_RW_ONLY_MASK},
    [ADC1000_CHAN_ISTART]       = {"istart"},
    [ADC1000_CHAN_WAITTIME]     = {"waittime"},
    [ADC1000_CHAN_CALIBRATE]    = {"calibrate",    0, PZFRAME_CHAN_IS_PARAM | PZFRAME_CHAN_RW_ONLY_MASK},
    [ADC1000_CHAN_CALC_STATS]   = {"calc_stats"},

    [ADC1000_CHAN_RUN_MODE]     = {"run_mode",     0, PZFRAME_CHAN_IMMEDIATE_MASK},
    [ADC1000_CHAN_RUN]          = {"run",          0, PZFRAME_CHAN_IS_PARAM | PZFRAME_CHAN_RW_ONLY_MASK},

    [ADC1000_CHAN_PTSOFS]       = {"ptsofs"},
    [ADC1000_CHAN_NUMPTS]       = {"numpts"},
    [ADC1000_CHAN_TIMING]       = {"timing"},
    [ADC1000_CHAN_FRQDIV]       = {"frqdiv"},
    [ADC1000_CHAN_RANGE]        = {"range"},
    [ADC1000_CHAN_TRIG_TYPE]    = {"trig_type"},
    [ADC1000_CHAN_TRIG_INPUT]   = {"trig_input"},

    [ADC1000_CHAN_PLL_PRESET]   = {"pll_preset",   0, PZFRAME_CHAN_IMMEDIATE_MASK | PZFRAME_CHAN_ON_CYCLE_MASK},

    [ADC1000_CHAN_DEVICE_ID]    = {"device_id",    0, PZFRAME_CHAN_IMMEDIATE_MASK | PZFRAME_CHAN_ON_CYCLE_MASK},
    [ADC1000_CHAN_BASE_SW_VER]  = {"base_sw_ver",  0, PZFRAME_CHAN_IMMEDIATE_MASK | PZFRAME_CHAN_ON_CYCLE_MASK},
    [ADC1000_CHAN_PGA_SW_VER]   = {"pga_sw_ver",   0, PZFRAME_CHAN_IMMEDIATE_MASK | PZFRAME_CHAN_ON_CYCLE_MASK},
    [ADC1000_CHAN_BASE_HW_VER]  = {"base_hw_ver",  0, PZFRAME_CHAN_IMMEDIATE_MASK | PZFRAME_CHAN_ON_CYCLE_MASK},
    [ADC1000_CHAN_PGA_HW_VER]   = {"pga_hw_ver",   0, PZFRAME_CHAN_IMMEDIATE_MASK | PZFRAME_CHAN_ON_CYCLE_MASK},
    [ADC1000_CHAN_PGA_VAR]      = {"pga_var",      0, PZFRAME_CHAN_IMMEDIATE_MASK | PZFRAME_CHAN_ON_CYCLE_MASK},
    [ADC1000_CHAN_BASE_UNIQ_ID] = {"base_uniq_id", 0, PZFRAME_CHAN_IMMEDIATE_MASK | PZFRAME_CHAN_ON_CYCLE_MASK},
    [ADC1000_CHAN_PGA_UNIQ_ID]  = {"pga_uniq_id",  0, PZFRAME_CHAN_IMMEDIATE_MASK | PZFRAME_CHAN_ON_CYCLE_MASK},

    [ADC1000_CHAN_PLL_LOCKED]   = {"pll_locked",   0, PZFRAME_CHAN_IMMEDIATE_MASK | PZFRAME_CHAN_ON_CYCLE_MASK},

    [ADC1000_CHAN_ELAPSED]      = {"elapsed",      0, PZFRAME_CHAN_IMMEDIATE_MASK | PZFRAME_CHAN_ON_CYCLE_MASK},
    [ADC1000_CHAN_XS_PER_POINT] = {"xs_per_point", 1},
    [ADC1000_CHAN_XS_DIVISOR]   = {"xs_divisor",   1},
    [ADC1000_CHAN_XS_FACTOR]    = {"xs_factor",    1},
    [ADC1000_CHAN_CUR_PTSOFS]   = {"cur_ptsofs",   1},
    [ADC1000_CHAN_CUR_NUMPTS]   = {"cur_numpts"},
    [ADC1000_CHAN_CUR_TIMING]   = {"cur_timing",   1},
    [ADC1000_CHAN_CUR_FRQDIV]   = {"cur_frqdiv",   1},
    [ADC1000_CHAN_CUR_RANGE]    = {"cur_range",    1},
    DSCR_X4   (OVERFLOW,,           "overflow","",  0, 0, 0, 0),

    [ADC1000_CHAN_ON]           = {"on"},
    [ADC1000_CHAN_RANGEMIN]     = {"rangemin",     0, 0, 0, 0, ADC1000_CHAN_DATA},
    [ADC1000_CHAN_RANGEMAX]     = {"rangemax",     0, 0, 0, 0, ADC1000_CHAN_DATA},

    DSCR_X4   (CLB_FAIL,,           "clb_fail","",  0, 0, 0, 0),
    DSCR_X4   (CLB_DYN,,            "clb_dyn", "",  0, 0, 0, 0),
    DSCR_X4   (CLB_ZERO,,           "clb_zero","",  0, 0, 0, 0),
    DSCR_X4   (CLB_GAIN,,           "clb_gain","",  0, 0, 0, 0),

    [ADC1000_CHAN_MIN]           = {"min",         0, 0, 0, 0, ADC1000_CHAN_DATA},
    [ADC1000_CHAN_MAX]           = {"max",         0, 0, 0, 0, ADC1000_CHAN_DATA},
    [ADC1000_CHAN_AVG]           = {"avg",         0, 0, 0, 0, ADC1000_CHAN_DATA},
    [ADC1000_CHAN_INT]           = {"int",         0, 0, 0, 0, ADC1000_CHAN_DATA},

    [ADC1000_CHAN_CUR_PLL1_CTRL] = {"cur_pll1_ctrl", 0, PZFRAME_CHAN_IMMEDIATE_MASK | PZFRAME_CHAN_ON_CYCLE_MASK},
    [ADC1000_CHAN_CUR_PLL2_CTRL] = {"cur_pll2_ctrl", 0, PZFRAME_CHAN_IMMEDIATE_MASK | PZFRAME_CHAN_ON_CYCLE_MASK},

    // TOTAL{MIN,MAX} and NUM_LINES are omitted

//    [] = {""},
    [ADC1000_NUMCHANS] = {"_devstate", 0, PZFRAME_CHAN__DEVSTATE_MASK},
};

static fastadc_line_dscr_t adc1000_line_dscrs[] =
{
    {"DATA", "V", NULL,
     ADC1000_CHAN_DATA,
     FASTADC_DATA_CN_MISSING, FASTADC_DATA_IS_ON_ALWAYS,
     1000000.0,
     {.int_r={ADC1000_MIN_VALUE, ADC1000_MAX_VALUE}},
     ADC1000_CHAN_RANGEMIN, ADC1000_CHAN_RANGEMAX}
};

pzframe_type_dscr_t *adc1000_get_type_dscr(void)
{
  static fastadc_type_dscr_t  dscr;
  static int                  dscr_inited;

    if (!dscr_inited)
    {
        FastadcDataFillStdDscr(&dscr, "adc1000",
                               0,
                               adc1000_chan_dscrs, countof(adc1000_chan_dscrs),
                               /* ADC characteristics */
                               ADC1000_MAX_NUMPTS, ADC1000_NUM_LINES,
                               ADC1000_CHAN_CUR_NUMPTS,
                               ADC1000_CHAN_CUR_PTSOFS,
                               ADC1000_CHAN_XS_PER_POINT,
                               ADC1000_CHAN_XS_DIVISOR,
                               ADC1000_CHAN_XS_FACTOR,
                               adc1000_info2mes,
                               adc1000_line_dscrs,
                               /* Data specifics */
                               "",
                               "% 6.3f",
                               adc1000_x2xs, "ns");
        dscr_inited = 1;
    }
    return &(dscr.ftd);
}
