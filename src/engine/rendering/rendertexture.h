#ifndef RENDERTEXTURE_H
#define RENDERTEXTURE_H

#include <glad/glad.h>
#include <stdint.h>

typedef enum {
	RENDER_TEX_COLOR,
	RENDER_TEX_DEPTH,
	RENDER_TEX_DEPTH_STENCIL
} rt_type;

typedef struct {
	uint32_t handle;
	uint32_t width;
	uint32_t height;
	uint32_t format;
	uint32_t target; // Default to GL_TEXTURE_2D
	rt_type type;
} render_texture;

void render_texture_init(render_texture* rt, uint32_t width, uint32_t height, rt_type type);
void render_texture_deinit(render_texture* rt);

void render_texture_resize(render_texture* rt, uint32_t width, uint32_t height);
void render_texture_bind(render_texture* rt, uint32_t unit);

#endif
