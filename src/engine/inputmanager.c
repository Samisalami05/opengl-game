#include "inputmanager.h"
#include "GLFW/glfw3.h"
#include "keys.h"
#include "math/vec2.h"
#include "util/arraylist.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

static uint8_t curr_keys[KEY_LAST + 1] = {0};
static uint8_t prev_keys[KEY_LAST + 1] = {0};

static uint8_t curr_mouse[MOUSE_BUTTON_LAST + 1] = {0};
static uint8_t prev_mouse[MOUSE_BUTTON_LAST + 1] = {0};

static vec2 mouse_pos = {0};
static vec2 mouse_delta = {0};
static vec2 scroll_delta = {0};

#define GAMEPAD_NO_DEFAULT 16
#define GAMEPAD_DEADZONE 0.1f

static GLFWgamepadstate gamepad_states[GLFW_JOYSTICK_LAST + 1] = {0};
static uint8_t prev_gamepad_buttons[GLFW_JOYSTICK_LAST + 1][15] = {0};
static uint8_t gamepad_updated[GLFW_JOYSTICK_LAST + 1] = {0}; // TODO: Switch to bitmap
static uint8_t gamepad_connected[GLFW_JOYSTICK_LAST + 1] = {0}; // if gamepad connected
static uint8_t default_gamepad = GAMEPAD_NO_DEFAULT; // starts outside of range

void inputman_init() {
	for (int i = 0; i <= GLFW_JOYSTICK_LAST; i++) {
		if (glfwJoystickPresent(i)) {
		    printf("Joystick %d already connected: %s\n", i, glfwGetJoystickName(i));
			gamepad_connected[i] = 1;
			default_gamepad = i;
		}
	}
}

void inputman_deinit() {}

void inputman_update(GLFWwindow* window) {
    memcpy(prev_keys, curr_keys, sizeof(curr_keys));
	memcpy(prev_mouse, curr_mouse, sizeof(curr_mouse));

	for (int i = 0; i < GLFW_JOYSTICK_LAST + 1; i++) {
		if (!gamepad_connected[i] && !gamepad_updated[i]) continue;
		memcpy(prev_gamepad_buttons[i], gamepad_states[i].buttons, sizeof(prev_gamepad_buttons[i]));
	}
	memset(gamepad_updated, 0, sizeof(gamepad_updated));
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

void inputman_scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	scroll_delta.x += xoffset;
	scroll_delta.y += yoffset;
}

static uint8_t find_connected_gamepad() {

	for (int i = 0; i < GLFW_JOYSTICK_LAST + 1; i++) {
		if (!gamepad_connected[i]) continue;
		return i;
	}
	return GAMEPAD_NO_DEFAULT;
}

void inputman_joystick_callback(int jid, int event)
{
    if (event == GLFW_CONNECTED)
    {
		printf("joystick connected %d\n", jid);
		default_gamepad = jid;
		gamepad_connected[jid] = 1;
    }
    else if (event == GLFW_DISCONNECTED)
    {
		printf("joystick disconnected %d\n", jid);
		gamepad_connected[jid] = 0;
		default_gamepad = find_connected_gamepad();
    }
}

// Keyboard

uint8_t isKeyDown(keycode key) {
	return curr_keys[key];
}

uint8_t isKeyPressed(keycode key) {
	return curr_keys[key] && !prev_keys[key];
}

uint8_t isKeyReleased(keycode key) {
	return !curr_keys[key] && prev_keys[key];
}

// Mouse

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

vec2 getScrollDelta() {
	vec2 delta = scroll_delta;
	scroll_delta = (vec2){0};
	return delta;
}

// Gamepad

