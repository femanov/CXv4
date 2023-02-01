ARLIBS=			libcxsd_fe_epics.a
SOLIBS=			cxsd_fe_epics.so
MONO_C_FILES=		test_libcas

libcxsd_fe_epics.a_COMPONENTS=	cxsd_fe_epics.o cxsd_fe_epics_meat.o
cxsd_fe_epics.so_COMPONENTS=	cxsd_fe_epics.o cxsd_fe_epics_meat.o

EXPORTSFILES=		$(SOLIBS)
EXPORTSDIR=		lib/server/frontends

EXPORTSDIR2=		lib
EXPORTSFILES2=		$(ARLIBS)

LOCAL_CFLAGS=		-fPIC
LOCAL_INCLUDES=		$(EPICS_INCLUDES)
LOCAL_LDFLAGS=		-L$(EPICS_OS_LIB_DIR) -lca -lCom -lcas -lgdd

test_libcas:		SPECIFIC_LDFLAGS+=-lstdc++

######################################################################
PRJDIR=		../..
include		../FrgnRules.mk
include		$(PRJDIR)/PrjRules.mk
######################################################################
