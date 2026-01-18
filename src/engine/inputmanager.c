#include "inputmanager.h"
#include "GLFW/glfw3.h"
#include "keys.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

static uint8_t curr_keys[KEY_UNDEFINED] = {0};
static uint8_t prev_keys[KEY_UNDEFINED] = {0};

static uint8_t curr_mouse[MOUSE_BUTTON_LAST] = {0};
static uint8_t prev_mouse[MOUSE_BUTTON_LAST] = {0};

static vec2 mouse_pos = {0};
static vec2 mouse_delta = {0};

void inputman_init() {}
void inputman_deinit() {}

void inputman_update(GLFWwindow* window) {
    memcpy(prev_keys, curr_keys, sizeof(curr_keys));
	memcpy(prev_mouse, curr_mouse, sizeof(curr_mouse));
}

void inputman_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	keycode code = glfw_to_keycode(key);
	switch (action) {
		case GLFW_PRESS:
			curr_keys[code] = 1;
			break;
		case GLFW_RELEASE:
			curr_keys[code] = 0;
			break;
	}
}

void inputman_mouse_callback(GLFWwindow* window, int button, int action, int mods) {
	mousebutton mbutton = glfw_to_mousebutton(button);
	switch (action) {
		case GLFW_PRESS:
			curr_mouse[mbutton] = 1;
			break;
		case GLFW_RELEASE:
			curr_mouse[mbutton] = 0;
			break;
	}
}

void inputman_cursor_callback(GLFWwindow* window, double xpos, double ypos) {
	mouse_delta.x += xpos - mouse_pos.x;
	mouse_delta.y += ypos - mouse_pos.y;
	mouse_pos.x = xpos;
	mouse_pos.y = ypos;
}

uint8_t isKeyDown(keycode key) {
	return curr_keys[key];
}

uint8_t isKeyPressed(keycode key) {
	return curr_keys[key] && !prev_keys[key];
}

uint8_t isKeyReleased(keycode key) {
	return !curr_keys[key] && prev_keys[key];
}

float getInputAxis(axis a) {
	float input = 0.0f;
	if (a == AXIS_HORIZONTAL) {
		if (isKeyDown(KEY_A) || isKeyDown(KEY_LEFT))
			input -= 1.0f;
		if (isKeyDown(KEY_D) || isKeyDown(KEY_RIGHT))
			input += 1.0f;
	}
	else {
		if (isKeyDown(KEY_S) || isKeyDown(KEY_DOWN))
			input -= 1.0f;
		if (isKeyDown(KEY_W) || isKeyDown(KEY_UP))
			input += 1.0f;
	}
	return input;
}

uint8_t isMouseDown(mousebutton button) {
	return curr_mouse[button];
}

uint8_t isMousePressed(mousebutton button) {
	return curr_mouse[button] && !prev_mouse[button];
}

uint8_t isMouseReleased(mousebutton button) {
	return !curr_mouse[button] && prev_mouse[button];
}

vec2 getMousePosition() {
	return mouse_pos;
}

vec2 getMouseDelta() {
	vec2 delta = mouse_delta;
	mouse_delta = (vec2){0};
	return delta;
}
