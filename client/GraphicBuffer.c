#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include <hardware/gralloc.h>
#include <hardware/hardware.h>
#include <system/graphics.h>
#include <system/window.h>

#include "android_hardware.h"
#include "GraphicBuffer.h"

extern void  (*__dl_free)(void *ptr) SOFTFP;
extern void *(*__dl_calloc)(size_t nmemb, size_t size) SOFTFP;

struct GraphicBuffer {
    ANativeWindowBuffer_t  buffer[1];
    gralloc_module_t      *gralloc_module;
    alloc_device_t        *alloc_device;
    int			   refCount;
};

/**********************************************************************/
static void incRef(struct android_native_base_t* base)
{
	__sync_fetch_and_add(&(((GraphicBuffer *)base)->refCount), 1);
}

static void decRef(struct android_native_base_t* base)
{
	if (__sync_fetch_and_sub(&(((GraphicBuffer *)base)->refCount), 1) == 1) {
		GraphicBufferDtor((GraphicBuffer *)base);
	}
}

/**********************************************************************/
static void ANativeWindowBufferCtor(ANativeWindowBuffer_t* buffer)
{
	buffer->common.magic = ANDROID_NATIVE_BUFFER_MAGIC;
	buffer->common.version = sizeof(ANativeWindowBuffer);
	memset(buffer->common.reserved, 0, sizeof(buffer->common.reserved));

	buffer->common.decRef = decRef;
	buffer->common.incRef = incRef;
	buffer->width = 0;
	buffer->height = 0;
	buffer->stride = 0;
	buffer->format = 0;
	buffer->usage = 0;
	buffer->handle = 0;
}

static void ANativeWindowBufferDtor(ANativeWindowBuffer_t* buffer)
{
	buffer->width = 0;
	buffer->height = 0;
	buffer->stride = 0;
	buffer->format = 0;
	buffer->usage = 0;
	buffer->handle = 0;
}

/**********************************************************************/
#if 0
static inline int gralloc_lock(gralloc_module_t const* module,
        buffer_handle_t handle, int usage,
        int l, int t, int w, int h,
        void** vaddr)
{
    return module->lock(module, handle, usage, l, t, w, h, vaddr);
}

static inline int gralloc_unlock(gralloc_module_t const* module, 
        buffer_handle_t handle)
{
    return module->unlock(module, handle);
}

static inline int gralloc_register_buffer(gralloc_module_t const* module,
        buffer_handle_t handle)
{
    return module->registerBuffer(module, handle);
}

static inline int gralloc_unregister_buffer(gralloc_module_t const* module,
        buffer_handle_t handle)
{
    return module->unregisterBuffer(module, handle);
}
#endif

static void
AndroidAllocNativeBuffer( GraphicBuffer *buffer, uint32_t width, uint32_t height, uint32_t format, uint32_t usage )
{
    int err = __dl_hw_get_module(GRALLOC_HARDWARE_MODULE_ID, (const struct hw_module_t **)&buffer->gralloc_module);
    if (err || !buffer->gralloc_module) {
         fprintf (stderr, "Android/FBO: hw_get_module failed (%d)\n", err );
         return ;
    }

    err = gralloc_open((const hw_module_t *)buffer->gralloc_module, &buffer->alloc_device);
    if (err || !buffer->alloc_device) {
         fprintf (stderr, "Android/FBO: open alloc failed\n");
         return ;
    }

    buffer_handle_t buffer_handle = NULL;
    int stride = 0;

    buffer->alloc_device->alloc(buffer->alloc_device, width, height, format, usage, &buffer_handle, &stride);
    if (!buffer_handle) {
         fprintf (stderr, "Android/FBO: failed to alloc buffer\n");
         return ;
    }

    ((ANativeWindowBuffer_t* )buffer)->width = width;
    ((ANativeWindowBuffer_t* )buffer)->height = height;
    ((ANativeWindowBuffer_t* )buffer)->stride = stride;
    ((ANativeWindowBuffer_t* )buffer)->format = format;
    ((ANativeWindowBuffer_t* )buffer)->usage = usage;
    ((ANativeWindowBuffer_t* )buffer)->handle = buffer_handle;
}

static void
AndroidFreeNativeBuffer( GraphicBuffer *buffer )
{
    if (!buffer->alloc_device || !((ANativeWindowBuffer_t* )buffer)->handle) {
        fprintf (stderr, "AndroidFreeNativeBuffer: FBO was never initialized correctly.\n");
        return;
    }

    buffer->alloc_device->free(buffer->alloc_device, ((ANativeWindowBuffer_t* )buffer)->handle);

    gralloc_close(buffer->alloc_device);
}

