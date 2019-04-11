#ifndef _ANDROID_DLFCN_H_
#define _ANDROID_DLFCN_H_

#ifdef __cplusplus
extern "C" {
#endif

extern void *android_dlopen (const char *filename, int flags);
extern int   android_dlclose(void *handle);
extern char *android_dlerror(void);
extern void *android_dlsym  (void *handle, const char *symbol);

#ifdef __cplusplus
}
#endif

#endif
