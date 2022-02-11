#include "cxsd_driver.h"
#include "pzframe_drv.h"
#include "vme_lyr.h"

#if (defined(DEVTYPE_ADC250) + defined(DEVTYPE_ADC500) + defined(DEVTYPE_ADC1000)) != 1
  #error Exactly ONE of "DEVTYPE_ADC250", "DEVTYPE_ADC500" or "DEVTYPE_ADC1000" must be defined
#endif

#include "adc4x250_defs.h"
#if   defined(DEVTYPE_ADC250)
  #include "drv_i/adc250_drv_i.h"
  enum {DEVDEP_NUM_LINES    = ADC250_NUM_LINES};
  enum {DEVDEP_MAX_NUMPTS   = ADC250_MAX_NUMPTS};
  enum {DEVDEP_PRETRIG      = ADC4X250_TIMER_PRETRIG};
  enum {DEVDEP_XS_PER_POINT = 4};
  #define FASTADC_NAME adc250
#elif defined(DEVTYPE_ADC500)
  #error ADC500 is not supported
  enum {DEVDEP_XS_PER_POINT = 2};
  #define FASTADC_NAME adc500
#elif defined(DEVTYPE_ADC1000)
  #include "drv_i/adc250_drv_i.h"
  #include "drv_i/adc1000_drv_i.h"
  enum {DEVDEP_NUM_LINES    = ADC1000_NUM_LINES};
  enum {DEVDEP_MAX_NUMPTS   = ADC1000_MAX_NUMPTS};
  enum {DEVDEP_PRETRIG      = ADC4X250_TIMER_PRETRIG * 4};
  enum {DEVDEP_XS_PER_POINT = 1};
  #define FASTADC_NAME adc1000
#endif
#define adcNNN_privrec_t __CX_CONCATENATE(FASTADC_NAME,_privrec_t)
#define adcNNN_params    __CX_CONCATENATE(FASTADC_NAME,_params)
#define adcNNN_rw_p      __CX_CONCATENATE(FASTADC_NAME,_rw_p)

typedef int16 ADC250_DATUM_T;
enum { ADC250_DTYPE = CXDTYPE_INT16};

/* We allow only as much measurements, not DEVDEP_MAX_NUMPTS  */
enum { MAX_ALLOWED_NUMPTS = 50000*1 + DEVDEP_MAX_NUMPTS*0 };

enum
{
                                  NLGE2 = (DEVDEP_NUM_LINES >= 2),                                  NLGE4 = (DEVDEP_NUM_LINES >= 4),
    PZFRAME_CHTYPE_BIGC2        = NLGE2 * PZFRAME_CHTYPE_BIGC,        PZFRAME_CHTYPE_BIGC4        = NLGE4 * PZFRAME_CHTYPE_BIGC,
    PZFRAME_CHTYPE_VALIDATE2    = NLGE2 * PZFRAME_CHTYPE_VALIDATE,    PZFRAME_CHTYPE_VALIDATE4    = NLGE4 * PZFRAME_CHTYPE_VALIDATE,
    PZFRAME_CHTYPE_STATUS2      = NLGE2 * PZFRAME_CHTYPE_STATUS,      PZFRAME_CHTYPE_STATUS4      = NLGE4 * PZFRAME_CHTYPE_STATUS,
    PZFRAME_CHTYPE_AUTOUPDATED2 = NLGE2 * PZFRAME_CHTYPE_AUTOUPDATED, PZFRAME_CHTYPE_AUTOUPDATED4 = NLGE4 * PZFRAME_CHTYPE_AUTOUPDATED,
};


//////////////////////////////////////////////////////////////////////

enum
{
    PLL_PRESET_NONE         = 0,
    PLL_PRESET_INTERNAL     = 1,
    PLL_PRESET_VEPP5_RFMEAS = 2,
};
typedef struct
{
    uint32  clk_src;
    uint32  pll1;
    uint32  pll2;
} pll_preset_t;
static pll_preset_t pll_presets_table[] =
{
    [0]                       = {0,                        0,          0},
    [PLL_PRESET_INTERNAL]     = {ADC4X250_CLK_SRC_VAL_INT, 0x0300A002, 0x00040F01},
    [PLL_PRESET_VEPP5_RFMEAS] = {ADC4X250_CLK_SRC_VAL_BP,  0x02008004, 0x30030F01},
};

//--------------------------------------------------------------------

