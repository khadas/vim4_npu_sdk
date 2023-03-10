LOCAL_PATH:= $(call my-dir)

$(warning $(LOCAL_PATH))

include $(CLEAR_VARS)

ADLA_TOOL_PATH=/home/adla-toolkit-binary-1.6.10.3
SDK_PATH=$(ADLA_TOOL_PATH)/bin/adla_plug_in/template/nnsdk

common_src_files := main.c postprocess_util.c

common_c_includes := $(SDK_PATH)/include

LOCAL_SRC_FILES := $(common_src_files)
LOCAL_C_INCLUDES := $(common_c_includes)

LOCAL_LDLIBS += -llog -ldl -lm
LOCAL_LDLIBS += -fuse-ld=gold

ifeq ($(APP_ABI), armeabi-v7a)
LOCAL_LDLIBS +=  -L$(SDK_PATH)/libraries/android/lib32 -lnnsdk
LOCAL_MODULE := adla_nnsdk_test_32
LOCAL_LICENSE_KINDS := SPDX-license-identifier-Apache-2.0 SPDX-license-identifier-BSD SPDX-license-identifier-LGPL legacy_by_exception_only
LOCAL_LICENSE_CONDITIONS := by_exception_only notice restricted
else ifeq ($(APP_ABI), arm64-v8a)
LOCAL_LDLIBS +=  -L$(SDK_PATH)/libraries/android/lib64 -lnnsdk
LOCAL_MODULE := adla_nnsdk_test_64
LOCAL_LICENSE_KINDS := SPDX-license-identifier-Apache-2.0 SPDX-license-identifier-BSD SPDX-license-identifier-LGPL legacy_by_exception_only
LOCAL_LICENSE_CONDITIONS := by_exception_only notice restricted
endif

include $(BUILD_EXECUTABLE)

