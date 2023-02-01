######################################################################
PRJDIR=		..
######################################################################

include		$(PRJDIR)/epics/FrgnRules.mk
include		$(PRJDIR)/tango/FrgnRules.mk

ifeq      "$(wildcard $(EPICS_INCLUDE_DIR)/cadef.h)" ""
  DUMMY:=	$(warning WARNING: $(EPICS_INCLUDE_DIR)/cadef.h is absent, so skipping build of EPICS-related stuff)
else ifeq "$(wildcard $(TANGO_SERVER_INCLUDE_DIR)/tango.h)" ""
  DUMMY:=	$(warning WARNING: $(wildcard $(TANGO_SERVER_INCLUDE_DIR)/tango.h) is absent, so skipping build of TANGO-related stuff)
else
  SUBDIRS+=	epics2smth
endif

######################################################################
include		$(PRJDIR)/PrjRules.mk
######################################################################
