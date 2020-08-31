#include "vcamimg_data.h"
#include "vcamimg_gui.h"
#include "vcamimg_main.h"

#include "img878_gui.h"


int main(int argc, char *argv[])
{
  vcamimg_gui_t       gui;
  pzframe_gui_dscr_t *gkd = img878_get_gui_dscr();

    VcamimgGuiInit(&gui, pzframe2vcamimg_gui_dscr(gkd));
    return VcamimgMain(argc, argv,
                       "img878", "IMG878",
                       &(gui.g), gkd);
}
