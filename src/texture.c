#include "texture.h"
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image/stb_image.h>

texture* texture_create(char* path, GLenum input_format) {
	texture* t = malloc(sizeof(texture));

	glGenTextures(1, &t->id);
	glBindTexture(GL_TEXTURE_2D, t->id);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	stbi_set_flip_vertically_on_load(true);

	int nrChannels;
	unsigned char* data = stbi_load(path, &t->width, &t->height, &nrChannels, 0);

	if (!data) {
		fprintf(stderr, "texture: Failed to open image %s\n", path);
		return NULL;
	}

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, t->width, t->height, 0, input_format, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(data);
	return t;
}

void texture_delete(texture* t) {
	glDeleteTextures(1, &t->id);
	free(t);
}

void texture_generate_mipmap(texture* t) {
	glBindTexture(GL_TEXTURE_2D, t->id);
	glGenerateMipmap(GL_TEXTURE_2D);
}

void texture_use(texture* t, unsigned int unit_index) {
	glActiveTexture(GL_TEXTURE0 + unit_index);
	glBindTexture(GL_TEXTURE_2D, t->id);
}
