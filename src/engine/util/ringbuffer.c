#include "ringbuffer.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void ringbuffer_init(ringbuffer* rb, size_t v_size) {
	rb->data = NULL;
	rb->allocated = 0;
	rb->count = 0;
	rb->start = 0;
	rb->v_size = v_size;
}

void ringbuffer_deinit(ringbuffer* rb) {
	free(rb->data);
}

static size_t calc_new_allocated(ringbuffer* rb, uint32_t count) {
	size_t allocated = rb->allocated;
	while (allocated + rb->start <= rb->count + count) {
		allocated = allocated != 0 ? allocated * 2 : 4;
	}
	return allocated;
}

static void expand(ringbuffer* rb, uint32_t count) {
	if (rb->allocated > rb->count + count) return;
	
	rb->allocated = calc_new_allocated(rb, rb->start);
	void* tmp = realloc(rb->data, rb->allocated * rb->v_size);
	if (tmp == NULL) {
		perror("ringbuffer: realloc");
		return;
	}
	rb->data = tmp;

	if (rb->start == 0) return;
	memcpy((uint8_t*)rb->data + (rb->allocated - rb->start) * rb->v_size, (uint8_t*)rb->data, rb->start * rb->v_size);
	memcpy((uint8_t*)rb->data, (uint8_t*)rb->data + rb->start * rb->v_size, rb->count * rb->v_size);
	rb->start = 0;
}

// [3, 5, 1, 2, 4, _]

static uint32_t get_end(ringbuffer* rb) {
	return (rb->start + rb->count) % rb->allocated;
}

void ringbuffer_append(ringbuffer* rb, void* v) {
	expand(rb, 1);
	
	uint32_t end = get_end(rb);
	memcpy((uint8_t*)rb->data + end * rb->v_size, v, rb->v_size);
	rb->count++;
}

void* ringbuffer_remove_copy(ringbuffer* rb) {
	if (rb->count <= 0) {
		fprintf(stderr, "ringbuffer: Cant remove element from buffer: The buffer is empty\n");
		return NULL;
	}
	rb->count--;

	void* val = malloc(rb->v_size);
	memcpy(val, (uint8_t*)rb->data + rb->start * rb->v_size, rb->v_size);
	rb->start = (rb->start + 1) % rb->allocated;
	return val;
}

void* ringbuffer_remove(ringbuffer* rb) {
	if (rb->count <= 0) {
		fprintf(stderr, "ringbuffer: Cant remove element from buffer: The buffer is empty\n");
		return NULL;
	}
	rb->count--;

	uint8_t* val = (uint8_t*)rb->data + rb->start * rb->v_size;
	rb->start = (rb->start + 1) % rb->allocated;
	return val;
}

void* ringbuffer_peek(ringbuffer* rb) {
	if (rb->count <= 0) return NULL;
	return (uint8_t*)rb->data + rb->start * rb->v_size;
}
