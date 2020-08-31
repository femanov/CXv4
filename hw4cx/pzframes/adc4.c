#include "fastadc_data.h"
#include "fastadc_gui.h"
#include "fastadc_main.h"

#include "adc4_gui.h"


int main(int argc, char *argv[])
{
  fastadc_gui_t       gui;
  pzframe_gui_dscr_t *gkd = adc4_get_gui_dscr();

    FastadcGuiInit(&gui, pzframe2fastadc_gui_dscr(gkd));
    return FastadcMain(argc, argv,
                       "adc4", "ADC4",
                       &(gui.g), gkd);
}