
#define _GNU_SOURCE
#include <features.h>
#include <assert.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>

#include "Server.h"
#include "Client.h"
#include "android_dlfcn.h"

/* android include files */
#include <cutils/properties.h>

//#include "android_pthread.h"
extern void android_pthread_initialize();
#include "android_hardware.h"
#include <hardware/gralloc.h>
extern int gralloc_open_r(const struct hw_module_t* module, 
        struct alloc_device_t** device);
extern int gralloc_close_r(struct alloc_device_t* device);
extern int gralloc_lock(struct gralloc_module_t const* module,
            buffer_handle_t handle, int usage,
            int l, int t, int w, int h,
            void** vaddr);
extern int gralloc_unlock(struct gralloc_module_t const* module,
            buffer_handle_t handle);
extern int gralloc_alloc(struct alloc_device_t* dev,
            int w, int h, int format, int usage,
            buffer_handle_t* handle, int* stride);
extern int gralloc_free(struct alloc_device_t* dev,
            buffer_handle_t handle);
#include <hardware/hardware.h>
#include <system/graphics.h>
#include <system/window.h>

#include <math.h>
#include <EGL/egl.h>
#define EGL_EGLEXT_PROTOTYPES
#include <EGL/eglext.h>
#include <GLES2/gl2.h>
#if ! defined(__gl2_h_)
#define __gl2_h_
#endif
#define GL_GLEXT_PROTOTYPES
#include <GLES2/gl2ext.h>

#include "GraphicBuffer.h"
#include "OffscreenNativeWindow.h"

#if defined(__LP64__)
#  define LIBM_SO	"/system/lib64/libm.so"
#else
#  define LIBM_SO	"/system/lib/libm.so"
#endif

extern void pipe_init(struct io_port* cport, struct io_port* sport);

