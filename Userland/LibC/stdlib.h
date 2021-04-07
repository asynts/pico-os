#pragma once

#include <stddef.h>

_Noreturn void abort(void);
_Noreturn void exit(int status);

char* getenv(const char *name);
