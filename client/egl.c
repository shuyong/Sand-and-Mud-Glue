
#include <dlfcn.h>
#include <stddef.h>
#include <errno.h>
/* EGL function pointers */
#define EGL_EGLEXT_PROTOTYPES
#include <EGL/egl.h>
#include <EGL/eglext.h>

#include <hybris/common/binding.h>

#pragma GCC visibility push(hidden)
HYBRIS_LIBRARY_INITIALIZE(egl, "libEGL.so");
#if 1
HYBRIS_LIRBARY_CHECK_SYMBOL(egl);
#endif
#pragma GCC visibility pop

/* EGL Functions */
HYBRIS_IMPLEMENT_FUNCTION0(egl, EGLint, eglGetError);

HYBRIS_IMPLEMENT_FUNCTION1(egl, EGLDisplay, eglGetDisplay, EGLNativeDisplayType);

HYBRIS_IMPLEMENT_FUNCTION3(egl, EGLBoolean, eglInitialize, EGLDisplay, EGLint *, EGLint *);

HYBRIS_IMPLEMENT_FUNCTION1(egl, EGLBoolean, eglTerminate, EGLDisplay);

HYBRIS_IMPLEMENT_FUNCTION2(egl, const char *, eglQueryString, EGLDisplay, EGLint);

HYBRIS_IMPLEMENT_FUNCTION4(egl, EGLBoolean, eglGetConfigs, EGLDisplay, EGLConfig *,
			 EGLint, EGLint *);

HYBRIS_IMPLEMENT_FUNCTION5(egl, EGLBoolean, eglChooseConfig, EGLDisplay, const EGLint *,
			   EGLConfig *, EGLint,
			   EGLint *);

HYBRIS_IMPLEMENT_FUNCTION4(egl, EGLBoolean, eglGetConfigAttrib, EGLDisplay, EGLConfig,
			      EGLint, EGLint *);

HYBRIS_IMPLEMENT_FUNCTION4(egl, EGLSurface, eglCreateWindowSurface, EGLDisplay, EGLConfig,
				  EGLNativeWindowType,
				  const EGLint *);

HYBRIS_IMPLEMENT_FUNCTION3(egl, EGLSurface, eglCreatePbufferSurface, EGLDisplay, EGLConfig,
				   const EGLint *);

HYBRIS_IMPLEMENT_FUNCTION4(egl, EGLSurface, eglCreatePixmapSurface, EGLDisplay, EGLConfig,
				  EGLNativePixmapType,
				  const EGLint *);

HYBRIS_IMPLEMENT_FUNCTION2(egl, EGLBoolean, eglDestroySurface, EGLDisplay, EGLSurface);

HYBRIS_IMPLEMENT_FUNCTION4(egl, EGLBoolean, eglQuerySurface, EGLDisplay, EGLSurface,
			   EGLint, EGLint *);

HYBRIS_IMPLEMENT_FUNCTION1(egl, EGLBoolean, eglBindAPI, EGLenum);

HYBRIS_IMPLEMENT_FUNCTION0(egl, EGLenum, eglQueryAPI);

HYBRIS_IMPLEMENT_FUNCTION0(egl, EGLBoolean, eglWaitClient);

HYBRIS_IMPLEMENT_FUNCTION0(egl, EGLBoolean, eglReleaseThread);

HYBRIS_IMPLEMENT_FUNCTION5(egl, EGLSurface, eglCreatePbufferFromClientBuffer,
	      EGLDisplay, EGLenum, EGLClientBuffer,
	      EGLConfig, const EGLint *);

HYBRIS_IMPLEMENT_FUNCTION4(egl, EGLBoolean, eglSurfaceAttrib, EGLDisplay, EGLSurface,
			    EGLint, EGLint);

HYBRIS_IMPLEMENT_FUNCTION3(egl, EGLBoolean, eglBindTexImage, EGLDisplay, EGLSurface, EGLint);

HYBRIS_IMPLEMENT_FUNCTION3(egl, EGLBoolean, eglReleaseTexImage, EGLDisplay, EGLSurface, EGLint);

HYBRIS_IMPLEMENT_FUNCTION2(egl, EGLBoolean, eglSwapInterval, EGLDisplay, EGLint);

HYBRIS_IMPLEMENT_FUNCTION4(egl, EGLContext, eglCreateContext, EGLDisplay, EGLConfig,
			    EGLContext,
			    const EGLint *);

HYBRIS_IMPLEMENT_FUNCTION2(egl, EGLBoolean, eglDestroyContext, EGLDisplay, EGLContext);

HYBRIS_IMPLEMENT_FUNCTION4(egl, EGLBoolean, eglMakeCurrent, EGLDisplay, EGLSurface,
			  EGLSurface, EGLContext);

HYBRIS_IMPLEMENT_FUNCTION0(egl, EGLContext, eglGetCurrentContext);

HYBRIS_IMPLEMENT_FUNCTION1(egl, EGLSurface, eglGetCurrentSurface, EGLint);

