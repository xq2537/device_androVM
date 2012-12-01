LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_CFLAGS+=-Dlinux

LOCAL_SRC_FILES:= test_vmci.c

LOCAL_MODULE:=test_vmci
LOCAL_MODULE_TAGS := optional

include $(BUILD_EXECUTABLE)
