#pragma once

#include <sys/types.h>

#define STDIN_FILENO 0
#define STDOUT_FILENO 1

#define X_OK 1

int chdir(const char *path);

pid_t fork(void);

int execle(const char *pathname, ...);

int access(const char *pathname, int mode);

int geteuid(void);
int getegid(void);
