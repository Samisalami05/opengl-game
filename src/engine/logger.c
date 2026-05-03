#include "logger.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

static void default_logger(LogInfo info, const char* fmt, va_list args, char* out);

static Logger logger = default_logger;
static LogPane pane = {0};

void logger_attach(Logger log) {
	logger = log;
}

void logger_log(LogInfo info, const char* fmt, ...) {
	if (!logger) return;

	switch (info.type) {
		case LOG_INFO: pane.data.info_count++; break;
		case LOG_WARNING: pane.data.warn_count++; break;
		case LOG_ERROR: pane.data.error_count++; break;
	}

	va_list args;
	va_start(args, fmt);

	LogLine line = {info.type, 0};
	logger(info, fmt, args, line.text);
	va_end(args);

	logger_push_line(line);
}

static int file_offset(char* path, int slash_count) {
	int pos = strlen(path) - 1;
	while (slash_count > 0 && pos >= 0) {
		if (path[pos] == '/' || path[pos] == '\\') slash_count--;
		if (slash_count == 0) break;
		pos--;
	}
	if (path[pos] != '/' && path[pos] != '\\') return pos;
	return pos + 1;
}

static void default_logger(LogInfo info, const char* fmt, va_list args, char* out) {
	int path_off = file_offset(info.file, 2);
	int offset = snprintf(out, LOGGER_LINE_MAX, "%s:%d - ", info.file + path_off, info.line);

    if (offset < 0 || offset >= LOGGER_LINE_MAX) return;

    vsnprintf(out + offset, LOGGER_LINE_MAX - offset, fmt, args);
	
	// TODO: maybe not log to terminal
	fprintf(stderr, "%s:%d - ", info.file + path_off, info.line);
	vfprintf(stderr, fmt, args);
	fprintf(stderr, "\n");
}

void logger_push_line(LogLine line) {
	pane.lines[pane.start] = line;
	pane.start = (pane.start + 1) % LOGGER_LINE_COUNT_MAX;

	if (pane.count < LOGGER_LINE_COUNT_MAX) pane.count++;
}

LogPane* logger_get_pane() {
	return &pane;
}

void logger_pane_clear(LogPane* pane) {
	pane->start = 0;
	pane->count = 0;

	pane->data = (LogData){0};
}
