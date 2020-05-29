#include "fastadc_data.h"
#include "fastadc_gui.h"
#include "fastadc_main.h"

#include "vsdc2_1_gui.h"


int main(int argc, char *argv[])
{
  fastadc_gui_t       gui;
  pzframe_gui_dscr_t *gkd = vsdc2_1_get_gui_dscr();

    FastadcGuiInit(&gui, pzframe2fastadc_gui_dscr(gkd));
    return FastadcMain(argc, argv,
                       "vsdc2_1", "VSDC2_1",
                       &(gui.g), gkd);
}
