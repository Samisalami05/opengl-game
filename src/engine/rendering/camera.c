#include "camera.h"
#include "inputmanager.h"
#include "math/mat4.h"
#include <math.h>
#include <string.h>
#include "math/vec2.h"
#include "math/vec3.h"
#include "util/util.h"

const float mouse_sensitivity = 0.005f;
const float gamepad_sensitivity = 0.0008f;
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

void camera_mouse_input(camera* cam) {
	vec2 mouse = getMouseDelta();
	vec2 joystick = getJoystickDelta(GAMEPAD_DEFAULT_ID, GAMEPAD_JOYSTICK_RIGHT);

	mouse = vec2_mul_f(mouse, mouse_sensitivity);
	joystick = vec2_mul_f(joystick, gamepad_sensitivity);

	mouse.y *= -1; // Flip mouse y
	
	vec2 input = vec2_add_v(mouse, joystick);

	cam->yaw += input.x;
	cam->pitch = clampf(cam->pitch + input.y, -M_PI / 2.001f, M_PI / 2.001f);
}

void camera_key_input(camera* cam, float deltatime) {
	float camera_speed = 10.0f;

	vec2 input = getInputAxes(INPUT_GROUP_MOVEMENT);

	vec3 forward = camera_forward(*cam);
	vec3 right = camera_right(*cam);

	forward = vec3_mul_f(forward, input.y * camera_speed * deltatime);
	right = vec3_mul_f(right, input.x * camera_speed * deltatime);

	cam->pos = vec3_add_v3(cam->pos, forward);
	cam->pos = vec3_add_v3(cam->pos, right);

	vec3 up = {0.0f, 1.0f, 0.0f};
	vec3 move = vec3_mul_f(up, deltatime * camera_speed);
	
	if (isKeyDown(KEY_SPACE) || isGamepadButtonDown(GAMEPAD_DEFAULT_ID, GAMEPAD_BUTTON_A)) {
		cam->pos = vec3_add_v3(cam->pos, move);
	}
	if (isKeyDown(KEY_LEFT_SHIFT) || isGamepadButtonDown(GAMEPAD_DEFAULT_ID, GAMEPAD_BUTTON_B)) {
		cam->pos = vec3_sub_v3(cam->pos, move);
	}

	float up_move = getGamepadTrigger(GAMEPAD_DEFAULT_ID, GAMEPAD_TRIGGER_RIGHT);
	up_move -= getGamepadTrigger(GAMEPAD_DEFAULT_ID, GAMEPAD_TRIGGER_LEFT);

	cam->pos.y += up_move * camera_speed * deltatime;
}
