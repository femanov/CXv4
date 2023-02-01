SUBDIRS=	include types #screens
ifeq "$(NOX)" ""
#  SUBDIRS+=	xmclients
#  SUBDIRS+=	pzframes
endif

# Conditionally add epics,...?
SUBDIRS+=	epics
SUBDIRS+=	tango
SUBDIRS+=	gw

######################################################################
PRJDIR=		.
include		$(PRJDIR)/PrjRules.mk
######################################################################
