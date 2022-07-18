#include "cxsd_driver.h"
#include "cxsd_hwP.h"


static int  const_init_d(int devid, void *devptr,
                         int businfocount, int businfo[],
                         const char *auxinfo)
{
  int             dev_first;
  int             numchans;
  int             chn;

  const char     *drvinfo;
  int             is_rw;
  cxdtype_t       dtype;
  int             max_nelems;

  CxAnyVal_t      val;
  void           *val_p;
  char           *endptr;

  static int       n_1         = 1;
  static rflags_t  zero_rflags = 0;

    GetDevPlace(devid, &dev_first, &numchans);
    if (numchans == 0)
    {
        DoDriverLog(devid, DRIVERLOG_ERR, "no channels, nothing to do");
        return -CXRF_CFG_PROBL;
    }

    for (chn = 0;  chn < numchans;  chn++)
    {
        if (CxsdHwGetChanAuxs(dev_first + chn, NULL, &drvinfo) < 0)
        {
            DoDriverLog(devid, 0, "CxsdHwGetChanAuxs(first:%d+chn:%d=%d)<0",
                        dev_first, chn, dev_first + chn);
            goto NEXT_CHANNEL;
        }
        if (drvinfo == NULL) goto NEXT_CHANNEL;

        if (CxsdHwGetChanType(dev_first + chn,
                              &is_rw, &dtype, &max_nelems) < 0)
        {
            DoDriverLog(devid, 0, "CxsdHwGetChanType(first:%d+chn:%d=%d)<0",
                        dev_first, chn, dev_first + chn);
            goto NEXT_CHANNEL;
        }
        if (max_nelems < 1) goto NEXT_CHANNEL;

        switch (dtype)
        {
            case CXDTYPE_UINT8:  val.u8  = strtoul (drvinfo, &endptr, 0); val_p = &(val.u8);  break;
            case CXDTYPE_INT8:   val.i8  = strtoul (drvinfo, &endptr, 0); val_p = &(val.i8);  break;
            case CXDTYPE_UINT16: val.u16 = strtoul (drvinfo, &endptr, 0); val_p = &(val.u16); break;
            case CXDTYPE_INT16:  val.i16 = strtoul (drvinfo, &endptr, 0); val_p = &(val.i16); break;
            case CXDTYPE_UINT32: val.u32 = strtoul (drvinfo, &endptr, 0); val_p = &(val.u32); break;
            case CXDTYPE_INT32:  val.i32 = strtoul (drvinfo, &endptr, 0); val_p = &(val.i32); break;

            case CXDTYPE_UINT64: val.u64 = strtoull(drvinfo, &endptr, 0); val_p = &(val.u64); break;
            case CXDTYPE_INT64:  val.i64 = strtoull(drvinfo, &endptr, 0); val_p = &(val.i64); break;

            case CXDTYPE_SINGLE: val.f32 = strtod  (drvinfo, &endptr);    val_p = &(val.f32); break;
            case CXDTYPE_DOUBLE: val.f64 = strtod  (drvinfo, &endptr);    val_p = &(val.f64); break;

            default: 
                DoDriverLog(devid, 0, "unsupported dtype=%d for channel first:%d+chn:%d=%d",
                            dtype,
                            dev_first, chn, dev_first + chn);
                goto NEXT_CHANNEL;
        }
        if (endptr == drvinfo  ||  *endptr != '\0')
        {
            DoDriverLog(devid, 0, "syntax error in channel first:%d+chn:%d=%d value \"%s\"",
                        dev_first, chn, dev_first + chn,
                        drvinfo);
            return -CXRF_CFG_PROBL;
        }

        SetChanReturnType(devid, chn,  1, IS_AUTOUPDATED_TRUSTED);
        ReturnDataSet(devid, 1,
                      &chn, &dtype, &n_1,
                      &val_p, &zero_rflags, NULL);

 NEXT_CHANNEL:;
    }

    return DEVSTATE_OPERATING;
}

static void const_rw_p(int devid, void *devptr __attribute__((unused)),
                       int action,
                       int count, int *addrs,
                       cxdtype_t *dtypes, int *nelems, void **values)
{
    if (action == DRVA_WRITE)
        StdSimulated_rw_p(devid, devptr, 
                          action,
                          count, addrs,
                          dtypes, nelems, values);
}


DEFINE_CXSD_DRIVER(const, "CONSTant-values driver",
                   NULL, NULL,
                   0, NULL,
                   0, 1000,
                   NULL, 0,
                   NULL,
                   -1, NULL, NULL,
                   const_init_d, NULL, const_rw_p);
