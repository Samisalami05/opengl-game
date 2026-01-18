#ifndef INPUTMANAGER_H
#define INPUTMANAGER_H

#include "GLFW/glfw3.h"
#include "keys.h"
#include "math/vec2.h"
#include <stdint.h>

void inputman_init();
void inputman_deinit();
void inputman_update(GLFWwindow* window);

void inputman_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void inputman_mouse_callback(GLFWwindow* window, int button, int action, int mods);
void inputman_cursor_callback(GLFWwindow* window, double xpos, double ypos);
void inputman_scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void inputman_joystick_callback(int jid, int event);

uint8_t isKeyDown(keycode key); // TODO: Maybe switch to snake-case
uint8_t isKeyPressed(keycode key);
uint8_t isKeyReleased(keycode key);

uint8_t isMouseDown(mousebutton button);
uint8_t isMousePressed(mousebutton button);
uint8_t isMouseReleased(mousebutton button);

vec2 getMousePosition();
vec2 getMouseDelta();
vec2 getScrollDelta();

#define GAMEPAD_DEFAULT_ID 16

uint8_t isGamepadButtonDown(uint8_t id, gamepad_button button);
uint8_t isGamepadButtonPressed(uint8_t id, gamepad_button button);
uint8_t isGamepadButtonReleased(uint8_t id, gamepad_button button);

vec2 getJoystickDelta(uint8_t id, gamepad_joystick stick);
float getGamepadTrigger(uint8_t id, gamepad_trigger trigger);

typedef enum {
	INPUT_GROUP_MOVEMENT,
	INPUT_GROUP_ORIENTATION,
} input_group;

vec2 getInputAxes(input_group group);

void print_gamepad_button(gamepad_button button);

#endif
