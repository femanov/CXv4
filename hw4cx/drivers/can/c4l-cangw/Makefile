# CAN
SRCDIR=		../src
CAN_HAL_PFX=	c4l
include		$(SRCDIR)/ShadowRules.mk

include		$(SRCDIR)/CanserverShadowRules.mk

#
MONO_C_FILES=			$(CAN_HAL_PFX)canmon
EXES=				v4$(CAN_HAL_PFX)canserver
v4$(CAN_HAL_PFX)canserver_COMPONENTS=	v4$(CAN_HAL_PFX)canserver.o canserver_drivers.o \
				$(CANKOZ_LYR_SRCS:.c=.o) \
				$(CANKOZ_DRV_SRCS:.c=.o)

EXPORTSFILES=	$(EXES) $(MONO_C_FILES)
EXPORTSDIR=	lib/server/drivers/cangw_ppc_drvlets

######################################################################
PRJDIR=         ../../..
X_RULES_DIR=	$(PRJDIR)/x-build/ppc860-linux

SECTION=	$(X_RULES_DIR)/x_rules.mk
include		$(PRJDIR)/PrjRules.mk
######################################################################

GIVEN_DIR=	$(PRJDIR)/given/cangw
LOCAL_INCLUDES+=-I$(GIVEN_DIR)

v4$(CAN_HAL_PFX)canserver.%:	SPECIFIC_DEFINES=-DCANSERVER_ISSUE='"CXv4 CANGW canserver"'
LOCAL_GNTDFILES+=v4$(CAN_HAL_PFX)canserver.c canserver_drivers.c $(CANKOZ_DRV_SRCS)

#
v4$(CAN_HAL_PFX)canserver:	$(LIBREMSRV) \
				$(LIBUSEFUL) $(LIBCXSCHEDULER) $(LIBMISC)
v4$(CAN_HAL_PFX)canserver:	SPECIFIC_LIBS+=$(LIBDL)
$(CAN_HAL_PFX)canmon:		$(LIBMISC)
