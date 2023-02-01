vepp4_CLIENTS=	
vepp5_CLIENTS=	
skif_CLIENTS=	skifinjmag
weld_CLIENTS=	baachi verona weldcc xweldcc lhebwcc
liu_CLIENTS=	liu
common_CLIENTS=	ist_cdac20 v1k_cdac20 v3h_a40d16 mps20_ceac124 \
		gid25x4
CLIENTS=	rfsyn_eng v5rfsyn v5ie v5kls v5kls_ionpumps v5cnv \
		linmag linvac linthermcan linslbpms linipp lin485s linrf_smc \
		ringmag ringcor23 ringcor45 ringrf ringcurmon \
		subharmonic lincamsel ringcamsel rotmag \
		v5h1adc200s \
		v5adc200_gun  v5adc200_kls1 v5adc200_kls2 v5adc200_kls3 \
		v5adc200_grp1 v5adc200_grp2 ionchamber \
		sukhpanel blm v5adc200_kkrs \
		ttv \
		ist_cdac20 v1k_cdac20 v3h_a40d16 mps20_ceac124 \
		v5p2camera \
		modbus_tcp_rb rfmeas_l_timer \
		k500gid25s \
		v5k500vepp4 \
		gid25x4 v4gid25s v4k500mag v4k500 \
		vepp34info vepp5info \
		v5ringcurmon4v4 \
		baachi verona weldcc xweldcc lhebwcc \
		skifinjmag
#CLIENTS=	$(sort $(foreach P, $(PARTS), $($(P)_CLIENTS)) )
AUX_SCREEN_FILES=	subsys_magx_macros.m4 gid25_subsys_macros.m4

EXPORTSFILES3=	weld_mes_up_mv2mka.lst
EXPORTSDIR3=	settings/common

#---
CLIENTS+=	liu
LOCAL_BCKPFILES+= panov_ubs.m4 kshd485.m4 dl200me_f.m4
AUX_SCREEN_FILES+=panov_ubs.m4 kshd485.m4 dl200me_f.m4
#---
######################################################################
PRJDIR=		..
SECTION=	programs/pults/DirRules.mk
include		$(PRJDIR)/PrjRules.mk
######################################################################
#---
panov_ubs.m4:	$(V4HDIR)/screens/panov_ubs.subsys
		cp -p $< $@
kshd485.m4:	$(V4HDIR)/screens/kshd485.subsys
		cp -p $< $@
dl200me_f.m4:	$(V4HDIR)/screens/dl200me_f.subsys
		cp -p $< $@
liu:		panov_ubs.m4 kshd485.m4 dl200me_f.m4
#---
