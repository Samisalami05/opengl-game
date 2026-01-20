#include "pipeline.h"
#include "rendering/rendertexture.h"
#include <stdint.h>
#include <stdlib.h>
#include <wctype.h>

static void init_render_targets(render_pipeline* pipeline, uint32_t width, uint32_t height) {
	render_texture_init(&pipeline->targets.shadow_map, width, height, RENDER_TEX_COLOR);
	render_texture_init(&pipeline->targets.lighting_result, width, height, RENDER_TEX_COLOR);
}

static void add_render_passes(render_pipeline* pipeline) {
	// Init render passes here
}

void render_pipeline_init(render_pipeline* pipeline, uint32_t width, uint32_t height) {
	pipeline->count = 1;
	pipeline->passes = malloc(sizeof(render_pass) * pipeline->count);
	add_render_passes(pipeline);
	init_render_targets(pipeline, width, height);
}

void render_pipeline_deinit(render_pipeline* pipeline) {
	free(pipeline->passes);
}

void execute_render_passes(render_pipeline* pipeline, render_context* context) {
	for (int i = 0; i < pipeline->count; i++) {
		render_pass* pass = &pipeline->passes[i];
		pass->execute(pass, context, &pipeline->targets);
	}
}

void render_pipeline_resize(render_pipeline* pipeline, uint32_t width, uint32_t height) {
	for (int i = 0; i < pipeline->count; i++) {
		render_pass* pass = &pipeline->passes[i];
		pass->resize(pass, width, height);
	}
}