#define CHECK_GL_ERROR(s)				\
do {							\
    int err = glGetError();				\
    if (err) {						\
        fprintf (stderr, "GLES(%s): GL_ERROR(%d)\n", #s, err);	\
    }							\
} while (0)

const char vertex_src [] =
"                                        \
   attribute vec4        position;       \
   varying mediump vec2  pos;            \
   uniform vec4          offset;         \
                                         \
   void main()                           \
   {                                     \
      gl_Position = position + offset;   \
      pos = position.xy;                 \
   }                                     \
";


const char fragment_src [] =
"                                                      \
   varying mediump vec2    pos;                        \
   uniform mediump float   phase;                      \
                                                       \
   void  main()                                        \
   {                                                   \
      gl_FragColor  =  vec4( 1., 0.9, 0.7, 1.0 ) *     \
        cos( 30.*sqrt(pos.x*pos.x + 1.5*pos.y*pos.y)   \
             + atan(pos.y,pos.x) - phase );            \
   }                                                   \
";

GLuint load_shader(const char *shader_source, GLenum type)
{
	GLuint  shader = glCreateShader(type);

	glShaderSource(shader, 1, &shader_source, NULL);
	glCompileShader(shader);

	return shader;
}

GLfloat norm_x    =  0.0;
GLfloat norm_y    =  0.0;
GLfloat offset_x  =  0.0;
GLfloat offset_y  =  0.0;
GLfloat p1_pos_x  =  0.0;
GLfloat p1_pos_y  =  0.0;

GLint phase_loc;
GLint offset_loc;
GLint position_loc;

const float vertexArray[] = {
   0.0,  0.5,  0.0,
  -0.5,  0.0,  0.0,
   0.0, -0.5,  0.0,
   0.5,  0.0,  0.0,
   0.0,  0.5,  0.0 
};

int main (int argc, const char *argv[])
{
    (void)argc; (void)argv;

    struct io_port cport[2];
    struct io_port sport[2];

    pipe_init(&cport[0], &sport[0]);
    pipe_init(&cport[1], &sport[1]);

    char *args[8] = {0};
    char str[32];
    args[0] = (char*)5;
    args[1] = strdup("./dlhook");
    /*  snprintf would be safer, but the above check protects us. */
    memset (str, 0, sizeof(str));
    sprintf (str, "%d", sport[0].rfd);
    args[2] = strdup(str);
    memset (str, 0, sizeof(str));
    sprintf (str, "%d", sport[0].wfd);
    args[3] = strdup(str);
    memset (str, 0, sizeof(str));
    sprintf (str, "%d", sport[1].rfd);
    args[4] = strdup(str);
    memset (str, 0, sizeof(str));
    sprintf (str, "%d", sport[1].wfd);
    args[5] = strdup(str);

    pthread_t server_thread;
    if(pthread_create(&server_thread, NULL, start_server, (void*)args)) {
        fprintf(stderr, "Error creating thread\n");
        return 1;
    }

    usleep (20 * 1000);

    MessageQueue *dlhook_queue;
    MessageQueue *call_queue;

    Client_initialize (&cport[0], &cport[1]);
    dlhook_queue = get_dlhook_queue();
    call_queue = get_call_queue();

    /* test case 1 */
    dlhook_initialize ();
    
    void *handle;
    double (*cosine)(double);
    const char *error;

    handle = android_dlopen(LIBM_SO, RTLD_NOW | RTLD_LOCAL);
    if (handle == NULL)
        handle = android_dlopen("libm.so.6", RTLD_NOW | RTLD_LOCAL);
    if (handle) {

        android_dlerror();    /* Clear any existing error */

        cosine = (double (*)(double)) android_dlsym(handle, "cos");

        error = android_dlerror();
        if (error != NULL) {
            fprintf(stderr, "%s\n", error);
            exit(1);
        }

        printf("consine(2.0) = %f\n", (*cosine)(2.0));

        android_dlclose(handle);
    } else {
        fprintf(stderr, "%s\n", android_dlerror());
    }

    /* test case 2 */
    extern void hybris_cutils_initialize();
    extern int hybris_cutils_check_for_symbol(const char *sym);

    hybris_cutils_initialize();
    if (hybris_cutils_check_for_symbol("property_get")) {
        char propValue[64];
	int version = 0;
	int n;

	n = property_get("ro.hardware", propValue, "0");
	printf ("Android Hardware = %s\n", propValue);

	n = property_get("ro.build.version.release", propValue, "0");
	printf ("Android Version = %s\n", propValue);

	n = property_get("ro.build.version.sdk", propValue, "0");
	version = atoi(propValue);
	printf ("Android SDK Version = %d\n", version);

	n = property_get("persist.sys.dalvik.vm.lib.2", propValue, "0");
	printf ("Android JVM lib: %s\n", propValue);

	n = property_get("ro.opengles.version", propValue, "0");
	version = atoi(propValue);
	printf ("OpenGLES Version = %08x\n", version);
    }

    /* test case 3 */
    android_pthread_initialize();
    android_hardware_initialize();

    extern int hybris_hardware_check_for_symbol(const char *sym);

    if (hybris_hardware_check_for_symbol("hw_get_module")) {
	int ret;
	alloc_device_t* mAlloc = NULL;
	gralloc_module_t* mGralloc = NULL;

	ret = hw_get_module_r(GRALLOC_HARDWARE_MODULE_ID, (const hw_module_t**)&mGralloc);
	printf ("ret = %d, gralloc = %p\n", ret, mGralloc);
	ret = gralloc_open_r((hw_module_t*)mGralloc, &mAlloc);
	printf ("ret = %d, alloc = %p\n", ret, mAlloc);

	buffer_handle_t handle;
	int width = 800;
	int height= 480;
	int stride;
	int format= HAL_PIXEL_FORMAT_RGBA_8888;
	int usage = GRALLOC_USAGE_HW_TEXTURE | GRALLOC_USAGE_HW_RENDER | GRALLOC_USAGE_SW_WRITE_OFTEN | GRALLOC_USAGE_SW_READ_OFTEN;

	ret = gralloc_alloc(mAlloc, width, height, format, usage, &handle, &stride);
	if (ret == 0) {
		printf ("alloc buffer(%dx%d) successful, format = %d, stride = %d.\n", width, height, format, stride);
	} else {
		printf ("ret = %d, alloc buffer(%dx%d) fail!\n", ret, width, height);
		exit (1);
	}

	void* vaddr;
	usage = GRALLOC_USAGE_SW_WRITE_OFTEN | GRALLOC_USAGE_SW_READ_OFTEN;
	ret = gralloc_lock(mGralloc, handle, usage, 0, 0, width, height, &vaddr);
	if (ret == 0) {
		printf ("lock buffer(%dx%d) successful, address = %p\n", width, height, vaddr);
	} else {
		printf ("ret = %d, lock buffer(%dx%d) fail!\n", ret, width, height);
		exit (1);
	}

	printf ("touch every byte of buffer.\n");
	memset (vaddr, 0, width * height * 4);

	ret = gralloc_unlock(mGralloc, handle);
	if (ret == 0) {
		printf ("unlock buffer(%dx%d) successful.\n", width, height);
	} else {
		printf ("ret = %d, unlock buffer(%dx%d) fail!\n", ret, width, height);
		exit (1);
	}

	ret = gralloc_free(mAlloc, handle);
	if (ret == 0) {
		printf ("free buffer(%dx%d) successful.\n", width, height);
	} else {
		printf ("ret = %d, free buffer(%dx%d) fail!\n", ret, width, height);
		/* FIXME: android 4.4.4 or Sony L39H bug? */
		/* random fail ? */
#if 0
		exit (1);
#endif
	}

	ret = gralloc_close_r(mAlloc);
	printf ("ret = %d, gralloc close\n", ret);
    }

    /* test case 4 */
    if (hybris_hardware_check_for_symbol("hw_get_module")) {
	int w, h, format;

#if 1
	w = 800;
	h = 480;
#else
	w = 64;
	h = 16;
#endif
	format = HAL_PIXEL_FORMAT_RGBA_8888;

	printf ("Allocate a native buffer.\n");
        GraphicBuffer* gbuffer;
	uint32_t usage = GRALLOC_USAGE_HW_TEXTURE | GRALLOC_USAGE_HW_RENDER | GRALLOC_USAGE_SW_WRITE_OFTEN | GRALLOC_USAGE_SW_READ_OFTEN;

	gbuffer = ANativeBufferCtor(w, h, format, usage);

	// Write bitmap data into 'pixels' here
	void *pixels;
	uint32_t *pixel;
	int i, j;
	printf ("CPU write to native buffer: ");
	ANativeBufferLock(gbuffer, GRALLOC_USAGE_SW_WRITE_OFTEN | GRALLOC_USAGE_SW_READ_OFTEN, &pixels);
	printf ("%p\n", pixels);
	pixel = (uint32_t *)pixels;
	for (j = 0; j < h; j++) {
	    for (i = 0; i < w; i++) {
		*pixel = 0xFFFFFFFF;
		pixel++;
	    }
	}
	ANativeBufferUnlock(gbuffer);

	printf ("Reallocate the native buffer.\n");
#if 1
	w = 1024;
	h = 600;
#else
	w = 128;
	h = 4;
#endif
	format = HAL_PIXEL_FORMAT_BGRA_8888;
	ANativeBufferReallocate(gbuffer, w, h, format, usage);

	printf ("CPU write to native buffer: ");
	pixel = NULL;
	ANativeBufferLock(gbuffer, GRALLOC_USAGE_SW_WRITE_OFTEN | GRALLOC_USAGE_SW_READ_OFTEN, &pixels);
	printf ("%p\n", pixels);
	pixel = (uint32_t *)pixels;
	for (j = 0; j < h; j++) {
	    for (i = 0; i < w; i++) {
		*pixel = 0xFFFFFFFF;
		pixel++;
	    }
	}
	ANativeBufferUnlock(gbuffer);

	printf ("Destroy the native buffer.\n");
	ANativeBufferDtor(gbuffer);

    }

    /* test case 5 */
    if (hybris_hardware_check_for_symbol("hw_get_module")) {
        // initialize OpenGL ES and EGL

	EGLDisplay display;
        /*
         * Here specify the attributes of the desired configuration.
         * Below, we select an EGLConfig with at least 8 bits per color
         * component compatible with on-screen windows
         */
        EGLint attributes[] = {
            EGL_RED_SIZE,		8,
            EGL_GREEN_SIZE,		8,
            EGL_BLUE_SIZE,		8,
            EGL_ALPHA_SIZE,		8,
#if 0
            EGL_BUFFER_SIZE,		32,
            EGL_DEPTH_SIZE,		24,
            EGL_STENCIL_SIZE,		8,
#endif
            EGL_SURFACE_TYPE,		EGL_WINDOW_BIT,
#ifdef __gl2_h_
    	    EGL_RENDERABLE_TYPE,	EGL_OPENGL_ES2_BIT,
#endif
            EGL_NONE
        };
#ifdef __gl2_h_
        EGLint ctx_attribs[] = {
    	    EGL_CONTEXT_CLIENT_VERSION, 2,
    	    EGL_NONE
        };
#endif
        EGLint w, h, format;
        EGLint numConfigs = 0;
        EGLConfig config = 0;
        EGLSurface surface = EGL_NO_SURFACE;
        EGLContext context = EGL_NO_CONTEXT;
        int major, minor;
	int ret;

	w = 800;
	h = 480;
	format = HAL_PIXEL_FORMAT_RGBA_8888;

        display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

        eglInitialize(display, &major, &minor);

        eglBindAPI(EGL_OPENGL_ES_API);

        /* Here, the application chooses the configuration it desires. In this
         * sample, we have a very simplified selection process, where we pick
         * the first EGLConfig that matches our criteria */
        ret = eglChooseConfig(display, attributes, &config, 1, &numConfigs);
	if (!ret || numConfigs <= 0) {
            fprintf (stderr, "Unable to eglChooseConfig: 0x%04x\n", eglGetError());
	    return -1;
	} else {
	    printf ("Choose config = %p\n", config);
	}

	eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format);
	printf ("get native format = %d\n", format);

        int pbuffer_attributes[] = {
            EGL_WIDTH,  w,
            EGL_HEIGHT, h,
            EGL_NONE
        };
      
        surface = eglCreatePbufferSurface(display, config, pbuffer_attributes);
        if (surface == EGL_NO_SURFACE) {
            fprintf (stderr, "Unable to eglCreatePbufferSurface");
    	    return -1;
        } else {
    	    printf("eglCreatePbufferSurface successful.\n");
        }

        context = eglCreateContext(display, config, EGL_NO_CONTEXT, ctx_attribs);
        if (context == EGL_NO_CONTEXT) {
    	    fprintf (stderr, "Unable create context: 0x%04x\n", eglGetError());
    	    return -1;
        }

        if (eglMakeCurrent(display, surface, surface, context) == EGL_FALSE) {
            fprintf (stderr, "Unable to eglMakeCurrent: 0x%04x\n", eglGetError());
            return -1;
        }

        eglQuerySurface(display, surface, EGL_WIDTH,  &w);
        eglQuerySurface(display, surface, EGL_HEIGHT, &h);
        printf ("surface: %d x %d\n", w, h);

        // Initialize GL state.
        glEnable(GL_CULL_FACE);
        glDisable(GL_DEPTH_TEST);

        printf ("\n");
        printf ("EGL informations:\n");
        printf ("EGL version %d.%d\n", major, minor);
        printf ("EGL vendor: %s\n", eglQueryString(display, EGL_VENDOR));
        printf ("EGL version: %s\n", eglQueryString(display, EGL_VERSION));
        printf ("EGL extensions: %s\n", eglQueryString(display, EGL_EXTENSIONS));
        printf ("EGL client apis: %s\n", eglQueryString(display, EGL_CLIENT_APIS));
        printf ("\n");
        printf ("OpenGLES informations:\n");
        printf ("OpenGLES Vendor: %s\n", glGetString(GL_VENDOR));
        printf ("OpenGLES Renderer: %s\n", glGetString(GL_RENDERER));
        printf ("OpenGLES Version: %s\n", glGetString(GL_VERSION));
        printf ("OpenGLES Extensions: %s\n", glGetString(GL_EXTENSIONS));
        printf ("\n");

	printf ("Allocate a native buffer.\n");
        GraphicBuffer* gbuffer;
	uint32_t usage = GRALLOC_USAGE_HW_TEXTURE | GRALLOC_USAGE_HW_RENDER | GRALLOC_USAGE_SW_WRITE_OFTEN | GRALLOC_USAGE_SW_READ_OFTEN;

	gbuffer = ANativeBufferCtor(w, h, format, usage);

	// Write bitmap data into 'pixels' here
	void *pixels;
	uint32_t *pixel;
	int i, j;
	printf ("CPU write to native buffer: ");
	ANativeBufferLock(gbuffer, GRALLOC_USAGE_SW_WRITE_OFTEN | GRALLOC_USAGE_SW_READ_OFTEN, &pixels);
	printf ("%p\n", pixels);
	pixel = (uint32_t *)pixels;
	for (j = 0; j < h; j++) {
	    for (i = 0; i < w; i++) {
		*pixel = 0xFFFFFFFF;
		pixel++;
	    }
	}
	ANativeBufferUnlock(gbuffer);
	
	ANativeWindowBuffer* nwb;
	nwb = GraphicBufferGetNativeBuffer(gbuffer);
	printf ("Increasing the buffer's reference count. Release the buffer by myself.\n");
	nwb->common.incRef(&nwb->common);

	// Create the EGLImageKHR from the native buffer
	EGLint eglImgAttrs[] = { EGL_IMAGE_PRESERVED_KHR, EGL_TRUE, EGL_NONE, EGL_NONE };
	EGLImageKHR image = eglCreateImageKHR(display, EGL_NO_CONTEXT, EGL_NATIVE_BUFFER_ANDROID, (EGLClientBuffer)nwb, eglImgAttrs);
	if (image != EGL_NO_IMAGE_KHR) {
	    printf ("create EGLImageKHR image successful.\n");
	} else {
	    fprintf (stderr, "create EGLImageKHR image fail!\n");
	    return -1;
	}

#if 1
	printf ("test EGLImageKHR image.\n");
	GLuint texture;
        GLuint framebuffer;

	/* Set up the TEX & FBO with the EGLImage as target */
	int tex, fbo;

        glGetIntegerv( GL_TEXTURE_BINDING_2D, &tex );
        glGetIntegerv( GL_FRAMEBUFFER_BINDING, &fbo );

        /* Initialize FBO texture. */	
        glGenTextures( 1, &texture );
	CHECK_GL_ERROR(glGenTextures);
        glBindTexture( GL_TEXTURE_2D, texture );
	CHECK_GL_ERROR(glBindTexture);
        /* Set filtering. */	
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	if (format == HAL_PIXEL_FORMAT_RGBA_8888 || format == HAL_PIXEL_FORMAT_RGBX_8888) {
	    printf ("native format = %d -> GL_RGBA\n", format);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	} else if (format == HAL_PIXEL_FORMAT_BGRA_8888) {
	    printf ("native format = %d -> GL_BGRA\n", format);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_BGRA_EXT, w, h, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, NULL);
	}
	CHECK_GL_ERROR(glTexImage2D);
        glEGLImageTargetTexture2DOES( GL_TEXTURE_2D, image );
	CHECK_GL_ERROR(glEGLImageTargetTexture2DOES);

        /*
         * Framebuffer
         */
	/* Initialize FBOs. */
        glGenFramebuffers( 1, &framebuffer );
	CHECK_GL_ERROR(glGenFramebuffers);
        /* Render to framebuffer object. */
        /* Bind our framebuffer for rendering. */	
        glBindFramebuffer( GL_FRAMEBUFFER, framebuffer );
	CHECK_GL_ERROR(glBindFramebuffer);

        /* Attach texture to the framebuffer. */
        printf ("Attach texture to the framebuffer.\n");
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
	CHECK_GL_ERROR(glFramebufferTexture2D);

        /* Check FBO is OK. */	
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            fprintf (stderr, "Framebuffer not complete\n");
	    return -1;
        } else {
            printf ("Framebuffer completed\n");
	}

        /* Render to framebuffer object. */
	printf ("GPU write to native buffer.\n");
        // Just fill the screen with a color.
        glClearColor(0.754f, 0.50f, 0.25f, 1.0f);
	CHECK_GL_ERROR(glClearColor);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	CHECK_GL_ERROR(glClear);

	while (!eglWaitGL() || !eglSwapBuffers(display, surface)) {};

	/* And unbind the FrameBuffer Object so subsequent drawing calls are to the EGL window surface. */
        glBindTexture( GL_TEXTURE_2D, tex );
	/* Unbind framebuffer. */
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	glDeleteFramebuffers(1, &framebuffer);
	CHECK_GL_ERROR(glDeleteFramebuffers);
	glDeleteTextures(1, &texture);
	CHECK_GL_ERROR(glDeleteTextures);

	/* dangerous */
	/* pixel = 0xff4080c0 */
	printf ("peek a pixel = 0x%08x\n", *((uint32_t *)pixels));
