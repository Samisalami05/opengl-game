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

uint8_t isKeyDown(keycode key);
uint8_t isKeyPressed(keycode key);
uint8_t isKeyReleased(keycode key);

vec2 getInputAxis(axis a);
vec2 getInputAxisRaw(axis a);

uint8_t isMouseDown(int button);
uint8_t isMousePressed(int button);
uint8_t isMouseReleased(int button);

vec2 getMousePosition();
vec2 getMouseDelta();

#endif
