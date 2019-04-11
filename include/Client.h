#ifndef _CLIENT_H_
#define _CLIENT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "Message.h"

/* argument 0 */
#define ANDROID_MESSAGE0(return_type, symbol) \
typedef struct Message_##symbol {             \
    Message common;                           \
    return_type result;                       \
} Message_##symbol;

#define ANDROID_HANDLER0(return_type, symbol)                           \
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

#define ANDROID_FUNCTION0(return_type, symbol)                                      \
return_type symbol ()                                                               \
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

#define ANDROID_IMPLEMENT0(return_type, symbol) \
ANDROID_MESSAGE0 (return_type, symbol)          \
ANDROID_HANDLER0 (return_type, symbol)          \
ANDROID_FUNCTION0(return_type, symbol)

/* argument 1 */
#define ANDROID_MESSAGE1(return_type, symbol, a1) \
typedef struct Message_##symbol {                 \
    Message common;                               \
    a1 n1;                                        \
    return_type result;                           \
} Message_##symbol;

#define ANDROID_HANDLER1(return_type, symbol, a1)                       \
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

#define ANDROID_FUNCTION1(return_type, symbol, a1)                                  \
return_type symbol (a1 n1)                                                          \
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

#define ANDROID_IMPLEMENT1(return_type, symbol, a1) \
ANDROID_MESSAGE1 (return_type, symbol, a1)          \
ANDROID_HANDLER1 (return_type, symbol, a1)          \
ANDROID_FUNCTION1(return_type, symbol, a1)

/* argument 2 */
#define ANDROID_MESSAGE2(return_type, symbol, a1, a2) \
typedef struct Message_##symbol {                     \
    Message common;                                   \
    a1 n1;                                            \
    a2 n2;                                            \
    return_type result;                               \
} Message_##symbol;

#define ANDROID_HANDLER2(return_type, symbol, a1, a2)                   \
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

#define ANDROID_FUNCTION2(return_type, symbol, a1, a2)                              \
return_type symbol (a1 n1, a2 n2)                                                   \
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

#define ANDROID_IMPLEMENT2(return_type, symbol, a1, a2) \
ANDROID_MESSAGE2 (return_type, symbol, a1, a2)          \
ANDROID_HANDLER2 (return_type, symbol, a1, a2)          \
ANDROID_FUNCTION2(return_type, symbol, a1, a2)

/* argument 3 */
#define ANDROID_MESSAGE3(return_type, symbol, a1, a2, a3) \
typedef struct Message_##symbol {                         \
    Message common;                                       \
    a1 n1;                                                \
    a2 n2;                                                \
    a3 n3;                                                \
    return_type result;                                   \
} Message_##symbol;

#define ANDROID_HANDLER3(return_type, symbol, a1, a2, a3)               \
static void *handleMessage_##symbol (Message_##symbol* message) SOFTFP; \
static void *handleMessage_##symbol (Message_##symbol* message)         \
{                                                                       \
    a1 n1 = message->n1;                                                \
    a2 n2 = message->n2;                                                \
    a3 n3 = message->n3;                                                \
    return_type result;                                                 \
                                                                        \
    result = __dl_##symbol (n1, n2, n3);                                \
                                                                        \
    message->result = result;                                           \
                                                                        \
    return 0;                                                           \
}

#define ANDROID_FUNCTION3(return_type, symbol, a1, a2, a3)                          \
return_type symbol (a1 n1, a2 n2, a3 n3)                                            \
{                                                                                   \
    MessageHandler handler[1];                                                      \
    Message_##symbol message[1];                                                    \
                                                                                    \
    handler->handleMessage = (void *(*) (Message* ))handleMessage_##symbol;         \
    message->n1 = n1;                                                               \
    message->n2 = n2;                                                               \
    message->n3 = n3;                                                               \
                                                                                    \
    (void)MessageQueue_push_message(get_call_queue(), (Message* )message, handler); \
                                                                                    \
    return message->result;                                                         \
}

#define ANDROID_IMPLEMENT3(return_type, symbol, a1, a2, a3) \
ANDROID_MESSAGE3 (return_type, symbol, a1, a2, a3)          \
ANDROID_HANDLER3 (return_type, symbol, a1, a2, a3)          \
ANDROID_FUNCTION3(return_type, symbol, a1, a2, a3)

