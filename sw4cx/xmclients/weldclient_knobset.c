#include"KnobsP.h"
#include"weldclient_knobset.h"

#include "weldclient_process_noop.h"

static knobs_knobset_t weldclient_knobset =
{
    (dataknob_unif_vmt_t *[]){
        (dataknob_unif_vmt_t *)&weldclient_process_noop_vmt,
        NULL
    },
    NULL
};

void RegisterWeldclientKnobset(void)
{
    RegisterKnobset(&weldclient_knobset);
}
