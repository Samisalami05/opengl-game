#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>

typedef struct texture {
	unsigned int id;

	int width;
	int height;
} texture;

void texture_init(texture* t, const char* path);
void texture_deinit(texture* t);
void texture_generate_mipmap(texture* t);
void texture_use(texture* t, unsigned int unit_index);

#endif
