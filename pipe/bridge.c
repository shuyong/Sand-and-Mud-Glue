
#include <stdio.h>
#include <stdlib.h>

#include "Message.h"

#define _GNU_SOURCE             /* See feature_test_macros(7) */
#include <features.h>
#if ! defined(__USE_GNU)
#define __USE_GNU
#endif
#include <fcntl.h>              /* Obtain O_* constant definitions */
#include <unistd.h>

#define handle_error(msg) \
	do { perror(msg); exit(EXIT_FAILURE); } while (0)

static void get_fd_pair(int pipefd[2])
{
#if defined(O_DIRECT)
	/* O_DIRECT  (since Linux 3.4)
	 * Create a pipe that performs I/O in "packet" mode.
	 */
	if (0 != pipe2 (pipefd, O_DIRECT)) {
		handle_error ("pipe2");
	}

#else
	if (0 != pipe (pipefd)) {
		handle_error ("pipe");
	}

#endif
}

void pipe_init(struct io_port* cport, struct io_port* sport)
{
	int fd_pair1[2];
	int fd_pair2[2];

	get_fd_pair(fd_pair1);
	get_fd_pair(fd_pair2);

	/*
	 * client space                server space
	 * fd_pair1[0] r <----+ +---> r fd_pair2[0]
	 *                    | |
	 *                    ^ ^
	 *                  +-\-/-+
	 *                  |  X  | LINUX
	 *                  +-/-\-+
	 *                    ^ ^
	 *                    | |
	 * fd_pair2[1] w >----+ +---< w fd_pair1[1]
	 */
	/* client space: read from server space */
	cport->rfd = fd_pair1[0];
	/* client space: write to server space */
	cport->wfd = fd_pair2[1];
	/* server space: read from client space */
	sport->rfd = fd_pair2[0];
	/* server space: write to client space */
	sport->wfd = fd_pair1[1];
}

