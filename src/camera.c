#include "camera.h"
#include "mat4.h"

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
