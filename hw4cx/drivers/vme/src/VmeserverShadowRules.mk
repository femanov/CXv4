#--- VmeserverShadowRules.mk -----------------------------------------
# A copy of CanserverShadowRules.mk, with "CAN" replaced by "VME"

LISTOFDRIVERS=	$(SRCDIR)/LISTOFVMEDRIVERS.mk
MAKEFILE_PARTS+=$(LISTOFDRIVERS)
include		$(LISTOFDRIVERS)

LISTOFVMELAYERS=$(VME_HAL_PFX)vme
VME_LYR_SRCS=	$(addsuffix _lyr.c, $(LISTOFVMELAYERS))

VME_DRV_SRCS=		$(addsuffix _drv.c, $(LISTOFVMEDRIVERS))
VME_SYMLINKS=		$(VME_DRV_SRCS)
$(VME_SYMLINKS):	%: $(SRCDIR)/%
			$(SCRIPTSDIR)/ln-sf_safe.sh $< $@

v4$(VME_HAL_PFX)vmeserver.c:	$(SRCDIR)/vmeserver_common.c
			$(SCRIPTSDIR)/ln-sf_safe.sh $< $@

vmeserver_drivers.c:	$(LISTOFDRIVERS) Makefile
		(                                                      \
		  echo \#include'"remsrv.h"';                          \
		  echo;                                                \
		  $(foreach N, $(LISTOFVMEDRIVERS),                    \
		    echo 'extern CxsdDriverModRec $N_driver_modrec;';) \
		  echo 'static CxsdDriverModRec * drvtable[] = {';     \
		  $(foreach N, $(LISTOFVMEDRIVERS),                    \
		    echo '    &$N_driver_modrec,';)                    \
		  echo '    NULL';                                     \
		  echo '};';                                           \
		  echo 'CxsdDriverModRec **remsrv_drivers = drvtable;';\
		  echo;                                                \
		  $(foreach N, $(LISTOFVMELAYERS),                     \
		    echo 'extern CxsdLayerModRec $N_layer_modrec;';)   \
		  echo 'static CxsdLayerModRec * lyrtable[] = {';      \
		  echo   '    NULL,';                                  \
		  $(foreach N, $(LISTOFVMELAYERS),                     \
		    echo '    &$N_layer_modrec,';)                     \
		  echo '};';                                           \
		  echo 'CxsdLayerModRec   **remcxsd_layers = lyrtable;';\
		  echo 'int remcxsd_numlyrs = countof(lyrtable);';     \
		) >$@

v4$(VME_HAL_PFX)vmeserver_COMPONENTS=	v4$(VME_HAL_PFX)vmeserver.o vmeserver_drivers.o \
				$(VME_LYR_SRCS:.c=.o) \
				$(VME_DRV_SRCS:.c=.o)

LOCAL_GNTDFILES+=v4$(VME_HAL_PFX)vmeserver.c vmeserver_drivers.c $(VME_DRV_SRCS)
#---------------------------------------------------------------------
