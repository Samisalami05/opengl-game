#ifndef CAMERA_H
#define CAMERA_H

#include "math/vec3.h"
#include "math/mat4.h"

typedef struct camera {
	vec3 pos;
	float pitch;
	float yaw;

	float fov;
	float width;
	float height;

	float near_clip;
	float far_clip;
} camera;

void camera_init(camera* cam, float width, float height);

mat4 camera_view(camera* cam);
mat4 camera_view_no_translate(camera* cam);

mat4 camera_proj(camera* cam);

vec3 camera_forward(const camera cam);
vec3 camera_right(const camera cam);
vec3 camera_up(const camera cam);

void camera_mouse_input(camera* cam, double dx, double dy);
void camera_key_input(camera* cam);

#endif
