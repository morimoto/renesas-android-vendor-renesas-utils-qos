
LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SHARED_LIBRARIES := liblog

LOCAL_C_INCLUDES:= $(LOCAL_PATH)/qos_if-module/files/qos_if/include \
                   $(TOP)/hardware/renesas/modules/qos/qos-module/files/qos/drv

LOCAL_SRC_FILES := qos_if-module/files/qos_if/if/qos_lib.c

LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE:= libqos

LOCAL_MULTILIB := 64

include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)

LOCAL_C_INCLUDES:= \
    $(LOCAL_PATH)/qos_if-module/files/qos_if/include \
    $(LOCAL_PATH)/hardware/renesas/modules/qos/qos-module/files/qos/drv

LOCAL_VENDOR_MODULE := true
LOCAL_MODULE := qos_tp
LOCAL_SHARED_LIBRARIES := libqos

LOCAL_SRC_FILES := \
    qos_if-tp-user/files/qos_if/qos_tp.c

LOCAL_MULTILIB := 64

include $(BUILD_EXECUTABLE)
