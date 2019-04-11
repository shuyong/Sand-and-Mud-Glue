
#define _GNU_SOURCE         /* See feature_test_macros(7) */
#include <unistd.h>
#include <sys/syscall.h>   /* For SYS_xxx definitions */
#include <sys/types.h>

pid_t gettid(void)
{
    pid_t tid;

    tid = syscall(SYS_gettid);

    return tid;
}
