SRCDIR=		../src
CAN_HAL_PFX=	mar
include		$(SRCDIR)/ShadowRules.mk
#

MONO_C_FILES=	$(CAN_HAL_PFX)cankoz_lyr.so $(CAN_HAL_PFX)canmon

######################################################################
PRJDIR=		../../..
SECTION=	programs/drivers/DirRules.mk
include		$(PRJDIR)/PrjRules.mk
######################################################################
GIVEN_DIR=	$(PRJDIR)/given/marathon-2.4
LOCAL_INCLUDES+=-I$(GIVEN_DIR)

marcanmon:	$(LIBMISC)
