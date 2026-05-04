#include "allocator.h"
#include "util/hashmap.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static Allocator* alloc;

size_t ptr_hash(const void* key) {
	const intptr_t* ptr = key;
	return (size_t)*ptr;
}

void allocator_attach(Allocator* allocator) {
	hashmap_init(&allocator->memory_map, sizeof(intptr_t), sizeof(AllocationEntry), ptr_hash);
	alloc = allocator;
}

void* mmalloc(size_t size, AllocationDetails details) {
	void* ptr = malloc(size);
	if (alloc == NULL) return ptr;

	alloc->total_allocations++;
	AllocationEntry entry = {0};
	entry.size = size;
	entry.line = details.line;
	strncpy(entry.file, details.file, 128);
	strncpy(entry.func, details.func, 64);

	printf("func: %s\n", details.func);

	printf("Adding allocation %ld\n", (intptr_t)ptr);
	intptr_t key = (intptr_t)ptr;
	hashmap_put(&alloc->memory_map, &key, &entry);
	printf("Done\n");

	return ptr;
}

void* mcalloc(size_t count, size_t size) {
	void* ptr = calloc(count, size);
	return ptr;
	
	if (alloc == NULL) return ptr;

	alloc->total_allocations++;
	hashmap_put(&alloc->memory_map, (intptr_t*)ptr, &size);

	return ptr;
}

void* mrealloc(void* ptr, size_t size) {
	void* p = realloc(ptr, size);
	return p;
	if (alloc == NULL) return p;

	alloc->total_allocations++;
	size_t* curr = hashmap_get(&alloc->memory_map, (intptr_t*)ptr);
	if (curr != NULL) {
		hashmap_remove(&alloc->memory_map, (intptr_t*)ptr);
	}
	hashmap_put(&alloc->memory_map, (intptr_t*)p, &size);
	
	return p;
}

void mfree(void* ptr) {
	free(ptr);
	return;
	if (alloc == NULL) return;

	alloc->total_allocations--;
	hashmap_remove(&alloc->memory_map, (intptr_t*)ptr);
}
