# CAN
SRCDIR=		../src
CAN_HAL_PFX=	skt
include		$(SRCDIR)/ShadowRules.mk

include		$(SRCDIR)/CanserverShadowRules.mk

#
EXES=				v4$(CAN_HAL_PFX)canserver
v4$(CAN_HAL_PFX)canserver_COMPONENTS=	v4$(CAN_HAL_PFX)canserver.o canserver_drivers.o \
				$(CANKOZ_LYR_SRCS:.c=.o) \
				$(CANKOZ_DRV_SRCS:.c=.o)

EXPORTSFILES2=	$(EXES)
EXPORTSDIR2=	lib/server/drivers

######################################################################
PRJDIR=         ../../..
SECTION=	programs/drivers/DirRules.mk
include		$(PRJDIR)/PrjRules.mk
######################################################################

GIVEN_DIR=	../socketcan
LOCAL_INCLUDES+=-I$(GIVEN_DIR)

v4$(CAN_HAL_PFX)canserver.%:	SPECIFIC_DEFINES=-DCANSERVER_ISSUE='"CXv4 SocketCAN canserver"'
LOCAL_GNTDFILES+=v4$(CAN_HAL_PFX)canserver.c canserver_drivers.c $(CANKOZ_DRV_SRCS)

#
v4$(CAN_HAL_PFX)canserver:	$(LIBREMSRV) \
				$(LIBUSEFUL) $(LIBCXSCHEDULER) $(LIBMISC)
v4$(CAN_HAL_PFX)canserver:	SPECIFIC_LIBS+=$(LIBDL)