static pzframe_chinfo_t chinfo[] =
{
    /* data */
    [ADC250_CHAN_DATA]          = {PZFRAME_CHTYPE_BIGC,        0},
    [ADC250_CHAN_LINE0]         = {PZFRAME_CHTYPE_BIGC2,       0},
    [ADC250_CHAN_LINE1]         = {PZFRAME_CHTYPE_BIGC2,       0},
    [ADC250_CHAN_LINE2]         = {PZFRAME_CHTYPE_BIGC4,       0},
    [ADC250_CHAN_LINE3]         = {PZFRAME_CHTYPE_BIGC4,       0},

    [ADC250_CHAN_MARKER]        = {PZFRAME_CHTYPE_MARKER,      0},

    /* controls */
    [ADC250_CHAN_SHOT]          = {PZFRAME_CHTYPE_PZFRAME_STD, 0},
    [ADC250_CHAN_STOP]          = {PZFRAME_CHTYPE_PZFRAME_STD, 0},
    [ADC250_CHAN_ISTART]        = {PZFRAME_CHTYPE_PZFRAME_STD, 0},
    [ADC250_CHAN_WAITTIME]      = {PZFRAME_CHTYPE_PZFRAME_STD, 0},
    [ADC250_CHAN_CALIBRATE]     = {PZFRAME_CHTYPE_INDIVIDUAL,  0},
    [ADC250_CHAN_CALC_STATS]    = {PZFRAME_CHTYPE_VALIDATE,    0},

    [ADC250_CHAN_RUN_MODE]      = {PZFRAME_CHTYPE_PZFRAME_STD, 0},
    [ADC250_CHAN_RUN]           = {PZFRAME_CHTYPE_PZFRAME_STD, 0},

    [ADC250_CHAN_PTSOFS]        = {PZFRAME_CHTYPE_VALIDATE,    0},
    [ADC250_CHAN_NUMPTS]        = {PZFRAME_CHTYPE_VALIDATE,    0},
    [ADC250_CHAN_TIMING]        = {PZFRAME_CHTYPE_VALIDATE,    0},
    [ADC250_CHAN_FRQDIV]        = {PZFRAME_CHTYPE_VALIDATE,    0},
    [ADC250_CHAN_RANGE0]        = {PZFRAME_CHTYPE_VALIDATE,    0},
    [ADC250_CHAN_RANGE1]        = {PZFRAME_CHTYPE_VALIDATE2,   0},
    [ADC250_CHAN_RANGE2]        = {PZFRAME_CHTYPE_VALIDATE4,   0},
    [ADC250_CHAN_RANGE3]        = {PZFRAME_CHTYPE_VALIDATE4,   0},
    [ADC250_CHAN_TRIG_TYPE]     = {PZFRAME_CHTYPE_VALIDATE,    0},
    [ADC250_CHAN_TRIG_INPUT]    = {PZFRAME_CHTYPE_VALIDATE,    0},

    [ADC250_CHAN_PLL_PRESET]    = {PZFRAME_CHTYPE_INDIVIDUAL,  0},

    /* status */
    [ADC250_CHAN_DEVICE_ID]     = {PZFRAME_CHTYPE_AUTOUPDATED, 0},
    [ADC250_CHAN_BASE_SW_VER]   = {PZFRAME_CHTYPE_AUTOUPDATED, 0},
    [ADC250_CHAN_PGA_SW_VER]    = {PZFRAME_CHTYPE_AUTOUPDATED, 0},
    [ADC250_CHAN_BASE_HW_VER]   = {PZFRAME_CHTYPE_AUTOUPDATED, 0},
    [ADC250_CHAN_PGA_HW_VER]    = {PZFRAME_CHTYPE_AUTOUPDATED, 0},
    [ADC250_CHAN_PGA_VAR]       = {PZFRAME_CHTYPE_AUTOUPDATED, 0},
    [ADC250_CHAN_BASE_UNIQ_ID]  = {PZFRAME_CHTYPE_AUTOUPDATED, 0},
    [ADC250_CHAN_PGA_UNIQ_ID]   = {PZFRAME_CHTYPE_AUTOUPDATED, 0},

    [ADC250_CHAN_PLL_LOCKED]    = {PZFRAME_CHTYPE_INDIVIDUAL,  0},

    [ADC250_CHAN_ELAPSED]       = {PZFRAME_CHTYPE_PZFRAME_STD, 0},
    [ADC250_CHAN_XS_PER_POINT]  = {PZFRAME_CHTYPE_STATUS,      -1},
    [ADC250_CHAN_XS_DIVISOR]    = {PZFRAME_CHTYPE_STATUS,      -1},
    [ADC250_CHAN_XS_FACTOR]     = {PZFRAME_CHTYPE_AUTOUPDATED, 0},
    [ADC250_CHAN_CUR_PTSOFS]    = {PZFRAME_CHTYPE_STATUS,      ADC250_CHAN_PTSOFS},
    [ADC250_CHAN_CUR_NUMPTS]    = {PZFRAME_CHTYPE_STATUS,      ADC250_CHAN_NUMPTS},
    [ADC250_CHAN_CUR_TIMING]    = {PZFRAME_CHTYPE_STATUS,      ADC250_CHAN_TIMING},
    [ADC250_CHAN_CUR_FRQDIV]    = {PZFRAME_CHTYPE_STATUS,      ADC250_CHAN_FRQDIV},
    [ADC250_CHAN_CUR_RANGE0]    = {PZFRAME_CHTYPE_STATUS,      ADC250_CHAN_RANGE0},
    [ADC250_CHAN_CUR_RANGE1]    = {PZFRAME_CHTYPE_STATUS2,     ADC250_CHAN_RANGE1},
    [ADC250_CHAN_CUR_RANGE2]    = {PZFRAME_CHTYPE_STATUS4,     ADC250_CHAN_RANGE2},
    [ADC250_CHAN_CUR_RANGE3]    = {PZFRAME_CHTYPE_STATUS4,     ADC250_CHAN_RANGE3},
    [ADC250_CHAN_OVERFLOW0]     = {PZFRAME_CHTYPE_STATUS,      -1},
    [ADC250_CHAN_OVERFLOW1]     = {PZFRAME_CHTYPE_STATUS,      -1},
    [ADC250_CHAN_OVERFLOW2]     = {PZFRAME_CHTYPE_STATUS,      -1},
    [ADC250_CHAN_OVERFLOW3]     = {PZFRAME_CHTYPE_STATUS,      -1},

    [ADC250_CHAN_LINE0ON]       = {PZFRAME_CHTYPE_STATUS,      -1},
    [ADC250_CHAN_LINE1ON]       = {PZFRAME_CHTYPE_STATUS2,     -1},
    [ADC250_CHAN_LINE2ON]       = {PZFRAME_CHTYPE_STATUS4,     -1},
    [ADC250_CHAN_LINE3ON]       = {PZFRAME_CHTYPE_STATUS4,     -1},
    [ADC250_CHAN_LINE0RANGEMIN] = {PZFRAME_CHTYPE_STATUS,      -1},
    [ADC250_CHAN_LINE1RANGEMIN] = {PZFRAME_CHTYPE_STATUS2,     -1},
    [ADC250_CHAN_LINE2RANGEMIN] = {PZFRAME_CHTYPE_STATUS4,     -1},
    [ADC250_CHAN_LINE3RANGEMIN] = {PZFRAME_CHTYPE_STATUS4,     -1},
    [ADC250_CHAN_LINE0RANGEMAX] = {PZFRAME_CHTYPE_STATUS,      -1},
    [ADC250_CHAN_LINE1RANGEMAX] = {PZFRAME_CHTYPE_STATUS2,     -1},
    [ADC250_CHAN_LINE2RANGEMAX] = {PZFRAME_CHTYPE_STATUS4,     -1},
    [ADC250_CHAN_LINE3RANGEMAX] = {PZFRAME_CHTYPE_STATUS4,     -1},

    [ADC250_CHAN_CLB_FAIL0]     = {PZFRAME_CHTYPE_AUTOUPDATED, 0},
    [ADC250_CHAN_CLB_FAIL1]     = {PZFRAME_CHTYPE_AUTOUPDATED, 0},
    [ADC250_CHAN_CLB_FAIL2]     = {PZFRAME_CHTYPE_AUTOUPDATED, 0},
    [ADC250_CHAN_CLB_FAIL3]     = {PZFRAME_CHTYPE_AUTOUPDATED, 0},
    [ADC250_CHAN_CLB_DYN0]      = {PZFRAME_CHTYPE_AUTOUPDATED, 0},
    [ADC250_CHAN_CLB_DYN1]      = {PZFRAME_CHTYPE_AUTOUPDATED, 0},
    [ADC250_CHAN_CLB_DYN2]      = {PZFRAME_CHTYPE_AUTOUPDATED, 0},
    [ADC250_CHAN_CLB_DYN3]      = {PZFRAME_CHTYPE_AUTOUPDATED, 0},
    [ADC250_CHAN_CLB_ZERO0]     = {PZFRAME_CHTYPE_AUTOUPDATED, 0},
    [ADC250_CHAN_CLB_ZERO1]     = {PZFRAME_CHTYPE_AUTOUPDATED, 0},
    [ADC250_CHAN_CLB_ZERO2]     = {PZFRAME_CHTYPE_AUTOUPDATED, 0},
    [ADC250_CHAN_CLB_ZERO3]     = {PZFRAME_CHTYPE_AUTOUPDATED, 0},
    [ADC250_CHAN_CLB_GAIN0]     = {PZFRAME_CHTYPE_AUTOUPDATED, 0},
    [ADC250_CHAN_CLB_GAIN1]     = {PZFRAME_CHTYPE_AUTOUPDATED, 0},
    [ADC250_CHAN_CLB_GAIN2]     = {PZFRAME_CHTYPE_AUTOUPDATED, 0},
    [ADC250_CHAN_CLB_GAIN3]     = {PZFRAME_CHTYPE_AUTOUPDATED, 0},

    [ADC250_CHAN_CUR_PLL1_CTRL] = {PZFRAME_CHTYPE_AUTOUPDATED, 0},
    [ADC250_CHAN_CUR_PLL2_CTRL] = {PZFRAME_CHTYPE_AUTOUPDATED, 0},

    [ADC250_CHAN_MIN0]          = {PZFRAME_CHTYPE_AUTOUPDATED,  0},
    [ADC250_CHAN_MIN1]          = {PZFRAME_CHTYPE_AUTOUPDATED2, 0},
    [ADC250_CHAN_MIN2]          = {PZFRAME_CHTYPE_AUTOUPDATED4, 0},
    [ADC250_CHAN_MIN3]          = {PZFRAME_CHTYPE_AUTOUPDATED4, 0},
    [ADC250_CHAN_MAX0]          = {PZFRAME_CHTYPE_AUTOUPDATED,  0},
    [ADC250_CHAN_MAX1]          = {PZFRAME_CHTYPE_AUTOUPDATED2, 0},
    [ADC250_CHAN_MAX2]          = {PZFRAME_CHTYPE_AUTOUPDATED4, 0},
    [ADC250_CHAN_MAX3]          = {PZFRAME_CHTYPE_AUTOUPDATED4, 0},
    [ADC250_CHAN_AVG0]          = {PZFRAME_CHTYPE_AUTOUPDATED,  0},
    [ADC250_CHAN_AVG1]          = {PZFRAME_CHTYPE_AUTOUPDATED2, 0},
    [ADC250_CHAN_AVG2]          = {PZFRAME_CHTYPE_AUTOUPDATED4, 0},
    [ADC250_CHAN_AVG3]          = {PZFRAME_CHTYPE_AUTOUPDATED4, 0},
    [ADC250_CHAN_INT0]          = {PZFRAME_CHTYPE_AUTOUPDATED,  0},
    [ADC250_CHAN_INT1]          = {PZFRAME_CHTYPE_AUTOUPDATED2, 0},
    [ADC250_CHAN_INT2]          = {PZFRAME_CHTYPE_AUTOUPDATED4, 0},
    [ADC250_CHAN_INT3]          = {PZFRAME_CHTYPE_AUTOUPDATED4, 0},

    [ADC250_CHAN_LINE0TOTALMIN] = {PZFRAME_CHTYPE_AUTOUPDATED,  0},
    [ADC250_CHAN_LINE1TOTALMIN] = {PZFRAME_CHTYPE_AUTOUPDATED2, 0},
    [ADC250_CHAN_LINE2TOTALMIN] = {PZFRAME_CHTYPE_AUTOUPDATED4, 0},
    [ADC250_CHAN_LINE3TOTALMIN] = {PZFRAME_CHTYPE_AUTOUPDATED4, 0},
    [ADC250_CHAN_LINE0TOTALMAX] = {PZFRAME_CHTYPE_AUTOUPDATED,  0},
    [ADC250_CHAN_LINE1TOTALMAX] = {PZFRAME_CHTYPE_AUTOUPDATED2, 0},
    [ADC250_CHAN_LINE2TOTALMAX] = {PZFRAME_CHTYPE_AUTOUPDATED4, 0},
    [ADC250_CHAN_LINE3TOTALMAX] = {PZFRAME_CHTYPE_AUTOUPDATED4, 0},

    [ADC250_CHAN_NUM_LINES]     = {PZFRAME_CHTYPE_AUTOUPDATED, 0},
};

