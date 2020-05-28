#ifndef __CXSD_ACCESS_H
#define __CXSD_ACCESS_H


#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


#include "misc_types.h"


enum
{
    CXSD_ACCESS_PERM_NONE    = 0,

    CXSD_ACCESS_PERM_CONNECT = 1 << 0,
    CXSD_ACCESS_PERM_READ    = 1 << 1,
    CXSD_ACCESS_PERM_WRITE   = 1 << 2,
    CXSD_ACCESS_PERM_ADMIN   = 1 << 3,  // Write to is_internal channels allowed; not implemented
    CXSD_ACCESS_PERM_LOCK    = 1 << 4,

    CXSD_ACCESS_PERM_ALL     = ~0,
};


typedef struct _CxsdAccessList_t_struct *CxsdAccessList;

typedef uint32 CxsdAccessPerms;


CxsdAccessList   CxsdAccessLoadList(const char *argv0,
                                    const char *def_scheme, const char *reference);
int              CxsdAccessDestroyList(CxsdAccessList    acl);

int              CxsdAccessSetPolicy  (CxsdAccessList    acl,
                                       CxsdAccessPerms   def_perms);

CxsdAccessPerms  CxsdAccessCheck      (CxsdAccessList    acl, uint32 ip);

int              CxsdAccessSetDefACL  (CxsdAccessList    acl);


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* __CXSD_ACCESS_H */
