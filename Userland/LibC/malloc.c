#include <assert.h>
#include <malloc.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

extern char __heap_start__[];
extern char __heap_end__[];

#define MAGIC 0xC0DECAFE

typedef struct MallocHeader {
  struct MallocHeader *next;
  struct MallocHeader *prev;
  size_t size; // Total size including header
  bool free;
  uint32_t magic;
} MallocHeader;

static MallocHeader *head = NULL;

static void initialize_heap() {
  head = (MallocHeader *)__heap_start__;
  head->next = NULL;
  head->prev = NULL;
  head->size = (size_t)(__heap_end__ - __heap_start__);
  head->free = true;
  head->magic = MAGIC;
}

static size_t align(size_t size) {
  if (size % 4 != 0)
    return size + 4 - (size % 4);
  return size;
}

void *malloc(size_t size) {
  if (size == 0)
    return NULL;

  if (head == NULL) {
    initialize_heap();
  }

  size_t required_size = align(size) + sizeof(MallocHeader);
  MallocHeader *current = head;

  while (current) {
    if (current->free && current->size >= required_size) {
      // Found a suitable block
      // Check if we can split it
      if (current->size >= required_size + sizeof(MallocHeader) + 4) {
        // Split
        MallocHeader *new_block =
            (MallocHeader *)((char *)current + required_size);
        new_block->size = current->size - required_size;
        new_block->free = true;
        new_block->magic = MAGIC;
        new_block->next = current->next;
        new_block->prev = current;

        if (current->next) {
          current->next->prev = new_block;
        }
        current->next = new_block;
        current->size = required_size;
      }

      current->free = false;
      return (void *)(current + 1);
    }
    current = current->next;
  }

  return NULL; // Out of memory
}

void free(void *pointer) {
  if (pointer == NULL)
    return;

  MallocHeader *header = (MallocHeader *)pointer - 1;
  assert(header->magic == MAGIC); // Corruption check

  header->free = true;

  // Coalesce with next
  if (header->next && header->next->free) {
    header->size += header->next->size;
    header->next = header->next->next;
    if (header->next) {
      header->next->prev = header;
    }
  }

  // Coalesce with prev
  if (header->prev && header->prev->free) {
    header->prev->size += header->size;
    header->prev->next = header->next;
    if (header->next) {
      header->next->prev = header->prev;
    }
  }
}

void *realloc(void *pointer, size_t size) {
  if (pointer == NULL)
    return malloc(size);
  if (size == 0) {
    free(pointer);
    return NULL;
  }

  MallocHeader *header = (MallocHeader *)pointer - 1;
  assert(header->magic == MAGIC);

  // Simple implementation: allocate new, copy, free old
  void *new_ptr = malloc(size);
  if (new_ptr == NULL)
    return NULL;

  size_t copy_size = header->size - sizeof(MallocHeader);
  if (size < copy_size)
    copy_size = size;

  // Manually copy since we don't include string.h for memcpy (avoid dependency
  // loop?) Actually malloc.c usually can include string.h
  char *src = (char *)pointer;
  char *dst = (char *)new_ptr;
  for (size_t i = 0; i < copy_size; ++i) {
    dst[i] = src[i];
  }

  free(pointer);
  return new_ptr;
}

void *calloc(size_t nmemb, size_t size) {
  size_t total_size = nmemb * size;
  void *ptr = malloc(total_size);
  if (ptr) {
    char *p = (char *)ptr;
    for (size_t i = 0; i < total_size; ++i)
      p[i] = 0;
  }
  return ptr;
}
