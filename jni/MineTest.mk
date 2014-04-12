LOCAL_PATH := $(call my-dir)/../gamesrc
IRRLICHT_PROJECT_PATH := $(LOCAL_PATH)
include $(CLEAR_VARS)
LOCAL_MODULE := Mine
LOCAL_CFLAGS := -DMINE_FULL_VERSION -D_IRR_ANDROID_PLATFORM_ -DANDROID -pipe -fexceptions -fstrict-aliasing

ifndef NDEBUG
LOCAL_CFLAGS += -g -D_DEBUG
else
LOCAL_CFLAGS += -fexpensive-optimizations -O3
endif

ifeq ($(TARGET_ARCH_ABI),x86)
LOCAL_CFLAGS += -fno-stack-protector
endif

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../projects/irrlicht/include \
		$(LOCAL_PATH)/../projects/irrlicht/source/Irrlicht \
		$(LOCAL_PATH)/../projects/irrlicht/source/Irrlicht/Android \
		$(LOCAL_PATH)/../projects/irrlicht/source/Irrlicht/editbox \
		$(LOCAL_PATH)/lua/src \
		$(LOCAL_PATH) $(LOCAL_PATH)/jthread $(LOCAL_PATH)/control \
                $(LOCAL_PATH)/game_loop \
 		$(LOCAL_PATH)/json $(LOCAL_PATH)/script $(LOCAL_PATH)/sqlite 
		

FILE_LIST := $(wildcard $(LOCAL_PATH)/*cpp $(LOCAL_PATH)/*/*cpp $(LOCAL_PATH)/*/*/*cpp $(LOCAL_PATH)/*/*c $(LOCAL_PATH)/*/*/*c )
LOCAL_SRC_FILES := $(FILE_LIST:$(LOCAL_PATH)/%=%)
		

LOCAL_LDLIBS :=  -lEGL -llog -lGLESv1_CM -lGLESv2 -lz -landroid -lOpenSLES 


LOCAL_STATIC_LIBRARIES := android_native_app_glue \
			
LOCAL_WHOLE_STATIC_LIBRARIES := libIrrlicht 

include $(BUILD_SHARED_LIBRARY)

$(call import-module,android/native_app_glue)

