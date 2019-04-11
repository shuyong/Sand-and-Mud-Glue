LOCAL_PATH := $(call my-dir)

# build libevent library
include $(CLEAR_VARS)

#-UDEBUG
LOCAL_C_INCLUDES := event-include libevent/include
LOCAL_CFLAGS     := -Wall -Wextra -fvisibility=hidden -D_FORTIFY_SOURCE=2 -DDEBUG
#LOCAL_LDFLAGS    := -Wl,-O2 -Wl,--as-needed -Wl,-Bsymbolic

LOCAL_ARM_NEON   := true

LIBEVNET_SOURCES := \
	buffer.c \
	bufferevent.c \
	bufferevent_filter.c \
	bufferevent_pair.c \
	bufferevent_ratelim.c \
	bufferevent_sock.c\
	epoll.c \
	evdns.c \
	event.c \
	event_tagging.c \
	evmap.c \
	evrpc.c \
	evthread.c \
	evthread_pthread.c \
	evutil.c \
	evutil_rand.c \
	evutil_time.c \
	http.c \
	listener.c \
	log.c \
	poll.c \
	select.c \
	signal.c \
	strlcpy.c \
	$(empty)

LOCAL_MODULE := event
LOCAL_SRC_FILES := $(addprefix libevent/, $(LIBEVNET_SOURCES))

include $(BUILD_STATIC_LIBRARY)
#include $(BUILD_SHARED_LIBRARY)

#build user land execute library
include $(CLEAR_VARS)

#-UDEBUG
LOCAL_C_INCLUDES := process
LOCAL_CFLAGS     := -Wall -Wextra -fvisibility=hidden -D_FORTIFY_SOURCE=2 -DDEBUG
#LOCAL_LDFLAGS    := -Wl,-O2 -Wl,--as-needed -Wl,-Bsymbolic

LOCAL_ARM_NEON   := true

LIBUEXEC_SOURCES := \
	load_elf_binary.c \
	uland_execl.c \
	uland_execle.c \
	uland_execlp.c \
	uland_execv.c \
	uland_execve.c \
	uland_execvp.c \
	$(empty)

LOCAL_MODULE := uexec
LOCAL_SRC_FILES := $(addprefix process/, $(LIBUEXEC_SOURCES))

include $(BUILD_STATIC_LIBRARY)
#include $(BUILD_SHARED_LIBRARY)

# build server programme
include $(CLEAR_VARS)

#-UDEBUG
LOCAL_C_INCLUDES:= include event-include libevent/include
LOCAL_CFLAGS    := -Wall -Wextra -fvisibility=hidden -D_FORTIFY_SOURCE=2 -DDEBUG
LOCAL_LDFLAGS   := -Wl,-O2 -Wl,--as-needed -Wl,-Bsymbolic
#LOCAL_SHARED_LIBRARIES := event
LOCAL_STATIC_LIBRARIES := event
LOCAL_EXPORT_C_INCLUDES := include event-include libevent/include

LOCAL_ARM_NEON  := true

LOCAL_MODULE    := dlhook
LOCAL_SRC_FILES := \
        pipe/io.c \
        server/Looper.c server/Servant.c server/server_main.c \
	$(empty)

include $(BUILD_EXECUTABLE)

# build client programme
include $(CLEAR_VARS)

#-UDEBUG
LOCAL_C_INCLUDES:= include event-include libevent/include process android/bionic/libc/include android/system/core/include android/hardware/libhardware/include
LOCAL_CFLAGS    := -Wall -Wextra -fvisibility=hidden -D_FORTIFY_SOURCE=2 -DDEBUG
LOCAL_LDFLAGS   := -Wl,-O2 -Wl,--as-needed -Wl,-Bsymbolic
#LOCAL_SHARED_LIBRARIES := event
LOCAL_STATIC_LIBRARIES := event uexec
LOCAL_EXPORT_C_INCLUDES := include event-include libevent/include process android/bionic/libc/include android/system/core/include android/hardware/libhardware/include

LOCAL_ARM_NEON  := true

LOCAL_MODULE    := dlhook_client
LOCAL_SRC_FILES := \
        client/android_dlfcn.c client/Client.c client/Message.c client/Server.c \
        pipe/bridge.c pipe/io.c \
	android/system/core/libcutils/properties.c \
	\
	client/android_pthread.c \
	client/GraphicBuffer.c \
	client/hardware.c \
	client/OffscreenNativeWindow.c \
	\
	client/egl.c \
	client/gl2.c \
	client/gralloc.c \
	\
	test/main.c \
	$(empty)

include $(BUILD_EXECUTABLE)