static GLFWgamepadstate getGamepadState(uint8_t id) {
	if (id < 0 || id >= 16) {
		if (default_gamepad == GAMEPAD_NO_DEFAULT) return (GLFWgamepadstate){0};
		id = default_gamepad;
	}
	if (!gamepad_connected[id]) {
		fprintf(stderr, "Invalid gamepad id %d: No gamepad with given id is connected\n", id);
		return (GLFWgamepadstate){0};
	}

	if (gamepad_updated[id]) return gamepad_states[id];
	glfwGetGamepadState(id, &gamepad_states[id]);
	gamepad_updated[id] = 1;
	return gamepad_states[id];
}

uint8_t isGamepadButtonDown(uint8_t id, gamepad_button button) {
	GLFWgamepadstate state = getGamepadState(id);
	return state.buttons[gamepadbutton_to_glfw(button)];
}

uint8_t isGamepadButtonPressed(uint8_t id, gamepad_button button) {
    GLFWgamepadstate state = getGamepadState(id);
	if (id == GAMEPAD_NO_DEFAULT) id = default_gamepad;
    uint8_t b = gamepadbutton_to_glfw(button);
    return state.buttons[b] && !prev_gamepad_buttons[id][b];
}

uint8_t isGamepadButtonReleased(uint8_t id, gamepad_button button) {
    GLFWgamepadstate state = getGamepadState(id);
	if (id == GAMEPAD_NO_DEFAULT) id = default_gamepad;
    uint8_t b = gamepadbutton_to_glfw(button);
    return !state.buttons[b] && prev_gamepad_buttons[id][b];
}

vec2 getJoystickDelta(uint8_t id, gamepad_joystick stick) { // TODO: apply deadzone
	GLFWgamepadstate state = getGamepadState(id);

	vec2 ret = {0};
	if (stick == GAMEPAD_JOYSTICK_LEFT) {
		vec2 left;
		left.x = state.axes[GLFW_GAMEPAD_AXIS_LEFT_X];
		left.y = -state.axes[GLFW_GAMEPAD_AXIS_LEFT_Y];
		if (vec2_magnitude(left) > GAMEPAD_DEADZONE)
			ret = left;
	}
	else if (stick == GAMEPAD_JOYSTICK_RIGHT) {
		vec2 right;
		right.x = state.axes[GLFW_GAMEPAD_AXIS_RIGHT_X];
		right.y = -state.axes[GLFW_GAMEPAD_AXIS_RIGHT_Y];

		if (vec2_magnitude(right) > GAMEPAD_DEADZONE)
			ret = right;
	}
	return ret;
}

float getGamepadTrigger(uint8_t id, gamepad_trigger trigger) {
	GLFWgamepadstate state = getGamepadState(id);
	return trigger == GAMEPAD_TRIGGER_LEFT ? 
		state.axes[GLFW_GAMEPAD_AXIS_LEFT_TRIGGER] : 
		state.axes[GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER];
}

vec2 getInputAxes(input_group group) {
	vec2 input = {0};

	if (isKeyDown(KEY_A) || isKeyDown(KEY_LEFT))
		input.x -= 1.0f;
	if (isKeyDown(KEY_D) || isKeyDown(KEY_RIGHT))
		input.x += 1.0f;

	if (isKeyDown(KEY_S) || isKeyDown(KEY_DOWN))
		input.y -= 1.0f;
	if (isKeyDown(KEY_W) || isKeyDown(KEY_UP))
		input.y += 1.0f;

	input = vec2_normalized(input);
	input = vec2_add_v(input, getJoystickDelta(GAMEPAD_DEFAULT_ID, GAMEPAD_JOYSTICK_LEFT));
	
	return input;
}

// TODO: Remove this
void print_gamepad_button(gamepad_button button) {
	for (int i = 0; i < GLFW_JOYSTICK_LAST + 1; i++) {
		printf("%d ", gamepad_states[i].buttons[button]);
	} printf("\n");

	for (int i = 0; i < GLFW_JOYSTICK_LAST + 1; i++) {
		printf("%d ", prev_gamepad_buttons[i][button]);
	} printf("\n");
}
