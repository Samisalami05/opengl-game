#include "inputmanager.h"
#include "GLFW/glfw3.h"
#include "keys.h"
#include <stdio.h>
#include <string.h>

static uint8_t curr_keys[KEY_UNDEFINED] = {0};
static uint8_t prev_keys[KEY_UNDEFINED] = {0};

void inputman_init() {
	
}

void inputman_deinit() {

}

void inputman_update(GLFWwindow* window) {
    memcpy(prev_keys, curr_keys, sizeof(curr_keys));
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

uint8_t isKeyDown(keycode key) {
	return curr_keys[key];
}

uint8_t isKeyPressed(keycode key) {
	return curr_keys[key] && !prev_keys[key];
}

uint8_t isKeyReleased(keycode key) {
	return !curr_keys[key] && prev_keys[key];
}

vec2 getInputAxis(axis a);
vec2 getInputAxisRaw(axis a); // TODO: add axis and mouse input

uint8_t isMouseDown(int button);
uint8_t isMousePressed(int button);
uint8_t isMouseReleased(int button);

vec2 getMousePosition();
vec2 getMouseDelta();
