#ifndef ARRAYLIST_H
#define ARRAYLIST_H

#include <stddef.h>
#include <stdint.h>

typedef struct arraylist {
	uint8_t* data;
	size_t allocated;
	size_t stride;
	uint32_t count;
} arraylist;

void arraylist_init(arraylist* a, size_t stride);
void arraylist_deinit(arraylist* a);

uint8_t arraylist_append(arraylist* a, void* data);
uint8_t arraylist_set(arraylist* a, void* data, uint64_t index);
void* arraylist_get(arraylist* a, uint64_t index);
void* arraylist_get_last(arraylist* a);
uint8_t arraylist_remove(arraylist* a, uint64_t index);
uint8_t arraylist_remove_last(arraylist* a);

#endif
