#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "Message.h"
#include "Looper.h"
#include "Server.h"

#include "uland_exec.h"

void *start_server (void *args)
{
    int argc;
    char **argv;
    int *ip = args;
    char **cpp = args;

    argc = *ip;
    argv = cpp + 1;

    int ret = server_main (argc, argv);

#if 1
    printf ("execute error : %s", argv[0]);
    for (int i = 1; i < argc; i++) {
        printf (" %s", argv[i]);
    }
    printf (" = %d\n", ret);
    exit (-1);
#endif

    return 0;
}

int server_main (int argc, char *argv[])
{
    if (argc != 5) {
        fprintf (stderr, "Usage: %s rfd1 wfd1 rfd2 wfd2\n", argv[0]);
	exit (-1);
    }

    return uland_execvp(argv[0], argv);
}

