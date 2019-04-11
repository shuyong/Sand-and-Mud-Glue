#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include "Message.h"
#include "Looper.h"
#include "Server.h"

#define handle_error(msg) \
	do { perror(msg); exit(EXIT_FAILURE); } while (0)

int main (int argc, char *argv[])
{
    if (argc != 5) {
        fprintf (stderr, "Usage: %s rfd1 wfd1 rfd2 wfd2\n", argv[0]);
	exit (-1);
    }

#if 0
    printf ("callee: %s", argv[0]);
    for (int i = 1; i < argc; i++) {
        printf (" %s", argv[i]);
    }
    printf ("\n");
#endif

    int rfd[2];
    int wfd[2];

    /* Check the availability of these fds */
    for (int i = 0; i < 2; i++) {
        int flags;

        rfd[i] = atoi(argv[i * 2 + 1]);
        flags = fcntl(rfd[i], F_GETFL);
        if (-1 == flags ) {
            fprintf (stderr, "fd(%d) ", rfd[i]);
            handle_error("fcntl - F_GETFL");
        }
        if (O_RDONLY != (flags & O_RDONLY)) {
            fprintf (stderr, "Bad read file descriptor: %d\n", rfd[i]);
            exit (-1);
        }

        wfd[i] = atoi(argv[i * 2 + 2]);
        flags = fcntl(wfd[i], F_GETFL);
        if (-1 == flags ) {
            fprintf (stderr, "fd(%d) ", wfd[i]);
            handle_error("fcntl - F_GETFL");
        }
        if (O_WRONLY != (flags & O_WRONLY)) {
            fprintf (stderr, "Bad write file descriptor: %d\n", wfd[i]);
	    exit (-1);
        }
    }

    Looper *looper = Looper_ctor();

    Looper_add_channel (looper, rfd[0], wfd[0], servant_handler);
    Looper_add_channel (looper, rfd[1], wfd[1], callee_handler);

    int rc;
    do {
        rc = Looper_poll_message (looper);
    } while (rc == 0);

    Looper_dtor(looper);

    return 0;
}

