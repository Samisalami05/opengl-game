#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <stdint.h>

typedef struct {
	uint32_t handle;
} framebuffer;

void framebuffer_init(framebuffer* fbo);
void framebuffer_deinit(framebuffer* fbo);

#endif
