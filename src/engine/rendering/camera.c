#include "camera.h"
#include "math/mat4.h"
#include <math.h>
#include <string.h>
#include "math/vec3.h"
#include "util/util.h"

const float sensitivity = 0.005f;
const float camera_speed = 10.0f;

void camera_init(camera* cam, float width, float height) {
	cam->pos = (vec3){0, 0, 0};
	cam->pitch = 0.0f;
	cam->yaw = 0.0f;

	cam->fov = 60;
	cam->width = width;
	cam->height = height;

	cam->near_clip = 0.1;
	cam->far_clip = 200;
}

mat4 camera_view(camera* cam) {
	vec3 forward = camera_forward(*cam);
	vec3 dest = vec3_add_v3(cam->pos, forward);
	return mat4_lookat(cam->pos, dest);
}

mat4 camera_view_no_translate(camera* cam) {
	vec3 forward = camera_forward(*cam);
	return mat4_lookat((vec3){0}, forward);
}

mat4 camera_proj(camera* cam) {
	mat4 proj;
	memset(proj.data, 0, 16 * sizeof(float));
	mat4_projection(
		&proj, 
		cam->fov, 
		cam->width / cam->height, 
		cam->near_clip, cam->far_clip
	);
	return proj;
}

vec3 camera_forward(const camera cam) {
	return vec3_normalized((vec3){
		cosf(cam.pitch) * sinf(cam.yaw),
		sinf(cam.pitch),
		-cosf(cam.pitch) * cosf(cam.yaw)
	});
}

vec3 camera_right(const camera cam) {
    vec3 f = camera_forward(cam);
    vec3 up = {0.0f, 1.0f, 0.0f};
    return vec3_normalized(vec3_cross(f, up));
}

vec3 camera_up(const camera cam) {
    vec3 right = camera_right(cam);
    vec3 forward = camera_forward(cam);
    return vec3_normalized(vec3_cross(right, forward));
}

void camera_mouse_input(camera* cam, double dx, double dy) {
	dx *= sensitivity;
	dy *= sensitivity;

	cam->yaw += dx;
	cam->pitch = clampf(cam->pitch - dy, -M_PI / 2, M_PI / 2);
}

void camera_key_input(camera* cam) {

}
