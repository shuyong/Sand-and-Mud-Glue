
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include <event2/event-config.h>
#include <event2/event.h>

#include "Message.h"

#define handle_error(msg) \
	do { perror(msg); exit(EXIT_FAILURE); } while (0)

extern ssize_t readn (int fd, void *ptr, size_t n);
extern ssize_t writen(int fd, const void *ptr, size_t n);

static void client_read(evutil_socket_t fd, short event, void *arg)
{
    (void)event;
    struct MessageQueue *queue = arg;
    //struct event *evread  = queue->evread;
    //struct event *evwrite = queue->evwrite;
    MessageBinder *binder = queue->binder;
    int n;

    n = readn(fd, binder, sizeof(MessageBinder));
    if (n != sizeof(MessageBinder)) {
        fprintf (stderr, "%s %d: read(%d) error!\n", __FUNCTION__, __LINE__, fd);
    }
}

static void client_write (evutil_socket_t fd, short event, void *arg)
{
    (void)event;
    struct MessageQueue *queue = arg;
    struct event *evread  = queue->evread;
    //struct event *evwrite = queue->evwrite;
    MessageBinder *binder = queue->binder;
    int n;

    n = writen(fd, binder, sizeof(MessageBinder));
    if (n != sizeof(MessageBinder)) {
        // stop looper
	return;
    }

    /* Add reader to the active events, without a timeout */
    event_add(evread, NULL);
}

MessageQueue *MessageQueue_ctor(int rfd, int wfd)
{
    MessageQueue *queue = calloc (1, sizeof(struct MessageQueue));
    /* Initalize the event library */
    struct event_base* base = event_base_new ();
    struct event *evread;
    struct event *evwrite;

    /* Check the availability of read / write fds */
    int flags;

    flags = fcntl(rfd, F_GETFL);
    if (-1 == flags ) {
        handle_error("fcntl - F_GETFL");
    }
    if (O_RDONLY != (flags & O_RDONLY)) {
        fprintf (stderr, "Bad read file descriptor: %d\n", rfd);
        exit (-1);
    }

    flags = fcntl(wfd, F_GETFL);
    if (-1 == flags ) {
        handle_error("fcntl - F_GETFL");
    }
    if (O_WRONLY != (flags & O_WRONLY)) {
        fprintf (stderr, "Bad write file descriptor: %d\n", wfd);
        exit (-1);
    }

    evread  = event_new(base, rfd, EV_READ , client_read , queue);
    evwrite = event_new(base, wfd, EV_WRITE, client_write, queue);

    queue->base = base;
    queue->evread = evread;
    queue->evwrite = evwrite;

    return queue;
}

void MessageQueue_dtor(MessageQueue *queue)
{
    struct event_base* base = queue->base;
    struct event *evread;
    struct event *evwrite;
    int fd;

    evread  = queue->evread;
    evwrite = queue->evwrite;

    fd = event_get_fd (evread);
    if (fd >=0) close (fd);
    fd = event_get_fd (evwrite);
    if (fd >=0) close (fd);

    event_base_free (base);
    free (queue);
}

int MessageQueue_push_message(MessageQueue *queue, Message* message, MessageHandler* handler)
{
    struct event_base* base = queue->base;
    //struct event *evread  = queue->evread;
    struct event *evwrite = queue->evwrite;
    MessageBinder *binder = queue->binder;

    binder->message = message;
    binder->handler = handler;

    /* Add writer to the active events, without a timeout */
    event_add(evwrite, NULL);

    return event_base_dispatch(base);
}

