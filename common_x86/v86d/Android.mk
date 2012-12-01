# Copyright 2011, The Android-x86 Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES :=		\
	v86.c			\
	v86_lrmi.c		\
	v86_common.c		\
	lrmi/lrmi.c		\
	lrmi/x86-common.c

LOCAL_C_INCLUDES += $(LOCAL_PATH)/lrmi

LOCAL_MODULE := v86d
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_ROOT_OUT_SBIN)

include $(BUILD_EXECUTABLE)
