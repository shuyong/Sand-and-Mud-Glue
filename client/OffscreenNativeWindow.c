#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#if defined (__ANDROID__)
#include <pthread.h>
#endif

#include <hardware/gralloc.h>
#include <hardware/hardware.h>
#include <system/graphics.h>
#include <system/window.h>

#include "android_pthread.h"
#include "GraphicBuffer.h"
#include "OffscreenNativeWindow.h"

#define NO_ERROR                0L
#define BAD_VALUE               -1

struct NativeBuffer {
	GraphicBuffer* gbuffer;
	struct ANativeWindowBuffer* anwb;
	int fenceFd;
};

#define DEFAULT_NUM_BUFFERS 2
struct OffscreenNativeWindow {
	ANativeWindow window[1];
	int width;
	int height;
	int format;
	int usage;
	struct NativeBuffer* buffers;
	int count;
	int frontbuffer;
	int tailbuffer;
	int refCount;
	pthread_mutex_t mutex;
	pthread_cond_t cond;

	void (*postBuffer) (OffscreenNativeWindow* window, GraphicBuffer* buffer);
	void (*waitForBuffer) (OffscreenNativeWindow* window, GraphicBuffer* buffer);
};

/**********************************************************************/
static void incRef(struct android_native_base_t* base)
{
	__sync_fetch_and_add(&(((OffscreenNativeWindow *)base)->refCount), 1);
}

static void decRef(struct android_native_base_t* base)
{
	if (__sync_fetch_and_sub(&(((OffscreenNativeWindow *)base)->refCount), 1) == 1) {
		OffscreenNativeWindowDtor((OffscreenNativeWindow *)base);
	}
}

/**********************************************************************/
static void ANativeWindowCtor(ANativeWindow* window)
{
	window->common.magic = ANDROID_NATIVE_WINDOW_MAGIC;
	window->common.version = sizeof(ANativeWindow);
	memset(window->common.reserved, 0, sizeof(window->common.reserved));

	window->common.decRef = decRef;
	window->common.incRef = incRef;
	*(uint32_t *)&(window->flags) = 0;
	*(int *)&(window->minSwapInterval) = 0;
	*(int *)&(window->maxSwapInterval) = 0;
	*(float *)&(window->xdpi) = 0;
	*(float *)&(window->ydpi) = 0;

	window->setSwapInterval = NULL;
	window->dequeueBuffer_DEPRECATED = NULL;
	window->lockBuffer_DEPRECATED = NULL;
	window->queueBuffer_DEPRECATED = NULL;
	window->query = NULL;
	window->perform = NULL;
	window->cancelBuffer_DEPRECATED = NULL;
	window->dequeueBuffer = NULL;
	window->queueBuffer = NULL;
	window->cancelBuffer = NULL;
}

static void ANativeWindowDtor(ANativeWindow* window)
{
	*(uint32_t *)&(window->flags) = 0;
	*(int *)&(window->minSwapInterval) = 0;
	*(int *)&(window->maxSwapInterval) = 0;
	*(float *)&(window->xdpi) = 0;
	*(float *)&(window->ydpi) = 0;
}

/**********************************************************************/
static int OffscreenNativeWindow_setSwapInterval(struct ANativeWindow* window,
                int interval);

static int OffscreenNativeWindow_dequeueBuffer_DEPRECATED(struct ANativeWindow* window,
                struct ANativeWindowBuffer** buffer);

static int OffscreenNativeWindow_lockBuffer_DEPRECATED(struct ANativeWindow* window,
                struct ANativeWindowBuffer* buffer);

static int OffscreenNativeWindow_queueBuffer_DEPRECATED(struct ANativeWindow* window,
                struct ANativeWindowBuffer* buffer);

static int OffscreenNativeWindow_query(const struct ANativeWindow* window,
                int what, int* value);

static int OffscreenNativeWindow_perform(struct ANativeWindow* window,
                int operation, ... );

static int OffscreenNativeWindow_cancelBuffer_DEPRECATED(struct ANativeWindow* window,
                struct ANativeWindowBuffer* buffer);

static int OffscreenNativeWindow_dequeueBuffer(struct ANativeWindow* window,
                struct ANativeWindowBuffer** buffer, int* fenceFd);

