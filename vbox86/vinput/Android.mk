LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES:= vinput.c

LOCAL_CFLAGS:=-O2 -g 

LOCAL_MODULE:=vinput
LOCAL_SHARED_LIBRARIES := liblog libcutils
LOCAL_MODULE_TAGS := optional

include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)

LOCAL_SRC_FILES:= get_bd_server.c

LOCAL_CFLAGS:=-O2 -g 

LOCAL_MODULE:=get_bd_server
LOCAL_SHARED_LIBRARIES := liblog libcutils
LOCAL_MODULE_TAGS := optional

include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)

LOCAL_SRC_FILES:= get_androVM_host.c

LOCAL_CFLAGS:=-O2 -g 

LOCAL_MODULE:=get_androVM_host
LOCAL_SHARED_LIBRARIES := liblog libcutils
LOCAL_MODULE_TAGS := optional

include $(BUILD_EXECUTABLE)

