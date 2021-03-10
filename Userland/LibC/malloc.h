#pragma once

#include <stddef.h>

void free(void* pointer);
void* malloc(size_t size);
void* realloc(void *pointer, size_t size);