#endif

	printf ("Destroy EGLImageKHR image.\n");
	eglDestroyImageKHR (display, image);

	printf ("CPU write to native buffer: ");
	ANativeBufferLock(gbuffer, GRALLOC_USAGE_SW_WRITE_OFTEN | GRALLOC_USAGE_SW_READ_OFTEN, &pixels);
	printf ("%p\n", pixels);
	pixel = (uint32_t *)pixels;
	for (j = 0; j < h; j++) {
	    for (i = 0; i < w; i++) {
		*pixel = 0xFFFFFFFF;
		pixel++;
	    }
	}
	ANativeBufferUnlock(gbuffer);

	printf ("Destroy EGL resource.\n");
        if (display != EGL_NO_DISPLAY) {
            eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
            if (context != EGL_NO_CONTEXT) {
                eglDestroyContext(display, context);
            }
            if (surface != EGL_NO_SURFACE) {
                eglDestroySurface(display, surface);
            }
            eglTerminate(display);
	}

	printf ("Destroy the native buffer by myself.\n");
	//ANativeBufferDtor(gbuffer);
	nwb->common.decRef(&nwb->common);

	/*************************************************************/
	printf ("******************************\n");
	printf ("Test Offscreen Native Window.\n");
        display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

        eglInitialize(display, &major, &minor);

        eglBindAPI(EGL_OPENGL_ES_API);

        /* Here, the application chooses the configuration it desires. In this
         * sample, we have a very simplified selection process, where we pick
         * the first EGLConfig that matches our criteria */
        eglChooseConfig(display, attributes, &config, 1, &numConfigs);

	eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format);

	ANativeWindow* window = (ANativeWindow* )OffscreenNativeWindowCtor(w, h, format, usage);
	/* FIXME: android space function. */
	printf ("Increasing the window's reference count. Release the window by myself.\n");
	window->common.incRef(&window->common);

        surface = eglCreateWindowSurface(display, config, (EGLNativeWindowType)window, NULL);
        if (surface == EGL_NO_SURFACE) {
	    fprintf (stderr, "Unable to eglCreateWindowSurface.\n");
    	    return -1;
        } else {
    	    printf("eglCreateWindowSurface successful.\n");
        }

        context = eglCreateContext(display, config, EGL_NO_CONTEXT, ctx_attribs);
        if (context == EGL_NO_CONTEXT) {
    	    fprintf (stderr, "Unable create context: 0x%04x\n", eglGetError());
    	    return -1;
        }

        if (eglMakeCurrent(display, surface, surface, context) == EGL_FALSE) {
            fprintf (stderr, "Unable to eglMakeCurrent: 0x%04x\n", eglGetError());
            return -1;
        }

	/* FIXME: android space function. */
