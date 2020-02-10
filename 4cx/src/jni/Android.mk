include $(CLEAR_VARS)
LOCAL_PATH :=  ./lib/misc
LOCAL_C_INCLUDES := ./include
LOCAL_MODULE := misc
include $(LOCAL_PATH)/libmisc.a_COMPONENTS.mk
LOCAL_SRC_FILES := $(libmisc.a_COMPONENTS:.o=.c)
include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_PATH :=  ./lib/useful
LOCAL_C_INCLUDES := ./include
LOCAL_MODULE := useful
include $(LOCAL_PATH)/libuseful.a_COMPONENTS.mk
LOCAL_SRC_FILES := $(libuseful.a_COMPONENTS:.o=.c)
include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_PATH :=  ./lib/useful
LOCAL_C_INCLUDES := ./include
LOCAL_MODULE := cxscheduler
include $(LOCAL_PATH)/libcxscheduler.a_COMPONENTS.mk
LOCAL_SRC_FILES := $(libcxscheduler.a_COMPONENTS:.o=.c)
include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_PATH :=  ./lib/cxlib
LOCAL_C_INCLUDES := ./include
LOCAL_MODULE := cx_async
include $(LOCAL_PATH)/libcx_async.a_COMPONENTS.mk
LOCAL_SRC_FILES := $(libcx_async.a_COMPONENTS:.o=.c)
include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_PATH :=  ./lib/cxlib
LOCAL_C_INCLUDES := ./include
LOCAL_MODULE := cx_sync
include $(LOCAL_PATH)/libcx_sync.a_COMPONENTS.mk
LOCAL_SRC_FILES := $(libcx_sync.a_COMPONENTS:.o=.c)
include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_PATH :=  ./lib/cda
LOCAL_C_INCLUDES := ./include
LOCAL_MODULE := cda
include $(LOCAL_PATH)/libcda.a_COMPONENTS.mk
LOCAL_SRC_FILES := $(libcda.a_COMPONENTS:.o=.c)
include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_PATH :=  ./lib/cda
LOCAL_C_INCLUDES := ./include
LOCAL_MODULE := cda_d_insrv
include $(LOCAL_PATH)/libcda_d_insrv.a_COMPONENTS.mk
LOCAL_SRC_FILES := $(libcda_d_insrv.a_COMPONENTS:.o=.c)
include $(BUILD_STATIC_LIBRARY)

