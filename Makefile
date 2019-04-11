LOCAL_C_INCLUDES = -I include -I process -I android/bionic/libc/include -I android/system/core/include -I android/hardware/libhardware/include
CFLAGS = -Wall -Wextra -g -O2 -std=c99 $(LOCAL_C_INCLUDES)
CFLAGS += -ffunction-sections -funwind-tables -fstack-protector-strong -fPIC -Wno-invalid-command-line-argument -Wno-unused-command-line-argument -fvisibility=hidden -D_FORTIFY_SOURCE=2 -DDEBUG -Wa,--noexecstack -Wformat -Werror=format-security -fPIE
LDFLAGS = -Wl,--gc-sections -Wl,--build-id -Wl,--no-undefined -Wl,-z,noexecstack -Wl,-z,relro -Wl,-z,now -Wl,--warn-shared-textrel -Wl,--fatal-warnings -fPIE -pie  -Wl,-O2 -Wl,--as-needed -Wl,-Bsymbolic

LDLIBS_S = -levent -ldl
LDLIBS_C = -L process -luexec -levent -lm -pthread

TARGET_S = dlhook
TARGET_C = dlhook_client

OBJS_S = \
       pipe/io.o \
       server/Looper.o server/Servant.o server/server_main.o \
       $(empty)

OBJS_C = \
       client/android_dlfcn.o client/Client.o client/Message.o client/Server.o \
       pipe/bridge.o pipe/io.o \
       android/system/core/libcutils/properties.o \
       \
       client/android_pthread.o \
       client/GraphicBuffer.o \
       client/hardware.o \
       client/OffscreenNativeWindow.o \
       \
       client/egl.o \
       client/gl2.o \
       client/gralloc.o \
       \
       test/main.o \
       $(empty)

LOBJS = \
	process/load_elf_binary.o \
	process/uland_execl.o \
	process/uland_execle.o \
	process/uland_execlp.o \
	process/uland_execv.o \
	process/uland_execve.o \
	process/uland_execvp.o \
	$(empty)

all: $(TARGET_S) $(TARGET_C)

$(TARGET_S) : $(OBJS_S)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(TARGET_S) $(OBJS_S) $(LDLIBS_S)

$(TARGET_C) : $(OBJS_C) process/libuexec.a
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(TARGET_C) $(OBJS_C) $(LDLIBS_C)

process/libuexec.a : $(LOBJS)
	$(AR) rcs $@ $(LOBJS)
	@#$(RANLIB) $@

clean :
	rm -f $(TARGET_S) $(OBJS_S) $(TARGET_C) $(OBJS_C) process/libuexec.a $(LOBJS)

%.o : %.c
	@$(CC) -c $(CFLAGS) $< -o $@

%.o : %.cpp
	@$(CXX) -c $(CXXFLAGS) $< -o $@

%.o : %.S
	@$(CC) -c $(CFLAGS) $< -o $@


.PHONY: all clean
