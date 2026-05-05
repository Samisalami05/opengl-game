#include "allocator.h"
#include "stack_trace.h"
#include "util/hashmap.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static Allocator* alloc = NULL;

size_t ptr_hash(const void* key) {
	const intptr_t* ptr = key;
	return (size_t)*ptr;
}

void allocator_attach(Allocator* allocator) {
	hashmap_init(&allocator->memory_map, sizeof(intptr_t), sizeof(AllocationEntry), ptr_hash);
	allocator->dirty = false;
	alloc = allocator;
}

static int file_offset(const char* path, int slash_count) {
	int pos = strlen(path) - 1;
	while (slash_count > 0 && pos >= 0) {
		if (path[pos] == '/' || path[pos] == '\\') slash_count--;
		if (slash_count == 0) break;
		pos--;
	}
	if (path[pos] != '/' && path[pos] != '\\') return pos;
	return pos + 1;
}

void* mmalloc(size_t size, AllocationDetails details) {
	void* ptr = malloc(size);
	if (alloc == NULL) return ptr;
	alloc->dirty = true;

	AllocationEntry entry = {0};
	entry.size = size;
	entry.line = details.line;
	strncpy(entry.file, details.file + file_offset(details.file, 2), 128);
	strncpy(entry.func, details.func, 64);
	stacktrace_capture(&entry.trace);

	intptr_t key = (intptr_t)ptr;
	hashmap_put(&alloc->memory_map, &key, &entry);

	return ptr;
}

void* mcalloc(size_t count, size_t size, AllocationDetails details) {
	void* ptr = calloc(count, size);
	
	if (alloc == NULL) return ptr;
	alloc->dirty = true;

	AllocationEntry entry = {0};
	entry.size = size;
	entry.line = details.line;
	strncpy(entry.file, details.file + file_offset(details.file, 2), 128);
	strncpy(entry.func, details.func, 64);
	stacktrace_capture(&entry.trace);

	intptr_t key = (intptr_t)ptr;
	hashmap_put(&alloc->memory_map, &key, &entry);

	return ptr;
}

void* mrealloc(void* ptr, size_t size, AllocationDetails details) {
	void* p = realloc(ptr, size);
	
	if (alloc == NULL) return p;
	alloc->dirty = true;

	AllocationEntry entry = {0};
	entry.size = size;
	entry.line = details.line;
	strncpy(entry.file, details.file + file_offset(details.file, 2), 128);
	strncpy(entry.func, details.func, 64);
	stacktrace_capture(&entry.trace);

	intptr_t key = (intptr_t)ptr;
	if (hashmap_get(&alloc->memory_map, &key) != NULL) {
		hashmap_remove(&alloc->memory_map, &key);
	}

	intptr_t new = (intptr_t)p;
	hashmap_put(&alloc->memory_map, &new, &entry);
	
	return p;
}

void mfree(void* ptr, AllocationDetails details) {
	if (alloc != NULL) {
		alloc->dirty = true;

		intptr_t key = (intptr_t)ptr;
		hashmap_remove(&alloc->memory_map, &key);
	}
	free(ptr);
}