//--------------------------------------------------------------------

typedef struct
{
    pzframe_drv_t      pz;
    vme_vmt_t         *lvmt;
    int                devid;
    int                handle;
    int                irq_n;
    int                irq_vect;
int jumpers;

    int                BASE_SW_VER;

    int32              cur_args[ADC250_NUMCHANS];
    int32              nxt_args[ADC250_NUMCHANS];
    int32              prv_args[ADC250_NUMCHANS];
    ADC250_DATUM_T     retdata [MAX_ALLOWED_NUMPTS*DEVDEP_NUM_LINES +1]; // "+1" to safely read one more than requested (odd'th measurement in high-16-bit half of uint32)
    int                do_return;
    int                force_read;
    pzframe_retbufs_t  retbufs;

    // This holds either ADC4X250_CTRL_START or ADC4X250_CTRL_CALIB
    int32              start_mask;

    int                data_rqd;
#ifndef DEVTYPE_ADC1000 // ADC1000 is 1-channel
    int                line_rqd[DEVDEP_NUM_LINES];
#endif
    struct
    {
        int            addrs [ADC250_NUMCHANS];
        cxdtype_t      dtypes[ADC250_NUMCHANS];
        int            nelems[ADC250_NUMCHANS];
        void          *val_ps[ADC250_NUMCHANS];
        rflags_t       rflags[ADC250_NUMCHANS];
    }                  r;
} adcNNN_privrec_t;

#define PDR2ME(pdr) ((adcNNN_privrec_t*)pdr) //!!! Should better subtract offsetof(pz)

//--------------------------------------------------------------------

static psp_lkp_t adcNNN_timing_lkp[] =
{
    {"int",   ADC4X250_CLK_SRC_VAL_INT},
    {"front", ADC4X250_CLK_SRC_VAL_FP},
    {"back",  ADC4X250_CLK_SRC_VAL_BP},
    {NULL, 0}
};

static psp_lkp_t adcNNN_trig_type_lkp[] =
{
    {"disable",   ADC4X250_TRIG_ENA_VAL_DISABLE},
    {"int",       ADC4X250_TRIG_ENA_VAL_INT},
    {"front",     ADC4X250_TRIG_ENA_VAL_EXT},
    {"back",      ADC4X250_TRIG_ENA_VAL_BP},
    {"back+sync", ADC4X250_TRIG_ENA_VAL_BP_SYNC},
    {NULL, 0}
};

static psp_lkp_t adcNNN_range_lkp[] =
{
    {"0.5", ADC4X250_RANGE_VAL_0_5V},
    {"1",   ADC4X250_RANGE_VAL_1V},
    {"2",   ADC4X250_RANGE_VAL_2V},
    {"4",   ADC4X250_RANGE_VAL_4V},
    {NULL, 0}
};

static psp_lkp_t adcNNN_pll_preset_lkp[] =
{
    {"internal",     PLL_PRESET_INTERNAL},
    {"vepp5_rfmeas", PLL_PRESET_VEPP5_RFMEAS},
    {NULL, 0}
};

static psp_paramdescr_t adcNNN_params[] =
{
    PSP_P_INT    ("ptsofs",     adcNNN_privrec_t, nxt_args[ADC250_CHAN_PTSOFS],     -1, 0, MAX_ALLOWED_NUMPTS-2),
    PSP_P_INT    ("numpts",     adcNNN_privrec_t, nxt_args[ADC250_CHAN_NUMPTS],     -1, 1, MAX_ALLOWED_NUMPTS),
    PSP_P_LOOKUP ("timing",     adcNNN_privrec_t, nxt_args[ADC250_CHAN_TIMING],     -1, adcNNN_timing_lkp),
    PSP_P_INT    ("frqdiv",     adcNNN_privrec_t, nxt_args[ADC250_CHAN_FRQDIV],     -1, ADC4X250_ADC_DECIM_RATIO_MIN, ADC4X250_ADC_DECIM_RATIO_MAX),
    PSP_P_LOOKUP ("trigger",    adcNNN_privrec_t, nxt_args[ADC250_CHAN_TRIG_TYPE],  -1, adcNNN_trig_type_lkp),
    PSP_P_INT    ("trig_n",     adcNNN_privrec_t, nxt_args[ADC250_CHAN_TRIG_INPUT], -1, 0, ADC4X250_ADC_TRIG_SOURCE_TTL_INPUT_bits),
#if   defined(DEVTYPE_ADC1000)
    PSP_P_LOOKUP ("range",      adcNNN_privrec_t, nxt_args[ADC250_CHAN_RANGE0 + 0], -1, adcNNN_range_lkp),
#elif defined(DEVTYPE_ADC250)  || defined(DEVTYPE_ADC500)
    PSP_P_LOOKUP ("rangeA",     adcNNN_privrec_t, nxt_args[ADC250_CHAN_RANGE0 + 0], -1, adcNNN_range_lkp),
    PSP_P_LOOKUP ("rangeB",     adcNNN_privrec_t, nxt_args[ADC250_CHAN_RANGE0 + 1], -1, adcNNN_range_lkp),
#endif
#if   defined(DEVTYPE_ADC250)
    PSP_P_LOOKUP ("rangeC",     adcNNN_privrec_t, nxt_args[ADC250_CHAN_RANGE0 + 2], -1, adcNNN_range_lkp),
    PSP_P_LOOKUP ("rangeD",     adcNNN_privrec_t, nxt_args[ADC250_CHAN_RANGE0 + 3], -1, adcNNN_range_lkp),
#endif
    PSP_P_LOOKUP ("pll_preset", adcNNN_privrec_t, nxt_args[ADC250_CHAN_PLL_PRESET],  0, adcNNN_pll_preset_lkp),
    PSP_P_LOOKUP ("run_mode",   adcNNN_privrec_t, nxt_args[ADC250_CHAN_RUN_MODE],   -1, pzframe_drv_run_mode_lkp),
    PSP_P_END()
};

//--------------------------------------------------------------------

static void Return1Param(adcNNN_privrec_t *me, int n, int32 v)
{
    ReturnInt32Datum(me->devid, n, me->cur_args[n] = me->nxt_args[n] = v, 0);
}

static void ReturnDevInfo(adcNNN_privrec_t *me)
{
  uint32  v_devid;
  uint32  v_ver;
  uint32  v_uniq;

    me->lvmt->a32rd32(me->handle, ADC4X250_R_DEVICE_ID, &v_devid);
    me->lvmt->a32rd32(me->handle, ADC4X250_R_VERSION,   &v_ver);
    me->lvmt->a32rd32(me->handle, ADC4X250_R_UNIQ_ID,   &v_uniq);

    me->BASE_SW_VER =                           v_ver         & 0xFF;

    Return1Param(me, ADC250_CHAN_DEVICE_ID,     v_devid);

    Return1Param(me, ADC250_CHAN_BASE_SW_VER,   v_ver         & 0xFF);
    Return1Param(me, ADC250_CHAN_PGA_SW_VER,   (v_ver  >>  8) & 0xFF);
    Return1Param(me, ADC250_CHAN_BASE_HW_VER,  (v_ver  >> 16) & 0xFF);
    Return1Param(me, ADC250_CHAN_PGA_HW_VER,   (v_ver  >> 24) & 0x3F);
    Return1Param(me, ADC250_CHAN_PGA_VAR,      (v_ver  >> 30) & 0x03);

    Return1Param(me, ADC250_CHAN_BASE_UNIQ_ID,  v_uniq        & 0xFFFF);
    Return1Param(me, ADC250_CHAN_PGA_UNIQ_ID,  (v_uniq >> 16) & 0xFFFF);
}

