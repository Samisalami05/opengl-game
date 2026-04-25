#include "editor_util.h"
#include "editor.h"

float editor_plot(void* buf, int index) {
	Editor* editor = editor_get();
	return ((float*)buf)[(index + editor->profiler.plot_offset) % PROFILER_SAMPLE_COUNT];
}

void plot_profiler_time(ProfilerTime* time, int id) {
	Editor* editor = editor_get();
	snprintf(editor->profiler.plot_label, sizeof(editor->profiler.plot_label), "##%d", id);
	editor->profiler.plot_offset = time->curr;
	igPlotLines_FnFloatPtr(editor->profiler.plot_label, editor_plot, 
			time->samples, 
			PROFILER_SAMPLE_COUNT, 0, NULL, 0.0f,
			time->max < 1.0f ? 1.0f : time->max,
			(ImVec2){0, 80}
	);
}
