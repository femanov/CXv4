.PHONY:		firsttarget
firsttarget:	all

ifeq "$(VME_HAL_DESCR)" ""
  VME_HAL_DESCR=	$(VME_HAL_PFX)
endif

SHD_SYMLINKS=	$(VME_HAL_PFX)vme_lyr.c $(VME_HAL_PFX)_test.c $(VME_HAL_PFX)_lab6_vme_firmware.c

$(VME_HAL_PFX)vme_lyr.c:		$(SRCDIR)/vme_lyr_common.c
$(VME_HAL_PFX)_test.c:			$(SRCDIR)/vme_test_common.c
$(VME_HAL_PFX)_lab6_vme_firmware.c:	$(SRCDIR)/lab6_vme_firmware_common.c

$(VME_HAL_PFX)vme_lyr.% $(VME_HAL_PFX)_test.% $(VME_HAL_PFX)_lab6_vme_firmware.%:	\
	SPECIFIC_DEFINES=-DVME_HAL_FILE_H='"$(VME_HAL_PFX)_hal.h"' -DVME_LYR_NAME=$(VME_HAL_PFX)vme -DVME_HAL_DESCR="$(VME_HAL_DESCR)"

$(SHD_SYMLINKS):
		$(SCRIPTSDIR)/ln-sf_safe.sh $< $@

SHD_GNTDFILES=	$(SHD_SYMLINKS)

SHD_INCLUDES=	-I$(SRCDIR)

# Declare ourself as "important"
MAKEFILE_PARTS+=$(SRCDIR)/ShadowRules.mk
