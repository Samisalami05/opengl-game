#ifndef LOGGER_H
#define LOGGER_H

#include <stdarg.h>
#include <stdint.h>

#define LOGGER_LINE_MAX 256
#define LOGGER_LINE_COUNT_MAX 128

typedef enum {
	LOG_INFO,
	LOG_WARNING,
	LOG_ERROR,
} LogType;

typedef struct {
	char file[64];
	uint32_t line;
	LogType type;
} LogInfo;

typedef struct {
	LogType type;
	char text[LOGGER_LINE_MAX];
} LogLine;

typedef struct {
	uint32_t info_count;
	uint32_t warn_count;
	uint32_t error_count;
} LogData;

typedef struct {
	LogData data;
	LogLine lines[LOGGER_LINE_COUNT_MAX]; // TODO: heap allocate in a ring buffer
	uint32_t start;
	uint32_t count;
} LogPane; // TODO: make thread safe

typedef void (*Logger)(LogInfo, const char*, va_list, char*);


void logger_attach(Logger log);
void logger_log(LogInfo info, const char* fmt, ...);
#define LOG(type, fmt, ...) \
	logger_log((LogInfo){__FILE__, __LINE__, type}, fmt, ##__VA_ARGS__)

void logger_push_line(LogLine line);
LogPane* logger_get_pane();
void logger_pane_clear(LogPane* pane);

#endif
