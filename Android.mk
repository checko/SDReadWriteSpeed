
LOCAL_PATH:= $(call my-dir)


include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := eng
LOCAL_SRC_FILES:=sdreadwrite.c
LOCAL_MODULE:=sdreadwrite
LOCAL_SHARED_LIBRARIES:=libc
include $(BUILD_EXECUTABLE)