#if 0
	printf ("buffer count: 3\n");
	window->perform(window, NATIVE_WINDOW_SET_BUFFER_COUNT, 3);
#endif
        eglQuerySurface(display, surface, EGL_WIDTH,  &w);
        eglQuerySurface(display, surface, EGL_HEIGHT, &h);
        printf ("surface: %d x %d\n", w, h);

        // Initialize GL state.
        glEnable(GL_CULL_FACE);
        glDisable(GL_DEPTH_TEST);
	eglSwapBuffers(display, surface);

#if 0
        printf ("\n");
        printf ("EGL informations:\n");
        printf ("EGL version %d.%d\n", major, minor);
        printf ("EGL vendor: %s\n", eglQueryString(display, EGL_VENDOR));
        printf ("EGL version: %s\n", eglQueryString(display, EGL_VERSION));
        printf ("EGL extensions: %s\n", eglQueryString(display, EGL_EXTENSIONS));
        printf ("EGL client apis: %s\n", eglQueryString(display, EGL_CLIENT_APIS));
        printf ("\n");
        printf ("OpenGLES informations:\n");
        printf ("OpenGLES Vendor: %s\n", glGetString(GL_VENDOR));
        printf ("OpenGLES Renderer: %s\n", glGetString(GL_RENDERER));
        printf ("OpenGLES Version: %s\n", glGetString(GL_VERSION));
        printf ("OpenGLES Extensions: %s\n", glGetString(GL_EXTENSIONS));
        printf ("\n");
