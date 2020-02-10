######################################################################
PRJDIR=		../..
SECTION=	Config.mk
include		$(PRJDIR)/PrjRules.mk
######################################################################

SUBDIRS=	local

# Value derived from ppc860-linux/x_rules.mk
PPC860_CANARY=$(PRJDIR)/../x-compile/ppc860-linux/tools/bin/ppc-linux-gcc
ifneq ":$(OS):$(CPU_X86_COMPAT):" ":LINUX:YES:"
  DUMMY:=	$(warning WARNING: skipping build of bivme2 because platform isn't LINUX-X86)
else
  ifeq "$(wildcard $(PPC860_CANARY))" ""
    DUMMY:=	$(warning WARNING: skipping build of bivme2 because of missing $(PPC860_CANARY))
  else
    MAY_BUILD_BIVME2=YES
  endif
endif

ifeq ":$(MAY_BUILD_BIVME2):" ":YES:"
  SUBDIRS+=	bivme2 mbivme2
endif
ifeq ":$(MAY_BUILD_A3818):" ":YES:"
  SUBDIRS+=	a3818
endif

######################################################################
include		$(TOPDIR)/TopRules.mk
######################################################################

LOCAL_BCKPFILES=src/*\~ nsrc/*\~ mbivme2/*\~ a3818/*\~
