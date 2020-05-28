#include <ctype.h>

#include "cxsd_access_via_ppf4td.h"


static int  IsAtEOL(ppf4td_ctx_t *ctx)
{
  int  r;
  int  ch;

    r = ppf4td_is_at_eol(ctx);
    if (r != 0) return r;
    r = ppf4td_peekc    (ctx, &ch);
    if (r < 0)  return r;
    if (ch == EOF) return -1; /*!!! -2?*/
    return ch == '#';
}

static void SkipToNextLine(ppf4td_ctx_t *ctx)
{
  int  ch;

    /* 1. Skip non-CR/LF */
    while (ppf4td_peekc(ctx, &ch) > 0  &&
           (ch != '\r'  &&  ch != '\n')) ppf4td_nextc(ctx, &ch);
    /* 2. Skip all CR/LF (this skips multiple empty lines too) */
    while (ppf4td_peekc(ctx, &ch) > 0  &&
           (ch == '\r'  ||  ch == '\n')) ppf4td_nextc(ctx, &ch);
}



//////////////////////////////////////////////////////////////////////

typedef struct
{
    const char      *name;
    CxsdAccessPerms  perms;
} perms_dscr_t;

typedef struct
{
    const char *name;
    int         kind;
    uint32      ip1;
    uint32      ip2;
} hosts_dscr_t;

static perms_dscr_t perms_keywords[] =
{
    {"deny",     CXSD_ACCESS_PERM_NONE},
    {"allow",    CXSD_ACCESS_PERM_ALL},
    {"readonly", CXSD_ACCESS_PERM_CONNECT | CXSD_ACCESS_PERM_READ},
    {"nolock",   CXSD_ACCESS_PERM_ALL &~ CXSD_ACCESS_PERM_LOCK},
    {NULL, 0},
};

static hosts_dscr_t hosts_keywords[] =
{
    {"local",     CXSD_ACCESS_KIND_RANGE, 0x7F000001, 0x7F000001},
    {"all",       CXSD_ACCESS_KIND_MASK,  0,          0},
    {"none",      CXSD_ACCESS_KIND_MASK,  0xFFFFFFFF, 0},
    {NULL, 0, 0, 0},
};


