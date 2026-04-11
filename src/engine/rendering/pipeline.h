#ifndef PIPELINE_H
#define PIPELINE_H

#include "modelloader.h"
#include "camera.h"
#include "rendertexture.h"
#include "util/arraylist.h"
#include <stdint.h>
#include <sys/types.h>


/*
 * Shadow pass – render depth from light POV
 * Geometry pass – fill G-buffer (for deferred)
 * Lighting pass – apply lights 
 * Forward pass – transparent objects, particles 
 * Post-process pass – bloom, tone mapping, FXAA 
 * UI pass
 */

typedef struct {
	camera* camera;
	model* drawlist;
	uint32_t draw_count;
	uint32_t frame_index;
} render_context;

// Stores shared targets among passes
typedef struct {
	render_texture opaque_tex;
	render_texture depth_tex;
	render_texture shadow_map;
	render_texture lighting_result;
} render_targets;

typedef struct render_pass {
	char name[32];
	uint32_t width;
	uint32_t height;

	uint32_t fbo;
	render_texture color_tex; // TODO: should add more
	
	void (*execute)(struct render_pass* rp, render_context* context, render_targets* targets);
	void (*resize)(struct render_pass* rp, uint32_t width, uint32_t height); // glViewport(0, 0, rp->width, rp->height);
	void (*deinit)(struct render_pass* rp);

	void* data;
} render_pass;

typedef struct {
	render_pass* passes;
	uint32_t count;
	render_targets targets;
} render_pipeline;

uint8_t render_pipeline_init(render_pipeline* pipeline, uint32_t width, uint32_t height);
void render_pipeline_deinit(render_pipeline* pipeline);
void execute_render_passes(render_pipeline* pipeline, render_context* context);
void render_pipeline_resize(render_pipeline* pipeline, uint32_t width, uint32_t height);

#endif
