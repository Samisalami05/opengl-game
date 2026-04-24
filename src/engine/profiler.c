#include "profiler.h"
#include "core/shader.h"
#include "rendering/pipeline.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static Profiler profiler = {0};

void profiler_attach() {
	// TODO: implement
}

bool profiler_is_attached() {
	return true;
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

static void profiler_time_push(ProfilerTime* ptime, float time) {
	if (time < 0) time = 0;

	if (time > ptime->max) ptime->max = time;
	else if (time < ptime->min) ptime->min = time;

	ptime->curr = (ptime->curr + 1) % PROFILER_SAMPLE_COUNT;
	
	if (ptime->samples[ptime->curr] >= ptime->max) ptime->max = ptime->value;
	else if (ptime->samples[ptime->curr] <= ptime->min) ptime->min = ptime->value;

	ptime->samples[ptime->curr] = time;

	float sum = 0;
	for (int i = 0; i < PROFILER_AVG_SIZE; i++) {
		sum += ptime->samples[(i + ptime->curr - PROFILER_AVG_SIZE) % PROFILER_SAMPLE_COUNT];
	}
	ptime->value = sum / PROFILER_AVG_SIZE;
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
		memset(profiler.pipeline.passes + prev_capacity, 0, (profiler.pipeline.capacity - prev_capacity) * sizeof(RenderPassStat));
	}

	RenderPassStat* pass = profiler.pipeline.passes + stat.id;

	pass->name = stat.name;
	if (stat.id >= profiler.pipeline.count) profiler.pipeline.count = stat.id + 1;
	if (stat.type == PROFILER_CPU_PASS_TIME) {
		profiler_time_push(&pass->cpu_time, stat.time);
		return;
	}

	profiler_time_push(&pass->gpu_time, stat.time);
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

void profiler_update(float deltatime) {
	profiler_time_push(&profiler.fps, 1.0 / deltatime);
	profiler.frame++;
	//printf("%s: %.2f\n", profiler.pipeline.passes[0].name,  profiler.pipeline.passes[0].cpu_time);
}

// Queries

void profiler_query_init(ProfilerQuery* query) {
	glGenQueries(2, (uint32_t*)query);
}

void profiler_query_begin(ProfilerQuery* query) {
	glQueryCounter(query->start, GL_TIMESTAMP);
}

void profiler_query_end(ProfilerQuery* query) {
	glQueryCounter(query->end, GL_TIMESTAMP);
}

bool profiler_query_finished(ProfilerQuery* query) {
	uint32_t available = 0;
	glGetQueryObjectuiv(query->end, GL_QUERY_RESULT_AVAILABLE, &available);
	return available;
}

// On success, returns the time in millis. Returns -1 if not finished.
float profiler_query_get(ProfilerQuery* query) {
	if (!profiler_query_finished(query)) return -1.0f;
	
	uint64_t startTime, endTime;
	glGetQueryObjectui64v(query->start, GL_QUERY_RESULT, &startTime);
    glGetQueryObjectui64v(query->end, GL_QUERY_RESULT, &endTime);

    return (endTime - startTime) / 1000000.0;
}

