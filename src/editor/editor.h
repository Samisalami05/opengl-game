#ifndef EDITOR_H
#define EDITOR_H

#include "GLFW/glfw3.h"
#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include "cimgui.h"

typedef struct {
	ImGuiContext* context;
	ImGuiIO* io;
} Editor;

void editor_init(GLFWwindow* window);
void editor_begin_frame();
void editor_update();
void editor_render();

#endif
