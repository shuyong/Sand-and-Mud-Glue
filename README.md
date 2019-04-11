# JUST FOR FUN
* * *
# Sand-and-Mud-Glue
Call Android Function From Glibc Space

# 项目说明
用 User-Land-Execute 技术从 glibc space 远程调用 Android 函数 (bionic space)。

项目分成2个小程序：
* dlhook : 基于 bionic base 的服务端。接受客户端的远程调用，用 dlopen / dlsym 获得 bionic space 的函数指针，并可以接受客户端的参数本地执行。
* dlhook_client : 基于 glibc base 的客户端 DEMO。可以远程执行 Android 的特有函数。测试了 gralloc / ANativeWindowBuffer / ANativeWindow 接口，以及 EGL / OpenGLES v2.0 函数。

# 编译
本项目依赖 [libevent](http://libevent.org/) 项目。其它只依赖 C 库里的函数。没有使用特别的函数。

## 服务端的编译
服务端使用 Android NDK 进行编译。没有使用 Android 特别的函数。理论上只要支持 ARM / Intel - 32bit / 64bit 平台的 NDK 都可以编译。
* Application.mk : 选择 APP_PLATFORM := android-23，是为了 armeabi-v7a 平台缺省编译参数就是支持 NEON 指令。运行速度快一些。
* Android.mk : 编译依赖 [libevent](http://libevent.org/) 项目。如果本地没有，编译脚本会自动下载。第一次编译保持联网。
* build-android.sh : 编译脚本。根据自家的 NDK 路径，修改里面的 ${NDK_DIR} 路径。

编译：
```
$ ./build-android.sh
```

成功后会在 libs 目录下保存各个平台的 dlhook 程序。

## 客户端的编译
客户端使用 Makefile 进行本地编译。程序很简单。也不想再交叉编译 [libevent](http://libevent.org/) 项目了。现在用 Debian / Ubuntu 制作 chroot 环境太容易了。找一台测试用的 Android 手机，rooted，即是编译环境，也是测试环境。

安装 libevent-dev
```
$ apt-get install libevent-dev
```

编译：
```
$ make
```

成功后会在当前目录下保存当前平台的 dlhook_client 程序。

# 测试
## 上传程序
通过 adb push 方式将 dlhook (bionic base) / dlhook_client (glibc base) 上传到 chroot 环境里的同一个目录里。

下载一个静态版本的 [busybox](https://busybox.net/downloads/binaries/)，上传到 rooted Android 手机里。

## chroot
```
$ adb shell
$ cd /data/local/tmp
$ su
# ./busybox mount --bind /proc    <ubuntu rootdir>/proc
# ./busybox mount --bind /sys     <ubuntu rootdir>/sys
# ./busybox mount --bind /dev     <ubuntu rootdir>/dev
# ./busybox mount --bind /dev/pts <ubuntu rootdir>/dev/pts
# ./busybox mount --bind /system  <ubuntu rootdir>/system
# ./busybox mount --bind /vendor  <ubuntu rootdir>/vendor
# ./busybox chroot <ubuntu rootdir> /bin/sh
# umask 022
# export PATH=/bin:/usr/bin:/sbin:/usr/sbin
# su - <normal user>
$ ls
dlhook  dlhook_client
```

## 运行程序
```
$ ./dlhook_client
```

将会看到一些输出。或者成功结束，或者崩溃在半路上:-(。

# 适用平台
这个技术理论上可以适用：
* ARM / Intel - 32bit / 64bit 平台
* Android >= v4.1

测试了 Android 6.x / 7.x 版本，OK。在 Android 9.0 上测试，调用 gralloc 接口会崩溃。大概是 Android 8.0 引入的 Project Treble，造成了 gralloc 调用接口变化造成的。

和 [libhybris](https://github.com/libhybris/libhybris) 项目相比，不需要修改并重新编译 bionic，就可以调用任何 Android 函数。而 [libhybris](https://github.com/libhybris/libhybris) 需要修改 bionic，否则无法控制 Thread Local Storage (TLS) 的使用，从而无法调用 Android's EGL / OpenGLES API。

