
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#include <event2/event-config.h>
#include <event2/event.h>

#include "Message.h"
#include "Looper.h"

extern ssize_t readn (int fd, void *ptr, size_t n);
extern ssize_t writen(int fd, const void *ptr, size_t n);

static void channel_read(evutil_socket_t fd, short event, void *arg)
{
    (void)event;
    struct Channel *channel = arg;
    //struct event *evread  = channel->evread;
    struct event *evwrite = channel->evwrite;
    MessageBinder *binder = channel->binder;
    void *(*handler) (MessageBinder* ) = channel->handler;
    int n;

    n = readn(fd, binder, sizeof(MessageBinder));
    if (n != sizeof(MessageBinder)) {
        // stop looper
	return;
    }

    (void)handler(binder);

    /* Add writer to the active events, without a timeout */
    event_add(evwrite, NULL);
}

static void channel_write (evutil_socket_t fd, short event, void *arg)
{
    (void)event;
    struct Channel *channel = arg;
    struct event *evread  = channel->evread;
    //struct event *evwrite = channel->evwrite;
    MessageBinder *binder = channel->binder;
    int n;

    n = writen(fd, binder, sizeof(MessageBinder));
    if (n != sizeof(MessageBinder)) {
        // stop looper
	return;
    }

    /* Add reader to the active events, without a timeout */
    event_add(evread, NULL);
}

/* On Unix, cleanup event if SIGINT is received. */
static void
signal_cb(evutil_socket_t fd, short event, void *arg)
{
    (void)fd;
    (void)event;
    struct event_base *base = arg;

    event_base_loopbreak(base);
}

Looper *Looper_ctor ()
{
    Looper *looper = calloc (1, sizeof(struct Looper));

    /* Initalize the event library */
    struct event_base* base = event_base_new ();
    looper->base = base;

    /* catch SIGINT so that event.fifo can be cleaned up */
    struct event *signal_int = evsignal_new(base, SIGINT, signal_cb, base);
    event_add(signal_int, NULL);

    /* Let's find out about broken client pipe via failed write() */
    if (signal(SIGPIPE, SIG_IGN) == SIG_ERR) {
        perror ("signal");
	exit (-1);
    }

    return looper;
}

void Looper_dtor (Looper *looper)
{
    struct event_base* base = looper->base;
    struct Channel *channel = NULL;
    struct event *evread;
    struct event *evwrite;
    int i;
    int fd;

    for (i = 0; i < MAX_CHANNELS; i++) {
        if (looper->channels[i].evread != NULL) {
            channel = &looper->channels[i];
	    evread  = channel->evread;
	    evwrite = channel->evwrite;

	    fd = event_get_fd (evread);
	    if (fd >=0) close (fd);
	    fd = event_get_fd (evwrite);
	    if (fd >=0) close (fd);

	    event_free (evread);
	    event_free (evwrite);
	}
    }

    event_base_free (base);
    free (looper);
}

int Looper_add_channel (Looper *looper, int rfd, int wfd, void *(*handler) (MessageBinder* ))
{
    struct Channel *channel = NULL;
    struct event_base* base = looper->base;
    struct event *evread;
    struct event *evwrite;
    int i;

    for (i = 0; i < MAX_CHANNELS; i++) {
        if (looper->channels[i].evread == NULL) {
            channel = &looper->channels[i];
	    break;
	}
    }

    if (channel == NULL) {
	return (-1);
    }

    evread  = event_new(base, rfd, EV_READ , channel_read , channel);
    evwrite = event_new(base, wfd, EV_WRITE, channel_write, channel);

    channel->evread  = evread ;
    channel->evwrite = evwrite;
    channel->handler = handler;

    /* Add reader to the active events, without a timeout */
    event_add(evread, NULL);

    return 0;
}

int Looper_poll_message (Looper *looper)
{
    struct event_base* base = looper->base;

    return event_base_dispatch(base);
}

// 预定义接口。export service
extern void *Dlhook_handler (Message* message) SOFTFP;
static struct MessageHandler sServantHandler[MSG_MAX] = {
    {Dlhook_handler},
};

void *servant_handler (MessageBinder* binder)
{
    Message        *message = binder->message;
    MessageHandler *handler;
    int what = message->what;

    if (MSG_FIRST <= what && what < MSG_MAX) {
        handler = &sServantHandler[what];
	return handler->handleMessage(message);
    } else {
        return NULL;
    }
}

// 远程调用。remote call
void *callee_handler (MessageBinder* binder)
{
    Message        *message = binder->message;
    MessageHandler *handler = binder->handler;

    return handler->handleMessage(message);
}

