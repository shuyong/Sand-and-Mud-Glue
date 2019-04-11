#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "Message.h"
#include "Client.h"
#include "android_dlfcn.h"

/* argument 0 */
#define DLFCN_MESSAGE0(return_type, symbol) \
typedef struct Message_##symbol {           \
    Message common;                         \
    return_type result;                     \
} Message_##symbol;

#define DLFCN_HANDLER0(return_type, symbol)                             \
static void *handleMessage_##symbol (Message_##symbol* message) SOFTFP; \
static void *handleMessage_##symbol (Message_##symbol* message)         \
{                                                                       \
    return_type result;                                                 \
                                                                        \
    result = __dl_##symbol ();                                          \
                                                                        \
    message->result = result;                                           \
                                                                        \
    return 0;                                                           \
}

#define DLFCN_FUNCTION0(return_type, symbol)                                        \
return_type android_##symbol ()                                                     \
{                                                                                   \
    MessageHandler handler[1];                                                      \
    Message_##symbol message[1];                                                    \
                                                                                    \
    handler->handleMessage = (void *(*) (Message* ))handleMessage_##symbol;         \
                                                                                    \
    (void)MessageQueue_push_message(get_call_queue(), (Message* )message, handler); \
                                                                                    \
    return message->result;                                                         \
}

#define DLFCN_IMPLEMENT0(return_type, symbol) \
DLFCN_MESSAGE0 (return_type, symbol)          \
DLFCN_HANDLER0 (return_type, symbol)          \
DLFCN_FUNCTION0(return_type, symbol)

/* argument 1 */
#define DLFCN_MESSAGE1(return_type, symbol, a1) \
typedef struct Message_##symbol {               \
    Message common;                             \
    a1 n1;                                      \
    return_type result;                         \
} Message_##symbol;

#define DLFCN_HANDLER1(return_type, symbol, a1)                         \
static void *handleMessage_##symbol (Message_##symbol* message) SOFTFP; \
static void *handleMessage_##symbol (Message_##symbol* message)         \
{                                                                       \
    a1 n1 = message->n1;                                                \
    return_type result;                                                 \
                                                                        \
    result = __dl_##symbol (n1);                                        \
                                                                        \
    message->result = result;                                           \
                                                                        \
    return 0;                                                           \
}

#define DLFCN_FUNCTION1(return_type, symbol, a1)                                    \
return_type android_##symbol (a1 n1)                                                \
{                                                                                   \
    MessageHandler handler[1];                                                      \
    Message_##symbol message[1];                                                    \
                                                                                    \
    handler->handleMessage = (void *(*) (Message* ))handleMessage_##symbol;         \
    message->n1 = n1;                                                               \
                                                                                    \
    (void)MessageQueue_push_message(get_call_queue(), (Message* )message, handler); \
                                                                                    \
    return message->result;                                                         \
}

#define DLFCN_IMPLEMENT1(return_type, symbol, a1) \
DLFCN_MESSAGE1 (return_type, symbol, a1)          \
DLFCN_HANDLER1 (return_type, symbol, a1)          \
DLFCN_FUNCTION1(return_type, symbol, a1)

/* argument 2 */
#define DLFCN_MESSAGE2(return_type, symbol, a1, a2) \
typedef struct Message_##symbol {                   \
    Message common;                                 \
    a1 n1;                                          \
    a2 n2;                                          \
    return_type result;                             \
} Message_##symbol;

#define DLFCN_HANDLER2(return_type, symbol, a1, a2)                     \
static void *handleMessage_##symbol (Message_##symbol* message) SOFTFP; \
static void *handleMessage_##symbol (Message_##symbol* message)         \
{                                                                       \
    a1 n1 = message->n1;                                                \
    a2 n2 = message->n2;                                                \
    return_type result;                                                 \
                                                                        \
    result = __dl_##symbol (n1, n2);                                    \
                                                                        \
    message->result = result;                                           \
                                                                        \
    return 0;                                                           \
}

