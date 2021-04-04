/*********************************************************************

  Notes:
      1. This driver was based on mqtt_mapping_drv.c
         and is slightly simpler, because there's NO need to:
             a. Know local channel names.
             b. Store foreign-channel-names
                (those are used at initialization only).
      2. data_evproc() is copied from trig_read_drv.c

*********************************************************************/

#include "cxsd_driver.h"
#include "cxsd_hwP.h"
#include "cda.h"


typedef struct
{
    int                 devid;

    cda_context_t       cid;

    int                 numchans;
    cda_dataref_t      *map;
} privrec_t;


static void data_evproc(int            devid,
                        void          *devptr,
                        cda_dataref_t  ref,
                        int            reason,
                        void          *info_ptr,
                        void          *privptr2)
{
  privrec_t  *me  = devptr;
  int         chn = ptr2lint(privptr2);

  cxdtype_t   dtype;
  int         nelems;
  void       *buf;
  rflags_t    rflags;
  cx_time_t   timestamp;
  int         phys_count;
  double     *phys_rds;
  cx_time_t   fresh_age;

    if (chn < 0  ||  chn >= me->numchans) return;

    if      (reason == CDA_REF_R_UPDATE)
    {
        /* Note: 
             we use "ref" instead of "me->map[chn]" because
             this can be first called during initialization,
             upon cda_add_chan(), when me->map[] isn't filled yet
             and is still ==CDA_DATAREF_ERROR */
        dtype  = cda_current_dtype_of_ref (ref);
        nelems = cda_current_nelems_of_ref(ref);
        cda_acc_ref_data                  (ref, &buf,     NULL);
        cda_get_ref_stat                  (ref, &rflags, &timestamp);
        ReturnDataSet(devid, 1,
                      &chn, &dtype,  &nelems,
                      &buf, &rflags, &timestamp);
    }
    else if (reason == CDA_REF_R_RDSCHG)
    {
#if 0
        if (cda_phys_rds_of_ref(ref, &phys_count, &phys_rds) == 0  &&
            phys_count >= 1)
            SetChanRDs     (devid, chn, 1, phys_rds[0], phys_rds[1]);
#endif
    }
    else if (reason == CDA_REF_R_FRESHCHG)
    {
        /* Note: <0 -- error, ==0 -- no fresh_age specified, >0 -- specified */
        if (cda_fresh_age_of_ref(ref, &fresh_age) > 0)
            SetChanFreshAge(devid, chn, 1, fresh_age);
        /* Note 2: is this a valid approach?  If 
                   1) some fresh age is specified;
                   2) reconnect happens,
                   3) ...with no fresh age specified in the new incarnation
                   4) thus the OLD fresh age remains in effect...
           Is it fixable at all? */
    }
}