static void ReturnClbInfo(adcNNN_privrec_t *me)
{
  uint32  status;
  int32   iv;
  int     n;

    me->lvmt->a32rd32(me->handle, ADC4X250_R_STATUS, &status);
    for (n = 0;  n < ADC250_NUM_ADCS;  n++)
    {
        Return1Param(me, ADC250_CHAN_CLB_FAIL0 + n,
                     (status >> (ADC4X250_STATUS_CALIB_FAILED_shift + n)) & 1);

        me->lvmt->a32rd32(me->handle,
                          ADC4X250_R_CALIB_CONST_DYN_CHx_base  +
                          ADC4X250_R_CALIB_CONST_DYN_CHx_incr  * n,
                          &iv);
        Return1Param(me, ADC250_CHAN_CLB_DYN0  + n, iv);

        me->lvmt->a32rd32(me->handle,
                          ADC4X250_R_CALIB_CONST_ZERO_CHx_base +
                          ADC4X250_R_CALIB_CONST_ZERO_CHx_incr * n,
                          &iv);
        Return1Param(me, ADC250_CHAN_CLB_ZERO0 + n, iv);

        me->lvmt->a32rd32(me->handle,
                          ADC4X250_R_CALIB_CONST_GAIN_CHx_base +
                          ADC4X250_R_CALIB_CONST_GAIN_CHx_incr * n,
                          &iv);
        Return1Param(me, ADC250_CHAN_CLB_GAIN0 + n, iv);
    }
}

static int32 ValidateParam(pzframe_drv_t *pdr, int n, int32 v)
{
  adcNNN_privrec_t *me = PDR2ME(pdr);

    if (0)
    ;
    else if (n == ADC250_CHAN_TIMING)
    {
        if (v != ADC4X250_CLK_SRC_VAL_INT  &&
            v != ADC4X250_CLK_SRC_VAL_FP   &&
            v != ADC4X250_CLK_SRC_VAL_BP) v = ADC4X250_CLK_SRC_VAL_INT;
    }
    else if (n == ADC250_CHAN_FRQDIV)
    {
        if      (me->BASE_SW_VER < 0x08)           v = ADC4X250_ADC_DECIM_RATIO_MIN;
#ifdef DEVTYPE_ADC1000
        else if (me->BASE_SW_VER < 999)            v = ADC4X250_ADC_DECIM_RATIO_MIN;
#endif
        else if (v < ADC4X250_ADC_DECIM_RATIO_MIN) v = ADC4X250_ADC_DECIM_RATIO_MIN;
        else if (v > ADC4X250_ADC_DECIM_RATIO_MAX) v = ADC4X250_ADC_DECIM_RATIO_MAX;
    }
    else if (n >= ADC250_CHAN_RANGE0  &&  n <= ADC250_CHAN_RANGE3)
    {
        if (v < ADC4X250_RANGE_VAL_0_5V) v = ADC4X250_RANGE_VAL_0_5V;
        if (v > ADC4X250_RANGE_VAL_4V)   v = ADC4X250_RANGE_VAL_4V;
    }
    else if (n == ADC250_CHAN_TRIG_TYPE)
    {
        if (v != ADC4X250_TRIG_ENA_VAL_DISABLE  &&
            v != ADC4X250_TRIG_ENA_VAL_INT      &&
            v != ADC4X250_TRIG_ENA_VAL_EXT      &&
            v != ADC4X250_TRIG_ENA_VAL_BP       &&
            v != ADC4X250_TRIG_ENA_VAL_BP_SYNC) v = ADC4X250_TRIG_ENA_VAL_DISABLE;
    }
    else if (n == ADC250_CHAN_TRIG_INPUT)
    {
        if (v < 0) v = 0;
        if (v > ADC4X250_ADC_TRIG_SOURCE_TTL_INPUT_bits)
            v = ADC4X250_ADC_TRIG_SOURCE_TTL_INPUT_bits;
    }
    else if (n == ADC250_CHAN_PTSOFS)
    {
        v &=~ 1;
        if (v < 0)                    v = 0;
        if (v > MAX_ALLOWED_NUMPTS-2) v = MAX_ALLOWED_NUMPTS-2;
    }
    else if (n == ADC250_CHAN_NUMPTS)
    {
        if (v < 2)                    v = 2;
        if (v > MAX_ALLOWED_NUMPTS)   v = MAX_ALLOWED_NUMPTS;
    }

    return v;
}

static void ActivatePLLPreset(adcNNN_privrec_t *me, int n)
{
fprintf(stderr, "[%d] %s %d: %d 0x%08x 0x%08x\n", me->devid, __FUNCTION__, n, pll_presets_table[n].clk_src, pll_presets_table[n].pll1, pll_presets_table[n].pll2);
    me->lvmt->a32wr32(me->handle, ADC4X250_R_CLK_SRC,    pll_presets_table[n].clk_src);
    me->lvmt->a32wr32(me->handle, ADC4X250_R_PLL1_CTRL,  pll_presets_table[n].pll1);
    me->lvmt->a32wr32(me->handle, ADC4X250_R_PLL2_CTRL,  pll_presets_table[n].pll2);
    me->lvmt->a32wr32(me->handle, ADC4X250_R_PLL_UPDATE, ADC4X250_PLL_UPDATE_PLL_CTRL_UPDATE_mask);
}

enum
{
    HEARTBEAT_FREQ  = 10,
    HEARTBEAT_USECS = 1000000 / HEARTBEAT_FREQ,
};
static void adcNNN_hbt(int devid, void *devptr,
                       sl_tid_t tid  __attribute__((unused)),
                       void *privptr __attribute__((unused)));
