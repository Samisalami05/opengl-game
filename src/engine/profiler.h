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
	ProfilerTime frame_cpu;
	ProfilerTime frame_gpu;
	uint64_t frame_count;
} Profiler;

typedef enum {
	PROFILER_CPU_PASS_TIME,
	PROFILER_GPU_PASS_TIME,
	PROFILER_CPU_FRAME_TIME,
	PROFILER_GPU_FRAME_TIME,
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

// Used to track time on the GPU
typedef struct {
	uint32_t start;
	uint32_t end;
} ProfilerQuery;

void profiler_query_init(ProfilerQuery* query);
void profiler_query_begin(ProfilerQuery* query);
void profiler_query_end(ProfilerQuery* query);
bool profiler_query_finished(ProfilerQuery* query);
float profiler_query_get(ProfilerQuery* query);

#endif
