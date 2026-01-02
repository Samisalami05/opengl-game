#include "cubemap.h"
#include "core/texture.h"
#include <stdint.h>
#include <stdio.h>
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

void cubemap_init(cubemap* cm, const char** faces) {
	glGenTextures(1, &cm->id);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cm->id);

	int channel_count, width, height;
	for (int i = 0; i < 6; i++) {
		uint8_t* data = stbi_load(faces[i], &width, &height, &channel_count, 0);
		if (!data) {
			fprintf(stderr, "cubemap: Failed to load image %s\n", faces[i]);
			stbi_image_free(data);
			continue;
		}
		
		GLenum format = texture_format(channel_count);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 
				0, GL_RGB, width, height, 0, format, GL_UNSIGNED_BYTE, data
		);
		stbi_image_free(data);
	}

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

void cubemap_deinit(cubemap* cm) {
	glDeleteTextures(1, &cm->id);
}

// TODO: dont know if this is possible
void cubemap_generate_mipmap(cubemap* cm) {
	glBindTexture(GL_TEXTURE_CUBE_MAP, cm->id);
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
}


void cubemap_use(cubemap* cm, unsigned int unit_index) {
	glActiveTexture(GL_TEXTURE0 + unit_index);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cm->id);
}
