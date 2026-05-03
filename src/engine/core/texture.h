#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>
#include <stdbool.h>

typedef struct texture {
	unsigned int id;

	int width;
	int height;
} texture;

bool texture_init(texture* t, const char* path);
void texture_deinit(texture* t);
void texture_generate_mipmap(texture* t);
void texture_use(const texture* t, unsigned int unit_index);

#endif
