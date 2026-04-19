#include "editor.h"
#include "cimgui.h"
#include "profiler.h"
#include "util/util.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CIMGUI_USE_GLFW
#define CIMGUI_USE_OPENGL3
#include <cimgui/cimgui_impl.h>

static Editor editor = {0};

void editor_init(GLFWwindow* window) {
	editor.context = igCreateContext(NULL);

	editor.io = igGetIO_ContextPtr(editor.context);
	editor.io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	igStyleColorsDark(NULL);

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");
}

void editor_begin_frame() {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	igNewFrame();
}

static int offset = 0;

static float editor_plot(void* buf, int index) {
	return ((float*)buf)[(index + offset) % PROFILER_SAMPLE_COUNT];
}

void editor_update() {
	/*
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

	ImGuiViewport* viewport = igGetMainViewport();
	igSetNextWindowPos(viewport->WorkPos, 0, (ImVec2_c){0});
	igSetNextWindowSize(viewport->WorkSize, 0);
	igSetNextWindowViewport(viewport->ID);

	window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
                ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

	igPushStyleVar_Float(ImGuiStyleVar_WindowRounding, 0.0f);
	igPushStyleVar_Float(ImGuiStyleVar_WindowBorderSize, 0.0f);

	igBegin("DockSpace Window", NULL, window_flags);

	igPopStyleVar(2);

	ImGuiID dockspace_id = igGetID_Str("MyDockSpace");
	igDockSpace(dockspace_id, (ImVec2){0.0f, 0.0f}, 0, NULL);

	igEnd(); */

	igBegin("Test", NULL, 0);


	igEnd();


	igBegin("Profiler", NULL, 0);
	Profiler* profiler = profiler_get();

	igText("FPS: %.1f   Frame: %d", profiler->fps.value, profiler->frame);

	if (editor.profiler.capacity < profiler->pipeline.capacity) {
		uint32_t prev_capacity = editor.profiler.capacity;
		editor.profiler.capacity = profiler->pipeline.capacity;
		void* tmp = realloc(editor.profiler.graphs, editor.profiler.capacity * sizeof(EditorProfilerGraph));
		if (tmp == NULL) {
			perror("editor: realloc");
			return;
		}
		editor.profiler.graphs = tmp;
		memset(editor.profiler.graphs + prev_capacity, 0, (editor.profiler.capacity - prev_capacity) * sizeof(EditorProfilerGraph));
	}

	float deltatime = 1.0f / profiler->fps.value; // TODO: this is bad

	for (int i = 0; i < profiler->pipeline.count; i++) {
		RenderPassStat* pass = &profiler->pipeline.passes[i];
	
		if (igCollapsingHeader_BoolPtr(pass->name, 0, 0)) {
			igText("CPU: %.3f ms", pass->cpu_time.value);

			char label[64];
			snprintf(label, sizeof(label), "##%d", i);

			// TODO: maybe lerp idk			
			editor.profiler.graphs[i].min = lerpf(editor.profiler.graphs[i].min, pass->cpu_time.min * 0.9f, deltatime * 10.0f);
			editor.profiler.graphs[i].max = lerpf(editor.profiler.graphs[i].max, pass->cpu_time.max * 1.1f, deltatime * 10.0f);

			offset = pass->cpu_time.curr;
			igPlotLines_FnFloatPtr(label, editor_plot, 
					pass->cpu_time.samples, 
					PROFILER_SAMPLE_COUNT, 0, NULL,
					0.0f,
					pass->cpu_time.max < 1.0f ? 1.0f : pass->cpu_time.max,
					(ImVec2){0, 80}
			);
		}
	}
	igEnd();
}

void editor_render() {
	igRender();
	ImGui_ImplOpenGL3_RenderDrawData(igGetDrawData());
}