static void GetNameOf(const CxsdDbDcNsp_t *type_nsp, int chn, const char **foreign_name)
{
  int         nsline;
  const char *channame;

    for (nsline = 0,                        channame =  NULL;
         nsline < type_nsp->items_used  &&  channame == NULL;
         nsline++)
    {
        if (type_nsp->items[nsline].devchan_n == chn)
            channame = CxsdDbGetStr(cxsd_hw_cur_db,
                                    type_nsp->items[nsline].name_ofs);
    }

    *foreign_name = channame;
}
static int bridge_init_d(int devid, void *devptr,
                         int businfocount, int businfo[],
                         const char *auxinfo)
{
  privrec_t      *me = (privrec_t *)devptr;

  int             dev_first;
  int             chn;
  size_t          map_size;

  const char     *type_name;
  int             type_nsp_id;
  const
  CxsdDbDcNsp_t  *type_nsp;

  const char     *foreign_base;

  const char     *foreign_name;
  int             is_rw;
  cxdtype_t       dtype;
  int             max_nelems;
  int             is_trusted;
  int             is_on_cycle;

  int             mirror_mode;
  int             all_trusted;
  int             all_on_cycle;

    me->devid = devid;
    GetDevPlace(devid, &dev_first, &(me->numchans));
    if (me->numchans == 0)
    {
        DoDriverLog(devid, DRIVERLOG_ERR, "mapping is empty, nothing to do");
        return -CXRF_CFG_PROBL;
    }

    map_size = sizeof(me->map[0]) * me->numchans;
    if ((me->map = malloc(map_size)) == NULL)
    {
        DoDriverLog(devid, DRIVERLOG_ERR | DRIVERLOG_ERRNO,
                    "unable to allocate %zd bytes for map",
                    map_size);
        return -CXRF_DRV_PROBL;
    }
    bzero(me->map, map_size);

    foreign_base = auxinfo;
    mirror_mode  = 0;
    all_trusted  = 0;
    all_on_cycle = 0;
    if (foreign_base != NULL)
    {
        if (*foreign_base == '@')
        {
            foreign_base++;
            while (1)
            {
                if      (*foreign_base == '*') mirror_mode  = 1;
                else if (*foreign_base == '!') all_trusted  = 1;
                else if (*foreign_base == '~') all_on_cycle = 1;
                else if (*foreign_base == ':') {foreign_base++; goto END_BASE_FLAGS;}
                else
                {
                    DoDriverLog(devid, 0, "bad @-spec for base");
                    return -CXRF_CFG_PROBL;
                }
                foreign_base++;
            }
        }
 END_BASE_FLAGS:;
    }

    if (mirror_mode)
    {
        if ((type_name = GetDevTypename(devid)) == NULL)
        {
            DoDriverLog(devid, DRIVERLOG_ERR, "type_name==NULL!");
            return -CXRF_DRV_PROBL;
        }
        if ((type_nsp_id = CxsdDbFindNsp(cxsd_hw_cur_db, type_name)) < 0)
        {
            DoDriverLog(devid, DRIVERLOG_ERR, "type_nsp_id<0!");
            return -CXRF_DRV_PROBL;
        }
        if ((type_nsp    = CxsdDbGetNsp (cxsd_hw_cur_db, type_nsp_id)) == NULL)
        {
            DoDriverLog(devid, DRIVERLOG_ERR, "type_nsp==NULL!");
            return -CXRF_DRV_PROBL;
        }
    }

    if ((me->cid = cda_new_context(devid, devptr,
                                   foreign_base, 0,
                                   NULL,
                                   0, NULL, 0)) < 0)
    {
        DoDriverLog(devid, 0, "cda_new_context(): %s", cda_last_err());
        return -CXRF_DRV_PROBL;
    }

    for (chn = 0;  chn < me->numchans;  chn++)
    {
        me->map[chn] = CDA_DATAREF_ERROR;
        is_trusted  = all_trusted;
        is_on_cycle = all_on_cycle;
        if (mirror_mode)
        {
            GetNameOf(type_nsp, chn, &foreign_name);
            if (foreign_name == NULL) goto NEXT_CHANNEL;
        }
        else
        {
            if (CxsdHwGetChanAuxs(dev_first + chn, NULL, &foreign_name) < 0)
            {
                DoDriverLog(devid, 0, "CxsdHwGetChanAuxs(first:%d+chn:%d=%d)<0",
                            dev_first, chn, dev_first + chn);
                goto NEXT_CHANNEL;
            }
            if (foreign_name == NULL) goto NEXT_CHANNEL;

            if (*foreign_name == '@')
            {
                foreign_name++;
                while (1)
                {
                    if      (*foreign_name == '!') is_trusted  = 1;
                    if      (*foreign_name == '~') is_on_cycle = 1;
                    else if (*foreign_name == ':') {foreign_name++; goto END_NAME_FLAGS;}
                    else
                    {
                        DoDriverLog(devid, 0, "bad @-spec for chn:%d", chn);
                        goto NEXT_CHANNEL;
                    }
                    foreign_name++;
                }
            }
 END_NAME_FLAGS:;
        }

        if (CxsdHwGetChanType(dev_first + chn,
                              &is_rw, &dtype, &max_nelems) < 0)
        {
            DoDriverLog(devid, 0, "CxsdHwGetChanType(first:%d+chn:%d=%d)<0",
                        dev_first, chn, dev_first + chn);
            goto NEXT_CHANNEL;
        }

        if ((me->map[chn] = cda_add_chan(me->cid, NULL, foreign_name,
                                         (is_on_cycle? 0 : CDA_DATAREF_OPT_ON_UPDATE) |
                                           CDA_DATAREF_OPT_NO_RD_CONV*0,
                                         dtype, max_nelems,
                                         CDA_REF_EVMASK_UPDATE     |
                                           CDA_REF_EVMASK_RDSCHG   |
                                           CDA_REF_EVMASK_FRESHCHG,
                                         data_evproc, lint2ptr(chn))) < 0)
        {
            DoDriverLog(devid, 0, "cda_new_chan(%d:\"%s\"): %s",
                        chn, foreign_name, cda_last_err());
            goto NEXT_CHANNEL;
        }
////fprintf(stderr, "[%d].%d/%d->%s\n", devid, chn, me->map[chn], foreign_name);

        if (is_rw == 0)
            SetChanReturnType(devid, chn,  1, is_trusted? IS_AUTOUPDATED_TRUSTED : IS_AUTOUPDATED_YES);

 NEXT_CHANNEL:;
    }

    return DEVSTATE_OPERATING;
}

static void bridge_term_d(int devid __attribute__((unused)), void *devptr)
{
  privrec_t      *me = (privrec_t *)devptr;

    safe_free(me->map);  me->map = NULL;
}

static void bridge_rw_p(int devid, void *devptr,
                        int action,
                        int count, int *addrs,
                        cxdtype_t *dtypes, int *nelems, void **values)
{
  privrec_t      *me = (privrec_t *)devptr;

  int             n;   // channel N in addrs[]/.../values[] (loop ctl var)
  int             chn; // channel

    for (n = 0;  n < count;  n++)
    {
        chn = addrs[n];

        if (chn < 0  ||  chn >= me->numchans) goto NEXT_CHANNEL;

        if (action == DRVA_WRITE  &&  me->map[chn] != CDA_DATAREF_ERROR)
            cda_snd_ref_data(me->map[chn],
                             dtypes[n], nelems[n], values[n]);

 NEXT_CHANNEL:;
    }
}

DEFINE_CXSD_DRIVER(bridge, "Channel-bridge driver",
                   NULL, NULL,
                   sizeof(privrec_t), NULL,
                   0, 0,
                   NULL, 0,
                   NULL,
                   -1, NULL, NULL,
                   bridge_init_d, bridge_term_d, bridge_rw_p);
