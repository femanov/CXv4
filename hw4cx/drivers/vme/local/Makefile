SRCDIR=		../nsrc

LISTOFDRIVERS=	$(SRCDIR)/LISTOFVMEDRIVERS.mk
MAKEFILE_PARTS+=$(LISTOFDRIVERS)
include		$(LISTOFDRIVERS)

DRIVERS=	$(addsuffix _drv.so, $(LISTOFVMEDRIVERS))
MONO_C_FILES=	$(DRIVERS)

######################################################################
PRJDIR=		../../..
SECTION=	programs/drivers/DirRules.mk
include		$(PRJDIR)/PrjRules.mk
######################################################################

SHD_INCLUDES=	-I$(SRCDIR)

VME_DRV_SRCS=		$(addsuffix _drv.c, $(LISTOFVMEDRIVERS))
VME_SYMLINKS=		$(VME_DRV_SRCS)
$(VME_SYMLINKS):	%: $(SRCDIR)/%

SYMLINKS=		$(VME_SYMLINKS)
LOCAL_GNTDFILES+=	$(SYMLINKS)

$(SYMLINKS):
			$(SCRIPTSDIR)/ln-sf_safe.sh $< $@