static int OffscreenNativeWindow_queueBuffer(struct ANativeWindow* window,
                struct ANativeWindowBuffer* buffer, int fenceFd);

static int OffscreenNativeWindow_cancelBuffer(struct ANativeWindow* window,
                struct ANativeWindowBuffer* buffer, int fenceFd);

static int OffscreenNativeWindow_setBufferCount(OffscreenNativeWindow* window, int count)
{
	__dl_pthread_mutex_lock(&((OffscreenNativeWindow* )window)->mutex);

	if (count < 1)
		return BAD_VALUE;

	int i;
	for (i = 0; i < window->count; i++) {
#if 1
		struct android_native_base_t* common = (struct android_native_base_t* )window->buffers[i].anwb;
		common->decRef (common);
#else
		GraphicBufferDtor(window->buffers[i]);
#endif
		if (window->buffers[i].fenceFd >= 0) {
			close (window->buffers[i].fenceFd);
		}
	}
	if (window->buffers) {
		__dl_free (window->buffers);
	}

	window->count = count;
	window->buffers = __dl_calloc(window->count, sizeof(struct NativeBuffer));
	for (i = 0; i < window->count; i++) {
		window->buffers[i].gbuffer = GraphicBufferCtor(window->width, window->height, window->format, window->usage);
		window->buffers[i].anwb = GraphicBufferGetNativeBuffer(window->buffers[i].gbuffer);
		struct android_native_base_t* common = (struct android_native_base_t* )window->buffers[i].anwb;
		common->incRef (common);
		window->buffers[i].fenceFd = -1;
	}

	window->frontbuffer = window->count - 1;
	window->tailbuffer = 0;

	__dl_pthread_mutex_unlock(&((OffscreenNativeWindow* )window)->mutex);

	return NO_ERROR;
}

static OffscreenNativeWindow* __dl_OffscreenNativeWindowCtor(int width, int height, int format, int usage)
{
	OffscreenNativeWindow* window = __dl_calloc(1, sizeof(OffscreenNativeWindow));
	ANativeWindowCtor((ANativeWindow* )window);
	window->width = width;
	window->height = height;
	window->format = format;
	window->usage = usage;
	window->refCount = 0;
	__dl_pthread_mutex_init(&window->mutex, NULL);
	__dl_pthread_cond_init(&window->cond, NULL);
	OffscreenNativeWindow_setBufferCount(window, DEFAULT_NUM_BUFFERS);

	((ANativeWindow* )window)->setSwapInterval = OffscreenNativeWindow_setSwapInterval;
	((ANativeWindow* )window)->dequeueBuffer_DEPRECATED = OffscreenNativeWindow_dequeueBuffer_DEPRECATED;
	((ANativeWindow* )window)->lockBuffer_DEPRECATED = OffscreenNativeWindow_lockBuffer_DEPRECATED;
	((ANativeWindow* )window)->queueBuffer_DEPRECATED = OffscreenNativeWindow_queueBuffer_DEPRECATED;
	((ANativeWindow* )window)->query = OffscreenNativeWindow_query;
	((ANativeWindow* )window)->perform = OffscreenNativeWindow_perform;
	((ANativeWindow* )window)->cancelBuffer_DEPRECATED = OffscreenNativeWindow_cancelBuffer_DEPRECATED;
	((ANativeWindow* )window)->dequeueBuffer = OffscreenNativeWindow_dequeueBuffer;
	((ANativeWindow* )window)->queueBuffer = OffscreenNativeWindow_queueBuffer;
	((ANativeWindow* )window)->cancelBuffer = OffscreenNativeWindow_cancelBuffer;

	window->postBuffer = NULL;
	window->waitForBuffer = NULL;

	return window;
}

static void __dl_OffscreenNativeWindowDtor(OffscreenNativeWindow* window)
{
	int i;
	for (i = 0; i < window->count; i++) {
#if 1
		struct android_native_base_t* common = (struct android_native_base_t* )window->buffers[i].anwb;
		common->decRef (common);
#else
		GraphicBufferDtor(window->buffers[i]);
#endif
		if (window->buffers[i].fenceFd >= 0) {
			close (window->buffers[i].fenceFd);
		}
	}
	if (window->buffers) {
		__dl_free (window->buffers);
	}
	ANativeWindowDtor((ANativeWindow* )window);
	__dl_free(window);
}

