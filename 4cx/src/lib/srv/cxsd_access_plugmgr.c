#include "cxsd_access.h"
#include "cxsd_accessP.h"


CxsdAclLdrModRec *first_aclldr_metric = NULL;


CxsdAccessList  CxsdAccessLoadList(const char *argv0,
                                   const char *def_scheme, const char *reference)
{
  CxsdAccessList    ret;
  char              scheme[20];
  const char       *location;
  CxsdAclLdrModRec *mp;

    /* Decide with scheme */
    if (def_scheme != NULL  &&  *def_scheme == '!')
    {
        strzcpy(scheme, def_scheme + 1, sizeof(scheme));
        location = reference;
    }
    else
        split_url(def_scheme, "::",
                  reference, scheme, sizeof(scheme),
                  &location);

    /* Find appropriate loader */
    for (mp = first_aclldr_metric;  mp != NULL;  mp = mp->next)
        if (strcasecmp(scheme, mp->mr.name) == 0)
            break;
    if (mp == NULL) return NULL;

    /* Do load! */
    ret = mp->ldr(argv0, location);
    
    return ret;
}


int  CxsdAccessRegisterAclLdr  (CxsdAclLdrModRec *metric)
{
  CxsdAclLdrModRec *mp;

    if (metric == NULL)
        return -1;

    /* Check if it is already registered */
    for (mp = first_aclldr_metric;  mp != NULL;  mp = mp->next)
        if (mp == metric) return +1;

    /* Insert at beginning of the list */
    metric->next = first_aclldr_metric;
    first_aclldr_metric = metric;

    return 0;
}

int  CxsdAccessDeregisterAclLdr(CxsdAclLdrModRec *metric)
{
}
