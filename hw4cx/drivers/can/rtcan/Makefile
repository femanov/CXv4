SRCDIR=		../src
CAN_HAL_PFX=	xrt
include		$(SRCDIR)/ShadowRules.mk
#

MONO_C_FILES=	$(CAN_HAL_PFX)cankoz_lyr.so $(CAN_HAL_PFX)canmon

######################################################################
PRJDIR=		../../..
SECTION=	programs/drivers/DirRules.mk
include		$(PRJDIR)/PrjRules.mk
######################################################################

xrtcanmon:	$(LIBMISC)
