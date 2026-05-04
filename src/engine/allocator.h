#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include "util/hashmap.h"
#include <stdint.h>
#include <stdio.h>

#ifndef MALLOC
#define MALLOC(size) mmalloc(size, (AllocationDetails){ __FILE__, __func__, __LINE__})
#endif

#ifndef CALLOC
#define CALLOC mcalloc
#endif

#ifndef REALLOC
#define REALLOC mrealloc
#endif

#ifndef FREE
#define FREE mfree
#endif

typedef struct {
	size_t total_memory;
	size_t frame_memory;
	int total_allocations;
	int frame_allocations;
	Hashmap memory_map;
} Allocator;

typedef struct {
	const char* file;
	const char* func;
	uint64_t line;
} AllocationDetails;

typedef struct {
	size_t size;
	char file[128];
	char func[64];
	uint64_t line;
} AllocationEntry;

void allocator_attach(Allocator* allocator);

void* mmalloc(size_t size, AllocationDetails details);
void* mcalloc(size_t count, size_t size);
void* mrealloc(void* ptr, size_t size);
void mfree(void* ptr);

#endif
