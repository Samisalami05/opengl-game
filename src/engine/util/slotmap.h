#ifndef SLOTMAP_H
#define SLOTMAP_H

#include "ringbuffer.h"
#include <stddef.h>
#include <stdint.h>

typedef struct {
	uint8_t* data;
	size_t allocated;
	size_t count;
	size_t elem_size;
	ringbuffer free;
	uint64_t* occupied_bits;
} slotmap;

void slotmap_init(slotmap* sm, size_t elem_size);
void slotmap_deinit(slotmap* sm);

uint64_t slotmap_add(slotmap* sm, void* value);
void slotmap_remove(slotmap* sm, uint64_t slot);
uint8_t slotmap_is_occupied(slotmap* sm, uint64_t slot);

#endif
