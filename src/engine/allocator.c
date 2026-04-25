#include "allocator.h"
#include <stdlib.h>

static Allocator* alloc;

void allocator_attach(Allocator* allocator) {
	alloc = allocator;
}

void* mmalloc(size_t size) {
	alloc->total_allocations++;
	return malloc(size);
}

void* mcalloc(size_t count, size_t size) {
	alloc->total_allocations++;
	return calloc(count, size);
}

void* mrealloc(void* ptr, size_t size) {
	alloc->total_allocations++;
	return realloc(ptr, size);
}

void mfree(void* ptr) {
	alloc->total_allocations--;
	free(ptr);
}
