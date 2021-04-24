#include <spawn.h>
#include <sys/system.h>
#include <errno.h>

int posix_spawn(
    pid_t *pid,
    const char *pathname,
    const posix_spawn_file_actions_t *file_actions,
    const posix_spawnattr_t *attrp,
    char **argv,
    char **envp)
{
    int retval = sys$posix_spawn(pid, pathname, file_actions, attrp, argv, envp);
    libc_check_errno(retval);
    return 0;
}