/* argument 4 */
#define ANDROID_MESSAGE4(return_type, symbol, a1, a2, a3, a4) \
typedef struct Message_##symbol {                             \
    Message common;                                           \
    a1 n1;                                                    \
    a2 n2;                                                    \
    a3 n3;                                                    \
    a4 n4;                                                    \
    return_type result;                                       \
} Message_##symbol;

#define ANDROID_HANDLER4(return_type, symbol, a1, a2, a3, a4)           \
static void *handleMessage_##symbol (Message_##symbol* message) SOFTFP; \
static void *handleMessage_##symbol (Message_##symbol* message)         \
{                                                                       \
    a1 n1 = message->n1;                                                \
    a2 n2 = message->n2;                                                \
    a3 n3 = message->n3;                                                \
    a4 n4 = message->n4;                                                \
    return_type result;                                                 \
                                                                        \
    result = __dl_##symbol (n1, n2, n3, n4);                            \
                                                                        \
    message->result = result;                                           \
                                                                        \
    return 0;                                                           \
}

#define ANDROID_FUNCTION4(return_type, symbol, a1, a2, a3, a4)                      \
return_type symbol (a1 n1, a2 n2, a3 n3, a4 n4)                                     \
{                                                                                   \
    MessageHandler handler[1];                                                      \
    Message_##symbol message[1];                                                    \
                                                                                    \
    handler->handleMessage = (void *(*) (Message* ))handleMessage_##symbol;         \
    message->n1 = n1;                                                               \
    message->n2 = n2;                                                               \
    message->n3 = n3;                                                               \
    message->n4 = n4;                                                               \
                                                                                    \
    (void)MessageQueue_push_message(get_call_queue(), (Message* )message, handler); \
                                                                                    \
    return message->result;                                                         \
}

#define ANDROID_IMPLEMENT4(return_type, symbol, a1, a2, a3, a4) \
ANDROID_MESSAGE4 (return_type, symbol, a1, a2, a3, a4)          \
ANDROID_HANDLER4 (return_type, symbol, a1, a2, a3, a4)          \
ANDROID_FUNCTION4(return_type, symbol, a1, a2, a3, a4)

/* argument 5 */
#define ANDROID_MESSAGE5(return_type, symbol, a1, a2, a3, a4, a5) \
typedef struct Message_##symbol {                                 \
    Message common;                                               \
    a1 n1;                                                        \
    a2 n2;                                                        \
    a3 n3;                                                        \
    a4 n4;                                                        \
    a5 n5;                                                        \
    return_type result;                                           \
} Message_##symbol;

#define ANDROID_HANDLER5(return_type, symbol, a1, a2, a3, a4, a5)       \
static void *handleMessage_##symbol (Message_##symbol* message) SOFTFP; \
static void *handleMessage_##symbol (Message_##symbol* message)         \
{                                                                       \
    a1 n1 = message->n1;                                                \
    a2 n2 = message->n2;                                                \
    a3 n3 = message->n3;                                                \
    a4 n4 = message->n4;                                                \
    a5 n5 = message->n5;                                                \
    return_type result;                                                 \
                                                                        \
    result = __dl_##symbol (n1, n2, n3, n4, n5);                        \
                                                                        \
    message->result = result;                                           \
                                                                        \
    return 0;                                                           \
}

#define ANDROID_FUNCTION5(return_type, symbol, a1, a2, a3, a4, a5)                  \
return_type symbol (a1 n1, a2 n2, a3 n3, a4 n4, a5 n5)                              \
{                                                                                   \
    MessageHandler handler[1];                                                      \
    Message_##symbol message[1];                                                    \
                                                                                    \
    handler->handleMessage = (void *(*) (Message* ))handleMessage_##symbol;         \
    message->n1 = n1;                                                               \
    message->n2 = n2;                                                               \
    message->n3 = n3;                                                               \
    message->n4 = n4;                                                               \
    message->n5 = n5;                                                               \
                                                                                    \
    (void)MessageQueue_push_message(get_call_queue(), (Message* )message, handler); \
                                                                                    \
    return message->result;                                                         \
}

#define ANDROID_IMPLEMENT5(return_type, symbol, a1, a2, a3, a4, a5) \
ANDROID_MESSAGE5 (return_type, symbol, a1, a2, a3, a4, a5)          \
ANDROID_HANDLER5 (return_type, symbol, a1, a2, a3, a4, a5)          \
ANDROID_FUNCTION5(return_type, symbol, a1, a2, a3, a4, a5)

