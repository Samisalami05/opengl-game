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
void arraylist_append(arraylist* a, void* data);
void arraylist_deinit(arraylist* a);

#endif
