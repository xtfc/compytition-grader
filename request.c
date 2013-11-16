/* ============================================================================
 *  request.c: Request processor.
 *  Copyright (C) 2013, Tyler J. Stachecki.
 *  All rights reserved.
 *
 *  This file is subject to the terms and conditions defined in
 *  file 'LICENSE', which is part of this source code package.
 * ========================================================================= */
#include "common.h"
#include "log.h"
#include "request.h"
#include <stddef.h>
#include <stdlib.h>

/* Allocates memory for, and initializes, a request pool. */
int create_request_pool(struct request_pool *pool, unsigned size) {
	if ((pool->requests = (struct request*)
		malloc(sizeof(*(pool->requests)))) == NULL)
		return -1;

	pool->size = size;
	pool->cur = 0;
	return 0;
}

/* Releases any memory allocated for a request pool. */
void destroy_request_pool(const struct request_pool *pool) {
	free(pool->requests);
}

/* Queues up the request from the main loop for processing. */
/* Note: the argument must be copied into local memory; once */
/*       this function returns, it will be reclaimed. */
void process_request(struct request_pool *pool, const char *name) {
	log_msg("autograder: got submission: %s", name);
}

