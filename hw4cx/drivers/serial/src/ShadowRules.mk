.PHONY:		firsttarget
firsttarget:	all

ifeq "$(strip $(SERIAL_TYPE))" ""
  DUMMY:=	$(shell echo 'ShadowRules.mk: *** SERIAL_TYPE is unset' >&2)
  error		# Makes `make' stop with a "missing separator" error
endif

######################################################################
LAYERSSOURCES=	$(SERIAL_TYPE)piv485_lyr.c
UTILSSOURCES=	$(SERIAL_TYPE)setupkshd485.c $(SERIAL_TYPE)dumpserial.c

HALSYMLINKS=	$(SERIAL_TYPE)serial_hal.h

$(SERIAL_TYPE)piv485_lyr.c:	$(SRCDIR)/piv485_lyr_common.c
$(SERIAL_TYPE)serial_hal.h:	$(SRCDIR)/stdserial_hal.h
$(SERIAL_TYPE)setupkshd485.c:	$(SRCDIR)/setupkshd485_common.c
$(SERIAL_TYPE)dumpserial.c:	$(SRCDIR)/dumpserial_common.c

$(SERIAL_TYPE)piv485_lyr.dep \
$(SERIAL_TYPE)setupkshd485.dep \
$(SERIAL_TYPE)dumpserial.dep:	$(SERIAL_TYPE)serial_hal.h

$(SERIAL_TYPE)%.o \
$(SERIAL_TYPE)%.dep:	SPECIFIC_DEFINES=-DSERIAL_HAL_FILE_H='"$(SERIAL_TYPE)serial_hal.h"'\
				-DSERIAL_HAL_PFX='"$(SERIALDEV_PFX)"' \
				-DSERIALDRV_PFX=$(SERIAL_TYPE) \
				-DSERIALLYR_NAME=$(SERIAL_TYPE)piv485

# --------------------------------------------------------------------

SHD_SYMLINKS=	$(HALSYMLINKS) \
		$(LAYERSSOURCES) $(UTILSSOURCES)
SHD_GNTDFILES=	$(SHD_SYMLINKS)

$(SHD_SYMLINKS):
		$(SCRIPTSDIR)/ln-sf_safe.sh $< $@

SHD_INCLUDES=	-I$(SRCDIR)

# Declare ourself as "important"
MAKEFILE_PARTS+=$(SRCDIR)/ShadowRules.mk
# ====================================================================
