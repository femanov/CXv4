#include <stdio.h>
// Next 2 are for PATH_MAX -- different Linux versions place it in different places
#include <limits.h>
#include <sys/param.h>

#include "ppf4td.h"
#include "ppf4td_pipe.h"
#include "ppf4td_m4.h"


static int ppf4td_m4_open (ppf4td_ctx_t *ctx, const char *reference, const char *path_info)
{
  static const char *model[] = {"/usr/bin/m4", "-s"};

  const char        *cmdline[countof(model) + 2 + 2 + 2]; // -I+basedir, -I+basedir+basedir/path_info, filename+NULL
  int                i;

  const char        *sl_p;
  size_t             path1_len;
  size_t             path2_len;
  char               Ipath1[PATH_MAX];
  char               Ipath2[PATH_MAX];

    /* Check file existence/readability */
    if (access(reference, R_OK) != 0) return -1;

    /*  */
    for (i = 0;  i < countof(model);  i++) cmdline[i] = model[i];
    /* Optional -I DIR */
    sl_p = strrchr(reference, '/');
    if (sl_p != NULL)
    {
        path1_len = (sl_p - reference) + 1 /* "+1" for slash itself */;
        if (path1_len < sizeof(Ipath1) - 1)
        {
            memcpy(Ipath1, reference, path1_len); Ipath1[path1_len] = '\0';
            cmdline[i++] = "-I";
            cmdline[i++] = Ipath1;

            if (path_info != NULL  &&  path_info[0] != '0')
            {
                path2_len = path1_len + 1/* '/' */ + strlen(path_info);
                if (path2_len < sizeof(Ipath2) - 1)
                {
                    memcpy(Ipath2, Ipath1, path1_len);
                    Ipath2[path1_len] = '/';
                    memcpy(Ipath2 + path1_len + 1, path_info, strlen(path_info));
                    Ipath2[path2_len] = '\0';
                    cmdline[i++] = "-I";
                    cmdline[i++] = Ipath2;
                }
            }
        }
    }
    /* File name and terminating NULL */
    cmdline[i++] = reference;
    cmdline[i++] = NULL;

    return ppf4td_pipe_open(ctx, cmdline);
}

static int ppf4td_m4_close(ppf4td_ctx_t *ctx)
{
    return ppf4td_pipe_close(ctx);
}

static int ppf4td_m4_peekc(ppf4td_ctx_t *ctx, int *ch_p)
{
    return ppf4td_pipe_peekc(ctx, ch_p);
}

static int ppf4td_m4_nextc(ppf4td_ctx_t *ctx, int *ch_p)
{
    return ppf4td_pipe_nextc(ctx, ch_p);
}

static int  ppf4td_m4_init_m(void);
static void ppf4td_m4_term_m(void);

DEFINE_PPF4TD_PLUGIN(m4, "M4 PPF4TD plugin",
                     ppf4td_m4_init_m, ppf4td_m4_term_m,
                     ppf4td_m4_open,   ppf4td_m4_close,
                     ppf4td_m4_peekc,  ppf4td_m4_nextc,
                     PPF4TD_LINESYNC_HLIN);

static int  ppf4td_m4_init_m(void)
{
    return ppf4td_register_plugin(&(PPF4TD_PLUGIN_MODREC_NAME(m4)));
}

static void ppf4td_m4_term_m(void)
{
    ppf4td_deregister_plugin(&(PPF4TD_PLUGIN_MODREC_NAME(m4)));
}
