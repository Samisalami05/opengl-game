#include "rendertexture.h"
#include <GL/gl.h>
#include <GL/glext.h>
#include <stdint.h>
#include <stdio.h>

typedef struct {
	uint32_t target;
	uint32_t internal_format;
	uint32_t format;
	uint32_t type;
} rt_data;

static rt_data get_data(rt_type type) {
	switch (type) {
		case RENDER_TEX_COLOR: 
			return (rt_data){
				.target = GL_TEXTURE_2D,
				.internal_format = GL_RGBA8,
				.format = GL_RGBA,
				.type = GL_UNSIGNED_BYTE,
			};
		case RENDER_TEX_DEPTH:
			return (rt_data){
				.target = GL_TEXTURE_2D,
				.internal_format = GL_DEPTH_COMPONENT24,
				.format = GL_DEPTH_COMPONENT,
				.type = GL_FLOAT,
			};
		case RENDER_TEX_DEPTH_STENCIL:
			return (rt_data){
				.target = GL_TEXTURE_2D,
				.internal_format = GL_DEPTH24_STENCIL8,
				.format = GL_DEPTH_STENCIL,
				.type = GL_UNSIGNED_BYTE, // TODO: maybe should be float
			};
	}
}

static void set_parameters(rt_type type) {
	if (type == RENDER_TEX_COLOR) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

void render_texture_init(render_texture* rt, uint32_t width, uint32_t height, rt_type type)
{
    glGenTextures(1, &rt->handle);
    glBindTexture(GL_TEXTURE_2D, rt->handle);

    rt->width  = width;
    rt->height = height;
    rt->type   = type;
    rt->target = GL_TEXTURE_2D;

	rt_data data = get_data(type); // TODO: use data


	rt->format = data.internal_format;
	glTexImage2D(
		data.target, 0,
		data.internal_format,
		width, height, 0,
        data.format,
        data.type,
        NULL
    );
	set_parameters(type);		
}

void render_texture_deinit(render_texture* rt) {
	glDeleteTextures(1, &rt->handle);
	rt->handle = 0;
}

void render_texture_resize(render_texture* rt, uint32_t width, uint32_t height) {
	rt->width  = width;
    rt->height = height;

    glBindTexture(GL_TEXTURE_2D, rt->handle);

	rt_data data = get_data(rt->type);

	glTexImage2D(
		data.target, 0,
		data.internal_format,
		width, height, 0,
        data.format,
        data.type,
        NULL
    );
}

void render_texture_bind(render_texture* rt, uint32_t unit) {
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(rt->target, rt->handle);
}

