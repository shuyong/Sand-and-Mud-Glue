
#include <stdio.h>
#include <dlfcn.h>

#include "Client.h"
#include "android_pthread.h"

#include <hybris/common/binding.h>

#define PTHREAD_DLSYSM(sym) HYBRIS_DLSYSM(pthread, &__dl_##sym, #sym)

int  (*__dl_pthread_cond_broadcast) (pthread_cond_t*) SOFTFP = NULL;
int  (*__dl_pthread_cond_init)      (pthread_cond_t*, const pthread_condattr_t*) SOFTFP = NULL;
int  (*__dl_pthread_cond_signal)    (pthread_cond_t*) SOFTFP = NULL;
int  (*__dl_pthread_cond_wait)      (pthread_cond_t*, pthread_mutex_t*) SOFTFP = NULL;

int  (*__dl_pthread_create) (pthread_t*, pthread_attr_t const*, void *(*)(void*), void*) SOFTFP = NULL;
int  (*__dl_pthread_detach) (pthread_t) SOFTFP = NULL;
void (*__dl_pthread_exit)   (void*) SOFTFP = NULL;
int  (*__dl_pthread_join)   (pthread_t, void**) SOFTFP = NULL;

int  (*__dl_pthread_mutex_init)   (pthread_mutex_t*, const pthread_mutexattr_t*) SOFTFP = NULL;
int  (*__dl_pthread_mutex_lock)   (pthread_mutex_t*) SOFTFP = NULL;
int  (*__dl_pthread_mutex_unlock) (pthread_mutex_t*) SOFTFP = NULL;

pthread_t (*__dl_pthread_self) (void) SOFTFP = NULL;

#if 0
#if defined (__ANDROID__)
HYBRIS_LIBRARY_INITIALIZE(pthread, "libc.so")
#else
HYBRIS_LIBRARY_INITIALIZE(pthread, "libpthread.so")
#endif
#else
void *pthread_handle;
void hybris_pthread_initialize()
{
    pthread_handle = android_dlopen("libpthread.so", RTLD_LAZY);
    /* android hasn't libpthread.so */
    if (pthread_handle == NULL)
        pthread_handle = android_dlopen("libc.so", RTLD_LAZY);
}
#endif

#define C_DLSYSM(sym) HYBRIS_DLSYSM(c, &__dl_##sym, #sym)

HYBRIS_LIBRARY_INITIALIZE(c, "libc.so")

void *(*__dl_malloc)(size_t size) SOFTFP = NULL;
void  (*__dl_free)(void *ptr) SOFTFP = NULL;
void *(*__dl_calloc)(size_t nmemb, size_t size) SOFTFP = NULL;
void *(*__dl_realloc)(void *ptr, size_t size) SOFTFP = NULL;

void android_pthread_initialize()
{
    PTHREAD_DLSYSM(pthread_cond_broadcast);
    PTHREAD_DLSYSM(pthread_cond_init);
    PTHREAD_DLSYSM(pthread_cond_signal);
    PTHREAD_DLSYSM(pthread_cond_wait);

    PTHREAD_DLSYSM(pthread_create);
    PTHREAD_DLSYSM(pthread_detach);
    PTHREAD_DLSYSM(pthread_exit);
    PTHREAD_DLSYSM(pthread_join);

    PTHREAD_DLSYSM(pthread_mutex_init);
    PTHREAD_DLSYSM(pthread_mutex_lock);
    PTHREAD_DLSYSM(pthread_mutex_unlock);

    PTHREAD_DLSYSM(pthread_self);

    C_DLSYSM(malloc);
    C_DLSYSM(free);
    C_DLSYSM(calloc);
    C_DLSYSM(realloc);
}