static int
AndroidReallocNativeBuffer( GraphicBuffer *buffer, uint32_t width, uint32_t height, uint32_t format, uint32_t usage )
{
    ((ANativeWindowBuffer_t* )buffer)->width  = width;
    ((ANativeWindowBuffer_t* )buffer)->height = height;
    ((ANativeWindowBuffer_t* )buffer)->format = format;
    ((ANativeWindowBuffer_t* )buffer)->usage  = usage;
    return buffer->alloc_device->alloc(buffer->alloc_device, width, height, format, usage, &(((ANativeWindowBuffer_t* )buffer)->handle), &(((ANativeWindowBuffer_t* )buffer)->stride));
}

static int
AndroidLockNativeBuffer( GraphicBuffer *buffer, uint32_t usage, void** vaddr )
{
    return (buffer->gralloc_module->lock(buffer->gralloc_module, ((ANativeWindowBuffer_t* )buffer)->handle, usage, 0, 0, ((ANativeWindowBuffer_t* )buffer)->width, ((ANativeWindowBuffer_t* )buffer)->height, vaddr));
}

static int
AndroidUnlockNativeBuffer( GraphicBuffer *buffer )
{
    return (buffer->gralloc_module->unlock(buffer->gralloc_module, ((ANativeWindowBuffer_t* )buffer)->handle));
}

/*********************************************************************/
GraphicBuffer* GraphicBufferCtor(uint32_t width, uint32_t height, uint32_t format, uint32_t usage)
{
    GraphicBuffer* buffer = __dl_calloc(1, sizeof(GraphicBuffer));
    ANativeWindowBufferCtor((ANativeWindowBuffer_t* )buffer);
    AndroidAllocNativeBuffer( buffer, width, height, format, usage );
    buffer->refCount = 0;
    return buffer;
}

void GraphicBufferDtor(GraphicBuffer* buffer)
{
    AndroidFreeNativeBuffer( buffer );
    ANativeWindowBufferDtor((ANativeWindowBuffer_t* )buffer);
    __dl_free(buffer);
}

int GraphicBufferReallocate(GraphicBuffer* buffer, uint32_t width, uint32_t height, uint32_t format, uint32_t usage)
{
    return AndroidReallocNativeBuffer(buffer, width, height, format, usage);
}

int GraphicBufferLock(GraphicBuffer* buffer, uint32_t usage, void** vaddr)
{
    return AndroidLockNativeBuffer(buffer, usage, vaddr);
}

int GraphicBufferUnlock(GraphicBuffer* buffer)
{
    return AndroidUnlockNativeBuffer(buffer);
}

ANativeWindowBuffer* GraphicBufferGetNativeBuffer(GraphicBuffer* buffer)
{
    return (ANativeWindowBuffer_t* )buffer;
}

/*********************************************************************/
#include "Client.h"

static GraphicBuffer* (*__dl_ANativeBufferCtor)(uint32_t width, uint32_t height, uint32_t format, uint32_t usage) SOFTFP = GraphicBufferCtor;
static void (*__dl_ANativeBufferDtor)(GraphicBuffer* object) SOFTFP = GraphicBufferDtor;
static int (*__dl_ANativeBufferReallocate)(GraphicBuffer* object, uint32_t width, uint32_t height, uint32_t format, uint32_t usage) SOFTFP = GraphicBufferReallocate;
static int (*__dl_ANativeBufferLock)(GraphicBuffer* object, uint32_t usage, void** vaddr) SOFTFP = GraphicBufferLock;
static int (*__dl_ANativeBufferUnlock)(GraphicBuffer* object) SOFTFP = GraphicBufferUnlock;
static ANativeWindowBuffer* (*__dl_ANativeBufferGetNativeBuffer)(GraphicBuffer* object) SOFTFP = GraphicBufferGetNativeBuffer;

ANDROID_IMPLEMENT4(GraphicBuffer*, ANativeBufferCtor, uint32_t , uint32_t , uint32_t , uint32_t )
ANDROID_VOID_IMPLEMENT1(ANativeBufferDtor, GraphicBuffer*)
ANDROID_IMPLEMENT5(int, ANativeBufferReallocate, GraphicBuffer* , uint32_t , uint32_t , uint32_t , uint32_t )
ANDROID_IMPLEMENT3(int, ANativeBufferLock, GraphicBuffer* , uint32_t , void** )
ANDROID_IMPLEMENT1(int, ANativeBufferUnlock, GraphicBuffer* )
ANDROID_IMPLEMENT1(ANativeWindowBuffer*, ANativeBufferGetNativeBuffer, GraphicBuffer* )


