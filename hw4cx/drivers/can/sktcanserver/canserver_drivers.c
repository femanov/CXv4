#include"remsrv.h"

extern CxsdDriverModRec cac208_driver_modrec;
extern CxsdDriverModRec ceac124_driver_modrec;
extern CxsdDriverModRec cdac20_driver_modrec;
extern CxsdDriverModRec candac16_driver_modrec;
extern CxsdDriverModRec canadc40_driver_modrec;
extern CxsdDriverModRec slio24_reg_driver_modrec;
extern CxsdDriverModRec cgvi8_driver_modrec;
extern CxsdDriverModRec cpks8_driver_modrec;
extern CxsdDriverModRec ckvch_driver_modrec;
extern CxsdDriverModRec curvv_driver_modrec;
extern CxsdDriverModRec vsdc2_driver_modrec;
extern CxsdDriverModRec caniva_driver_modrec;
extern CxsdDriverModRec canipp_driver_modrec;
extern CxsdDriverModRec senkov_vip_driver_modrec;
extern CxsdDriverModRec senkov_ebc_driver_modrec;
extern CxsdDriverModRec weld02_driver_modrec;
extern CxsdDriverModRec tvac320_driver_modrec;
extern CxsdDriverModRec panov_ubs_driver_modrec;
extern CxsdDriverModRec smc8_driver_modrec;
static CxsdDriverModRec * drvtable[] = {
    &cac208_driver_modrec,
    &ceac124_driver_modrec,
    &cdac20_driver_modrec,
    &candac16_driver_modrec,
    &canadc40_driver_modrec,
    &slio24_reg_driver_modrec,
    &cgvi8_driver_modrec,
    &cpks8_driver_modrec,
    &ckvch_driver_modrec,
    &curvv_driver_modrec,
    &vsdc2_driver_modrec,
    &caniva_driver_modrec,
    &canipp_driver_modrec,
    &senkov_vip_driver_modrec,
    &senkov_ebc_driver_modrec,
    &weld02_driver_modrec,
    &tvac320_driver_modrec,
    &panov_ubs_driver_modrec,
    &smc8_driver_modrec,
    NULL
};
CxsdDriverModRec **remsrv_drivers = drvtable;

extern CxsdLayerModRec sktcankoz_layer_modrec;
static CxsdLayerModRec * lyrtable[] = {
NULL,
    &sktcankoz_layer_modrec,
};
CxsdLayerModRec   **remcxsd_layers = lyrtable;
int remcxsd_numlyrs = countof(lyrtable);