static int OffscreenNativeWindow_setSwapInterval(struct ANativeWindow* window,
                int interval)
{
	(void)window;
	(void)interval;
	return 0;
}

static int OffscreenNativeWindow_dequeueBuffer_DEPRECATED(struct ANativeWindow* window,
                struct ANativeWindowBuffer** buffer)
{
	ANativeWindowBuffer* buf;
	int fenceFd = -1;
	int ret = window->dequeueBuffer(window, &buf, &fenceFd);
	if (fenceFd >= 0) {
		close (fenceFd);
	}
	*buffer = buf;
	return ret;
}

static int OffscreenNativeWindow_lockBuffer_DEPRECATED(struct ANativeWindow* window,
                struct ANativeWindowBuffer* buffer)
{
	(void)window;
	(void)buffer;
	return NO_ERROR;
}

static int OffscreenNativeWindow_queueBuffer_DEPRECATED(struct ANativeWindow* window,
                struct ANativeWindowBuffer* buffer)
{
	return window->queueBuffer(window, buffer, -1);
}

static int OffscreenNativeWindow_query(const struct ANativeWindow* window,
                int what, int* value)
{
	switch (what) {
		case NATIVE_WINDOW_WIDTH:
			*value = ((OffscreenNativeWindow* )window)->width;
			return NO_ERROR;
		case NATIVE_WINDOW_HEIGHT:
			*value = ((OffscreenNativeWindow* )window)->height;
			return NO_ERROR;
		case NATIVE_WINDOW_FORMAT:
			*value = ((OffscreenNativeWindow* )window)->format;
			return NO_ERROR;
		case NATIVE_WINDOW_CONCRETE_TYPE:
#if 1
			/* https://android.googlesource.com/platform/system/core/+/bcfa910611b42018db580b3459101c564f802552%5E!/ */
			*value = NATIVE_WINDOW_SURFACE;
#else
			*value = NATIVE_WINDOW_SURFACE_TEXTURE_CLIENT;
#endif
			return NO_ERROR;
		case NATIVE_WINDOW_QUEUES_TO_WINDOW_COMPOSER:
			*value = 1;
			return NO_ERROR;
		case NATIVE_WINDOW_DEFAULT_WIDTH:
			*value = ((OffscreenNativeWindow* )window)->width;
			return NO_ERROR;
		case NATIVE_WINDOW_DEFAULT_HEIGHT:
			*value = ((OffscreenNativeWindow* )window)->height;
			return NO_ERROR;
		case NATIVE_WINDOW_TRANSFORM_HINT:
			*value = 0;
			return NO_ERROR;
		case NATIVE_WINDOW_MIN_UNDEQUEUED_BUFFERS:
			*value = 1;
			return NO_ERROR;
		/* ANDROID_VERSION_MAJOR>=6 */
		case NATIVE_WINDOW_DEFAULT_DATASPACE:
			*value = /*HAL_DATASPACE_UNKNOWN*/ 0;
			return NO_ERROR;
#if 0
		case NATIVE_WINDOW_CONSUMER_USAGE_BITS:
			*value = self->getUsage();
			return NO_ERROR;
#endif
		/* ANDROID_VERSION_MAJOR>=8 */
		case NATIVE_WINDOW_IS_VALID:
			// sure :)
			*value = 1;
			return NO_ERROR;
		case NATIVE_WINDOW_BUFFER_AGE:
			// sure :)
			*value = 2;
			return NO_ERROR;
	}

	*value = 0;
	return BAD_VALUE;
}

