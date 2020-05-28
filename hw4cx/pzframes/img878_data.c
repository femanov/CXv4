#include "vcamimg_data.h"
#include "img878_data.h"
#include "drv_i/img878_drv_i.h"


enum { IMG878_DTYPE = CXDTYPE_UINT8 };

static pzframe_chan_dscr_t img878_chan_dscrs[] =
{
    [IMG878_CHAN_DATA]         = {"data",        0, PZFRAME_CHAN_IS_FRAME | PZFRAME_CHAN_MARKER_MASK,
                                  IMG878_DTYPE,
                                  IMG878_MAX_W*IMG878_MAX_H},

    [IMG878_NUMCHANS] = {"_devstate", 0, PZFRAME_CHAN__DEVSTATE_MASK},
};

pzframe_type_dscr_t *img878_get_type_dscr(void)
{
  static vcamimg_type_dscr_t  dscr;
  static int                  dscr_inited;

    if (!dscr_inited)
    {
        VcamimgDataFillStdDscr(&dscr, "img878",
                               0,
                               img878_chan_dscrs, countof(img878_chan_dscrs),
                               /* Image characteristics */
                               IMG878_CHAN_DATA,
                               IMG878_MAX_W,   IMG878_MAX_H,
                               IMG878_MAX_W-1, IMG878_MAX_H,
                               0,              0,
                               IMG878_BPP,
                               IMG878_SRCMAXVAL);
        dscr_inited = 1;
    }
    return &(dscr.ftd);
}
