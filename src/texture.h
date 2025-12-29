#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>

typedef struct texture {
	unsigned int id;

	int width;
	int height;
} texture;

texture* texture_create(char* path, GLenum input_format);
void texture_delete(texture* t);
void texture_generate_mipmap(texture* t);
void texture_use(texture* t, unsigned int unit_index);

#endif
