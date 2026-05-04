#include "engine.h"

#include <glad/glad.h>
#include "GLFW/glfw3.h"
#include "allocator.h"
#include "asset_manager.h"
#include "inputmanager.h"
#include "profiler.h"
#include "rendering/pipeline.h"
#include "rendering/renderer.h"
#include "resourcemanager.h"
#include "scenemanager.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <AL/alc.h>

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480

static Engine engine = {0};

// Callbacks
static void glfw_error_callback(int error, const char* description);
static void glfw_framebuffer_size_callback(GLFWwindow* window, int width, int height);

// Initializers
static GLFWwindow* init_glfw();
static uint8_t init_opengl();
static uint8_t init_openal();
static void init_managers();

// Deinitializers
static void deinit_glfw(game* g);
static void deinit_openal();

uint8_t engine_init(game* g) {
	allocator_attach(&engine.allocator);

	GLFWwindow* window = init_glfw();
	if (window == NULL) return 1;
	if (!init_opengl()) return 1;
	if (!init_openal()) return 1;
	init_managers();

	g->window = window;
	g->deltatime = 0.001f;

	glfwSetWindowUserPointer(window, g);

	if (renderer_init(&engine.renderer, WINDOW_WIDTH, WINDOW_HEIGHT)) return 1;

	return 0;
}

static float frame_start = 0;

void engine_begin_frame(game* g) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	float current_frame = glfwGetTime();
	g->deltatime = current_frame - engine.last_frame;
	engine.last_frame = current_frame;
	frame_start = current_frame;
}

void engine_end_frame(game* g) {
	inputman_update(g->window);

	profiler_push_stat((ProfilerStat){PROFILER_CPU_FRAME_TIME, 0, NULL, (glfwGetTime() - frame_start) * 1000});
	
	glfwSwapBuffers(g->window);
	glfwPollEvents();
}

void engine_deinit(game* g) {
	scenemanager_deinit(&engine.sceneman);
	resource_manager_deinit();
	inputman_deinit();
	renderer_deinit(&engine.renderer);
	//asset_manager_deinit(&engine.assetman);

	deinit_openal();
	deinit_glfw(g);
}

Engine* engine_get() {
	return &engine;
}

// Callbacks
static void glfw_error_callback(int error, const char* description) {
	fprintf(stderr, "GLFW: %d Error: %s\n", error, description);
}

static void glfw_framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	game* g = glfwGetWindowUserPointer(window);
	glViewport(0, 0, width, height);
	render_pipeline_resize(&engine.renderer.pipeline, width, height);

	scene* scene = sm_get_current_scene();
	scene->cam.width = width;
	scene->cam.height = height;
}

// Initializers
static GLFWwindow* init_glfw() {
	glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_X11);
	if (!glfwInit()) {
		fprintf(stderr, "engine: Failed to initialize glfw\n");
        return NULL;
	}

	glfwWindowHint(GLFW_DEPTH_BITS, 24);

	GLFWwindow* window = glfwCreateWindow(640, 480, "game", NULL, NULL);
	if (!window) {
		glfwTerminate();
		fprintf(stderr, "engine: Failed to create glfw window\n");
		return NULL;
	}

	// Callbacks
	glfwSetFramebufferSizeCallback(window, glfw_framebuffer_size_callback);
	glfwSetErrorCallback(glfw_error_callback);
	glfwSetKeyCallback(window, inputman_key_callback);
	glfwSetMouseButtonCallback(window, inputman_mouse_callback);
	glfwSetCursorPosCallback(window, inputman_cursor_callback);
	glfwSetScrollCallback(window, inputman_scroll_callback);
	glfwSetJoystickCallback(inputman_joystick_callback);

	glfwMakeContextCurrent(window);

	glfwSwapInterval(1); // Enable vsync

	return window;
}

static uint8_t init_opengl() {
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		fprintf(stderr, "engine: Failed to load glad\n");
		return 0;
	}

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  

	return 1;
}

static uint8_t init_openal() {
	engine.al_device = alcOpenDevice(NULL);
	if (engine.al_device == NULL) {
		fprintf(stderr, "engine: Failed to initialize OpenAl\n");
		return 0;
	}

	engine.al_context = alcCreateContext(engine.al_device, NULL);
	if (engine.al_context == NULL) {
		alcCloseDevice(engine.al_device);
		fprintf(stderr, "engine: Could not create OpenAl context\n");
		return 0;
	}

	if (!alcMakeContextCurrent(engine.al_context)) {
		alcCloseDevice(engine.al_device);
		fprintf(stderr, "engine: Failed to make OpenAl context current\n");
		return 0;
	}
	return 1;
}

static void init_managers() {
	resource_manager_init();
	//asset_manager_init(&engine.assetman);
	scenemanager_init(&engine.sceneman);
	inputman_init();
}

// Deinitializers
static void deinit_glfw(game* g) {
	glfwDestroyWindow(g->window);	
	glfwTerminate();
}

static void deinit_openal() {
	alcMakeContextCurrent(NULL);
	alcDestroyContext(engine.al_context);
	alcCloseDevice(engine.al_device);
}
