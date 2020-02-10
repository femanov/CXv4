SRCDIR=		../nsrc
VME_HAL_PFX=	a3818
include		$(SRCDIR)/ShadowRules.mk

MONO_C_FILES=	$(VME_HAL_PFX)vme_lyr.so $(VME_HAL_PFX)_test

#---------------------------------------------------------------------
#  CAENVMElibDefs.mk
#      Defines parameters for building with CAENVMElib
#---------------------------------------------------------------------
CAENVMElib_BASE_DIR=	/tmp/CAENVMELib-2.50
CAENVMElib_INCLUDE_DIR=	$(CAENVMElib_BASE_DIR)/include
CAENVMElib_LIB_DIR=	$(CAENVMElib_BASE_DIR)/lib/x64

ifeq ":$(CAENVMElib_INSTALLED):" ":YES:"
  CAENVMElib_INCLUDES=
  CAENVMElib_LIB_PARAMS=	-lCAENVME
else
  CAENVMElib_INCLUDES=		-I$(CAENVMElib_INCLUDE_DIR)
#  CAENVMElib_LIB_PARAMS=	$(CAENVMElib_LIB_DIR)/libCAENVME.so.2.50
  CAENVMElib_LIB_PARAMS=	-L$(CAENVMElib_LIB_DIR) -l:libCAENVME.so.2.50
endif
######################################################################
PRJDIR=		../../..
#include		CAENVMElibDefs.mk
SECTION=	programs/drivers/DirRules.mk
include		$(PRJDIR)/PrjRules.mk
######################################################################

$(VME_HAL_PFX)_test:	$(LIBMISC)

LOCAL_INCLUDES=		$(CAENVMElib_INCLUDES)
LOCAL_DEFINES=		-DVME_BUS_COMPONENTS_SENSIBLE=2
SPECIFIC_LDFLAGS=	$(CAENVMElib_LIB_PARAMS)
