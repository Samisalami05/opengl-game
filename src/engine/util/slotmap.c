#include "slotmap.h"
#include "allocator.h"
#include "util/ringbuffer.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static uint8_t expand_if_necessary(slotmap* sm) {
	if (sm->allocated <= sm->count) {
		sm->allocated = sm->allocated <= 0 ? 4 : sm->allocated * 2;
		void* tmp = REALLOC(sm->data, sm->elem_size * sm->allocated);
		if (tmp == NULL) {
			perror("slotmap: realloc");
			return 1;
		}
		sm->data = tmp;

		tmp = REALLOC(sm->occupied_bits, (sm->allocated + 63) / 64 * sizeof(uint64_t));
		if (tmp == NULL) {
			perror("slotmap: realloc");
			return 1;
		}
		sm->occupied_bits = tmp;
	}
	return 0;
}

static uint8_t slot_is_occupied(slotmap* sm, uint64_t index) {
	uint64_t bits = sm->occupied_bits[index / 64];
	return (bits >> index) & 1ULL;
}

static void slot_set_occupied(slotmap* sm, uint64_t index) {
	uint64_t* bits = &sm->occupied_bits[index / 64];
	*bits |= (1ULL << index);
}

static void slot_set_free(slotmap* sm, uint64_t index) {
	uint64_t* bits = &sm->occupied_bits[index / 64];
	*bits &= ~(1ULL << index);
}

void slotmap_init(slotmap* sm, size_t elem_size) {
	sm->data = NULL;
	sm->occupied_bits = NULL;
	sm->allocated = 0;
	sm->count = 0;
	sm->elem_size = elem_size;
	ringbuffer_init(&sm->free, sizeof(uint64_t));
}

void slotmap_deinit(slotmap* sm) {
	ringbuffer_deinit(&sm->free);
	FREE(sm->data);
	FREE(sm->occupied_bits);
}

// Returns slot of added value
uint64_t slotmap_add(slotmap* sm, void* value) {
	if (sm->free.count > 0) {
		uint64_t index = *(uint64_t*)ringbuffer_peek(&sm->free);
		memcpy(sm->data + index * sm->elem_size, value, sm->elem_size);
		slot_set_occupied(sm, index);
		ringbuffer_remove(&sm->free);
		return index;
	}

	expand_if_necessary(sm);
	memcpy(sm->data + sm->count * sm->elem_size, value, sm->elem_size);
	slot_set_occupied(sm, sm->count);
	sm->count++;
	return sm->count - 1;
}

void slotmap_remove(slotmap* sm, uint64_t slot) {
	if (!slot_is_occupied(sm, slot)) {
		fprintf(stderr, "slotmap: Cant remove element from slot: Slot is already empty\n");
		return;
	}

	slot_set_free(sm, slot);
	if (slot < sm->count - 1) {
		ringbuffer_append(&sm->free, &slot);
	}
}

uint8_t slotmap_is_occupied(slotmap* sm, uint64_t slot) {
	return slot_is_occupied(sm, slot);
}

