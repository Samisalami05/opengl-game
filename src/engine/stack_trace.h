#ifndef STACK_TRACE_H
#define STACK_TRACE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define STACK_TRACE_MAX_FRAMES 64

typedef struct {
	uint8_t* frames[STACK_TRACE_MAX_FRAMES];
	uint32_t count;
	size_t size;
} StackTrace;

bool stacktrace_capture(StackTrace* trace);
bool stacktrace_print(StackTrace* trace);

#endif
