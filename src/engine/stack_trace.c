#include "stack_trace.h"
#include "allocator.h"
#include <execinfo.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

bool stacktrace_capture(StackTrace* trace) {
	trace->count = backtrace((void**)trace->frames, STACK_TRACE_MAX_FRAMES);
	return true;
}

bool stacktrace_print(StackTrace* trace) {
	printf("Stack trace (%d frames):\n", trace->count);

    char **strings = backtrace_symbols((void**)trace->frames, trace->count);
    if (strings == NULL) {
        perror("backtrace_symbols");
		return false;
	}

    for (int i = 0; i < trace->count; i++) {
        printf("%s\n", strings[i]);
    }

    free(strings);
	return true;
}
