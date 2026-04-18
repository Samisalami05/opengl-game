#include "editor.h"
#include "cimgui.h"
#include "profiler.h"
#include <stdio.h>
#define CIMGUI_USE_GLFW
#define CIMGUI_USE_OPENGL3
#include <cimgui/cimgui_impl.h>

static Editor editor = {0};

void editor_init(GLFWwindow* window) {
	editor.context = igCreateContext(NULL);

	editor.io = igGetIO_ContextPtr(editor.context);
	(void)editor.io; // TODO: What does this do?

	igStyleColorsDark(NULL);

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");
}

void editor_begin_frame() {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	igNewFrame();
}

void editor_update() {
	igBegin("Profiler", NULL, 0);
	Profiler* profiler = profiler_get();

	for (int i = 0; i < profiler->pipeline.count; i++) {
		RenderPassStat* pass = &profiler->pipeline.passes[i];
	
		if (igCollapsingHeader_BoolPtr(pass->name, 0, 0)) {
			igText("CPU: %.3f ms", pass->cpu_time.value);

			// Graph goes here
			char label[64];
			snprintf(label, sizeof(label), "CPU Samples##%d", i);

			igPlotLines_FloatPtr(label,
				pass->cpu_time.samples,
				PROFILER_SAMPLE_COUNT,
				0, NULL, 0.0f, 1.0f,
				(ImVec2){0, 80}, sizeof(float));
		}
	}
	igEnd();
}

void editor_render() {
	igRender();
	ImGui_ImplOpenGL3_RenderDrawData(igGetDrawData());
}
