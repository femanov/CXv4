#--- CanserverShadowRules.mk -----------------------------------------
LISTOFDRIVERS=	$(SRCDIR)/LISTOFCANDRIVERS.mk
MAKEFILE_PARTS+=$(LISTOFDRIVERS)
include		$(LISTOFDRIVERS)
#
LISTOFCANLAYERS=$(CAN_HAL_PFX)cankoz
CANKOZ_LYR_SRCS=$(addsuffix _lyr.c, $(LISTOFCANLAYERS))
#
CANKOZ_DRV_SRCS=	$(addsuffix _drv.c, $(LISTOFCANDRIVERS))
CANKOZ_SYMLINKS=	$(CANKOZ_DRV_SRCS)
$(CANKOZ_SYMLINKS):	%: $(SRCDIR)/%
			$(SCRIPTSDIR)/ln-sf_safe.sh $< $@
#
v4$(CAN_HAL_PFX)canserver.c:		$(SRCDIR)/canserver_common.c
			$(SCRIPTSDIR)/ln-sf_safe.sh $< $@
#
canserver_drivers.c:	$(LISTOFDRIVERS) Makefile
		(                                                      \
		  echo \#include'"remsrv.h"';                          \
		  echo;                                                \
		  $(foreach N, $(LISTOFCANDRIVERS),                    \
		    echo 'extern CxsdDriverModRec $N_driver_modrec;';) \
		  echo 'static CxsdDriverModRec * drvtable[] = {';     \
		  $(foreach N, $(LISTOFCANDRIVERS),                    \
		    echo '    &$N_driver_modrec,';)                    \
		  echo '    NULL';                                     \
		  echo '};';                                           \
		  echo 'CxsdDriverModRec **remsrv_drivers = drvtable;';\
		  echo;                                                \
		  $(foreach N, $(LISTOFCANLAYERS),                     \
		    echo 'extern CxsdLayerModRec $N_layer_modrec;';)   \
		  echo 'static CxsdLayerModRec * lyrtable[] = {';      \
		  echo   'NULL,';                                      \
		  $(foreach N, $(LISTOFCANLAYERS),                     \
		    echo '    &$N_layer_modrec,';)                     \
		  echo '};';                                           \
		  echo 'CxsdLayerModRec   **remcxsd_layers = lyrtable;';\
		  echo 'int remcxsd_numlyrs = countof(lyrtable);';     \
		) >$@
#---------------------------------------------------------------------
