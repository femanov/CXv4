#include "cxsd_access.h"
#include "cxsd_accessP.h"


enum
{
    INITIAL_ITEMS_ALLOCD = 20,
    ITEMS_ALLOC_INC      = 30,
};

static CxsdAccessList cxsd_access_default_acl = NULL;


CxsdAccessList   CxsdAccessCreateList (void)
{
  CxsdAccessList  acl;

    acl = malloc(sizeof(*acl));
    if (acl == NULL) return NULL;

    bzero(acl, sizeof(*acl));
    acl->items = malloc(sizeof(acl->items[0]) * INITIAL_ITEMS_ALLOCD);
    if (acl->items == NULL)
    {
        free(acl);
        return NULL;
    }
    acl->items_allocd = INITIAL_ITEMS_ALLOCD;

    return acl;
}

int              CxsdAccessAddListItem(CxsdAccessList    acl,
                                       CxsdAccessItem_t *item)
{
  CxsdAccessItem_t *new_items;

    if (!(acl->items_used < acl->items_allocd))
    {
        new_items = safe_realloc(acl->items,
                                 sizeof(*new_items) * (acl->items_allocd + ITEMS_ALLOC_INC));
        if (new_items == NULL) return -1;
        acl->items = new_items;
        acl->items_allocd += ITEMS_ALLOC_INC;
    }

    acl->items[acl->items_used] = *item;
    acl->items_used++;

    return 0;
}

int              CxsdAccessDestroyList(CxsdAccessList    acl)
{
    if (acl == NULL) return 0;

    safe_free(acl->items);
    free(acl);

    return 0;
}

int              CxsdAccessSetPolicy  (CxsdAccessList    acl,
                                       CxsdAccessPerms   def_perms)
{
    if (acl == NULL) return -1;

    acl->def_perms = def_perms;

    return 0;
}

CxsdAccessPerms  CxsdAccessCheck      (CxsdAccessList    acl, uint32 ip)
{
  CxsdAccessItem_t *item;
  int               left;

    if (acl == NULL) acl = cxsd_access_default_acl;
    if (acl == NULL) return CXSD_ACCESS_PERM_ALL;

    if (ip == 0) ip = 0x7f000001; // 0 means "local" - turn it into 127.0.0.1

    for (left = acl->items_used, item = acl->items;
         left > 0;
         left--,                 item++)
    {
        if ((item->kind == CXSD_ACCESS_KIND_RANGE  &&
             (ip >= item->ip1  &&  ip <= item->ip2))
            ||
            (item->kind == CXSD_ACCESS_KIND_MASK   &&
             ((ip & item->ip1)     ==    item->ip2))
           )
            return item->perms;
    }

    return acl->def_perms;
}

int              CxsdAccessSetDefACL  (CxsdAccessList    acl)
{
    CxsdAccessDestroyList(cxsd_access_default_acl);
    cxsd_access_default_acl = acl;

    return 0;
}