#define DLFCN_FUNCTION2(return_type, symbol, a1, a2)                                \
return_type android_##symbol (a1 n1, a2 n2)                                         \
{                                                                                   \
    MessageHandler handler[1];                                                      \
    Message_##symbol message[1];                                                    \
                                                                                    \
    handler->handleMessage = (void *(*) (Message* ))handleMessage_##symbol;         \
    message->n1 = n1;                                                               \
    message->n2 = n2;                                                               \
                                                                                    \
    (void)MessageQueue_push_message(get_call_queue(), (Message* )message, handler); \
                                                                                    \
    return message->result;                                                         \
}

#define DLFCN_IMPLEMENT2(return_type, symbol, a1, a2) \
DLFCN_MESSAGE2 (return_type, symbol, a1, a2)          \
DLFCN_HANDLER2 (return_type, symbol, a1, a2)          \
DLFCN_FUNCTION2(return_type, symbol, a1, a2)

#if 0
typedef struct Message_dlopen {
    Message common;
    const char *filename;
    int flags;
    void *result;
} Message_dlopen;

static void *handleMessage_dlopen (Message_dlopen* message) SOFTFP;
static void *handleMessage_dlopen (Message_dlopen* message)
{
    const char *filename = message->filename;
    int flags = message->flags;
    void *result;
    
    result = __dl_dlopen (filename, flags);

    message->result = result;

    return 0;
};

void *android_dlopen (const char *filename, int flags)
{
    MessageHandler handler[1];
    Message_dlopen message[1];

    handler->handleMessage = (void *(*) (Message* ) SOFTFP)handleMessage_dlopen;
    message->filename = filename;
    message->flags = flags;

    (void)MessageQueue_push_message(get_call_queue(), (Message* )message, handler);

    return message->result;
}

typedef struct Message_dlclose {
    Message common;
    void *handle;
    int result;
} Message_dlclose;

static void *handleMessage_dlclose (Message_dlclose* message) SOFTFP;
static void *handleMessage_dlclose (Message_dlclose* message) 
{
    void *handle = message->handle;
    int result;
    
    result = __dl_dlclose (handle);

    message->result = result;

    return 0;
};

int   android_dlclose(void *handle)
{
    MessageHandler handler[1];
    Message_dlclose message[1];

    handler->handleMessage = (void *(*) (Message* ) SOFTFP)handleMessage_dlclose;
    message->handle = handle;

    (void)MessageQueue_push_message(get_call_queue(), (Message* )message, handler);

    return message->result;
}

typedef struct Message_dlerror {
    Message common;
    char *result;
} Message_dlerror;

static void *handleMessage_dlerror (Message_dlerror* message) SOFTFP;
static void *handleMessage_dlerror (Message_dlerror* message) 
{
    char *result;
    
    result = __dl_dlerror ();

    message->result = result;

    return 0;
};

char *android_dlerror(void)
{
    MessageHandler handler[1];
    Message_dlerror message[1];

    handler->handleMessage = (void *(*) (Message* ) SOFTFP)handleMessage_dlerror;

    (void)MessageQueue_push_message(get_call_queue(), (Message* )message, handler);

    return message->result;
}

typedef struct Message_dlsym {
    Message common;
    void *handle;
    const char *symbol;
    void *result;
} Message_dlsym;

static void *handleMessage_dlsym (Message_dlsym* message) SOFTFP;
static void *handleMessage_dlsym (Message_dlsym* message) 
{
    void *handle = message->handle;
    const char *symbol = message->symbol;
    void *result;
    
    result = __dl_dlsym (handle, symbol);

    message->result = result;

    return 0;
};

void *android_dlsym  (void *handle, const char *symbol)
{
    MessageHandler handler[1];
    Message_dlsym message[1];

    handler->handleMessage = (void *(*) (Message* ) SOFTFP)handleMessage_dlsym;
    message->handle = handle;
    message->symbol = symbol;

    (void)MessageQueue_push_message(get_call_queue(), (Message* )message, handler);

    return message->result;
}

#else
DLFCN_IMPLEMENT2(void *, dlopen , const char *, int)
DLFCN_IMPLEMENT1(int   , dlclose, void *)
DLFCN_IMPLEMENT0(char *, dlerror)
DLFCN_IMPLEMENT2(void *, dlsym  , void *, const char *)
#endif
