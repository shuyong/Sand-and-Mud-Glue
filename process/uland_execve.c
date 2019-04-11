#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "uland_exec.h"

#define BINPRM_BUF_SIZE 128

#ifndef O_LARGEFILE
#define O_LARGEFILE	00100000
#endif

extern int load_elf_binary(char* buf, int file, int argc, char *const argv[], int envc, char *const envp[]);

int uland_execve(const char *path, char *const argv[], char *const envp[])
{
	char* buf;
	int fd;
	int retval;

	if ((buf = malloc(BINPRM_BUF_SIZE)) == NULL) {
		fprintf (stderr, "malloc buffer[%d] : out of memory", BINPRM_BUF_SIZE);
		exit (-1);
	}

	if ((fd = open(path, O_LARGEFILE | O_RDONLY)) < 0) {
		//perror("open binary");
		free (buf);
		return -1;
	}

	if ((retval = read(fd, buf, BINPRM_BUF_SIZE)) != BINPRM_BUF_SIZE) {
		//perror("read binary");
		free (buf);
		return -1;
	}

	char * const *ppc;
	int argc = 0;
	int envc = 0;

	for (ppc = argv; NULL != *ppc; ppc++) {
		argc++;
	}

	for (ppc = envp; NULL != *ppc; ppc++) {
		envc++;
	}

	/* do we need to use environ if envp is null? */
	retval = load_elf_binary(buf, fd, argc, argv, envc, envp);

	return retval;
}

