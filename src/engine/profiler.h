#ifndef PROFILER_H
#define PROFILER_H

#include <stdbool.h>
#include <stdint.h>

#define PROFILER_SAMPLE_COUNT 1024
#define PROFILER_AVG_SIZE 16

typedef struct {
	float samples[PROFILER_SAMPLE_COUNT];
	uint32_t curr; // The current sample as a index
	float value; // The resulting average  (TODO: maybe not be in struct)
	float min;
	float max;
} ProfilerTime; // TODO: Maybe rename to ProfilerValue or something

typedef struct {
	char* name;
	ProfilerTime cpu_time;
	ProfilerTime gpu_time;
} RenderPassStat;

typedef struct {
	RenderPassStat* passes;
	uint32_t capacity;
	uint32_t count;
} RenderPipelineStats;

typedef struct {
	RenderPipelineStats pipeline;
	ProfilerTime fps;
	uint64_t frame;
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

void profiler_attach();
bool profiler_is_attached();
void profiler_detach();

Profiler* profiler_get();
void profiler_push_stat(ProfilerStat stat);
void profiler_update(float deltatime);

#endif
