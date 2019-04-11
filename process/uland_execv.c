#include <unistd.h>

#include "uland_exec.h"

extern char **environ;

int uland_execv(const char *path, char *const argv[])
{
	return uland_execve(path, argv, environ);
}
