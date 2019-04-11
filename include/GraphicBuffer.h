#ifndef _GRAPHICBUFFER_H_
#define _GRAPHICBUFFER_H_

typedef struct GraphicBuffer GraphicBuffer;
typedef struct ANativeWindowBuffer ANativeWindowBuffer;

/* running on android space */
GraphicBuffer* GraphicBufferCtor(uint32_t width, uint32_t height, uint32_t format, uint32_t usage);
void GraphicBufferDtor(GraphicBuffer* object);
int GraphicBufferReallocate(GraphicBuffer* object, uint32_t width, uint32_t height, uint32_t format, uint32_t usage);
int GraphicBufferLock(GraphicBuffer* object, uint32_t usage, void** vaddr);
int GraphicBufferUnlock(GraphicBuffer* object);
ANativeWindowBuffer* GraphicBufferGetNativeBuffer(GraphicBuffer* object);

/* running on glibc space */
GraphicBuffer* ANativeBufferCtor(uint32_t width, uint32_t height, uint32_t format, uint32_t usage);
void ANativeBufferDtor(GraphicBuffer* object);
int ANativeBufferReallocate(GraphicBuffer* object, uint32_t width, uint32_t height, uint32_t format, uint32_t usage);
int ANativeBufferLock(GraphicBuffer* object, uint32_t usage, void** vaddr);
int ANativeBufferUnlock(GraphicBuffer* object);
ANativeWindowBuffer* ANativeBufferGetNativeBuffer(GraphicBuffer* object);

#endif /*_GRAPHICBUFFER_H_*/
