######################################################################
#                                                                    #
#  lib/useful/Makefile                                               #
#      Rules for making "useful" library                             #
#                                                                    #
######################################################################

ARLIBS=		libuseful.a libcxscheduler.a libmt_cxscheduler.a
include		libuseful.a_COMPONENTS.mk
include		libcxscheduler.a_COMPONENTS.mk
include		libmt_cxscheduler.a_COMPONENTS.mk

MAKEFILE_PARTS+=libuseful.a_COMPONENTS.mk \
		libcxscheduler.a_COMPONENTS.mk \
		libmt_cxscheduler.a_COMPONENTS.mk

EXPORTSFILES=	$(ARLIBS)
EXPORTSDIR=	lib

LOCAL_CFLAGS=	-fPIC

TOPDIR=		../..
include $(TOPDIR)/TopRules.mk

# #### END OF Makefile ###############################################
