LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := vpx
LOCAL_SRC_FILES := src/vpx/$(TARGET_ARCH_ABI)/lib/libvpx.a

include $(PREBUILT_STATIC_LIBRARY)