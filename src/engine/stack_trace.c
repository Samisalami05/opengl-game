#include "stack_trace.h"
#include "allocator.h"
#include <execinfo.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

bool stacktrace_capture(StackTrace* trace) {
#ifdef __linux
	trace->count = backtrace((void**)trace->frames, STACK_TRACE_MAX_FRAMES);
	return true;
#endif
}

bool stacktrace_print(StackTrace* trace) {
#ifdef __linux
    char **strings = backtrace_symbols((void**)trace->frames, trace->count);
    if (strings == NULL) {
        perror("backtrace_symbols");
		return false;
	}

    for (int i = 0; i < trace->count - 3; i++) {
        printf("\t%s\n", strings[i]);
    }

    free(strings);
#endif
	return true;
}