static void  Init1Param(adcNNN_privrec_t *me, int n, int32 v)
{
    if (me->nxt_args[n] < 0) me->nxt_args[n] = v;
    me->nxt_args[n] = ValidateParam(&(me->pz), n, me->nxt_args[n]);
}
static int   InitParams(pzframe_drv_t *pdr)
{
  adcNNN_privrec_t *me = PDR2ME(pdr);

  int               n;
  uint32            w;
  int               r;

/*
    Note: to stop the device, do
          ADC4X250_R_INT_ENA:=0
          ADC4X250_R_CTRL:=ADC4X250_CTRL_ADC_BREAK_ACK
*/

    /*!!! Stop device */
    me->lvmt->a32wr32(me->handle, ADC4X250_R_CTRL,       ADC4X250_CTRL_ADC_BREAK_ACK); // Stop (or drop ADC_CMPLT)
    me->lvmt->a32rd32(me->handle, ADC4X250_R_INT_STATUS, &w); // Drop INT_STATUS bits (in fact, not required)

    /*!!! Program IRQ? */
    if (me->irq_n != 5)
        DoDriverLog(me->devid, 0, "WARNING: irq_n=%d, but ADC4x250 supports only =5", me->irq_n);
    me->lvmt->a32wr32(me->handle, ADC4X250_R_INT_LINE,   me->irq_n);
    me->lvmt->a32wr32(me->handle, ADC4X250_R_INT_VECTOR, me->irq_vect);
    me->lvmt->a32wr32(me->handle, ADC4X250_R_INT_ENA,
                      ADC4X250_INT_ENA_ALL       |
                      ADC4X250_INT_ENA_ADC_CMPLT |
                      ADC4X250_INT_ENA_CALIB_CMPLT);

    ReturnDevInfo(me);
    ReturnClbInfo(me);

    /* Read current values from device */
    r = me->lvmt->a32rd32(me->handle, ADC4X250_R_TIMER,         &w);
fprintf(stderr, "%s *** %s[%d] ***\n", strcurtime_msc(), __FUNCTION__, me->devid);
fprintf(stderr, "\t\tTIMER=%08x r=%d\n", w, r);
    if (r < 0)
    {
        DoDriverLog(me->devid, 0, "a32rd32(ADC4X250_R_TIMER): %d; probably a missing/unconfigured device, terminating", r);
        return -CXRF_CAMAC_NO_X;
    }
    if (w <= 1)                 w = 1024;
    if (w > MAX_ALLOWED_NUMPTS) w = MAX_ALLOWED_NUMPTS;
    Init1Param(me, ADC250_CHAN_NUMPTS, w);
    Init1Param(me, ADC250_CHAN_PTSOFS, 0);

    if (me->BASE_SW_VER < 8) w = ADC4X250_ADC_DECIM_RATIO_MIN;
    else
        me->lvmt->a32rd32(me->handle, ADC4X250_R_ADC_DECIM_RATIO,   &w);
    Init1Param(me, ADC250_CHAN_FRQDIV, w);

    n = me->nxt_args[ADC250_CHAN_PLL_PRESET];
    if (n > 0  &&  n < countof(pll_presets_table))
    {
        ActivatePLLPreset(me, n);
        Init1Param(me, ADC250_CHAN_TIMING, pll_presets_table[n].clk_src);
    }
    else
    {
        me->lvmt->a32rd32(me->handle, ADC4X250_R_CLK_SRC,           &w);
        Init1Param(me, ADC250_CHAN_TIMING,
                   (w >> ADC4X250_CLK_SRC_shift) & ADC4X250_CLK_SRC_bits);
    }

    me->lvmt->a32rd32(me->handle, ADC4X250_R_ADC_TRIG_SOURCE,   &w);
fprintf(stderr, "\t\tTRIG_SOURCE=%08x\n", w);
    Init1Param(me, ADC250_CHAN_TRIG_TYPE, 
               (w >> ADC4X250_ADC_TRIG_SOURCE_TRIG_ENA_shift)
                   & ADC4X250_ADC_TRIG_SOURCE_TRIG_ENA_bits);
    Init1Param(me, ADC250_CHAN_TRIG_INPUT,
               (w >> ADC4X250_ADC_TRIG_SOURCE_TTL_INPUT_shift)
                   & ADC4X250_ADC_TRIG_SOURCE_TTL_INPUT_bits);
fprintf(stderr, "\t\t[ADC250_CHAN_TRIG_TYPE]=%d [ADC250_CHAN_TRIG_INPUT]=%d\n", me->nxt_args[ADC250_CHAN_TRIG_TYPE], me->nxt_args[ADC250_CHAN_TRIG_INPUT]);

    for (n = 0;  n < DEVDEP_NUM_LINES;  n++)
    {
        me->lvmt->a32rd32(me->handle,
                          ADC4X250_R_PGA_RANGE_CHx_base + n * ADC4X250_R_PGA_RANGE_CHx_incr,
                          &w);
        Init1Param(me, ADC250_CHAN_RANGE0 + n,
                   (w >> ADC4X250_PGA_RANGE_shift) & ADC4X250_PGA_RANGE_bits);
    }

    /* Device properties */
    Return1Param(me, ADC250_CHAN_XS_FACTOR,  -9);
    for (n = 0;  n < DEVDEP_NUM_LINES;  n++)
    {
        Return1Param(me, ADC250_CHAN_LINE0TOTALMIN + n, ADC250_MIN_VALUE);
        Return1Param(me, ADC250_CHAN_LINE0TOTALMAX + n, ADC250_MAX_VALUE);
    }
    Return1Param    (me, ADC250_CHAN_NUM_LINES,         DEVDEP_NUM_LINES);

    SetChanRDs(me->devid, ADC250_CHAN_DATA, 1
#ifndef DEVTYPE_ADC1000 // ADC1000 is 1-channel
                                             +DEVDEP_NUM_LINES
#endif
                                                              , 4096.0, 0.0);
    SetChanRDs(me->devid, ADC250_CHAN_STATS_first,
                          ADC250_CHAN_STATS_last-ADC250_CHAN_STATS_first+1,
                                                                 4096.0, 0.0);
    SetChanRDs(me->devid, ADC250_CHAN_ELAPSED, 1, 1000.0, 0.0);

#if 1
    sl_enq_tout_after(me->devid, me, HEARTBEAT_USECS, adcNNN_hbt, NULL);
#endif

    //pzframe_drv_rw_p(&(me->pz), ADC250_CHAN_RUN_MODE, me->nxt_args[ADC250_CHAN_RUN_MODE], DRVA_WRITE);
    for (n = 0;  n < countof(chinfo);  n++)
        if (chinfo[n].chtype == PZFRAME_CHTYPE_PZFRAME_STD  &&
            me->nxt_args[n] >= 0)
            pzframe_drv_rw_p(&(me->pz), n, me->nxt_args[n], DRVA_WRITE);

    return DEVSTATE_OPERATING;
}
static void  StopDevice(pzframe_drv_t *pdr)
{
  adcNNN_privrec_t *me = PDR2ME(pdr);

    me->lvmt->a32wr32(me->handle, ADC4X250_R_INT_VECTOR, 0);
    me->lvmt->a32wr32(me->handle, ADC4X250_R_INT_ENA,    0);
}

static int  StartMeasurements(pzframe_drv_t *pdr)
{
  adcNNN_privrec_t *me = PDR2ME(pdr);

  int               nl;
  uint32            w;

    /* "Actualize" requested parameters */
    memcpy(me->cur_args, me->nxt_args, sizeof(me->cur_args));

    /* Zeroth step: check PTSOFS against NUMPTS */
    if (me->cur_args    [ADC250_CHAN_PTSOFS] > MAX_ALLOWED_NUMPTS - me->cur_args[ADC250_CHAN_NUMPTS])
        Return1Param(me, ADC250_CHAN_PTSOFS,   MAX_ALLOWED_NUMPTS - me->cur_args[ADC250_CHAN_NUMPTS]);

    /* Check if calibration-affecting parameters had changed */
    if (0)
        /*!!! InvalidateCalibrations(me) */;

    /* Store parameters for future comparison */
    memcpy(me->prv_args, me->cur_args, sizeof(me->prv_args));

    /* a. Prepare: stop/init the device */
    me->lvmt->a32wr32(me->handle, ADC4X250_R_CTRL,       ADC4X250_CTRL_ADC_BREAK_ACK); // Stop
    me->lvmt->a32rd32(me->handle, ADC4X250_R_INT_STATUS, &w); // Drop INT_STATUS bits
    /* b. Set parameters */
    me->lvmt->a32wr32(me->handle, ADC4X250_R_TIMER,
                      me->cur_args[ADC250_CHAN_PTSOFS] +
                      me->cur_args[ADC250_CHAN_NUMPTS]); // 31.01.2022: no need to add ADC4X250_TIMER_PRETRIG=18, since this "+18" is performed by device automatically; thus, this "+ADC4X250_TIMER_PRETRIG" was commented out long ago
    ////me->lvmt->a32wr32(me->handle, ADC4X250_R_CLK_SRC, me->cur_args[ADC250_CHAN_TIMING]); // 19.01.2022: NO need to set CLK_SRC at every start, because that is set individually along with other PLL parameters
    /* FRQDIV, if supported... */
    if (me->BASE_SW_VER >= 0x08)
        me->lvmt->a32wr32(me->handle, ADC4X250_R_ADC_DECIM_RATIO,
                          me->cur_args[ADC250_CHAN_FRQDIV]);
    for (nl = 0;  nl < DEVDEP_NUM_LINES;  nl++)
    {
        me->lvmt->a32wr32(me->handle,
                          ADC4X250_R_PGA_RANGE_CHx_base + nl * ADC4X250_R_PGA_RANGE_CHx_incr,
                          me->cur_args[ADC250_CHAN_RANGE0 + nl]);
    }
    me->lvmt->a32wr32(me->handle, ADC4X250_R_ADC_TRIG_SOURCE,
                      ((me->cur_args[ADC250_CHAN_TRIG_TYPE]  & ADC4X250_ADC_TRIG_SOURCE_TRIG_ENA_bits)  << ADC4X250_ADC_TRIG_SOURCE_TRIG_ENA_shift)
                      |
                      ((me->cur_args[ADC250_CHAN_TRIG_INPUT] & ADC4X250_ADC_TRIG_SOURCE_TTL_INPUT_bits) << ADC4X250_ADC_TRIG_SOURCE_TTL_INPUT_shift));

    /* Calibration-related... */
    /* a. Decide */
    if ((me->cur_args[ADC250_CHAN_CALIBRATE] & CX_VALUE_LIT_MASK) != 0)
    {
        me->start_mask = ADC4X250_CTRL_CALIB;
        Return1Param(me, ADC250_CHAN_CALIBRATE, 0);
    }
    else
        me->start_mask = ADC4X250_CTRL_START;
    /* b. Related activity */
    me->do_return  = me->start_mask != ADC4X250_CTRL_CALIB;
    me->force_read = me->start_mask == ADC4X250_CTRL_CALIB;

    /* Let's go! */
#if 0
    /* A pause between R_CTRL:=BREAK_ACK and R_CTRL:=START
       was required some time ago because of a bug in ADC250 */
    SleepBySelect(10000);
#endif
    me->lvmt->a32wr32(me->handle, ADC4X250_R_CTRL, me->start_mask);

    return PZFRAME_R_DOING;
}

