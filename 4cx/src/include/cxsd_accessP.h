#ifndef __CXSD_ACCESSP_H
#define __CXSD_ACCESSP_H


#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


#include "cx_module.h"
#include "cxsd_driver.h"
#include "cxsd_access.h"


enum
{
    CXSD_ACCESS_KIND_RANGE = 1,
    CXSD_ACCESS_KIND_MASK  = 2,
};

typedef struct
{
    int              kind;
    CxsdAccessPerms  perms;
    uint32           ip1;
    uint32           ip2;
} CxsdAccessItem_t;

typedef struct _CxsdAccessList_t_struct
{
    CxsdAccessPerms   def_perms;
    CxsdAccessItem_t *items;
    int               items_used;
    int               items_allocd;
} CxsdAccessList_t;


CxsdAccessList   CxsdAccessCreateList (void);
int              CxsdAccessAddListItem(CxsdAccessList acl,
                                       CxsdAccessItem_t *item);


//////////////////////////////////////////////////////////////////////

#define CXSD_ACLLDR_MODREC_SUFFIX _aclldr_rec
#define CXSD_ACLLDR_MODREC_SUFFIX_STR __CX_STRINGIZE(CXSD_ACLLDR_MODREC_SUFFIX)

enum {CXSD_ACLLDR_MODREC_MAGIC = 0x644c6341}; // Little-endian 'AcLd'
enum {
    CXSD_ACLLDR_MODREC_VERSION_MAJOR = 1,
    CXSD_ACLLDR_MODREC_VERSION_MINOR = 0,
    CXSD_ACLLDR_MODREC_VERSION = CX_ENCODE_VERSION(CXSD_ACLLDR_MODREC_VERSION_MAJOR,
                                                   CXSD_ACLLDR_MODREC_VERSION_MINOR)
};

typedef CxsdAccessList (*CxsdAclLdr_t)(const char *argv0,
                                       const char *reference);

typedef struct _CxsdAclLdrModRec_struct
{
    cx_module_rec_t  mr;

    CxsdAclLdr_t     ldr;

    struct _CxsdAclLdrModRec_struct *next;
} CxsdAclLdrModRec;

#define CXSD_ACLLDR_MODREC_NAME(name) \
    __CX_CONCATENATE(name, CXSD_ACLLDR_MODREC_SUFFIX)

#define DECLARE_CXSD_ACLLDR(name) \
    CxsdAclLdrModRec CXSD_ACLLDR_MODREC_NAME(name)

#define DEFINE_CXSD_ACLLDR(name, comment,                            \
                           init_m, term_m,                           \
                           ldr_f)                                    \
    CxsdAclLdrModRec CXSD_ACLLDR_MODREC_NAME(name) =                 \
    {                                                                \
        {                                                            \
            CXSD_ACLLDR_MODREC_MAGIC, CXSD_ACLLDR_MODREC_VERSION,    \
            __CX_STRINGIZE(name), comment,                           \
            init_m, term_m                                           \
        },                                                           \
        ldr_f,                                                       \
        NULL                                                         \
    }


int  CxsdAccessRegisterAclLdr  (CxsdAclLdrModRec *metric);
int  CxsdAccessDeregisterAclLdr(CxsdAclLdrModRec *metric);


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* __CXSD_ACCESSP_H */
