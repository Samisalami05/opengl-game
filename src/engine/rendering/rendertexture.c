#include "rendertexture.h"
#include <GL/glext.h>
#include <stdint.h>
#include <stdio.h>

static uint32_t get_format(rt_type type) {
	switch (type) {
		case RENDER_TEX_COLOR: return GL_RGBA;
		case RENDER_TEX_DEPTH: return GL_DEPTH_COMPONENT24;
		case RENDER_TEX_DEPTH_STENCIL: return GL_DEPTH24_STENCIL8;
	}
}

void render_texture_init(render_texture* rt, uint32_t width, uint32_t height, rt_type type) {
	glGenTextures(1, &rt->handle);
	glBindTexture(GL_TEXTURE_2D, rt->handle);

	rt->width = width;
	rt->height = height;
	rt->type = type;
	rt->target = GL_TEXTURE_2D;
	rt->format = get_format(type);

	glTexImage2D(GL_TEXTURE_2D, 0, rt->format, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

void render_texture_deinit(render_texture* rt) {
	glDeleteTextures(1, &rt->handle);
	rt->handle = 0;
}

void render_texture_resize(render_texture* rt, uint32_t width, uint32_t height) {
	rt->width  = width;
	rt->height = height;

	glBindTexture(GL_TEXTURE_2D, rt->handle);

	glTexImage2D(GL_TEXTURE_2D, 0, rt->format,
	             width, height, 0,
	             rt->type == RENDER_TEX_COLOR ? GL_RGBA : GL_DEPTH_COMPONENT,
	             GL_UNSIGNED_BYTE, NULL);
}

void render_texture_bind(render_texture* rt, uint32_t unit) {
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(rt->target, rt->handle);
}