static int OffscreenNativeWindow_perform(struct ANativeWindow* window,
                int operation, ... )
{
	va_list args;
	va_start(args, operation);

	// FIXME
	switch(operation) {
	case NATIVE_WINDOW_SET_USAGE                 : //  0,
	{
		int usage = va_arg(args, int);
		((OffscreenNativeWindow* )window)->usage = usage;
		break;
	}
	case NATIVE_WINDOW_CONNECT                   : //  1,   /* deprecated */
		break;
	case NATIVE_WINDOW_DISCONNECT                : //  2,   /* deprecated */
		break;
	case NATIVE_WINDOW_SET_CROP                  : //  3,   /* private */
		break;
	case NATIVE_WINDOW_SET_BUFFER_COUNT          : //  4,
	{
		int count = va_arg(args, int);
		//TRACE("set buffer count %i", cnt);
		va_end(args);
		return OffscreenNativeWindow_setBufferCount((struct OffscreenNativeWindow *)window, count);
		break;
	}
	case NATIVE_WINDOW_SET_BUFFERS_GEOMETRY      : //  5,   /* deprecated */
		break;
	case NATIVE_WINDOW_SET_BUFFERS_TRANSFORM     : //  6,
		break;
	case NATIVE_WINDOW_SET_BUFFERS_TIMESTAMP     : //  7,
		break;
	case NATIVE_WINDOW_SET_BUFFERS_DIMENSIONS    : //  8,
	{
		int width  = va_arg(args, int);
		int height = va_arg(args, int);
		((OffscreenNativeWindow* )window)->width = width;
		((OffscreenNativeWindow* )window)->height = height;
		break;
	}
	case NATIVE_WINDOW_SET_BUFFERS_FORMAT        : //  9,
	{
		int format = va_arg(args, int);
		((OffscreenNativeWindow* )window)->format = format;
		break;
	}
	case NATIVE_WINDOW_SET_SCALING_MODE          : // 10,   /* private */
		break;
	case NATIVE_WINDOW_LOCK                      : // 11,   /* private */
		break;
	case NATIVE_WINDOW_UNLOCK_AND_POST           : // 12,   /* private */
		break;
	case NATIVE_WINDOW_API_CONNECT               : // 13,   /* private */
		break;
	case NATIVE_WINDOW_API_DISCONNECT            : // 14,   /* private */
		break;
	case NATIVE_WINDOW_SET_BUFFERS_USER_DIMENSIONS : // 15, /* private */
		break;
	case NATIVE_WINDOW_SET_POST_TRANSFORM_CROP   : // 16,
		break;
	}

	va_end(args);

	return NO_ERROR;
}

static int OffscreenNativeWindow_cancelBuffer_DEPRECATED(struct ANativeWindow* window,
                struct ANativeWindowBuffer* buffer)
{
	return window->cancelBuffer(window, buffer, -1);
}

static int OffscreenNativeWindow_dequeueBuffer(struct ANativeWindow* window,
                struct ANativeWindowBuffer** buffer, int* fenceFd)
{
	__dl_pthread_mutex_lock(&((OffscreenNativeWindow* )window)->mutex);

	while (((OffscreenNativeWindow* )window)->tailbuffer == ((OffscreenNativeWindow* )window)->frontbuffer) {
		__dl_pthread_cond_wait(&((OffscreenNativeWindow* )window)->cond, &((OffscreenNativeWindow* )window)->mutex);
	}

	int tail = ((OffscreenNativeWindow* )window)->tailbuffer;
	GraphicBuffer* gbuffer = ((OffscreenNativeWindow* )window)->buffers[tail].gbuffer;
	struct ANativeWindowBuffer* selectedBuffer = ((OffscreenNativeWindow* )window)->buffers[tail].anwb;
	int fence = ((OffscreenNativeWindow* )window)->buffers[tail].fenceFd;
	((OffscreenNativeWindow* )window)->buffers[tail].fenceFd = -1;
	if (selectedBuffer->width  != ((OffscreenNativeWindow* )window)->width
	 || selectedBuffer->height != ((OffscreenNativeWindow* )window)->height
	 || selectedBuffer->format != ((OffscreenNativeWindow* )window)->format) {
#if 0
		GraphicBufferReallocate((GraphicBuffer* )selectedBuffer,
					((OffscreenNativeWindow* )window)->width,
					((OffscreenNativeWindow* )window)->height,
					((OffscreenNativeWindow* )window)->format,
					((OffscreenNativeWindow* )window)->usage);
#else
		struct android_native_base_t* common = (struct android_native_base_t* )selectedBuffer;
		common->decRef (common);

		gbuffer = GraphicBufferCtor(((OffscreenNativeWindow* )window)->width,
					    ((OffscreenNativeWindow* )window)->height,
					    ((OffscreenNativeWindow* )window)->format,
					    ((OffscreenNativeWindow* )window)->usage);
		selectedBuffer = GraphicBufferGetNativeBuffer(gbuffer);
		common = (struct android_native_base_t* )selectedBuffer;
		common->incRef (common);
		((OffscreenNativeWindow* )window)->buffers[tail].gbuffer = gbuffer;
		((OffscreenNativeWindow* )window)->buffers[tail].anwb = selectedBuffer;
#endif
		if (fence >= 0) {
			close (fence);
			fence = -1;
		}
	}
	*buffer = selectedBuffer;
	*fenceFd = fence;
	if (((OffscreenNativeWindow* )window)->waitForBuffer)
		((OffscreenNativeWindow* )window)->waitForBuffer((OffscreenNativeWindow* )window, gbuffer);
	((OffscreenNativeWindow* )window)->tailbuffer = (tail + 1) % ((OffscreenNativeWindow* )window)->count;

