TOPDIR=		$(PRJDIR)/../4cx/src
V4HDIR=		$(PRJDIR)/../hw4cx

PRJ_INCLUDES=	-I$(PRJDIR)/include

SW4CX_PARTS=	vepp4 vepp5 weld liu

ifeq "$(strip $(SECTION))" ""
  SECTION=	TopRules.mk
endif

ifeq "$(filter-out .% /%,$(SECTION))" ""
  include	$(SECTION)
else
  include	$(TOPDIR)/$(SECTION)
endif
