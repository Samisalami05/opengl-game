#include "camera.h"
#include "mat4.h"
#include "math.h"
#include "vec3.h"

void camera_init(camera* cam, float width, float height) {
	cam->pos = (vec3){0, 0, 0};
	cam->rot = (vec3){0, 0, 0};

	cam->fov = 60;
	cam->width = width;
	cam->height = height;

	cam->near_clip = 0.1;
	cam->far_clip = 200;
}

mat4 camera_view(camera* cam) {
	mat4 view;
	mat4_identity(&view);
	mat4_rotate(&view, vec3_inverse(cam->rot));
	mat4_translate(&view, vec3_inverse(cam->pos));
	return view;
	//vec3 forward = camera_forward(*cam);
	//vec3 dest = {cam->pos.x, cam->pos.y, cam->pos.z};
	//vec3_add_v3(&dest, forward);
	//return mat4_lookat(cam->pos, dest);
}

mat4 camera_proj(camera* cam) {
	mat4 proj;
	mat4_identity(&proj);
	mat4_projection(
		&proj, 
		cam->fov, 
		cam->width, cam->height, 
		cam->near_clip, cam->far_clip
	);
	return proj;
}

vec3 camera_forward(const camera cam) {
	return (vec3){
		sin(cam.rot.y) * sin(cam.rot.x),
		sin(cam.rot.x),
		-cos(cam.rot.y) * cos(cam.rot.x)
	};
}

vec3 camera_right(const camera cam) {
	return vec3_cross(camera_forward(cam), (vec3){0.0f, 1.0f, 0.0});
}

vec3 camera_up(const camera cam) {
	return vec3_cross(camera_right(cam), camera_forward(cam));
}