HYBRIS_IMPLEMENT_FUNCTION0(egl, EGLDisplay, eglGetCurrentDisplay);

HYBRIS_IMPLEMENT_FUNCTION4(egl, EGLBoolean, eglQueryContext, EGLDisplay, EGLContext,
			   EGLint, EGLint *);

HYBRIS_IMPLEMENT_FUNCTION0(egl, EGLBoolean, eglWaitGL);

HYBRIS_IMPLEMENT_FUNCTION1(egl, EGLBoolean, eglWaitNative, EGLint);

HYBRIS_IMPLEMENT_FUNCTION2(egl, EGLBoolean, eglSwapBuffers, EGLDisplay, EGLSurface);

HYBRIS_IMPLEMENT_FUNCTION4(egl, EGLBoolean, eglSwapBuffersWithDamageEXT, EGLDisplay, EGLSurface, EGLint *, EGLint);

HYBRIS_IMPLEMENT_FUNCTION3(egl, EGLBoolean, eglCopyBuffers, EGLDisplay, EGLSurface,
			  EGLNativePixmapType);

HYBRIS_IMPLEMENT_FUNCTION1(egl, __eglMustCastToProperFunctionPointerType,
       eglGetProcAddress, const char *);

/* EGL Ext Functions */
#ifdef EGL_EGLEXT_PROTOTYPES
HYBRIS_IMPLEMENT_FUNCTION3(egl, EGLBoolean, eglLockSurfaceKHR, EGLDisplay, EGLSurface, const EGLint *);

HYBRIS_IMPLEMENT_FUNCTION2(egl, EGLBoolean, eglUnlockSurfaceKHR, EGLDisplay, EGLSurface);

#endif /* EGL_EGLEXT_PROTOTYPES */

#ifdef EGL_EGLEXT_PROTOTYPES
HYBRIS_IMPLEMENT_FUNCTION5(egl, EGLImageKHR, eglCreateImageKHR, EGLDisplay, EGLContext, EGLenum, EGLClientBuffer, const EGLint *);

HYBRIS_IMPLEMENT_FUNCTION2(egl, EGLBoolean, eglDestroyImageKHR, EGLDisplay, EGLImageKHR);

#endif /* EGL_EGLEXT_PROTOTYPES */

#ifdef EGL_EGLEXT_PROTOTYPES
HYBRIS_IMPLEMENT_FUNCTION3(egl, EGLSyncKHR, eglCreateSyncKHR, EGLDisplay, EGLenum, const EGLint *);

HYBRIS_IMPLEMENT_FUNCTION2(egl, EGLBoolean, eglDestroySyncKHR, EGLDisplay, EGLSyncKHR);

HYBRIS_IMPLEMENT_FUNCTION4(egl, EGLint, eglClientWaitSyncKHR, EGLDisplay, EGLSyncKHR, EGLint, EGLTimeKHR);

HYBRIS_IMPLEMENT_FUNCTION3(egl, EGLBoolean, eglSignalSyncKHR, EGLDisplay, EGLSyncKHR, EGLenum);

HYBRIS_IMPLEMENT_FUNCTION4(egl, EGLBoolean, eglGetSyncAttribKHR, EGLDisplay, EGLSyncKHR, EGLint, EGLint *);

#endif /* EGL_EGLEXT_PROTOTYPES */

#ifdef EGL_EGLEXT_PROTOTYPES
HYBRIS_IMPLEMENT_FUNCTION3(egl, EGLSyncNV, eglCreateFenceSyncNV, EGLDisplay, EGLenum, const EGLint *);

HYBRIS_IMPLEMENT_FUNCTION1(egl, EGLBoolean, eglDestroySyncNV, EGLSyncNV);

HYBRIS_IMPLEMENT_FUNCTION1(egl, EGLBoolean, eglFenceNV, EGLSyncNV);

HYBRIS_IMPLEMENT_FUNCTION3(egl, EGLint, eglClientWaitSyncNV, EGLSyncNV, EGLint, EGLTimeNV);

HYBRIS_IMPLEMENT_FUNCTION2(egl, EGLBoolean, eglSignalSyncNV, EGLSyncNV, EGLenum);

HYBRIS_IMPLEMENT_FUNCTION3(egl, EGLBoolean, eglGetSyncAttribNV, EGLSyncNV, EGLint, EGLint *);

#endif /* EGL_EGLEXT_PROTOTYPES */

#ifdef EGL_EGLEXT_PROTOTYPES
HYBRIS_IMPLEMENT_FUNCTION6(egl, EGLBoolean, eglSetSwapRectangleANDROID, EGLDisplay, EGLSurface, EGLint, EGLint, EGLint, EGLint);

#endif /* EGL_EGLEXT_PROTOTYPES */

#ifdef EGL_EGLEXT_PROTOTYPES
HYBRIS_IMPLEMENT_FUNCTION0(egl, EGLuint64NV, eglGetSystemTimeFrequencyNV);

HYBRIS_IMPLEMENT_FUNCTION0(egl, EGLuint64NV, eglGetSystemTimeNV);

#endif

