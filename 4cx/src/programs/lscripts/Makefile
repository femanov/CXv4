######################################################################
#                                                                    #
#  programs/lscripts/Makefile                                        #
#      Rules for making launch-scripts                               #
#                                                                    #
######################################################################

SH_UTILS=	run-cx-server start-all-servers stop-all-servers
UNCFILES=	$(SH_UTILS)

SUBDIRS=
TOPDIR=		../..
include $(TOPDIR)/TopRules.mk

$(SH_UTILS):	%: %.sh
		cp -p $< $@  &&  chmod +x $@

EXPORTSFILES=	$(SH_UTILS)
EXPORTSDIR=	bin

# #### END OF Makefile ###############################################
