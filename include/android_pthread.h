
#ifndef _ANDROID_PTHREAD_H_
#define _ANDROID_PTHREAD_H_

#if ! defined(__ANDROID__)

typedef long pthread_t;

typedef struct {
  uint32_t flags;
  void* stack_base;
  size_t stack_size;
  size_t guard_size;
  int32_t sched_policy;
  int32_t sched_priority;
#ifdef __LP64__
  char __reserved[16];
#endif
} pthread_attr_t;

typedef struct {
#if defined(__LP64__)
  int32_t __private[10];
#else
  int32_t __private[1];
#endif
} pthread_mutex_t;

typedef long pthread_mutexattr_t;

enum {
    PTHREAD_MUTEX_NORMAL = 0,
    PTHREAD_MUTEX_RECURSIVE = 1,
    PTHREAD_MUTEX_ERRORCHECK = 2,

    PTHREAD_MUTEX_ERRORCHECK_NP = PTHREAD_MUTEX_ERRORCHECK,
    PTHREAD_MUTEX_RECURSIVE_NP  = PTHREAD_MUTEX_RECURSIVE,

    PTHREAD_MUTEX_DEFAULT = PTHREAD_MUTEX_NORMAL
};

#define PTHREAD_MUTEX_INITIALIZER { { ((PTHREAD_MUTEX_NORMAL & 3) << 14) } }
#define PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP { { ((PTHREAD_MUTEX_RECURSIVE & 3) << 14) } }
#define PTHREAD_ERRORCHECK_MUTEX_INITIALIZER_NP { { ((PTHREAD_MUTEX_ERRORCHECK & 3) << 14) } }

typedef struct {
#if defined(__LP64__)
  int32_t __private[12];
#else
  int32_t __private[1];
#endif
} pthread_cond_t;

typedef long pthread_condattr_t;

#define PTHREAD_COND_INITIALIZER  { { 0 } }

#endif

/*
 * Attribute for softfp-calling-style functions
 */
#ifdef __ARM_PCS_VFP
#    define SOFTFP __attribute__((pcs("aapcs")))
#else
#    define SOFTFP
#endif

__BEGIN_DECLS

extern int  (*__dl_pthread_cond_broadcast) (pthread_cond_t*) SOFTFP;
extern int  (*__dl_pthread_cond_init)      (pthread_cond_t*, const pthread_condattr_t*) SOFTFP;
extern int  (*__dl_pthread_cond_signal)    (pthread_cond_t*) SOFTFP;
extern int  (*__dl_pthread_cond_wait)      (pthread_cond_t*, pthread_mutex_t*) SOFTFP;

extern int  (*__dl_pthread_create) (pthread_t*, pthread_attr_t const*, void *(*)(void*), void*) SOFTFP;
extern int  (*__dl_pthread_detach) (pthread_t) SOFTFP;
extern void (*__dl_pthread_exit)   (void*) SOFTFP;
extern int  (*__dl_pthread_join)   (pthread_t, void**) SOFTFP;

extern int  (*__dl_pthread_mutex_init)   (pthread_mutex_t*, const pthread_mutexattr_t*) SOFTFP;
extern int  (*__dl_pthread_mutex_lock)   (pthread_mutex_t*) SOFTFP;
extern int  (*__dl_pthread_mutex_unlock) (pthread_mutex_t*) SOFTFP;

extern pthread_t (*__dl_pthread_self) (void) SOFTFP;

extern void android_pthread_initialize();

extern void *(*__dl_malloc)(size_t size) SOFTFP;
extern void  (*__dl_free)(void *ptr) SOFTFP;
extern void *(*__dl_calloc)(size_t nmemb, size_t size) SOFTFP;
extern void *(*__dl_realloc)(void *ptr, size_t size) SOFTFP;

__END_DECLS

#endif /* _ANDROID_PTHREAD_H_ */
