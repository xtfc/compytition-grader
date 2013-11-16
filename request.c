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
#include <string.h>

void *request_thread(void *request_);

/* Allocates memory for, and initializes, a request pool. */
int create_request_pool(struct request_pool *pool, unsigned size) {
	unsigned i;

	if ((pool->requests = (struct request*)
		malloc(sizeof(*(pool->requests)) * size)) == NULL)
		return -1;

	if ((pool->freelist = (struct request**)
		malloc(sizeof(*(pool->freelist)) * size)) == NULL) {
		free(pool->requests);
		return -2;
	}

	/* Populate the free list. */
	for (i = 0; i < size; i++) {
		pool->freelist[i] = pool->requests + i;
		pool->requests[i].pool = pool;
	}

	pool->size = size;
	pool->cur = 0;

	pthread_cond_init(&pool->cond, NULL);
	pthread_mutex_init(&pool->lock, NULL);
	return 0;
}

/* Releases any memory allocated for a request pool. */
void destroy_request_pool(const struct request_pool *pool) {
	free(pool->freelist);
	free(pool->requests);
}

/* Queues up the request from the main loop for processing. */
/* Note: the argument must be copied into local memory; once */
/*       this function returns, it will be reclaimed. */
void process_request(struct request_pool *pool, const char *name) {
	struct request *request;

	pthread_mutex_lock(&pool->lock);

	/* We're full, sit around and */
	/* wait until requests free up. */
	if (pool->cur == pool->size)
		pthread_cond_wait(&pool->cond, &pool->lock);

	request = pool->freelist[pool->cur++];
	pthread_mutex_unlock(&pool->lock);

	/* Okay, we got the request... */
	/* Fill it and issue it. */
	strcpy(request->name, name);
	pthread_create(&request->thread, NULL,
		request_thread, (void*) request);
}

/* Thread function; fulfills the request. */
void *request_thread(void *request_) {
	struct request *request = (struct request*) request_;
	struct request_pool *pool = request->pool;

	log_msg("autograder: grading submission: %s", request->name);

	/* We're done; give the thread back to the pool. */
	/* Tap the condvar, too, so the dispatcher is aware. */
	pthread_mutex_lock(&pool->lock);
	pool->freelist[--(pool->cur)] = request;
	pthread_cond_signal(&pool->cond);
	pthread_mutex_unlock(&pool->lock);

	return NULL;
}

