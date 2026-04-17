#include "profiler.h"
#include "rendering/pipeline.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

static Profiler profiler = {0};

void profiler_attach(game* game) {
	profiler.game = game;
}

bool profiler_is_attached() {
	return profiler.game != NULL;
}

void profiler_detach() {
	free(profiler.pipeline.passes);
	profiler.pipeline.passes = NULL;
	profiler.pipeline.capacity = 0;
	profiler.pipeline.count = 0;
}

Profiler* profiler_get() {
	return &profiler;
}

static void set_pass_time(ProfilerStat stat) {
	uint32_t prev_capacity = profiler.pipeline.capacity;
	while (stat.id + 1 > profiler.pipeline.capacity) {
		profiler.pipeline.capacity = profiler.pipeline.capacity == 0 ? 4 : profiler.pipeline.capacity * 2;
	}
	
	if (prev_capacity != profiler.pipeline.capacity) {
		void* tmp = realloc(profiler.pipeline.passes, profiler.pipeline.capacity * sizeof(RenderPassStat));
		if (tmp == NULL) {
			perror("Profiler: Realloc");
			return;
		}
		profiler.pipeline.passes = tmp;
	}

	profiler.pipeline.passes[stat.id].name = stat.name;
	if (stat.type == PROFILER_CPU_PASS_TIME) {
		profiler.pipeline.passes[stat.id].cpu_time = stat.time;
		return;
	}

	profiler.pipeline.passes[stat.id].gpu_time = stat.time;
}

void profiler_push_stat(ProfilerStat stat) {
	switch (stat.type) {
		case PROFILER_CPU_PASS_TIME:
		case PROFILER_GPU_PASS_TIME:
			set_pass_time(stat);
			break;
		default:
			fprintf(stderr, "Profiler: Unknown stat type %d\n", stat.type);
	}
}

void profiler_update() {
	//printf("%s: %.2f\n", profiler.pipeline.passes[0].name,  profiler.pipeline.passes[0].cpu_time);
}