static int  TrggrMeasurements(pzframe_drv_t *pdr)
{
  adcNNN_privrec_t *me = PDR2ME(pdr);

    /* Don't even try anything in CALIBRATE mode */
    if (me->start_mask == ADC4X250_CTRL_CALIB) return PZFRAME_R_DOING;

    /* 0. Switch to prog-start mode */
    me->lvmt->a32wr32(me->handle, ADC4X250_R_ADC_TRIG_SOURCE,
                      (ADC4X250_TRIG_ENA_VAL_INT            << ADC4X250_ADC_TRIG_SOURCE_TRIG_ENA_shift)
                      |
                      (me->cur_args[ADC250_CHAN_TRIG_INPUT] << ADC4X250_ADC_TRIG_SOURCE_TTL_INPUT_shift));

    /* 1. Re-start with new settings: */
    /* 1.1. Stop device */
    me->lvmt->a32wr32(me->handle, ADC4X250_R_CTRL, ADC4X250_CTRL_ADC_BREAK_ACK);
    /* 1.2. And do prog-start */
    me->lvmt->a32wr32(me->handle, ADC4X250_R_CTRL, ADC4X250_CTRL_START);

    /* 2. Revert to what-programmed start mode */
    me->lvmt->a32wr32(me->handle, ADC4X250_R_ADC_TRIG_SOURCE,
                      ((me->cur_args[ADC250_CHAN_TRIG_TYPE]  & ADC4X250_ADC_TRIG_SOURCE_TRIG_ENA_bits)  << ADC4X250_ADC_TRIG_SOURCE_TRIG_ENA_shift)
                      |
                      ((me->cur_args[ADC250_CHAN_TRIG_INPUT] & ADC4X250_ADC_TRIG_SOURCE_TTL_INPUT_bits) << ADC4X250_ADC_TRIG_SOURCE_TTL_INPUT_shift));

    return PZFRAME_R_DOING;
}

static int  AbortMeasurements(pzframe_drv_t *pdr)
{
  adcNNN_privrec_t *me = PDR2ME(pdr);
  uint32            w;
  int               n;

    me->lvmt->a32wr32(me->handle, ADC4X250_R_CTRL,       ADC4X250_CTRL_ADC_BREAK_ACK); // Stop
    me->lvmt->a32rd32(me->handle, ADC4X250_R_INT_STATUS, &w); // Drop INT_STATUS bits

    if (me->cur_args[ADC250_CHAN_NUMPTS] != 0)
        bzero(me->retdata,
              sizeof(me->retdata[0])
              *
              me->cur_args[ADC250_CHAN_NUMPTS] * DEVDEP_NUM_LINES);

    for (n = ADC250_CHAN_STATS_first;  n <= ADC250_CHAN_STATS_last;  n++)
        me->cur_args[n] = me->nxt_args[n] = 0;

    return PZFRAME_R_READY;
}

static rflags_t ReadMeasurements(pzframe_drv_t *pdr)
{
  adcNNN_privrec_t *me = PDR2ME(pdr);

  int               numduplets;
  int32             w; // Note SIGNEDNESS
  uint32            status;

  int               nl;
  uint32            ro; // Read Offset
  ADC250_DATUM_T   *dp;
  int               n;
  ADC250_DATUM_T    tmpv;

////fprintf(stderr, "%s %s ENTRY\n", strcurtime_msc(), __FUNCTION__);
    /* Was it a calibration? */
    if (me->start_mask == ADC4X250_CTRL_CALIB)
        ;

    /* Stop the device */
    me->lvmt->a32rd32(me->handle, ADC4X250_R_INT_STATUS, &w);
////fprintf(stderr, "INT_STATUS=%08x\n", w);
    me->lvmt->a32wr32(me->handle, ADC4X250_R_CTRL,       ADC4X250_CTRL_ADC_BREAK_ACK); // Stop

    numduplets = (me->cur_args[ADC250_CHAN_NUMPTS] + 1) / 2;
    for (nl = 0;  nl < DEVDEP_NUM_LINES;  nl++)
    {
        dp = me->retdata + nl * me->cur_args[ADC250_CHAN_NUMPTS];
        ro = ADC4X250_DATA_ADDR_CHx_base + nl * ADC4X250_DATA_ADDR_CHx_incr +
             (me->cur_args[ADC250_CHAN_PTSOFS] / 2) * 4 +
             (DEVDEP_PRETRIG                   / 2) * 4;
#if 1 // Vector/BLT read
        // 1. Read the whole data block as 32-bit words
        n = me->lvmt->a32rd32v(me->handle, ro, (uint32*)dp, numduplets);
////fprintf(stderr, "[%d] a32rd32v=%d err=%d/<%s>\n", me->devid, n, errno, strerror(errno));
        // 2. Swap even and odd 16-bit halfwords (those arrive swapped because of big-endian)
        // ...but only for firmvare older than version 7, when word-order was changed
        if (me->BASE_SW_VER < 7)
            for (n = numduplets;  n > 0;  n--)
            {
                tmpv  = dp[0];
                dp[0] = dp[1]; dp++;
                dp[0] = tmpv;  dp++;
            }
#else // Word-by-word read
        if (me->BASE_SW_VER < 7)
            for (n = numduplets;  n > 0;  n--, ro += 4)
            {
                me->lvmt->a32rd32(me->handle, ro, &w);
                *dp++ =  w        & 0xFFFF;
                *dp++ = (w >> 16) & 0xFFFF;
            }
        else
            for (n = numduplets;  n > 0;  n--, ro += 4)
            {
                me->lvmt->a32rd32(me->handle, ro, &w);
                *dp++ = (w >> 16) & 0xFFFF;
                *dp++ =  w        & 0xFFFF;
            }
#endif
    }
    for (n = ADC250_CHAN_STATS_first;  n <= ADC250_CHAN_STATS_last;  n++)
        me->cur_args[n] = me->nxt_args[n] = 0;

/*!!! ADC250_CHAN_CLB_FAILx, ADC250_CHAN_OVERFLOWx */
    me->lvmt->a32rd32(me->handle, ADC4X250_R_STATUS, &status);
    for (n = 0;  n < ADC250_NUM_ADCS;  n++)
        me->cur_args[ADC250_CHAN_OVERFLOW0 + n] =
        me->nxt_args[ADC250_CHAN_OVERFLOW0 + n] =
            (status >> (ADC4X250_STATUS_PGA_OVERRNG_shift + n)) & 1;

////fprintf(stderr, "%s EXIT\n", __FUNCTION__);
    return 0;
}

static void adcNNN_hbt(int devid, void *devptr,
                       sl_tid_t tid  __attribute__((unused)),
                       void *privptr __attribute__((unused)))
{
  adcNNN_privrec_t  *me     = (adcNNN_privrec_t *) devptr;
  uint32             w;

    sl_enq_tout_after(devid, devptr, HEARTBEAT_USECS, adcNNN_hbt, NULL);

return;
    me->lvmt->a32rd32(me->handle, ADC4X250_R_INT_STATUS, &w);
    if (w == 0) return;
fprintf(stderr, "%s %s(): INT_STATUS=%08x, simulating IRQ\n", strcurtime_msc(), __FUNCTION__, w);

    if (me->do_return == 0  &&  me->force_read) ReadMeasurements(&(me->pz));
    pzframe_drv_drdy_p(&(me->pz), me->do_return, 0);
}

