
#ifndef _LOOPER_H_
#define _LOOPER_H_

#include <event2/event-config.h>
#include <event2/event.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_CHANNELS	8

typedef struct Channel {
    struct event *evread;
    struct event *evwrite;
    MessageBinder binder[1];
    void *(*handler) (MessageBinder* );
} Channel;

typedef struct Looper {
    struct event_base* base;
    struct Channel channels[MAX_CHANNELS];
} Looper;

extern Looper *Looper_ctor ();

extern void Looper_dtor (Looper *looper);

extern int Looper_add_channel (Looper *looper, int rfd, int wfd, void *(*handler) (MessageBinder* ));

extern int Looper_poll_message (Looper *looper);

// 预定义接口。export service
extern void *servant_handler (MessageBinder* binder);
// 远程调用。remote call
extern void *callee_handler (MessageBinder* binder);

#ifdef __cplusplus
}
#endif

#endif
