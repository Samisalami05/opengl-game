#ifndef PIPELINE_H
#define PIPELINE_H

#include <stdint.h>

/*
 * Shadow pass – render depth from light POV
 * Geometry pass – fill G-buffer (for deferred)
 * Lighting pass – apply lights 
 * Forward pass – transparent objects, particles 
 * Post-process pass – bloom, tone mapping, FXAA 
 * UI pass
 */

// Stores shared targets among passes
typedef struct {
	uint32_t shadow_map; // Example
	uint32_t lighting_result;
} rendertargets;

typedef struct {
	const char* name;

	uint32_t fbo;
	uint32_t color_tex;
	uint32_t depth_tex;

	void* data;
} renderpass;

typedef struct {
	renderpass* passes;
	uint32_t count;
} renderpipeline;

#endif
