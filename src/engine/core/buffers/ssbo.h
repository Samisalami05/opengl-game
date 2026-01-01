#ifndef SSBO_H
#define SSBO_H

#include <stddef.h>
#include <glad/glad.h>

typedef struct ssbo {
	unsigned int handle;
	size_t capacity;
	GLenum usage;
} ssbo;

void ssbo_init(ssbo* ssbo, size_t capacity, GLenum usage);
void ssbo_deinit(ssbo* ssbo);
void ssbo_set_data(ssbo* ssbo, size_t offset, size_t size, void* data);
void ssbo_bind(ssbo* ssbo, unsigned int binding);

#endif
