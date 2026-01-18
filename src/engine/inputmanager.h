#ifndef INPUTMANAGER_H
#define INPUTMANAGER_H

#include "GLFW/glfw3.h"
#include "keys.h"
#include "math/vec2.h"
#include <stdint.h>

typedef enum {
	AXIS_HORIZONTAL,
	AXIS_VERTICAL,
} axis;

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

float getInputAxis(axis a);

uint8_t isMouseDown(mousebutton button);
uint8_t isMousePressed(mousebutton button);
uint8_t isMouseReleased(mousebutton button);

vec2 getMousePosition();
vec2 getMouseDelta();
vec2 getScrollDelta();

#endif
