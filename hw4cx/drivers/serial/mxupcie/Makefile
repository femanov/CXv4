SRCDIR=		../src
SERIAL_TYPE=	mxu
SERIALDEV_PFX=	MUE
include		$(SRCDIR)/ShadowRules.mk

MONO_C_FILES=   $(SERIAL_TYPE)piv485_lyr.so $(UTILSSOURCES:.c=)

LOCAL_DEFINES=	-DMXUPCIE_KSHD485
LOCAL_DEFINES+=	-DSPECIFIC_SERIAL_SETUP_FILE_H='"mxupcie_serial_setup_file.h"'

######################################################################
PRJDIR=		../../..
SECTION=	programs/drivers/DirRules.mk
include		$(PRJDIR)/PrjRules.mk
######################################################################

$(SERIAL_TYPE)setupkshd485 $(SERIAL_TYPE)dumpserial:	$(LIBMISC)
