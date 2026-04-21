#ifndef ENGINE_H
#define ENGINE_H

#include "rendering/renderer.h"
#include <AL/alc.h>
#include <GLFW/glfw3.h>
#include <stdint.h>

typedef enum {
	ENGINE_USE_EDITOR,
	ENGINE_USE_VSYNC,
} EngineSettings;

typedef struct {
	uint8_t setting_flags;
	renderer renderer;
	float last_frame; // For deltatime calculation
	
	// OpenAl
	ALCdevice* al_device;
	ALCcontext* al_context;
} Engine;

typedef struct game {
	GLFWwindow* window;
	float deltatime;
} game;

uint8_t engine_init(game* g);
void engine_begin_frame(game* g);
void engine_end_frame(game* g);
void engine_deinit(game* g);

Engine* engine_get();

#endif
