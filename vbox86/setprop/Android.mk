LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES:= setdpi.c

LOCAL_CFLAGS:=-O2 -g 

LOCAL_MODULE:=setdpi
LOCAL_SHARED_LIBRARIES := liblog libcutils
LOCAL_MODULE_TAGS := optional

include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)

LOCAL_SRC_FILES:= androVM_setprop.c

LOCAL_CFLAGS:=-O2 -g 

LOCAL_MODULE:=androVM_setprop
LOCAL_SHARED_LIBRARIES := liblog libcutils
LOCAL_MODULE_TAGS := optional

include $(BUILD_EXECUTABLE)
