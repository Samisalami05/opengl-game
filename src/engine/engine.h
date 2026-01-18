#ifndef ENGINE_H
#define ENGINE_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdint.h>

typedef struct game {
	GLFWwindow* window;
	float deltatime;
	float last_frame;
} game;

game* engine_init();
void engine_begin_frame(game* g);
void engine_end_frame(game* g);
void engine_deinit(game* g);

#endif
