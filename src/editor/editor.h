#ifndef EDITOR_H
#define EDITOR_H

#include "GLFW/glfw3.h"
#include "profiler.h"
#include <stdint.h>
#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include "cimgui.h"

typedef struct {
	float min;
	float max;
} EditorProfilerGraph;

typedef struct {
	EditorProfilerGraph* graphs;
	uint32_t capacity;
	uint32_t count;
} EditorProfiler;

typedef struct {
	ImGuiContext* context;
	ImGuiIO* io;

	EditorProfiler profiler;
} Editor;

void editor_init(GLFWwindow* window);
void editor_begin_frame();
void editor_update();
void editor_render();

#endif
