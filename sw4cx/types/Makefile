TYPES_LIST=	ist_cdac20 vnn_ceac51 v1k_cdac20 v3h_a40d16 mps20_ceac124 \
		sim_ist_cdac20 \
		kurrez_cac208 \
		iset_walker \
		lebedev_subharmonic \
		lebedev_lincamsel lebedev_linsenctl \
		lebedev_ringcamsel lebedev_rotmag \
		modbus_tcp_rb \
		vepp4_gimn \
		v5k5045 \
		v5phaseconv \
		weldproc lhebw_catctl \
		liu_key liu_doorilks
TYPES_FILES=	$(foreach T, $(TYPES_LIST), $T.devtype)

EXPORTSFILES=	$(TYPES_FILES)
EXPORTSDIR=	configs/types

######################################################################
PRJDIR=		..
include		$(PRJDIR)/PrjRules.mk
######################################################################
