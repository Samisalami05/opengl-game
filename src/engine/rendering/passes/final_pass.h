#ifndef FINAL_PASS_H
#define FINAL_PASS_H

#include "rendering/pipeline.h"
#include <stdint.h>

void final_pass_init(render_pass* rp);
void final_pass_execute(render_pass* rp, render_context* context, render_targets* targets);
void final_pass_resize(render_pass* rp, uint32_t width, uint32_t height);
void final_pass_deinit(render_pass* rp);

#endif