static void CxsdAccessListList(FILE *fp, CxsdAccessList acl)
{
  int               n;
  CxsdAccessItem_t *item;

    for (n = 0;  n < acl->items_used;  n++)
    {
        item = acl->items + n;
        fprintf(fp, "%c%c%c%c %d.%d.%d.%d%c%d.%d.%d.%d\n",
                (item->perms & CXSD_ACCESS_PERM_CONNECT) != 0? 'c':'-',
                (item->perms & CXSD_ACCESS_PERM_READ)    != 0? 'r':'-',
                (item->perms & CXSD_ACCESS_PERM_WRITE)   != 0? 'w':'-',
                (item->perms & CXSD_ACCESS_PERM_ADMIN)   != 0? 'a':'-',
                (item->ip1 >> 24) & 0xFF,
                (item->ip1 >> 16) & 0xFF,
                (item->ip1 >>  8) & 0xFF,
                (item->ip1      ) & 0xFF,
                (item->kind == CXSD_ACCESS_KIND_RANGE)?        '-':'/',
                (item->ip2 >> 24) & 0xFF,
                (item->ip2 >> 16) & 0xFF,
                (item->ip2 >>  8) & 0xFF,
                (item->ip2      ) & 0xFF);
    }
}
CxsdAccessList CxsdAccessLoadListViaPPF4TD(const char *argv0, ppf4td_ctx_t *ctx)
{
  CxsdAccessList    acl;

  int               r;
  int               ch;
  char              token[50];

  perms_dscr_t     *pdp;
  hosts_dscr_t     *hdp;

  CxsdAccessItem_t  li; // Line Item

  uint32            ip_val;
  int               byte_val;
  int               ip_n;
  int               byte_n;

  char             *errp;

    acl = CxsdAccessCreateList();
    if (acl == NULL)
    {
        fprintf(stderr, "%s %s: unable to CxsdAccessCreateList()\n",
                strcurtime(), argv0);
        goto FATAL;
    }

    while (1)
    {
        ppf4td_skip_white(ctx);
        if (IsAtEOL(ctx)) goto SKIP_TO_NEXT_LINE;

        r = ppf4td_get_ident(ctx, PPF4TD_FLAG_NONE, token, sizeof(token));
        if (r < 0)
        {
            fprintf(stderr, "%s %s: %s:%d: access-permission-keyword expected, %s\n",
                    strcurtime(), argv0,
                    ppf4td_cur_ref(ctx), ppf4td_cur_line(ctx), ppf4td_strerror(errno));
            goto FATAL;
        }

        for (pdp = perms_keywords;  pdp->name != NULL;  pdp++)
            if (strcasecmp(token, pdp->name) == 0) break;
        if (pdp->name == NULL)
        {
            fprintf(stderr, "%s %s: %s:%d: unknown keyword \"%s\"\n",
                    strcurtime(), argv0, ppf4td_cur_ref(ctx), ppf4td_cur_line(ctx),
                    token);
            goto FATAL;
        }
        li.perms = pdp->perms;

        // Get the address (or its first part, in case of IP)
        ppf4td_skip_white(ctx);
        if (ppf4td_get_ident(ctx, PPF4TD_FLAG_NONE, token, sizeof(token)) < 0)
        {
            fprintf(stderr, "%s %s: %s:%d: address expected after \"%s\"; error: %s\n",
                    strcurtime(), argv0,
                    ppf4td_cur_ref(ctx), ppf4td_cur_line(ctx), pdp->name, ppf4td_strerror(errno));
            goto FATAL;
        }
        // Is it one of predefined names?
        for (hdp = hosts_keywords;  hdp->name != NULL;  hdp++)
            if (strcasecmp(token, hdp->name) == 0) break;
        // Is it?
        if (hdp->name != NULL)
        {
            // Yes
            li.kind = hdp->kind;
            li.ip1  = hdp->ip1;
            li.ip2  = hdp->ip2;
        }
        else
        {
            // No - let's parse the rest as IP range
            /*
                A.B.C.D
                A.B.C.D-E.F.G.H
                A.B.C.D/E.F.G.H
                A.B.C.D/N
            */
            ip_val = 0;
            ip_n   = 1;
            byte_n = 3;
            while (1)
            {
                if (ip_n == 1  &&  byte_n == 3) /* Use what in token[] */;
                else if (ppf4td_get_ident(ctx, PPF4TD_FLAG_NONE, token, sizeof(token)) < 0)
                {
                    fprintf(stderr, "%s %s: %s:%d: IP-address component expected; error: %s\n",
                            strcurtime(), argv0,
                            ppf4td_cur_ref(ctx), ppf4td_cur_line(ctx), ppf4td_strerror(errno));
                    goto FATAL;
                }

                byte_val = strtoul(token, &errp, 0);
                if (errp == token  ||  *errp != '\0')
                {
                    fprintf(stderr, "%s %s: %s:%d: invalid IP-address component \"%s\"\n",
                            strcurtime(), argv0,
                            ppf4td_cur_ref(ctx), ppf4td_cur_line(ctx), token);
                    goto FATAL;
                }
                if (byte_val < 0  ||  byte_val > 255)
                {
                    fprintf(stderr, "%s %s: %s:%d: IP-address component %d is out of range 0...255\n",
                            strcurtime(), argv0,
                            ppf4td_cur_ref(ctx), ppf4td_cur_line(ctx), byte_val);
                    goto FATAL;
                }

                // Store the byte in IP
                ip_val |= byte_val << (byte_n * 8);

                // And what's than?
                // EOL?
                if (IsAtEOL(ctx)) goto END_PARSE_IPS;
                // No - get the next character; in fact, it can't be an error, because of preceding IsAtEOL() which is false
                if (ppf4td_nextc(ctx, &ch) <= 0) goto END_PARSE_IPS;
                if (isspace(ch))                 goto END_PARSE_IPS;
                else if (ch == '.')
                {
                    if (byte_n == 0)
                    {
                        fprintf(stderr, "%s %s: %s:%d: too many IP-address components after %d.%d.%d.%d\n",
                                strcurtime(), argv0,
                                ppf4td_cur_ref(ctx), ppf4td_cur_line(ctx),
                                (ip_val >> 24) & 0xFF, (ip_val >> 16) & 0xFF, (ip_val >> 8) & 0xFF, ip_val & 0xFF);
                        goto FATAL;
                    }
                    --byte_n;
                }
                else if (ch == '/'  ||  ch == '-')
                {
                    if (ip_n > 1)
                    {
                        fprintf(stderr, "%s %s: %s:%d: too many IPs in specification\n",
                                strcurtime(), argv0,
                                ppf4td_cur_ref(ctx), ppf4td_cur_line(ctx));
                        goto FATAL;
                    }
                    // Store data
                    li.kind = (ch == '/')? CXSD_ACCESS_KIND_MASK : CXSD_ACCESS_KIND_RANGE;
                    li.ip1  = ip_val;
                    // ...and proceed to next element
                    ip_val = 0;
                    ip_n   = 2;
                    byte_n = 3;
                }
                else
                {
                    fprintf(stderr, "%s %s: %s:%d: syntax error after \"%s\"\n",
                            strcurtime(), argv0,
                            ppf4td_cur_ref(ctx), ppf4td_cur_line(ctx), token);
                    goto FATAL;
                }
            }
 END_PARSE_IPS:

            // Okay, what have we got?
            if (ip_n == 1)
            {
                // Just a single IP
                li.kind = CXSD_ACCESS_KIND_MASK;
                li.ip1  = li.ip2 = ip_val;
            }
            else
            {
                // TWO IPs - what kind?
                // "A.B.C.D/N"?
                if (li.kind == CXSD_ACCESS_KIND_MASK  &&  byte_n == 3)
                {
                    if (byte_val > 31)
                    {
                        fprintf(stderr, "%s %s: %s:%d: invalid mask spec \"/%d\"\n",
                                strcurtime(), argv0,
                                ppf4td_cur_ref(ctx), ppf4td_cur_line(ctx), byte_val);
                        goto FATAL;
                    }
                    li.ip2 = 0xFFFFFFFF << (32 - byte_val);
                }
                // ...no, jist "IP1/MASK" or "IP1-IP2"
                else
                {
                    li.ip2  = ip_val;
                }
            }
        }

        if (CxsdAccessAddListItem(acl, &li) < 0)
        {
            fprintf(stderr, "%s %s: %s:%d: unable to CxsdAccessAddListItem(): %s\n",
                    strcurtime(), argv0,
                    ppf4td_cur_ref(ctx), ppf4td_cur_line(ctx), strerror(errno));
            goto FATAL;
        }

 SKIP_TO_NEXT_LINE:
        SkipToNextLine(ctx);
        /* Check for EOF */
        if (ppf4td_peekc(ctx, &ch) <= 0) goto END_PARSE_FILE;
    }
 END_PARSE_FILE:

    ppf4td_close(ctx);

    if (1) CxsdAccessListList(stderr, acl);

    return acl;

 FATAL:
    ppf4td_close(ctx);
    CxsdAccessDestroyList(acl);
    return NULL;
}
