#pragma once

#include <sys/types.h>
#include <Kernel/Interface/Types.hpp>

int posix_spawn(
    pid_t *pid,
    const char *pathname,
    const posix_spawn_file_actions_t *file_actions,
    const posix_spawnattr_t *attrp,
    char **argv,
    char **envp);