/* argument 6 */
#define ANDROID_MESSAGE6(return_type, symbol, a1, a2, a3, a4, a5, a6) \
typedef struct Message_##symbol {                                     \
    Message common;                                                   \
    a1 n1;                                                            \
    a2 n2;                                                            \
    a3 n3;                                                            \
    a4 n4;                                                            \
    a5 n5;                                                            \
    a6 n6;                                                            \
    return_type result;                                               \
} Message_##symbol;

#define ANDROID_HANDLER6(return_type, symbol, a1, a2, a3, a4, a5, a6)   \
static void *handleMessage_##symbol (Message_##symbol* message) SOFTFP; \
static void *handleMessage_##symbol (Message_##symbol* message)         \
{                                                                       \
    a1 n1 = message->n1;                                                \
    a2 n2 = message->n2;                                                \
    a3 n3 = message->n3;                                                \
    a4 n4 = message->n4;                                                \
    a5 n5 = message->n5;                                                \
    a6 n6 = message->n6;                                                \
    return_type result;                                                 \
                                                                        \
    result = __dl_##symbol (n1, n2, n3, n4, n5, n6);                    \
                                                                        \
    message->result = result;                                           \
                                                                        \
    return 0;                                                           \
}

#define ANDROID_FUNCTION6(return_type, symbol, a1, a2, a3, a4, a5, a6)              \
return_type symbol (a1 n1, a2 n2, a3 n3, a4 n4, a5 n5, a6 n6)                       \
{                                                                                   \
    MessageHandler handler[1];                                                      \
    Message_##symbol message[1];                                                    \
                                                                                    \
    handler->handleMessage = (void *(*) (Message* ))handleMessage_##symbol;         \
    message->n1 = n1;                                                               \
    message->n2 = n2;                                                               \
    message->n3 = n3;                                                               \
    message->n4 = n4;                                                               \
    message->n5 = n5;                                                               \
    message->n6 = n6;                                                               \
                                                                                    \
    (void)MessageQueue_push_message(get_call_queue(), (Message* )message, handler); \
                                                                                    \
    return message->result;                                                         \
}

#define ANDROID_IMPLEMENT6(return_type, symbol, a1, a2, a3, a4, a5, a6) \
ANDROID_MESSAGE6 (return_type, symbol, a1, a2, a3, a4, a5, a6)          \
ANDROID_HANDLER6 (return_type, symbol, a1, a2, a3, a4, a5, a6)          \
ANDROID_FUNCTION6(return_type, symbol, a1, a2, a3, a4, a5, a6)

/* argument 7 */
#define ANDROID_MESSAGE7(return_type, symbol, a1, a2, a3, a4, a5, a6, a7) \
typedef struct Message_##symbol {                                         \
    Message common;                                                       \
    a1 n1;                                                                \
    a2 n2;                                                                \
    a3 n3;                                                                \
    a4 n4;                                                                \
    a5 n5;                                                                \
    a6 n6;                                                                \
    a7 n7;                                                                \
    return_type result;                                                   \
} Message_##symbol;

#define ANDROID_HANDLER7(return_type, symbol, a1, a2, a3, a4, a5, a6, a7) \
static void *handleMessage_##symbol (Message_##symbol* message) SOFTFP;   \
static void *handleMessage_##symbol (Message_##symbol* message)           \
{                                                                         \
    a1 n1 = message->n1;                                                  \
    a2 n2 = message->n2;                                                  \
    a3 n3 = message->n3;                                                  \
    a4 n4 = message->n4;                                                  \
    a5 n5 = message->n5;                                                  \
    a6 n6 = message->n6;                                                  \
    a7 n7 = message->n7;                                                  \
    return_type result;                                                   \
                                                                          \
    result = __dl_##symbol (n1, n2, n3, n4, n5, n6, n7);                  \
                                                                          \
    message->result = result;                                             \
                                                                          \
    return 0;                                                             \
}

