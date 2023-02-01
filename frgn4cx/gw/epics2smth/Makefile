EXES=			epics2tango_gw epics2tango epics2cda

epics2tango_gw_COMPONENTS=epics2tango_gw.o
epics2tango_COMPONENTS=	epics2tango.o epics2smth.o
epics2cda_COMPONENTS=	epics2cda.o epics2smth.o

EXPORTSFILES=		$(EXES)
EXPORTSDIR=		bin

LOCAL_CPPFLAGS=		-std=c++11
LOCAL_INCLUDES=		$(EPICS_INCLUDES) \
			$(TANGO_INCLUDES)
LOCAL_LDFLAGS=		-L$(EPICS_OS_LIB_DIR) -lca -lCom -lcas -lgdd \
			-lpthread \
			$(TANGO_LIBS)

######################################################################
PRJDIR=		../..
include		$(PRJDIR)/epics/FrgnRules.mk
include		$(PRJDIR)/tango/FrgnRules.mk
include		$(PRJDIR)/PrjRules.mk
######################################################################

epics2tango:		$(LIBMISC)

epics2cda:		$(LIBCDA) $(LIBCX_ASYNC) \
			$(LIBUSEFUL) $(LIBMT_CXSCHEDULER) $(LIBMISC)
epics2cda:		SPECIFIC_LIBS=$(LIBDL) -lm
epics2cda:		SPECIFIC_LDFLAGS=-Wl,-export-dynamic # for dynamic loading of dat-plugins
