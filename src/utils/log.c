#include "log.h"

#include <stdarg.h>
#include <stdio.h>
#include <time.h>

// TODO: thread safety: lock the log level and output file

int log_level = LOG_INFO;
FILE *log_file = NULL;

void log_set_level(int level) {
	log_level = level;
}

int log_get_level(void) {
	return log_level;
}

void log_set_output(FILE *file) {
	log_file = file;
}

static char *log_get_log_type(int level) {
	switch (level) {
		case LOG_TRACE:
			return "TRACE";
		case LOG_DEBUG:
			return "DEBUG";
		case LOG_INFO:
			return "INFO ";
		case LOG_WARN:
			return "WARN ";
		case LOG_ERROR:
			return "ERROR";
		default:
			return "UNKNOWN";
	}
}

void log_write(int level, const char *file, int line, const char *fmt, ...) {
	time_t now = time(NULL);
	struct tm *tm_info = localtime(&now);
	char timebuf[20];
	strftime(timebuf, sizeof(timebuf), "%Y-%m-%d %H:%M:%S", tm_info);

	FILE *out = log_file ? log_file : stderr;

	//  example output: [ERROR] 2025-06-28 06:38:44 tests/log_test.c:10:
	fprintf(out, "[%s] %s %s:%d: ", log_get_log_type(level), timebuf, file, line);

	// append contents
	va_list args;
	va_start(args, fmt);
	vfprintf(out, fmt, args);
	va_end(args);

	fprintf(out, "\n");
}
