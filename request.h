/* ============================================================================
 *  request.h: Request processor.
 *  Copyright (C) 2013, Tyler J. Stachecki.
 *  All rights reserved.
 *
 *  This file is subject to the terms and conditions defined in
 *  file 'LICENSE', which is part of this source code package.
 * ========================================================================= */
#ifndef __REQUEST_H__
#define __REQUEST_H__
#include <limits.h>
#include <pthread.h>

struct request_pool;

struct request {
	pthread_t thread;

	struct request_pool *pool;
	char name[NAME_MAX];
};

struct request_pool {
	pthread_mutex_t lock;
	pthread_cond_t cond;
	unsigned size;
	unsigned cur;

	struct request **freelist;
	struct request *requests;
};

int create_request_pool(struct request_pool *pool, unsigned size);
void destroy_request_pool(const struct request_pool *pool);

void process_request(struct request_pool *pool, const char *name);

#endif

