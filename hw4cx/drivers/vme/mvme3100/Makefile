SRCDIR=		../src
VME_HAL_PFX=	mvme3100
include		$(SRCDIR)/ShadowRules.mk

include		$(SRCDIR)/VmeserverShadowRules.mk

MONO_C_FILES=	$(VME_HAL_PFX)_test $(VME_HAL_PFX)_lab6_vme_firmware
EXES=		v4$(VME_HAL_PFX)vmeserver

EXPORTSFILES=	$(EXES) $(MONO_C_FILES)
EXPORTSDIR=	lib/server/drivers/mvme3100_drvlets

###---------------------------------------------------------------------
##MONO_C_FILES+=	zzz_drv.so

###---------------------------------------------------------------------
##MONO_C_FILES+=	test_rfmeas_a32r16

######################################################################
PRJDIR=         ../../..
##X_RULES_DIR=	$(PRJDIR)/x-build/ppc860-linux

##SECTION=	$(X_RULES_DIR)/x_rules.mk
SECTION=	programs/drivers/DirRules.mk
include		$(PRJDIR)/PrjRules.mk
######################################################################

### Switch off optimization for libvmedirect.h users, because otherwise memory-mapped VME accesses work incorrectly
##$(VME_HAL_PFX)vme_lyr.o $(VME_HAL_PFX)_test.o $(VME_HAL_PFX)_lab6_vme_firmware.o test_rfmeas_a32r16.o: OPTIMIZATION=-O0

##GIVEN_DIR=	$(PRJDIR)/given/bivme2
##LOCAL_INCLUDES+=-I$(GIVEN_DIR)

##LIBVMEDIRECT=	$(GIVEN_DIR)/libvmedirect.a

v4$(VME_HAL_PFX)vmeserver.%:	SPECIFIC_DEFINES=-DVMESERVER_ISSUE='"CXv4 MVME3100 vmeserver"'
v4$(VME_HAL_PFX)vmeserver:	$(LIBREMSRV) \
				$(LIBPZFRAME_DRV) \
				$(LIBUSEFUL) $(LIBCXSCHEDULER) $(LIBMISC)
v4$(VME_HAL_PFX)vmeserver:	SPECIFIC_LIBS+=$(LIBDL)

###???
##$(VME_HAL_PFX)_test $(VME_HAL_PFX)_lab6_vme_firmware:	$(LIBVMEDIRECT) $(LIBMISC)
$(VME_HAL_PFX)_test $(VME_HAL_PFX)_lab6_vme_firmware:	$(LIBMISC)

##test_rfmeas_a32r16: $(LIBVMEDIRECT)

