#include "texture.h"
#include <stdbool.h>
#include <stdio.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image/stb_image.h>

static GLenum texture_format(int channel_count) {
	switch (channel_count) {
		case 1: return GL_R;
		case 2: return GL_RG;
		case 3: return GL_RGB;
		case 4: return GL_RGBA;
		default:
			fprintf(stderr, "No texture format available for channel count %d\n", channel_count);
	}

	return GL_RGB; // Default
}

void texture_init(texture* t, const char* path) {
	glGenTextures(1, &t->id);
	glBindTexture(GL_TEXTURE_2D, t->id);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	stbi_set_flip_vertically_on_load(true);

	int channel_count;
	unsigned char* data = stbi_load(path, &t->width, &t->height, &channel_count, 0);

	if (!data) {
		fprintf(stderr, "texture: Failed to open image %s\n", path);
		return;
	}

	GLenum format = texture_format(channel_count);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, t->width, t->height, 0, format, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(data);
}

void texture_deinit(texture* t) {
	glDeleteTextures(1, &t->id);
}

void texture_generate_mipmap(texture* t) {
	glBindTexture(GL_TEXTURE_2D, t->id);
	glGenerateMipmap(GL_TEXTURE_2D);
}

void texture_use(texture* t, unsigned int unit_index) {
	glActiveTexture(GL_TEXTURE0 + unit_index);
	glBindTexture(GL_TEXTURE_2D, t->id);
}
