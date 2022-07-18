ARLIBS=			libcxsd_fe_epics.a
SOLIBS=			cxsd_fe_epics.so

libcxsd_fe_epics.a_COMPONENTS=	cxsd_fe_epics.o cxsd_fe_epics_meat.o
cxsd_fe_epics.so_COMPONENTS=	cxsd_fe_epics.o cxsd_fe_epics_meat.o

EXPORTSFILES=		$(SOLIBS)
EXPORTSDIR=		lib/server/frontends

EXPORTSDIR2=		lib
EXPORTSFILES2=		$(ARLIBS)

SPECIFIC_CFLAGS=	-fPIC
DIR_INCLUDES=		$(EPICS_INCLUDES)
SPECIFIC_LDFLAGS=	-L$(EPICS_OS_LIB_DIR) -lca -lCom -lcas

######################################################################
PRJDIR=		../..
include		../FrgnRules.mk
include		$(PRJDIR)/PrjRules.mk
######################################################################