static void PrepareRetbufs     (pzframe_drv_t *pdr, rflags_t add_rflags)
{
  adcNNN_privrec_t *me = PDR2ME(pdr);

  int               n;
  int               x;

////fprintf(stderr, "%s %s ENTRY\n", strcurtime_msc(), __FUNCTION__);
    n = 0;

    /* 1. Device-specific: calc  */

    me->cur_args[ADC250_CHAN_XS_PER_POINT] = DEVDEP_XS_PER_POINT * (me->cur_args[ADC250_CHAN_FRQDIV] + 1); // Note: FRQDIV value is maintained sane even for BASE_SW_VER<0x08 -- in that case it is forced =0 by ValidateParam() and InitParams()
    me->cur_args[ADC250_CHAN_XS_DIVISOR]   = 1;
    for (x = 0;  x < DEVDEP_NUM_LINES;  x++)
    {
        me->cur_args[ADC250_CHAN_LINE0ON       + x] = 1;
        me->cur_args[ADC250_CHAN_LINE0RANGEMIN + x] =
ADC250_MIN_VALUE;
//            (    0-2048) * quants[me->cur_args[ADC250_CHAN_RANGE0 + x] & 3];
        me->cur_args[ADC250_CHAN_LINE0RANGEMAX + x] =
ADC250_MAX_VALUE;
//            (0xFFF-2048) * quants[me->cur_args[ADC250_CHAN_RANGE0 + x] & 3];
    }

    /* 2. Device-specific: mark-for-returning */

    /* Optional calculated channels */
    if (me->cur_args[ADC250_CHAN_CALC_STATS])
        for (x = ADC250_CHAN_STATS_first;  x <= ADC250_CHAN_STATS_last; x++)
        {
            me->r.addrs [n] = x;
            me->r.dtypes[n] = CXDTYPE_INT32;
            me->r.nelems[n] = 1;
            me->r.val_ps[n] = me->cur_args + x;
            me->r.rflags[n] = 0;
            n++;
        }

    /* 3. General */

    /* Scalar STATUS channels */
    for (x = 0;  x < countof(chinfo);  x++)
        if (chinfo[x].chtype == PZFRAME_CHTYPE_STATUS)
        {
            if (chinfo[x].refchn >= 0) // Copy CUR value from respective control one
                me->cur_args[x] = me->cur_args[chinfo[x].refchn];
            me->nxt_args[x] = me->cur_args[x]; // For them to stay in place upon copy cur_args[]=nxt_args[]
            me->r.addrs [n] = x;
            me->r.dtypes[n] = CXDTYPE_INT32;
            me->r.nelems[n] = 1;
            me->r.val_ps[n] = me->cur_args + x;
            me->r.rflags[n] = 0;
            n++;
        }

    /* Vector channels, which were requested */
    if (me->data_rqd)
    {
        me->r.addrs [n] = ADC250_CHAN_DATA;
        me->r.dtypes[n] = ADC250_DTYPE;
        me->r.nelems[n] = me->cur_args[ADC250_CHAN_NUMPTS] * DEVDEP_NUM_LINES;
        me->r.val_ps[n] = me->retdata;
        me->r.rflags[n] = add_rflags;
        n++;
    }
#ifndef DEVTYPE_ADC1000 // ADC1000 is 1-channel
    for (x = 0;  x < DEVDEP_NUM_LINES;  x++)
        if (me->line_rqd[x])
        {
            me->r.addrs [n] = ADC250_CHAN_LINE0 + x;
            me->r.dtypes[n] = ADC250_DTYPE;
            me->r.nelems[n] = me->cur_args[ADC250_CHAN_NUMPTS];
            me->r.val_ps[n] = me->retdata + me->cur_args[ADC250_CHAN_NUMPTS] * x;
            me->r.rflags[n] = add_rflags;
            n++;
        }
#endif
    /* A marker */
    me->r.addrs [n] = ADC250_CHAN_MARKER;
    me->r.dtypes[n] = CXDTYPE_INT32;
    me->r.nelems[n] = 1;
    me->r.val_ps[n] = me->cur_args + ADC250_CHAN_MARKER;
    me->r.rflags[n] = add_rflags;
    n++;
    /* ...and drop "requested" flags */
    me->data_rqd = 0;
#ifndef DEVTYPE_ADC1000 // ADC1000 is 1-channel
    bzero(me->line_rqd, sizeof(me->line_rqd));
#endif

    /* Store retbufs data */
    me->pz.retbufs.count      = n;
    me->pz.retbufs.addrs      = me->r.addrs;
    me->pz.retbufs.dtypes     = me->r.dtypes;
    me->pz.retbufs.nelems     = me->r.nelems;
    me->pz.retbufs.values     = me->r.val_ps;
    me->pz.retbufs.rflags     = me->r.rflags;
    me->pz.retbufs.timestamps = NULL;

#if 0
////fprintf(stderr, "retbufs.count=%d\n", n);
////for (x = 0;  x < me->pz.retbufs.count;  x++)
////    fprintf(stderr, "[%d] #%d dt=%d nel=%d\n", x, me->pz.retbufs.addrs[x], me->pz.retbufs.dtypes[x], me->pz.retbufs.nelems[x]);
#endif
////fprintf(stderr, "%s EXIT\n", __FUNCTION__);
}

static void adcNNN_rw_p (int devid, void *devptr,
                         int action,
                         int count, int *addrs,
                         cxdtype_t *dtypes, int *nelems, void **values)
{
  adcNNN_privrec_t *me = devptr;

  int               n;     // channel N in addrs[]/.../values[] (loop ctl var)
  int               chn;   // channel
  int               ct;
  int32             val;   // Value

  uint32            status;

  uint32            clk_src;
  uint32            pll1;
  uint32            pll2;

    for (n = 0;  n < count;  n++)
    {
        chn = addrs[n];

#if 1
        if (chn == 51) // That's absent ADC250_CHAN_CLB_STATE
        {
            ReturnInt32Datum(devid, chn, me->pz.measuring_now, 0);
            goto NEXT_CHANNEL;
        }
#endif

        if (chn < 0  ||  chn >= countof(chinfo)  ||
            (ct = chinfo[chn].chtype) == PZFRAME_CHTYPE_UNSUPPORTED)
        {
            ReturnInt32Datum(devid, chn, 0, CXRF_UNSUPPORTED);
        }
        else if (ct == PZFRAME_CHTYPE_BIGC)
        {
////fprintf(stderr, "%s chn=%d action=%d\n", strcurtime_msc(), chn, action);
            if (chn == ADC250_CHAN_DATA)
                me->data_rqd                        = 1;
#ifndef DEVTYPE_ADC1000 // ADC1000 is 1-channel
            else
                me->line_rqd[chn-ADC250_CHAN_LINE0] = 1;
#endif

//if (chn == ADC250_CHAN_LINE0) fprintf(stderr, "%s 0x%02x line0:req_mes\n", strcurtime_msc(), me->jumpers);
            pzframe_drv_req_mes(&(me->pz));
        }
        else if (ct == PZFRAME_CHTYPE_MARKER)
        {/* Ignore */}
        else
        {
            if (action == DRVA_WRITE)
            {
                if (nelems[n] != 1  ||
                    (dtypes[n] != CXDTYPE_INT32  &&  dtypes[n] != CXDTYPE_UINT32))
                    goto NEXT_CHANNEL;
                val = *((int32*)(values[n]));
                ////fprintf(stderr, " write #%d:=%d\n", chn, val);
            }
            else
                val = 0xFFFFFFFF; // That's just to make GCC happy

            if      (ct == PZFRAME_CHTYPE_STATUS  ||  ct == PZFRAME_CHTYPE_AUTOUPDATED)
            {
                ////if (chn == ADC250_CHAN_CUR_NUMPTS) fprintf(stderr, "ADC250_CHAN_CUR_NUMPTS. =%d\n", me->cur_args[chn]);
                ReturnInt32Datum(devid, chn, me->cur_args[chn], 0);
            }
            else if (ct == PZFRAME_CHTYPE_VALIDATE)
            {
                if (action == DRVA_WRITE)
                    me->nxt_args[chn] = ValidateParam(&(me->pz), chn, val);
                ReturnInt32Datum(devid, chn, me->nxt_args[chn], 0);
            }
            else if (ct == PZFRAME_CHTYPE_INDIVIDUAL)
            {
                if      (chn == ADC250_CHAN_CALIBRATE)
                {
                    if (action == DRVA_WRITE)
                         me->nxt_args[chn] = (val != 0);
                    ReturnInt32Datum(devid, chn, me->nxt_args[chn], 0);
                }
                else if (chn == ADC250_CHAN_PLL_LOCKED)
                {
                    me->lvmt->a32rd32(me->handle, ADC4X250_R_STATUS, &status);
                    ReturnInt32Datum(devid, chn, (status & ADC4X250_STATUS_PLL_LOCKED) != 0, 0);
                }
                else if (chn == ADC250_CHAN_PLL_PRESET)
                {
                    if (action == DRVA_WRITE  &&  
                        val > 0  &&  val < countof(pll_presets_table))
                    {
                        ActivatePLLPreset(me, val);
                        if (me->BASE_SW_VER < 999) // A fallback for FW versions which are unable to read-what-was-just-written
                        {
                            ReturnInt32Datum(devid, ADC250_CHAN_PLL_PRESET, val,                            0);
                            Return1Param    (me,    ADC250_CHAN_TIMING,     pll_presets_table[val].clk_src);
                            goto NEXT_CHANNEL;
                        }
                    }
                    // Read current values
                    me->lvmt->a32rd32(me->handle, ADC4X250_R_CLK_SRC,   &clk_src);
                    me->lvmt->a32rd32(me->handle, ADC4X250_R_PLL1_CTRL, &pll1);
                    me->lvmt->a32rd32(me->handle, ADC4X250_R_PLL2_CTRL, &pll2);
                    // Try to find a correspondig line in presets
                    for (val = countof(pll_presets_table) - 1;  val > 0;  val--)
                        if (clk_src == pll_presets_table[val].clk_src  &&
                            pll1    == pll_presets_table[val].pll1     &&
                            pll2    == pll_presets_table[val].pll2) break;
fprintf(stderr, "\t[%d] detectPLL: %d: %d 0x%08x 0x%08x\n", me->devid, val, clk_src, pll1, pll2);
                    // Return what's found
                    ReturnInt32Datum(devid, ADC250_CHAN_PLL_PRESET,    val,     0);
                    Return1Param    (me,    ADC250_CHAN_TIMING,        clk_src);
                    ReturnInt32Datum(devid, ADC250_CHAN_CUR_PLL1_CTRL, pll1,    0);
                    ReturnInt32Datum(devid, ADC250_CHAN_CUR_PLL2_CTRL, pll2,    0);
                }
            }
            else /*  ct == PZFRAME_CHTYPE_PZFRAME_STD, which also returns UPSUPPORTED for unknown */
                ////fprintf(stderr, "PZFRAME_STD(%d,%d)\n", chn, val),
                 pzframe_drv_rw_p  (&(me->pz), chn,
                                   action == DRVA_WRITE? val : 0,
                                   action);
        }

 NEXT_CHANNEL:;
    }
}


