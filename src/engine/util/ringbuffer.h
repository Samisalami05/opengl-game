#ifndef RINGBUFFER_H
#define RINGBUFFER_H

#include <stddef.h>
#include <stdint.h>
typedef struct {
	void* data;
	size_t allocated;
	size_t count;
	uint32_t start;
	size_t v_size;
} ringbuffer;

void ringbuffer_init(ringbuffer* rb, size_t v_size);
void ringbuffer_deinit(ringbuffer* rb);

void ringbuffer_append(ringbuffer* rb, void* v);
void* ringbuffer_remove_copy(ringbuffer* rb);
void* ringbuffer_remove(ringbuffer* rb);
void* ringbuffer_peek(ringbuffer* rb);

#endif
