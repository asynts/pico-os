#include <spawn.h>
#include <sys/system.h>

int posix_spawn(
    pid_t *pid,
    const char *pathname,
    const posix_spawn_file_actions_t *file_actions,
    const posix_spawnattr_t *attrp,
    char **argv,
    char **envp)
{
    return sys$posix_spawn(pid, pathname, file_actions, attrp, argv, envp);
}
