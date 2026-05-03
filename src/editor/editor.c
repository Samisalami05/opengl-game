#include "editor.h"
#include "cimgui.h"
#include "engine.h"
#include "logger.h"
#include "profiler.h"
#include "scene.h"
#include "scenemanager.h"
#include "util/slotmap.h"
#include "util/util.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#define CIMGUI_USE_GLFW
#define CIMGUI_USE_OPENGL3
#include <cimgui/cimgui_impl.h>

#include "editor_util.h"

static Editor editor = {0};

void editor_init(GLFWwindow* window) {
	editor.context = igCreateContext(NULL);

	editor.io = igGetIO_ContextPtr(editor.context);
	editor.io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	editor.logger.show_info = 1;
	editor.logger.show_warn = 1;
	editor.logger.show_error = 1;

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
	Engine* engine = engine_get();
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

	igBegin("Scenes", NULL, 0);
	
	SceneManager sceneman = engine->sceneman;

	igText("count: %d", sceneman.scenes.count);
	igText("current: %d", sceneman.curr_scene_id);

	igSpacing();

	igBeginChild_Str("Scene list", (ImVec2_c){0}, 0, 0);
	for (int i = 0; i < sceneman.scenes.allocated; i++) {
		if (!slotmap_is_occupied(&sceneman.scenes, i)) continue;
		scene* scene = slotmap_get(&sceneman.scenes, i);
		if (igCollapsingHeader_BoolPtr(scene->name, NULL, 0)) {
			igText("ID: %d", scene->id);
			igText("Entities: %d", scene->entities.count);
			igText("Lights: %d", scene->lights.count);
			igText("cam: %.1f %.1f %.1f", scene->cam.pos.x, scene->cam.pos.y, scene->cam.pos.z);
		}
	}

	igEndChild();


	igEnd();

	// --- LOGGER ---

	igBegin("Logger", NULL, 0);
	
	LogPane* pane = logger_get_pane();
	if (igBeginTable("logger_bar", 2, ImGuiTableFlags_SizingStretchSame, (ImVec2){0,0}, 0)) {
		igTableNextColumn();

		if (igButton("Clear", (ImVec2){0, 0}))
			logger_pane_clear(pane);
		igSameLine(0, 10);
		igCheckbox("show info", &editor.logger.show_info);
		igSameLine(0, 10);
		igCheckbox("show warn", &editor.logger.show_warn);
		igSameLine(0, 10);
		igCheckbox("show error", &editor.logger.show_error);
		
		igTableNextColumn();

		char buf[64];
		snprintf(buf, sizeof(buf), "info: %d, warn: %d, error: %d",
				 pane->data.info_count,
				 pane->data.warn_count,
				 pane->data.error_count);

		float text_width = igCalcTextSize(buf, NULL, 0, -1.0f).x;
		float col_width  = igGetColumnWidth(1);

		// Move cursor so text ends at right edge of column
		igSetCursorPosX(igGetCursorPosX() + col_width - text_width);

		igText("%s", buf);

		igEndTable();
	}

	igBeginChild_ID(1, (ImVec2_c){0}, 0, 0);
	for (int i = 0; i < pane->count; i++) {
		LogLine line = pane->lines[(i + pane->start) % pane->count];
	
		ImVec4_c col = {0.0f, 0.0f, 0.0f, 1.0f};
		switch (line.type) {
			case LOG_INFO:
				if (!editor.logger.show_info) continue;
				col = (ImVec4_c){1.0f, 1.0f, 1.0f, 1.0f};
				break;
			case LOG_WARNING:
				if (!editor.logger.show_warn) continue;
				col.x = 1.0f; 
				col.y = 0.8f; 
				break;
			case LOG_ERROR:
				if (!editor.logger.show_error) continue;
				col.x = 1.0f; 
				break;
		}

		igPushStyleColor_Vec4(ImGuiCol_Text, col);
		igText("%s", line.text);
		igPopStyleColor(1);
	}
	igEndChild();
	igEnd();

	// --- PROFILER ---

	igBegin("Profiler", NULL, 0);
	Profiler* profiler = profiler_get();

	igText("FPS: %.1f   Frame: %d", profiler->fps.value, profiler->frame_count);

	igText("CPU: %.3f ms [%.3f - %.3f]", profiler->frame_cpu.value, profiler->frame_cpu.min, profiler->frame_cpu.max);
	plot_profiler_time(&profiler->frame_cpu, -1);
	
	igText("GPU: %.3f ms [%.3f - %.3f]", profiler->frame_gpu.value, profiler->frame_gpu.min, profiler->frame_gpu.max);
	plot_profiler_time(&profiler->frame_gpu, -2);

	if (igCollapsingHeader_BoolPtr("Device Info", 0, 0)) {
		igText("Vendor:   %s\n", glGetString(GL_VENDOR));
		igText("Renderer: %s\n", glGetString(GL_RENDERER));
		igText("Graphics: OpenGL\n");
		igText("Version:  %s\n", glGetString(GL_VERSION));
		igText("GLSL:     %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

		igSpacing();

		if (igCollapsingHeader_BoolPtr("Constraints", 0, 0)) {
			igIndent(20);
			int value;
			glGetIntegerv(GL_MAX_TEXTURE_SIZE, &value);
			igText("Max texture size: %d\n", value);

			glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &value);
			igText("Texture units: %d\n", value);

			glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &value);
			igText("Vertex attribs: %d\n", value);

			glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &value);
			igText("Uniform block size: %d\n", value);

			glGetIntegerv(GL_MAX_DRAW_BUFFERS, &value);
			igText("Max draw buffers: %d\n", value);

			glGetIntegerv(GL_MAX_SAMPLES, &value);
			igText("Max samples (MSAA): %d\n", value);

			glGetIntegerv(GL_MAX_ELEMENTS_VERTICES, &value);
			igText("Max elements vertices: %d\n", value);
			igIndent(-20);
		}

		if (igCollapsingHeader_BoolPtr("Extensions", 0, 0)) {
			igIndent(20);
			igBeginChild_Str("Extensions", (ImVec2_c){0, 200}, 0, 0);

			int count;
			glGetIntegerv(GL_NUM_EXTENSIONS, &count);
				
			for (int i = 0; i < count; i++) {
				igText("%s\n", glGetStringi(GL_EXTENSIONS, i));
			}
			igEndChild();
			igIndent(-20);
		}
	}

	if (igCollapsingHeader_BoolPtr("Components", 0, 0)) {
		igText("Not implemented...");
	}

	if (igCollapsingHeader_BoolPtr("Pipeline", 0, 0)) {
		igIndent(20);
		for (int i = 0; i < profiler->pipeline.count; i++) {
			RenderPassStat* pass = &profiler->pipeline.passes[i];

			if (igCollapsingHeader_BoolPtr(pass->name, 0, 0)) {
				igText("CPU: %.3f ms [%.3f - %.3f] %2.0f%%", pass->cpu_time.value, pass->cpu_time.min, pass->cpu_time.max, profiler->frame_cpu.value > 0 ? pass->cpu_time.value / profiler->frame_cpu.value * 100 : 0);
				plot_profiler_time(&pass->cpu_time, i);

				igText("GPU: %.3f ms [%.3f - %.3f] %2.0f%%", pass->gpu_time.value, pass->gpu_time.min, pass->gpu_time.max, profiler->frame_gpu.value > 0 ? pass->gpu_time.value / profiler->frame_gpu.value * 100 : 0);
				plot_profiler_time(&pass->gpu_time, i + profiler->pipeline.count);
			}
		}
		igIndent(-20);
	}

	if (igCollapsingHeader_BoolPtr("Memory", 0, 0)) {
		igText("Not implemented...");
	}
	
	
	igEnd();
}

void editor_render() {
	igRender();
	ImGui_ImplOpenGL3_RenderDrawData(igGetDrawData());
}

Editor* editor_get() {
	return &editor;
}
