SRCDIR=		../src
VME_HAL_PFX=	a3818
include		$(SRCDIR)/ShadowRules.mk

MONO_C_FILES=	$(VME_HAL_PFX)vme_lyr.so $(VME_HAL_PFX)_test $(VME_HAL_PFX)_lab6_vme_firmware
MONO_C_FILES+=	a3818_u

#---------------------------------------------------------------------
#  CAENVMElibDefs.mk
#      Defines parameters for building with CAENVMElib
#---------------------------------------------------------------------
CAENVMElib_BASE_DIR=	$(HOME)/compile/CAENVMELib-2.50
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
# 3.2,0 with poll() support
ifeq ":$(USE320):" ":YES:"
  CAENVMElib_BASE_DIR=	$(HOME)/compile/CAENVMELib-3.2.0_sources.new
  CAENVMElib_LIB_DIR=	$(CAENVMElib_BASE_DIR)
  CAENVMElib_LIB_PARAMS=-L$(CAENVMElib_LIB_DIR) -l:libCAENVME.so.3.1.0
endif
######################################################################
PRJDIR=		../../..
#include		CAENVMElibDefs.mk
SECTION=	programs/drivers/DirRules.mk
include		$(PRJDIR)/PrjRules.mk
######################################################################

# Note: IRQ polling in a separate thread does NOT work, because 2nd CAENVME_Init() fails with cvGenericError=-3 "Unspecified error"
ifeq ":$(USE_THREAD_FOR_IRQ):" ":1:"
  A3818_HAL_DEFINES=	-DUSE_THREAD_FOR_IRQ=1
  A3818_HAL_LDFLAGS=	-lpthread
else
  A3818_HAL_DEFINES=	-DUSE_THREAD_FOR_IRQ=0
  A3818_HAL_LDFLAGS=	
endif

#---------------------------------------------------------------------

$(VME_HAL_PFX)_test $(VME_HAL_PFX)_lab6_vme_firmware:	$(LIBMISC)

LOCAL_INCLUDES=		$(CAENVMElib_INCLUDES)
LOCAL_DEFINES=		-DVME_BUS_COMPONENTS_SENSIBLE=2 $(A3818_HAL_DEFINES)
LOCAL_LDFLAGS=		$(CAENVMElib_LIB_PARAMS) $(A3818_HAL_LDFLAGS)

#---------------------------------------------------------------------
