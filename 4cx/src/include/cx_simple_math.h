#ifndef __CX_SIMPLE_MATH_H
#define __CX_SIMPLE_MATH_H


#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


typedef struct
{
    int     numpts;
    double  data[0];
} csm_lapprox_rec_t;

struct
{
    int     notnew;
    double  avg;
    double  avg2;
} csm_devn_rec_t;

struct
{
    double *buf;
    int     bufcap;
    int     bufused;
} csm_minmax_rec_t;


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* __CX_SIMPLE_MATH_H */
//////////////////////////////////////////////////////////////////////
#if 0

#include "cx_simple_math.h"


int csm_do_linear_approximation(double *rv_p, double x,
                                int npts, double *matrix)
{
  int     ret = 0;
  int     lb;
  double  x0, y0, x1, y1;

    if (npts == 1)
    {
        *rv_p = matrix[1];
        return 1;
    }
    if (npts <  1)
    {
        *rv_p = x;
        return 1;
    }
  
    if      (x < matrix[0])
    {
        ret = 1;
        lb = 0;
    }
    else if (x > matrix[(npts - 1) * 2])
    {
        ret = 1;
        lb = npts - 2;
    }
    else
    {
        /*!!!Should replace with binary search!*/
        for (lb = 0;  lb < npts - 1/*!!!?*/;  lb++)
            if (matrix[lb*2] <= x  &&  x <= matrix[(lb + 1) * 2])
                break;
    }

    x0 = matrix[lb * 2];
    y0 = matrix[lb * 2 + 1];
    x1 = matrix[lb * 2 + 2];
    y1 = matrix[lb * 2 + 3];
    
    *rv_p = y0 + (x - x0) * (y1 - y0) / (x1 - x0);

    if (_cda_debug_lapprox)
    {
        fprintf(stderr, "LAPPROX: x=%f, npts=%d; lb=%d:[%f,%f] =>%f  r=%d\n",
                                    x,       npts,  lb, x0,x1,  *rv_p, ret);
        //    if (npts == 121) fprintf(stderr, "x=%f: lb=%d v=%f\n", x, lb, *rv_p);
    }

    return ret;
}

typedef void (*csm_err_reporter_t)(void *privptr,
                                   const char *format, va_list ap);
static void csm_report_error(csm_err_reporter_t err_reporter, void *err_privptr,
                             const char *format, ...)
{
  va_list     ap;

    va_start(ap, format);
    if (err_reporter != NULL)
        err_reporter(err_privptr, format, ap);
    else
    {
        fprintf (stderr, "%s ", strcurtime());
        vfprintf(stderr, format, ap);
        fprintf (stderr, "\n");
    }
    va_end(ap);
}
static int compare_doubles (const void *a, const void *b)
{
  const double *da = (const double *) a;
  const double *db = (const double *) b;
    
    return (*da > *db) - (*da < *db);
}
lapprox_rec_t *csm_load_lapprox_table(const char *table_name,
                                      int xcol, int ycol,
                                      csm_err_reporter_t err_reporter, void *err_privptr)
{
  FILE              *fp;
  char               line[1000];

  int                maxcol;
  char              *p;
  double             v;
  double             a;
  double             b;
  char              *err;
  int                col;
  
  int                tab_size;
  int                tab_used;
  lapprox_rec_t *rec;
  lapprox_rec_t *nrp;

    if (table_name == NULL) return NULL;

    if (xcol < 1) xcol = 1;
    if (ycol < 1) ycol = 2;

    fp = findfilein(table_name,
#if OPTION_HAS_PROGRAM_INVOCATION_NAME
                    program_invocation_name,
#else
                    NULL,
#endif /* OPTION_HAS_PROGRAM_INVOCATION_NAME */
                    NULL, ".lst",
                    "./"
                        FINDFILEIN_SEP "../common"
                        FINDFILEIN_SEP "!/"
                        FINDFILEIN_SEP "!/../settings/common"
                        FINDFILEIN_SEP "$PULT_ROOT/settings/common"
                        FINDFILEIN_SEP "~/4pult/settings/common");
    if (fp == NULL)
    {
        csm_report_error(err_reporter, err_privptr,
                         "%s(): couldn't find a table \"%s.lst\"", __FUNCTION__, table_name);
        return NULL;
    }

    tab_size = 0;
    tab_used = 0;
    rec      = malloc(sizeof(*rec));
    bzero(rec, sizeof(*rec));

    maxcol = xcol;
    if (ycol > maxcol) maxcol = ycol;
    
    while (fgets(line, sizeof(line) - 1, fp) != NULL)
    {
        line[sizeof(line) - 1] = '\0';

        /* Skip leading whitespace and ignore empty/comment lines */
        p = line;
        while(*p != '\0'  &&  isspace(*p)) p++;
        if (*p == '\0'  ||  *p == '#')
            goto NEXT_LINE;

        /* Obtain "x" and "y" */
        /* Note: gcc barks
               "warning: `a' might be used uninitialized in this function"
               "warning: `b' might be used uninitialized in this function"
           here, but in fact either a/b are initialized, or we skip their usage.
         */
        for (col = 1;  col <= maxcol;  col++)
        {
            v = strtod(p, &err);
            if (err == p  ||  (*err != '\0'  &&  !isspace(*err)))
                goto NEXT_LINE;
            p = err;

            if (col == xcol) a = v;
            if (col == ycol) b = v;
        }

        /* Grow table if required */
        if (tab_used >= tab_size)
        {
            tab_size += 20;
            nrp = safe_realloc(rec, sizeof(*rec) + sizeof(double)*2*tab_size);
            if (nrp == NULL) goto END_OF_FILE;
            rec = nrp;
        }

        rec->data[tab_used * 2]     = a;
        rec->data[tab_used * 2 + 1] = b;
        tab_used++;
        
 NEXT_LINE:;
    }
 END_OF_FILE:
    
    fclose(fp);

    ////fprintf(stderr, "%d points read from %s\n", tab_used, fname);
    
    rec->numpts = tab_used;

    /* Sort the table in ascending order */
    qsort(rec->data, rec->numpts, sizeof(double)*2, compare_doubles);
    
    return rec;
}


#endif
