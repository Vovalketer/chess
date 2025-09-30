#ifndef SEARCH_H
#define SEARCH_H

#include "search_types.h"

int	 search_thread(void *arg);
void search_shutdown(void);
void search_reset(void);

void search_start(struct board *board, struct search_options options);
void search_stop(void);

#endif	// SEARCH_H
