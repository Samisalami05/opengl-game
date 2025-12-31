#include "engine.h"
#include "GLFW/glfw3.h"
#include "resourcemanager.h"
#include "scenemanager.h"
#include <stdio.h>
#include <stdlib.h>

// Callbacks
static void error_callback(int error, const char* description) {
    fprintf(stderr, "%d Error: %s\n", error, description);
}

static void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
	scene* scene = sm_get_current_scene();
	scene->cam.width = width;
	scene->cam.height = height;
}

// Initializers
static GLFWwindow* init_glfw() {
	if (!glfwInit()) {
		fprintf(stderr, "Failed to initialize glfw\n");
        return NULL;
	}

	glfwWindowHint(GLFW_DEPTH_BITS, 24);

    GLFWwindow* window = glfwCreateWindow(640, 480, "game", NULL, NULL);
    if (!window) {
        glfwTerminate();
		fprintf(stderr, "Failed to create glfw window\n");
        return NULL;
    }

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetErrorCallback(error_callback);
    glfwMakeContextCurrent(window);
	
	return window;
}

static uint8_t init_opengl() {
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		fprintf(stderr, "Failed to load glad\n");
		return 0;
	}

	glEnable(GL_DEPTH_TEST);

	return 1;
}

static void init_managers() {
	resource_manager_init();
	scenemanager_init();
}

game* engine_init() {
	GLFWwindow* window = init_glfw();
	if (window == NULL) return NULL;
	if (!init_opengl()) return NULL;
	init_managers();

	game* g = malloc(sizeof(game));
	if (g == NULL) {
		perror("engine: malloc");
		return NULL;
	}
	g->window = window;
	g->deltatime = 0.001f;
	return g;
}

// Deinitializers
static void deinit_glfw(game* g) {
	glfwDestroyWindow(g->window);	
	glfwTerminate();
}

void engine_deinit(game* g) {
	scenemanager_deinit();
	resource_manager_deinit();
	deinit_glfw(g);

	free(g);
}
