SRVHOSTS=	cxhw canhw foxy v2k-k500-1 vepp4-pult6 can2 \
		bike tower romeo ep1-berezin2 xweld \
		rack0 rack1
CLNHOSTS=	linac3 worker1 camilla2 io io2 vepp4-pult6 PULT \
		tower ep1-berezin2 xweld
ACCESSGROUPS=	vepp5

cxhw_SERVERS=		15 16 17 18 24 25 26 27
canhw_SERVERS=		11 12 13 14 19 21 22 23
foxy_SERVERS=		20
v2k-k500-1_SERVERS=	23
vepp4-pult6_SERVERS=	0 2
can2_SERVERS=		0 2
bike_SERVERS=		47
tower_SERVERS=		47
romeo_SERVERS=		48
ep1-berezin2_SERVERS=	49
xweld_SERVERS=		50
rack0_SERVERS=		24 26
rack1_SERVERS=		23

DEVLISTS=	$(foreach H, $(SRVHOSTS), \
		  $(foreach N, $($(H)_SERVERS), devlist-$H-$N.lst))
SRVLISTS=	$(foreach H, $(SRVHOSTS), cx-servers-$H.conf)
STARTERCONFS=	$(foreach H, $(CLNHOSTS), cx-starter-$H.conf) srvparams.conf
ACCESSCONFS=	$(foreach G, $(ACCESSGROUPS), cxhosts-$G.conf)

GIVEN_FILES=	devlist_magx_macros.m4 \
		canhw_CAN_LINES_LIST.m4 \
		$(DEVLISTS) $(STARTERCONFS) $(ACCESSCONFS)
UNCFILES=	$(SRVLISTS)

EXPORTSFILES=	$(GIVEN_FILES) $(UNCFILES)
EXPORTSDIR=	configs

######################################################################
PRJDIR=		..
include		$(PRJDIR)/PrjRules.mk
######################################################################

$(SRVLISTS):	cx-servers-%.conf: Makefile
		echo $($*_SERVERS) >$@
