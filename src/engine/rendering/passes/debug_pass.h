#ifndef DEBUG_PASS_H
#define DEBUG_PASS_H

#include "rendering/pipeline.h"
#include <stdint.h>

uint8_t debug_pass_init(render_pass* rp, render_targets* targets);
void debug_pass_execute(render_pass* rp, render_context* context, render_targets* targets);
void debug_pass_resize(render_pass* rp, uint32_t width, uint32_t height);
void debug_pass_deinit(render_pass* rp);

uint32_t debug_pass_base();
uint8_t debug_pass_add_vertices(vec3* vertices, uint32_t count);
uint8_t debug_pass_add_indices(uint32_t* indices, uint32_t count);

#endif
