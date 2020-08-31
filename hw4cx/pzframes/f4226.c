#include "fastadc_data.h"
#include "fastadc_gui.h"
#include "fastadc_main.h"

#include "f4226_gui.h"


int main(int argc, char *argv[])
{
  fastadc_gui_t       gui;
  pzframe_gui_dscr_t *gkd = f4226_get_gui_dscr();

    FastadcGuiInit(&gui, pzframe2fastadc_gui_dscr(gkd));
    return FastadcMain(argc, argv,
                       "f4226", "F4226",
                       &(gui.g), gkd);
}
