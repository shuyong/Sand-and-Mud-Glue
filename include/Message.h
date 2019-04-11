
#ifndef _MESSAGE_H_
#define _MESSAGE_H_

#include <event2/event-config.h>
#include <event2/event.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * A message that can be posted to a Looper.
 */
typedef struct Message {
    /* The message type. (interpretation is left up to the handler) */
    int what;
} Message;

enum {
    MSG_FIRST = 0,
    MSG_DLHOOK = MSG_FIRST,
    MSG_MAX,
};

typedef struct MessageDlhook {
    Message common;
    void *(*dlopen) (const char *filename, int flags);
    int   (*dlclose)(void *handle);
    char *(*dlerror)(void);
    void *(*dlsym)  (void *handle, const char *symbol);
} MessageDlhook;

/*
 * Attribute for softfp-calling-style functions
 */
#ifdef __ARM_PCS_VFP
#    define SOFTFP __attribute__((pcs("aapcs")))
#else
#    define SOFTFP
#endif

/**
 * Interface for a Looper message handler.
 */
typedef struct MessageHandler {
    /**
     * Handles a message.
     */
    void *(*handleMessage) (Message* message) SOFTFP;
} MessageHandler;

typedef struct MessageBinder {
    Message        *message;
    MessageHandler *handler;
} MessageBinder;

typedef struct io_port {
    int rfd;
    int wfd;
} io_port;

typedef struct MessageQueue {
    struct event_base* base;
    struct event *evread;
    struct event *evwrite;
    MessageBinder binder[1];
} MessageQueue;

MessageQueue *MessageQueue_ctor(int rfd, int wfd);

void MessageQueue_dtor(MessageQueue *queue);

/**
 * Enqueues a message to be processed by the specified handler.
 *
 * The handler must not be null.
 * This method can be called on any thread.
 */
int MessageQueue_push_message(MessageQueue *queue, Message* message, MessageHandler* handler);

#ifdef __cplusplus
}
#endif

#endif