#endif

	printf ("Resize Native Window.\n");
	OffscreenNativeWindowResize((struct OffscreenNativeWindow *)window, 1024, 600, format);
        eglQuerySurface(display, surface, EGL_WIDTH,  &w);
        eglQuerySurface(display, surface, EGL_HEIGHT, &h);
        printf ("surface old size: %d x %d\n", w, h);

	printf ("Create shader program.\n");
	GLuint vertexShader   = load_shader ( vertex_src , GL_VERTEX_SHADER  );     // load vertex shader
	GLuint fragmentShader = load_shader ( fragment_src , GL_FRAGMENT_SHADER );  // load fragment shader

	GLuint shaderProgram  = glCreateProgram ();                 // create program object
	glAttachShader ( shaderProgram, vertexShader );             // and attach both...
	glAttachShader ( shaderProgram, fragmentShader );           // ... shaders to it

	glLinkProgram ( shaderProgram );    // link the program
	glUseProgram  ( shaderProgram );    // and select it for usage

	//// now get the locations (kind of handle) of the shaders variables
	position_loc  = glGetAttribLocation  ( shaderProgram , "position" );
	phase_loc     = glGetUniformLocation ( shaderProgram , "phase"    );
	offset_loc    = glGetUniformLocation ( shaderProgram , "offset"   );
	if ( position_loc < 0  ||  phase_loc < 0  ||  offset_loc < 0 ) {
		return 1;
	}

	printf ("Draw surface.\n");
	float r, g, b, a;
	for (i = 0; i < 128; i++) {
		r = i * 0.01f;
		g = i * 0.01f;
		b = i * 0.01f;
		a = i * 0.01f;
		// Just fill the screen with a color.
		glClearColor(r, g, b, a);
		glClear(GL_COLOR_BUFFER_BIT);

		eglSwapBuffers(display, surface);
	}
	/* FIXME: android space function. */
