#include <stdio.h>

#include "mt_cxscheduler.h"
#include "cda.h"

#include "cda_adapter.h"


static cda_context_t cda_ctx;
static cda_dataref_t cda_refs[4];

void cda_adapter_init(const char *defpfx, int nbytes, const char *channames[])
{
    char channame[200];

    mt_sl_start();
    mt_sl_lock();
    cda_ctx = cda_new_context(0, NULL, defpfx, 0, NULL, 0, NULL, NULL);
    for (int i = 0;  i < 4;  i++)
    {
        sprintf(channame, "%s.data", channames[i]);
        cda_refs[i] = cda_add_chan(cda_ctx, NULL,
                                   channame, CDA_DATAREF_OPT_NONE,
                                   CXDTYPE_UINT8, nbytes,
                                   0, NULL, NULL);
    }
    mt_sl_unlock();
}

void cda_adapter_send_one(int n, int nbytes, void *buf)
{
        mt_sl_lock();
        cda_snd_ref_data(cda_refs[n], CXDTYPE_UINT8, nbytes, buf);
        mt_sl_unlock();
}
