######################################################################
#                                                                    #
#  lib/cxlib/Makefile                                                #
#      Rules for making CX library                                   #
#                                                                    #
######################################################################

ARLIBS=			libcx_async.a libcx_sync.a
include 		libcx_async.a_COMPONENTS.mk
include			libcx_sync.a_COMPONENTS.mk

MAKEFILE_PARTS+=	libcx_async.a_COMPONENTS.mk libcx_sync.a_COMPONENTS.mk

EXPORTSFILES=		$(ARLIBS)
EXPORTSDIR=		lib

LOCAL_CFLAGS=		-fPIC

TOPDIR=         ../..
include $(TOPDIR)/TopRules.mk

# #### END OF Makefile ###############################################
