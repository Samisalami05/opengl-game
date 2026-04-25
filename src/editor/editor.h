#ifndef EDITOR_H
#define EDITOR_H

#include <glad_impl.h>
#include "profiler.h"
#include <stdint.h>
#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include "cimgui.h"

typedef struct {
	char plot_label[64]; // Used for imgui labels
	int plot_offset; // Used for rolling average plotting
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
Editor* editor_get();

#endif
