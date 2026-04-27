#include "pipeline.h"
#include "GLFW/glfw3.h"
#include "allocator.h"
#include "core/shader.h"
#include "profiler.h"
#include "rendering/passes/debug_pass.h"
#include "rendering/passes/depth_pass.h"
#include "rendering/passes/final_pass.h"
#include "rendering/passes/opaque_pass.h"
#include "rendering/rendertexture.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

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
	if (render_pipeline_register(pipeline, depth_pass_init) == -1) return 1;
	if (render_pipeline_register(pipeline, opaque_pass_init) == -1) return 1;
	if (render_pipeline_register(pipeline, debug_pass_init) == -1) return 1;
	if (render_pipeline_register(pipeline, final_pass_init) == -1) return 1;
	return 0;
}

uint8_t render_pipeline_init(render_pipeline* pipeline, uint32_t width, uint32_t height) {
	init_render_targets(pipeline, width, height);
	if (init_render_passes(pipeline)) return 1;
	
	return 0;
}

void render_pipeline_deinit(render_pipeline* pipeline) {
	for (int i = 0; i < pipeline->count; i++) {
		render_pass* pass = &pipeline->passes[i];
		pass->deinit(pass);
	}
	deinit_render_targets(&pipeline->targets);
	FREE(pipeline->passes); // TODO: deinit render targets
}

int32_t render_pipeline_register(render_pipeline* pipeline, pass_init_func pass_init) {
	if (pipeline->count + 1 > pipeline->capacity) {
		pipeline->capacity = pipeline->capacity == 0 ? 4 : pipeline->capacity * 2;
		void* tmp = REALLOC(pipeline->passes, pipeline->capacity * sizeof(render_pass)); // TODO: maybe set data to zero
		pipeline->passes = tmp;
	}

	if (pass_init(&pipeline->passes[pipeline->count], &pipeline->targets)) {
		fprintf(stderr, "render pipeline: Failed to register render pass with id %d: Failed to initialize\n", pipeline->count);
		return -1;
	}
	profiler_query_init(&pipeline->passes[pipeline->count].query);
	
	pipeline->count++;
	return pipeline->count - 1;
}

void execute_render_passes(render_pipeline* pipeline, render_context* context) {
	for (int i = 0; i < pipeline->count; i++) {
		render_pass* pass = &pipeline->passes[i];

		float gpu_time = profiler_query_get(&pass->query);
		if (gpu_time != -1.0f) {
			profiler_push_stat((ProfilerStat){
				.type = PROFILER_GPU_PASS_TIME,
				.id = i,
				.name = pass->name,
				.time = gpu_time,
			});
		}

		float cpu_before = glfwGetTime();
		profiler_query_begin(&pass->query);
		pass->execute(pass, context, &pipeline->targets);
		profiler_query_end(&pass->query);
		float cpu_time = (glfwGetTime() - cpu_before) * 1000;

		if (cpu_time < 0) cpu_time = 0;

		profiler_push_stat((ProfilerStat){
			.type = PROFILER_CPU_PASS_TIME,
			.id = i,
			.name = pass->name,
			.time = cpu_time,
		});
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

