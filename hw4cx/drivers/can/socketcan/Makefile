SRCDIR=		../src
CAN_HAL_PFX=	skt
include		$(SRCDIR)/ShadowRules.mk
#

MONO_C_FILES=	$(CAN_HAL_PFX)cankoz_lyr.so $(CAN_HAL_PFX)canmon

######################################################################
PRJDIR=		../../..
SECTION=	programs/drivers/DirRules.mk
include		$(PRJDIR)/PrjRules.mk
######################################################################

sktcanmon:	$(LIBMISC)
