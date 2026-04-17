#ifndef PROFILER_H
#define PROFILER_H

#include "engine.h"
#include <stdbool.h>
#include <stdint.h>

typedef struct {
	char* name;
	float cpu_time;
	float gpu_time;
} RenderPassStat;

typedef struct {
	RenderPassStat* passes;
	uint32_t capacity;
	uint32_t count;
} RenderPipelineStats;

typedef struct {
	game* game;
	RenderPipelineStats pipeline;
} Profiler;

typedef enum {
	PROFILER_CPU_PASS_TIME,
	PROFILER_GPU_PASS_TIME,
} ProfilerStatType;

typedef struct {
	ProfilerStatType type;
	uint32_t id;
	char* name;
	float time;
} ProfilerStat;

void profiler_attach(game* game);
bool profiler_is_attached();
void profiler_detach();

Profiler* profiler_get();
void profiler_push_stat(ProfilerStat stat);
void profiler_update();


#endif