#define ANDROID_FUNCTION7(return_type, symbol, a1, a2, a3, a4, a5, a6, a7)          \
return_type symbol (a1 n1, a2 n2, a3 n3, a4 n4, a5 n5, a6 n6, a7 n7)                \
{                                                                                   \
    MessageHandler handler[1];                                                      \
    Message_##symbol message[1];                                                    \
                                                                                    \
    handler->handleMessage = (void *(*) (Message* ))handleMessage_##symbol;         \
    message->n1 = n1;                                                               \
    message->n2 = n2;                                                               \
    message->n3 = n3;                                                               \
    message->n4 = n4;                                                               \
    message->n5 = n5;                                                               \
    message->n6 = n6;                                                               \
    message->n7 = n7;                                                               \
                                                                                    \
    (void)MessageQueue_push_message(get_call_queue(), (Message* )message, handler); \
                                                                                    \
    return message->result;                                                         \
}

#define ANDROID_IMPLEMENT7(return_type, symbol, a1, a2, a3, a4, a5, a6, a7) \
ANDROID_MESSAGE7 (return_type, symbol, a1, a2, a3, a4, a5, a6, a7)          \
ANDROID_HANDLER7 (return_type, symbol, a1, a2, a3, a4, a5, a6, a7)          \
ANDROID_FUNCTION7(return_type, symbol, a1, a2, a3, a4, a5, a6, a7)

/* argument 8 */
#define ANDROID_MESSAGE8(return_type, symbol, a1, a2, a3, a4, a5, a6, a7, a8) \
typedef struct Message_##symbol {                                             \
    Message common;                                                           \
    a1 n1;                                                                    \
    a2 n2;                                                                    \
    a3 n3;                                                                    \
    a4 n4;                                                                    \
    a5 n5;                                                                    \
    a6 n6;                                                                    \
    a7 n7;                                                                    \
    a8 n8;                                                                    \
    return_type result;                                                       \
} Message_##symbol;

#define ANDROID_HANDLER8(return_type, symbol, a1, a2, a3, a4, a5, a6, a7, a8) \
static void *handleMessage_##symbol (Message_##symbol* message) SOFTFP;       \
static void *handleMessage_##symbol (Message_##symbol* message)               \
{                                                                             \
    a1 n1 = message->n1;                                                      \
    a2 n2 = message->n2;                                                      \
    a3 n3 = message->n3;                                                      \
    a4 n4 = message->n4;                                                      \
    a5 n5 = message->n5;                                                      \
    a6 n6 = message->n6;                                                      \
    a7 n7 = message->n7;                                                      \
    a8 n8 = message->n8;                                                      \
    return_type result;                                                       \
                                                                              \
    result = __dl_##symbol (n1, n2, n3, n4, n5, n6, n7, n8);                  \
                                                                              \
    message->result = result;                                                 \
                                                                              \
    return 0;                                                                 \
}

#define ANDROID_FUNCTION8(return_type, symbol, a1, a2, a3, a4, a5, a6, a7, a8)      \
return_type symbol (a1 n1, a2 n2, a3 n3, a4 n4, a5 n5, a6 n6, a7 n7, a8 n8)         \
{                                                                                   \
    MessageHandler handler[1];                                                      \
    Message_##symbol message[1];                                                    \
                                                                                    \
    handler->handleMessage = (void *(*) (Message* ))handleMessage_##symbol;         \
    message->n1 = n1;                                                               \
    message->n2 = n2;                                                               \
    message->n3 = n3;                                                               \
    message->n4 = n4;                                                               \
    message->n5 = n5;                                                               \
    message->n6 = n6;                                                               \
    message->n7 = n7;                                                               \
    message->n8 = n8;                                                               \
                                                                                    \
    (void)MessageQueue_push_message(get_call_queue(), (Message* )message, handler); \
                                                                                    \
    return message->result;                                                         \
}

#define ANDROID_IMPLEMENT8(return_type, symbol, a1, a2, a3, a4, a5, a6, a7, a8) \
ANDROID_MESSAGE8 (return_type, symbol, a1, a2, a3, a4, a5, a6, a7, a8)          \
ANDROID_HANDLER8 (return_type, symbol, a1, a2, a3, a4, a5, a6, a7, a8)          \
ANDROID_FUNCTION8(return_type, symbol, a1, a2, a3, a4, a5, a6, a7, a8)

/*********************************************************************/
/* argument 0 */
#define ANDROID_VOID_MESSAGE0(symbol) \
typedef struct Message_##symbol {     \
    Message common;                   \
} Message_##symbol;

