#ifndef DEPTH_PASS_H
#define DEPTH_PASS_H

#include "rendering/pipeline.h"
#include <stdint.h>

uint8_t depth_pass_init(render_pass* rp, render_targets* targets);
void depth_pass_execute(render_pass* rp, render_context* context, render_targets* targets);
void depth_pass_resize(render_pass* rp, uint32_t width, uint32_t height);
void depth_pass_deinit(render_pass* rp);

#endif
