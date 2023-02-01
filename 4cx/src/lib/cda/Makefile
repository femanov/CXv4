######################################################################
#                                                                    #
#  lib/cda/Makefile                                                  #
#      Rules for making Cx Data Access library                       #
#                                                                    #
######################################################################

ifeq  "$(wildcard ../../../../cx/src/include/cx.h)" ""
  NOV2CX=YES
  DUMMY:=	$(warning WARNING: ../../../../cx/src/include/cx.h is absent, so skipping build of cda_d_v2cx.o)
else
  V2LIBS_DIR=		../v2cx
  v2all_COMPONENTS_file=$(V2LIBS_DIR)/v2all_COMPONENTS.mk
  include		$(v2all_COMPONENTS_file)
  MAKEFILE_PARTS+=	$(v2all_COMPONENTS_file)
endif

ARLIBS=		libcda.a libcda_d_insrv.a
include		libcda.a_COMPONENTS.mk
ifeq "$(NOV2CX)" ""
  libcda.a_COMPONENTS+=	cda_d_v2cx.o v2subsysaccess.o
endif
libcda.a_GIVEN_COMPONENTS=	$(foreach F, $(v2all_COMPONENTS), $(V2LIBS_DIR)/$F)
include		libcda_d_insrv.a_COMPONENTS.mk

MAKEFILE_PARTS+=	libcda.a_COMPONENTS.mk libcda_d_insrv.a_COMPONENTS.mk

EXPORTSFILES=		$(ARLIBS)
EXPORTSDIR=		lib

LOCAL_CFLAGS=		-fPIC

TOPDIR=         ../..
include $(TOPDIR)/TopRules.mk

cda_d_v2cx.o   v2subsysaccess.o \
cda_d_v2cx.d   v2subsysaccess.d \
cda_d_v2cx.dep v2subsysaccess.dep: SPECIFIC_INCLUDES+=-I$(V2LIBS_DIR)
ifneq "$(NOV2CX)" ""
  cda_plugmgr.%:	SPECIFIC_DEFINES=-DNOV2CX
endif

# #### END OF Makefile ###############################################
