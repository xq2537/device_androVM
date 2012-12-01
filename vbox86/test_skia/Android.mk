LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES:= test_skia.c

LOCAL_MODULE:=test_skia
LOCAL_MODULE_TAGS := optional

include $(BUILD_EXECUTABLE)