enum
{
    PARAM_SHOT     = ADC250_CHAN_SHOT,
    PARAM_ISTART   = ADC250_CHAN_ISTART,
    PARAM_WAITTIME = ADC250_CHAN_WAITTIME,
    PARAM_STOP     = ADC250_CHAN_STOP,
    PARAM_ELAPSED  = ADC250_CHAN_ELAPSED,
    PARAM_RUN_MODE = ADC250_CHAN_RUN_MODE,
    PARAM_RUN      = ADC250_CHAN_RUN,
};

enum
{
    FASTADC_SPACE_SIZE       = 0xB80000,
    FASTADC_ADDRESS_SIZE     = 32,
    FASTADC_ADDRESS_MODIFIER = ADC4X250_ADDRESS_MODIFIER,
};

//#include "vme_fastadc_common.h"
#include "misc_macros.h"
#include "pzframe_drv.h"


#ifndef FASTADC_NAME
  #error The "FASTADC_NAME" is undefined
#endif


#define FASTADC_PRIVREC_T __CX_CONCATENATE(FASTADC_NAME,_privrec_t)
#define FASTADC_PARAMS    __CX_CONCATENATE(FASTADC_NAME,_params)
#define FASTADC_INIT_D    __CX_CONCATENATE(FASTADC_NAME,_init_d)
#define FASTADC_TERM_D    __CX_CONCATENATE(FASTADC_NAME,_term_d)
#define FASTADC_RW_P      __CX_CONCATENATE(FASTADC_NAME,_rw_p)
#define FASTADC_IRQ_P     __CX_CONCATENATE(FASTADC_NAME,_irq_p)


static void FASTADC_IRQ_P (int devid, void *devptr,
                           int irq_n, int irq_vect)
{
  FASTADC_PRIVREC_T   *me = (FASTADC_PRIVREC_T *)devptr;
  uint32               w;

static int total_irqs = 0;

////fprintf(stderr, "%s IRQ[%d]: n=%d vector=%d\n", strcurtime_msc(), devid, irq_n, irq_vect);
    if (irq_vect != me->irq_vect) return;
fprintf(stderr, "\r%s devid=%-2d total=%d", strcurtime_msc(), devid, ++total_irqs);
////fprintf(stderr, "Yes!\n");
me->lvmt->a32rd32(me->handle, ADC4X250_R_INT_STATUS, &w);
////fprintf(stderr, "zINT_STATUS=%08x\n", w);
me->lvmt->a32wr32(me->handle, ADC4X250_R_CTRL,       ADC4X250_CTRL_ADC_BREAK_ACK); // Stop (or drop ADC_CMPLT)

//fprintf(stderr, "%s 0x%02x IRQ_P(do_return=%d)\n", strcurtime_msc(), me->jumpers, me->do_return);
    /*!!! Here MUST call smth to read ADC4X250_R_INT_STATUS (and, probably, analyze its content) */
    if (me->do_return == 0  &&  me->force_read) ReadMeasurements(&(me->pz));
    pzframe_drv_drdy_p(&(me->pz), me->do_return, 0);
//    pzframe_drv_req_mes(&(me->pz));
}

static int  FASTADC_INIT_D(int devid, void *devptr, 
                           int businfocount, int businfo[],
                           const char *auxinfo __attribute__((unused)))
{
  FASTADC_PRIVREC_T   *me = (FASTADC_PRIVREC_T *)devptr;
  int                  n;
  int                  bus_major;
  int                  bus_minor;
  int                  jumpers;

    bus_major    = businfo[0];
    bus_minor    = businfo[1];
    jumpers      = businfo[2]; /*!!!*/
    me->irq_n    = businfo[3] &  0x7;
    me->irq_vect = businfo[4] & 0xFF;
me->jumpers = jumpers;
fprintf(stderr, "%s businfo[2]=%08x jumpers=0x%x irq=%d vector=%d\n", strcurtime_msc(), businfo[2], jumpers, me->irq_n, me->irq_vect);

    me->lvmt   = GetLayerVMT(devid);
    me->devid  = devid;
    me->handle = me->lvmt->add(devid, devptr,
                               bus_major, bus_minor,
                               jumpers << 24, FASTADC_SPACE_SIZE,
                               32, FASTADC_ADDRESS_MODIFIER,
                               me->irq_n, me->irq_vect, FASTADC_IRQ_P,
                               NULL, VME_LYR_OPTION_NONE);
    if (me->handle < 0) return me->handle;

    for (n = 0;  n < countof(chinfo);  n++)
    {
        if      (chinfo[n].chtype == PZFRAME_CHTYPE_AUTOUPDATED  ||
                 chinfo[n].chtype == PZFRAME_CHTYPE_STATUS)
            SetChanReturnType(me->devid, n, 1, IS_AUTOUPDATED_TRUSTED);
        else if (chinfo[n].chtype == PZFRAME_CHTYPE_BIGC)
            SetChanReturnType(me->devid, n, 1, DO_IGNORE_UPD_CYCLE);
    }

    pzframe_drv_init(&(me->pz), devid,
                     PARAM_SHOT, PARAM_ISTART, PARAM_WAITTIME, PARAM_STOP, PARAM_ELAPSED,
                     PARAM_RUN_MODE, PARAM_RUN, -1, -1,
                     StartMeasurements, TrggrMeasurements,
                     AbortMeasurements, ReadMeasurements,
                     PrepareRetbufs, NULL, NULL);

    return InitParams(&(me->pz));
}

static void FASTADC_TERM_D(int devid, void *devptr)
{
  FASTADC_PRIVREC_T   *me = (FASTADC_PRIVREC_T *)devptr;

    if (me->handle < 0) return; // For non-initialized devices

    StopDevice      (&(me->pz));
    pzframe_drv_term(&(me->pz));
}

DEFINE_CXSD_DRIVER(FASTADC_NAME, __CX_STRINGIZE(FASTADC_NAME) " fast-ADC",
                   NULL, NULL,
                   sizeof(FASTADC_PRIVREC_T), FASTADC_PARAMS,
                   5, 5,
                   VME_LYR_API_NAME, VME_LYR_API_VERSION,
                   NULL,
                   -1, NULL, NULL,
                   FASTADC_INIT_D, FASTADC_TERM_D, FASTADC_RW_P);
