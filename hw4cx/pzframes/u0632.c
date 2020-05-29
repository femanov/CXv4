#include "wirebpm_data.h"
#include "wirebpm_gui.h"
#include "wirebpm_main.h"

#include "u0632_gui.h"


int main(int argc, char *argv[])
{
  wirebpm_gui_t       gui;
  pzframe_gui_dscr_t *gkd = u0632_get_gui_dscr();

    WirebpmGuiInit(&gui, pzframe2wirebpm_gui_dscr(gkd));
    return WirebpmMain(argc, argv,
                       "u0632", "U0632",
                       &(gui.g), gkd);
}
