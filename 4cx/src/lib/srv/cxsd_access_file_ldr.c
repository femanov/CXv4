#include "cxsd_access_via_ppf4td.h"



/////////////////////////////////////////////////////////////////////
static CxsdAccessList file_acl_ldr(const char *argv0,
                                   const char *reference)
{
  ppf4td_ctx_t  ctx;

    if (ppf4td_open(&ctx, "plaintext", reference, NULL) != 0)
    {
        fprintf(stderr, "%s %s(): failed to open \"%s\": %s\n",
               strcurtime(), __FUNCTION__, reference, ppf4td_strerror(errno));
        return NULL;
    }

    return CxsdAccessLoadListViaPPF4TD(argv0, &ctx);
}


static int  file_aclldr_init_m(void);
static void file_aclldr_term_m(void);


DEFINE_CXSD_ACLLDR(file, "ACL-loader from file",
                   file_aclldr_init_m, file_aclldr_term_m,
                   file_acl_ldr);

static int  file_aclldr_init_m(void)
{
    return CxsdAccessRegisterAclLdr(&(CXSD_ACLLDR_MODREC_NAME(file)));
}

static void file_aclldr_term_m(void)
{
    CxsdAccessDeregisterAclLdr(&(CXSD_ACLLDR_MODREC_NAME(file)));
}
