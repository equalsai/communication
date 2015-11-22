LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := libevent2
LOCAL_SRC_FILES := libevent2.a
include $(PREBUILT_STATIC_LIBRARY)


include $(CLEAR_VARS)

LOCAL_MODULE := cardReaderServer
LOCAL_SRC_FILES:= \
  deviceServer.cpp \
  parseData.cpp \
  GwiCRMidInterface.cpp


LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/android \
    $(LOCAL_PATH)/include 


LOCAL_STATIC_LIBRARIES += libevent2

LOCAL_LDFLAGS += -L$(LOCAL_PATH)/lib/ -lGWI_CardReader_Driver

include $(BUILD_EXECUTABLE)