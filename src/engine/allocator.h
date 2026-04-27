#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <stdio.h>

#ifndef MALLOC
#define MALLOC malloc
#endif

#ifndef CALLOC
#define CALLOC calloc
#endif

#ifndef REALLOC
#define REALLOC realloc
#endif

#ifndef FREE
#define FREE free
#endif

typedef struct {
	size_t total_memory;
	size_t frame_memory;
	int total_allocations;
	int frame_allocations;
} Allocator;

void allocator_attach(Allocator* allocator);

void* mmalloc(size_t size);
void* mcalloc(size_t count, size_t size);
void* mrealloc(void* ptr, size_t size);
void mfree(void* ptr);

#endif
