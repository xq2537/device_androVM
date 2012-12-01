LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES:= test_lib.c

LOCAL_CFLAGS:=-O2 -g 

LOCAL_MODULE:=test_lib
LOCAL_SHARED_LIBRARIES := liblog libcutils
LOCAL_MODULE_TAGS := optional

include $(BUILD_EXECUTABLE)
