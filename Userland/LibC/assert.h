#pragma once

#include <sys/types.h>

__attribute__((noinline))
void breakpoint(const char *filename, size_t line);

#define assert(condition) ((condition) ? (void)0 : breakpoint(__FILE__, __LINE__))
