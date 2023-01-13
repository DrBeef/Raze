
GZDOOM_TOP_PATH := $(call my-dir)/../

include $(GZDOOM_TOP_PATH)/android/Android_lzma.mk
include $(GZDOOM_TOP_PATH)/android/Android_zlib.mk
include $(GZDOOM_TOP_PATH)/android/Android_gdtoa.mk
include $(GZDOOM_TOP_PATH)/android/Android_bzip2.mk
include $(GZDOOM_TOP_PATH)/android/Android_vpx.mk
include $(GZDOOM_TOP_PATH)/android/Android_tess.mk

include $(GZDOOM_TOP_PATH)/android/Android_src.mk