	__dl_pthread_mutex_unlock(&((OffscreenNativeWindow* )window)->mutex);

	return NO_ERROR;
}

static int OffscreenNativeWindow_queueBuffer(struct ANativeWindow* window,
                struct ANativeWindowBuffer* buffer, int fenceFd)
{
	__dl_pthread_mutex_lock(&((OffscreenNativeWindow* )window)->mutex);

	int front = ((OffscreenNativeWindow* )window)->frontbuffer;
	int i = 0;
	do {
		front = (front + 1) % ((OffscreenNativeWindow* )window)->count;
		i++;
	} while (((OffscreenNativeWindow* )window)->buffers[front].anwb != buffer && i < ((OffscreenNativeWindow* )window)->count);
	GraphicBuffer* gbuffer = ((OffscreenNativeWindow* )window)->buffers[front].gbuffer;
#if 1
	if (fenceFd >= 0) {
		close (fenceFd);
		fenceFd = -1;
	}
#endif
	((OffscreenNativeWindow* )window)->buffers[front].fenceFd = fenceFd;
	((OffscreenNativeWindow* )window)->frontbuffer = front;
	if (((OffscreenNativeWindow* )window)->postBuffer)
		((OffscreenNativeWindow* )window)->postBuffer((OffscreenNativeWindow* )window, gbuffer);

	__dl_pthread_cond_signal(&((OffscreenNativeWindow* )window)->cond);

	__dl_pthread_mutex_unlock(&((OffscreenNativeWindow* )window)->mutex);

	return NO_ERROR;
}

static int OffscreenNativeWindow_cancelBuffer(struct ANativeWindow* window,
                struct ANativeWindowBuffer* buffer, int fenceFd)
{
	__dl_pthread_mutex_lock(&((OffscreenNativeWindow* )window)->mutex);

	int tail = ((OffscreenNativeWindow* )window)->tailbuffer;
	tail = tail > 0 ? tail - 1 : ((OffscreenNativeWindow* )window)->count - 1;
	int i = 0;
	while (((OffscreenNativeWindow* )window)->buffers[tail].anwb != buffer && i < ((OffscreenNativeWindow* )window)->count) {
		tail = (tail + 1) % ((OffscreenNativeWindow* )window)->count;
		i++;
	}
#if 1
	if (fenceFd >= 0) {
		close (fenceFd);
		fenceFd = -1;
	}
#endif
	((OffscreenNativeWindow* )window)->buffers[tail].fenceFd = fenceFd;
	((OffscreenNativeWindow* )window)->tailbuffer = tail;

	__dl_pthread_cond_signal(&((OffscreenNativeWindow* )window)->cond);

	__dl_pthread_mutex_unlock(&((OffscreenNativeWindow* )window)->mutex);

	return NO_ERROR;
}

/*********************************************************************/
#include "Client.h"

ANDROID_IMPLEMENT4(OffscreenNativeWindow*, OffscreenNativeWindowCtor, int , int , int , int )
ANDROID_VOID_IMPLEMENT1(OffscreenNativeWindowDtor, OffscreenNativeWindow*)

void OffscreenNativeWindowResize(OffscreenNativeWindow* window, int width, int height, int format)
{
	window->width  = width;
	window->height = height;
	window->format = format;
}

ANativeWindow* OffscreenNativeWindowGetNativeWindow(OffscreenNativeWindow* window)
{
	return (ANativeWindow* )window;
}


