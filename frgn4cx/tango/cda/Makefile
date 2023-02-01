ARLIBS=			libcda_d_tango.a
SOLIBS=			cda_d_tango.so

libcda_d_tango.a_COMPONENTS=	cda_d_tango.o
cda_d_tango.so_COMPONENTS=	cda_d_tango.o

EXPORTSFILES=		$(SOLIBS)
EXPORTSDIR=		lib/cda/plugins

EXPORTSDIR2=		lib
EXPORTSFILES2=		$(ARLIBS)

LOCAL_CFLAGS=		-fPIC
LOCAL_CPPFLAGS=		-std=c++11
LOCAL_INCLUDES=		$(TANGO_INCLUDES)
LOCAL_LDFLAGS=		$(TANGO_LIBS)

######################################################################
PRJDIR=		../..
include		../FrgnRules.mk
include		$(PRJDIR)/PrjRules.mk
######################################################################
