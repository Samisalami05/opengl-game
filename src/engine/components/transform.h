#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "math/vec3.h"

typedef struct {
	vec3 position;
	vec3 rotation;
	vec3 scale;

	// TODO: Add parent and children
} transform;

void transform_init(void* v);
void transform_deinit(void* v);

void transform_update(void* v, float deltatime);
void transform_render(void* v);
void transform_render_debug(void* v);

#endif
