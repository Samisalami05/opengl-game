#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include "stack_trace.h"
#include "util/hashmap.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#ifndef MALLOC
#define MALLOC(size) mmalloc(size, (AllocationDetails){ __FILE__, __func__, __LINE__})
#endif

#ifndef CALLOC
#define CALLOC(count, size) mcalloc(count, size, (AllocationDetails){ __FILE__, __func__, __LINE__})
#endif

#ifndef REALLOC
#define REALLOC(ptr, size) mrealloc(ptr, size, (AllocationDetails){ __FILE__, __func__, __LINE__})
#endif

#ifndef FREE
#define FREE(ptr) mfree(ptr, (AllocationDetails){ __FILE__, __func__, __LINE__})
#endif

typedef struct {
	Hashmap memory_map;
	bool dirty;
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
	StackTrace trace;
} AllocationEntry;

void allocator_attach(Allocator* allocator);

void* mmalloc(size_t size, AllocationDetails details);
void* mcalloc(size_t count, size_t size, AllocationDetails details);
void* mrealloc(void* ptr, size_t size, AllocationDetails details);
void mfree(void* ptr, AllocationDetails details);

#endif
