.PHONY:		firsttarget
firsttarget:	all

ifeq "$(CAN_HAL_DESCR)" ""
  CAN_HAL_DESCR=	$(CAN_HAL_PFX)
endif

SHD_SYMLINKS=	$(CAN_HAL_PFX)cankoz_lyr.c $(CAN_HAL_PFX)canmon.c

$(CAN_HAL_PFX)cankoz_lyr.c:	$(SRCDIR)/cankoz_lyr_common.c
$(CAN_HAL_PFX)canmon.c:		$(SRCDIR)/canmon_common.c

$(CAN_HAL_PFX)cankoz_lyr.% $(CAN_HAL_PFX)canmon.%:	\
	SPECIFIC_DEFINES=-DCAN_HAL_FILE_H='"$(CAN_HAL_PFX)can_hal.h"' -DCANKOZ_LYR_NAME=$(CAN_HAL_PFX)cankoz -DCAN_HAL_DESCR="$(CAN_HAL_DESCR)"

$(SHD_SYMLINKS):
		$(SCRIPTSDIR)/ln-sf_safe.sh $< $@

SHD_GNTDFILES=	$(SHD_SYMLINKS)

SHD_INCLUDES=	-I$(SRCDIR)

# Declare ourself as "important"
MAKEFILE_PARTS+=$(SRCDIR)/ShadowRules.mk
