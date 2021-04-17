#pragma once

#include <sys/types.h>

#define STDIN_FILENO 0
#define STDOUT_FILENO 1

#define X_OK 1

int chdir(const char *pathname);

int access(const char *pathname, int mode);

int geteuid(void);
int getegid(void);

char* get_current_dir_name(void);
