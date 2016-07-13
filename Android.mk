
LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SHARED_LIBRARIES := liblog

LOCAL_C_INCLUDES:= $(LOCAL_PATH)/qos_if-module/files/qos_if/include

LOCAL_SRC_FILES := qos_if-module/files/qos_if/if/qos_lib.c

LOCAL_MODULE:= libqos

include $(BUILD_SHARED_LIBRARY)
