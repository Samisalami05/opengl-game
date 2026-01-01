#include "ssbo.h"

void ssbo_init(ssbo* ssbo, size_t capacity, GLenum usage) {
	glGenBuffers(1, &ssbo->handle);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo->handle);
	glBufferData(GL_SHADER_STORAGE_BUFFER, capacity, NULL, usage);
}

void ssbo_deinit(ssbo* ssbo) {
	glDeleteBuffers(1, &ssbo->handle);
}

void ssbo_set_data(ssbo* ssbo, size_t offset, size_t size, void* data) {
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset, size, data);
}

void ssbo_bind(ssbo* ssbo, unsigned int binding) {
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding, ssbo->handle);
}
