#include "pipeline.h"
#include "rendering/passes/debug_pass.h"
#include "rendering/passes/depth_pass.h"
#include "rendering/passes/final_pass.h"
#include "rendering/passes/opaque_pass.h"
#include "rendering/rendertexture.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

static void init_render_targets(render_pipeline* pipeline, uint32_t width, uint32_t height) {
	render_texture_init(&pipeline->targets.opaque_tex, width, height, RENDER_TEX_COLOR);
	render_texture_init(&pipeline->targets.depth_tex, width, height, RENDER_TEX_DEPTH);
	render_texture_init(&pipeline->targets.shadow_map, width, height, RENDER_TEX_COLOR);
	render_texture_init(&pipeline->targets.lighting_result, width, height, RENDER_TEX_COLOR);
}

static void deinit_render_targets(render_targets* targets) {
	render_texture_deinit(&targets->opaque_tex);
	render_texture_deinit(&targets->depth_tex);
	render_texture_deinit(&targets->shadow_map);
	render_texture_deinit(&targets->lighting_result);
}

static void resize_render_targets(render_targets* targets, uint32_t width, uint32_t height) {
	render_texture_resize(&targets->opaque_tex, width, height);
	render_texture_resize(&targets->depth_tex, width, height);
	render_texture_resize(&targets->shadow_map, width, height);
	render_texture_resize(&targets->lighting_result, width, height);
}

static int init_render_passes(render_pipeline* pipeline) {
	// Init render passes here
	if (depth_pass_init(&pipeline->passes[0], &pipeline->targets)) return 1;
	if (opaque_pass_init(&pipeline->passes[1], &pipeline->targets)) return 2;
	if (debug_pass_init(&pipeline->passes[2], &pipeline->targets)) return 3;
	if (final_pass_init(&pipeline->passes[3])) return 4;
	return 0;
}

uint8_t render_pipeline_init(render_pipeline* pipeline, uint32_t width, uint32_t height) {
	pipeline->count = 4;
	pipeline->passes = malloc(sizeof(render_pass) * pipeline->count);

	init_render_targets(pipeline, width, height);
	int status = init_render_passes(pipeline);
	if (status > 0) {
		fprintf(stderr, "render pipeline: Failed to initialize pass '%s'\n", pipeline->passes[status - 1].name);
		return 1;
	}
	return 0;
}

void render_pipeline_deinit(render_pipeline* pipeline) {
	for (int i = 0; i < pipeline->count; i++) {
		render_pass* pass = &pipeline->passes[i];
		pass->deinit(pass);
	}
	deinit_render_targets(&pipeline->targets);
	free(pipeline->passes); // TODO: deinit render targets
}

void execute_render_passes(render_pipeline* pipeline, render_context* context) {
	for (int i = 0; i < pipeline->count; i++) {
		render_pass* pass = &pipeline->passes[i];
		pass->execute(pass, context, &pipeline->targets);
	}
}

void render_pipeline_resize(render_pipeline* pipeline, uint32_t width, uint32_t height) {
	resize_render_targets(&pipeline->targets, width, height);
	for (int i = 0; i < pipeline->count; i++) {
		render_pass* pass = &pipeline->passes[i];
		printf("Resizing pass: %s\n", pass->name);
		pass->resize(pass, width, height);
	}
}

