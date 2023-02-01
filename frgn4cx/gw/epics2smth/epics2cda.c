#include <stdio.h>

#include "misclib.h"

#include "mt_cxscheduler.h"
#include "cda.h"

#include "epics2smth.h"


enum
{
    EC_OK      = 0,
    EC_HELP    = 1,
    EC_USR_ERR = 2,
    EC_ERR     = 3,
};


static cda_context_t  the_cid = -1;


static void ProcessDatarefEvent(int            uniq,
                                void          *privptr1,
                                cda_dataref_t  ref,
                                int            reason,
                                void          *info_ptr,
                                void          *privptr2)
{
  int            mid = ptr2lint(privptr2);

  int            rslvstat;

  cxdtype_t      chn_dtype;
  int            chn_max_nelems;
  int            chn_is_rw;

  rflags_t       cur_rflags;
  cx_time_t      cur_timestamp;
  cxdtype_t      cur_dtype;
  int            cur_nelems;
  void          *cur_data_p;
  size_t         cur_data_size;

    if      (reason == CDA_REF_R_RSLVSTAT)
    {
        rslvstat = ptr2lint(info_ptr);
        if (rslvstat == CDA_RSLVSTAT_FOUND  ||
            rslvstat == CDA_RSLVSTAT_NOTFOUND)
        {
            if (rslvstat == CDA_RSLVSTAT_FOUND)
            {
                cda_hwinfo_of_ref(ref, &chn_is_rw, &chn_dtype, &chn_max_nelems, NULL, NULL);
                cda_set_type     (ref,              chn_dtype,  chn_max_nelems);
                e2s_set_props(mid, chn_dtype, chn_max_nelems, chn_is_rw);
            }
            e2s_set_found(mid, rslvstat == CDA_RSLVSTAT_FOUND);
        }
    }
    else if (reason == CDA_REF_R_UPDATE)
    {
fprintf(stderr, "update %s\n", strcurtime_msc());
        cur_rflags = 0;
        bzero(&cur_timestamp, sizeof(cur_timestamp));
        cda_get_ref_stat(ref, &cur_rflags, &cur_timestamp);

        cur_dtype  = cda_current_dtype_of_ref (ref);
        cur_nelems = cda_current_nelems_of_ref(ref);
        if (cda_acc_ref_data(ref,
                             &cur_data_p, &cur_data_size) >= 0)
        {
            e2s_update_data(mid, cur_dtype, cur_nelems, cur_data_p,
                            0/*!!!some_flags*/, cur_timestamp);
        }
    }
}
static e2s_backend_obj_ref_t epics2cda_add_chan  (const char *name, int mid)
{
  cda_dataref_t  ref;

    mt_sl_lock();
    ref = cda_add_chan(the_cid,
                       NULL, name, 
                       CDA_DATAREF_OPT_NONE,
                       CXDTYPE_UNKNOWN, 16,
                       CDA_REF_EVMASK_UPDATE | CDA_REF_EVMASK_RSLVSTAT,
                       ProcessDatarefEvent,
                       lint2ptr(mid));
    mt_sl_unlock();

    return ref <= 0? NULL : lint2ptr(ref);
}

static int epics2cda_del_chan  (e2s_backend_obj_ref_t obj_ref)
{
    return cda_del_chan(ptr2lint(obj_ref));
}

static int epics2cda_snd_data  (e2s_backend_obj_ref_t obj_ref,
                                cxdtype_t dtype, int nelems, const void *data)
{
    return cda_snd_ref_data(ptr2lint(obj_ref), dtype, nelems, data);
}

static e2s_backend_t  epics2cda_backend =
{
    names_case_sensitive: 0,
    check_name:           NULL,
    add_chan:             epics2cda_add_chan,
    del_chan:             epics2cda_del_chan,
    snd_data:             epics2cda_snd_data,
};

int main(int argc, char *argv[])
{
  const char *option_defpfx = NULL;

    if (argc > 1) option_defpfx = argv[1];

    mt_sl_start();
    mt_sl_lock();
    the_cid = cda_new_context(0, NULL, option_defpfx, 0, argv[0],
                              CDA_CONTEXT_OPT_NONE, NULL, NULL);
    mt_sl_unlock();
    if (the_cid == CDA_CONTEXT_ERROR)
    {
        fprintf(stderr, "%s %s: cda_new_context(\"%s\"): %s\n",
                strcurtime(), argv[0],
                option_defpfx != NULL? option_defpfx : "",
                cda_last_err());
        exit(EC_ERR);
    }

    e2s_run(&epics2cda_backend);

    return 0;
}