#define ANDROID_VOID_HANDLER0(symbol)                                   \
static void *handleMessage_##symbol (Message_##symbol* message) SOFTFP; \
static void *handleMessage_##symbol (Message_##symbol* message)         \
{                                                                       \
    __dl_##symbol ();                                                   \
                                                                        \
    return 0;                                                           \
}

#define ANDROID_VOID_FUNCTION0(symbol)                                              \
void symbol ()                                                                      \
{                                                                                   \
    MessageHandler handler[1];                                                      \
    Message_##symbol message[1];                                                    \
                                                                                    \
    handler->handleMessage = (void *(*) (Message* ))handleMessage_##symbol;         \
                                                                                    \
    (void)MessageQueue_push_message(get_call_queue(), (Message* )message, handler); \
                                                                                    \
    return;                                                                         \
}

#define ANDROID_VOID_IMPLEMENT0(symbol) \
ANDROID_VOID_MESSAGE0 (symbol)          \
ANDROID_VOID_HANDLER0 (symbol)          \
ANDROID_VOID_FUNCTION0(symbol)

/* argument 1 */
#define ANDROID_VOID_MESSAGE1(symbol, a1) \
typedef struct Message_##symbol {         \
    Message common;                       \
    a1 n1;                                \
} Message_##symbol;

#define ANDROID_VOID_HANDLER1(symbol, a1)                               \
static void *handleMessage_##symbol (Message_##symbol* message) SOFTFP; \
static void *handleMessage_##symbol (Message_##symbol* message)         \
{                                                                       \
    a1 n1 = message->n1;                                                \
                                                                        \
    __dl_##symbol (n1);                                                 \
                                                                        \
    return 0;                                                           \
}

#define ANDROID_VOID_FUNCTION1(symbol, a1)                                          \
void symbol (a1 n1)                                                                 \
{                                                                                   \
    MessageHandler handler[1];                                                      \
    Message_##symbol message[1];                                                    \
                                                                                    \
    handler->handleMessage = (void *(*) (Message* ))handleMessage_##symbol;         \
    message->n1 = n1;                                                               \
                                                                                    \
    (void)MessageQueue_push_message(get_call_queue(), (Message* )message, handler); \
                                                                                    \
    return;                                                                         \
}

#define ANDROID_VOID_IMPLEMENT1(symbol, a1) \
ANDROID_VOID_MESSAGE1 (symbol, a1)          \
ANDROID_VOID_HANDLER1 (symbol, a1)          \
ANDROID_VOID_FUNCTION1(symbol, a1)

/* argument 2 */
#define ANDROID_VOID_MESSAGE2(symbol, a1, a2)         \
typedef struct Message_##symbol {                     \
    Message common;                                   \
    a1 n1;                                            \
    a2 n2;                                            \
} Message_##symbol;

#define ANDROID_VOID_HANDLER2(symbol, a1, a2)                           \
static void *handleMessage_##symbol (Message_##symbol* message) SOFTFP; \
static void *handleMessage_##symbol (Message_##symbol* message)         \
{                                                                       \
    a1 n1 = message->n1;                                                \
    a2 n2 = message->n2;                                                \
                                                                        \
    __dl_##symbol (n1, n2);                                             \
                                                                        \
    return 0;                                                           \
}

#define ANDROID_VOID_FUNCTION2(symbol, a1, a2)                                      \
void symbol (a1 n1, a2 n2)                                                          \
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
    return;                                                                         \
}

#define ANDROID_VOID_IMPLEMENT2(symbol, a1, a2) \
ANDROID_VOID_MESSAGE2 (symbol, a1, a2)          \
ANDROID_VOID_HANDLER2 (symbol, a1, a2)          \
ANDROID_VOID_FUNCTION2(symbol, a1, a2)

/*********************************************************************/
extern void *(*__dl_dlopen) (const char *filename, int flags) SOFTFP;
extern int   (*__dl_dlclose)(void *handle) SOFTFP;
extern char *(*__dl_dlerror)(void) SOFTFP;
extern void *(*__dl_dlsym)  (void *handle, const char *symbol) SOFTFP;

extern int Client_initialize (struct io_port* port1, struct io_port* port2);
extern MessageQueue *get_dlhook_queue ();
extern MessageQueue *get_call_queue ();
extern void  dlhook_initialize ();

#ifdef __cplusplus
}
#endif

#endif