#if 0
	printf ("buffer count: 4\n");
	window->perform(window, NATIVE_WINDOW_SET_BUFFER_COUNT, 4);
#endif
	glViewport ( 0 , 0 , w, h);
	glClearColor ( 0.08 , 0.06 , 0.07 , 1.);    // background color
	float phase = 0;
	int n;
	for (n = 0; n < 128; n++) {
		//printf ("draw surface: count = %03d\n", n);
		glUniform1f ( phase_loc , phase );  // write the value of phase to the shaders phase
		phase  =  fmodf ( phase + 0.5f , 2.f * 3.141f );    // and update the local variable

		glUniform4f ( offset_loc  ,  offset_x , offset_y , 0.0 , 0.0 );

		glVertexAttribPointer ( position_loc, 3, GL_FLOAT, GL_FALSE, 0, vertexArray );
		glEnableVertexAttribArray ( position_loc );
		glDrawArrays ( GL_TRIANGLE_STRIP, 0, 5 );

		eglSwapBuffers ( (EGLDisplay) display, surface );  // get the rendered buffer to the screen
	}

        eglQuerySurface(display, surface, EGL_WIDTH,  &w);
        eglQuerySurface(display, surface, EGL_HEIGHT, &h);
        printf ("surface new size: %d x %d\n", w, h);

	printf ("Destroy EGL resource.\n");
        if (display != EGL_NO_DISPLAY) {
            eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
            if (context != EGL_NO_CONTEXT) {
                eglDestroyContext(display, context);
            }
            if (surface != EGL_NO_SURFACE) {
                eglDestroySurface(display, surface);
            }
            eglTerminate(display);
        }

	/* FIXME: android space function. */
	printf ("Destroy Native Window resource by myself.\n");
	//OffscreenNativeWindowDtor((OffscreenNativeWindow* )window);
	window->common.decRef(&window->common);

    }
   
    /* test case 6 */

    /* end of test */
    MessageQueue_dtor(dlhook_queue);
    MessageQueue_dtor(call_queue);

    printf ("end of test\n");

    return 0;
}
