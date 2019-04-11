
#ifndef _ULAND_EXEC_H_
#define _ULAND_EXEC_H_

#ifdef __cplusplus
extern "C" {
#endif

int uland_execl(const char *path, const char *arg, ...
                       /* (char  *) NULL */);
int uland_execlp(const char *file, const char *arg, ...
                       /* (char  *) NULL */);
int uland_execle(const char *path, const char *arg, ...
                       /*, (char *) NULL, char * const envp[] */);
int uland_execv(const char *path, char *const argv[]);
int uland_execvp(const char *file, char *const argv[]);
int uland_execvpe(const char *file, char *const argv[],
                       char *const envp[]);
int uland_execve(const char *filename, char *const argv[],
                       char *const envp[]);

#ifdef __cplusplus
}
#endif

#endif

