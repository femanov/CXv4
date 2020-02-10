#!/bin/sh

while read DIRNAME LIBNAME
do
    echo 'include $(CLEAR_VARS)'
    echo 'LOCAL_PATH :=  ./lib/'$DIRNAME
    echo 'LOCAL_C_INCLUDES := ./include'
    echo 'LOCAL_MODULE := '$LIBNAME
    echo 'include $(LOCAL_PATH)/lib'$LIBNAME'.a_COMPONENTS.mk'
    echo 'LOCAL_SRC_FILES := $(lib'$LIBNAME'.a_COMPONENTS:.o=.c)'
    echo 'include $(BUILD_STATIC_LIBRARY)'
    echo ''
done
