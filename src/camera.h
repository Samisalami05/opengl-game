#ifndef CAMERA_H
#define CAMERA_H

#include "vec3.h"
#include "mat4.h"

typedef struct camera {
	vec3 pos;
	vec3 rot;

	float fov;
	float width;
	float height;

	float near_clip;
	float far_clip;
} camera;

void camera_init(camera* cam, float width, float height);

mat4 camera_view(camera* cam);

mat4 camera_proj(camera* cam);

#endif
