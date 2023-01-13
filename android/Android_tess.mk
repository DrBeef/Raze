
LOCAL_PATH := $(call my-dir)/../libraries/libtess


include $(CLEAR_VARS)


LOCAL_MODULE  := tess_gl3

LOCAL_CFLAGS :=

LOCAL_LDLIBS += -llog

LOCAL_C_INCLUDES :=   . $(GZDOOM_TOP_PATH)/android/src/extrafiles \
						 $(GZDOOM_TOP_PATH)/libraries/libtess/Include

LOCAL_SRC_FILES =  	\
	Source/bucketalloc.c \
	Source/dict.c \
	Source/geom.c \
	Source/mesh.c \
	Source/priorityq.c \
	Source/sweep.c \
	Source/tess.c \


include $(BUILD_STATIC_LIBRARY)








