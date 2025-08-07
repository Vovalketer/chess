#ifndef LOG_H
#define LOG_H

#include <stdarg.h>
#include <stdio.h>

#define LOG_TRACE 0
#define LOG_DEBUG 1
#define LOG_INFO  2
#define LOG_WARN  3
#define LOG_ERROR 4
#define LOG_NONE  10

typedef struct {
	char msg[128];
} LogMessage;

void	   log_set_level(int level);
int		   log_get_level(void);
void	   log_set_output(FILE *file);
void	   log_write(int level, const char *file, int line, const char *fmt, ...);
LogMessage log_create_message(const char *fmt, ...);

#define log_trace(...)                                             \
	do {                                                           \
		if (LOG_TRACE >= log_get_level())                          \
			log_write(LOG_TRACE, __FILE__, __LINE__, __VA_ARGS__); \
	} while (0)
#define log_debug(...)                                             \
	do {                                                           \
		if (LOG_DEBUG >= log_get_level())                          \
			log_write(LOG_DEBUG, __FILE__, __LINE__, __VA_ARGS__); \
	} while (0)
#define log_info(...)                                             \
	do {                                                          \
		if (LOG_INFO >= log_get_level())                          \
			log_write(LOG_INFO, __FILE__, __LINE__, __VA_ARGS__); \
	} while (0)
#define log_warning(...)                                          \
	do {                                                          \
		if (LOG_WARN >= log_get_level())                          \
			log_write(LOG_WARN, __FILE__, __LINE__, __VA_ARGS__); \
	} while (0)
#define log_error(...)                                             \
	do {                                                           \
		if (LOG_ERROR >= log_get_level())                          \
			log_write(LOG_ERROR, __FILE__, __LINE__, __VA_ARGS__); \
	} while (0)
#endif
