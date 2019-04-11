
#include <stdio.h>

#include "Client.h"

/* android include files */
#include <hardware/hardware.h>
#include <hardware/gralloc.h>

static inline int __dl_gralloc_open_r(const struct hw_module_t* module, 
        struct alloc_device_t** device) SOFTFP;
static inline int __dl_gralloc_open_r(const struct hw_module_t* module, 
        struct alloc_device_t** device) {
    return module->methods->open(module, 
            GRALLOC_HARDWARE_GPU0, (struct hw_device_t**)device);
}

static inline int __dl_gralloc_close_r(struct alloc_device_t* device) SOFTFP;
static inline int __dl_gralloc_close_r(struct alloc_device_t* device) {
    return device->common.close(&device->common);
}

ANDROID_IMPLEMENT2(int, gralloc_open_r, const struct hw_module_t*, struct alloc_device_t**)
ANDROID_IMPLEMENT1(int, gralloc_close_r, struct alloc_device_t*)

static int __dl_gralloc_registerBuffer(struct gralloc_module_t const* module,
            buffer_handle_t handle) SOFTFP;
static int __dl_gralloc_registerBuffer(struct gralloc_module_t const* module,
            buffer_handle_t handle) {
    return module->registerBuffer(module, handle);
}

static int __dl_gralloc_unregisterBuffer(struct gralloc_module_t const* module,
            buffer_handle_t handle) SOFTFP;
static int __dl_gralloc_unregisterBuffer(struct gralloc_module_t const* module,
            buffer_handle_t handle) {
    return module->unregisterBuffer(module, handle);
}

ANDROID_IMPLEMENT2(int, gralloc_registerBuffer  , struct gralloc_module_t const*, buffer_handle_t)
ANDROID_IMPLEMENT2(int, gralloc_unregisterBuffer, struct gralloc_module_t const*, buffer_handle_t)

static int __dl_gralloc_lock(struct gralloc_module_t const* module,
            buffer_handle_t handle, int usage,
            int l, int t, int w, int h,
            void** vaddr) SOFTFP;
static int __dl_gralloc_lock(struct gralloc_module_t const* module,
            buffer_handle_t handle, int usage,
            int l, int t, int w, int h,
            void** vaddr) {
    return module->lock(module, handle, usage, l, t, w, h, vaddr);
}

static int __dl_gralloc_unlock(struct gralloc_module_t const* module,
            buffer_handle_t handle) SOFTFP;
static int __dl_gralloc_unlock(struct gralloc_module_t const* module,
            buffer_handle_t handle) {
    return module->unlock(module, handle);
}

ANDROID_IMPLEMENT8(int, gralloc_lock, struct gralloc_module_t const*, buffer_handle_t, int, int, int, int, int, void**)
ANDROID_IMPLEMENT2(int, gralloc_unlock, struct gralloc_module_t const*, buffer_handle_t)

static int __dl_gralloc_alloc(struct alloc_device_t* dev,
            int w, int h, int format, int usage,
            buffer_handle_t* handle, int* stride) SOFTFP;
static int __dl_gralloc_alloc(struct alloc_device_t* dev,
            int w, int h, int format, int usage,
            buffer_handle_t* handle, int* stride) {
    return dev->alloc(dev, w, h, format, usage, handle, stride);
}

static int __dl_gralloc_free(struct alloc_device_t* dev,
            buffer_handle_t handle) SOFTFP;
static int __dl_gralloc_free(struct alloc_device_t* dev,
            buffer_handle_t handle) {
    return dev->free(dev, handle);
}

ANDROID_IMPLEMENT7(int, gralloc_alloc, struct alloc_device_t*, int, int, int, int, buffer_handle_t*, int*)
ANDROID_IMPLEMENT2(int, gralloc_free, struct alloc_device_t*, buffer_handle_t)

