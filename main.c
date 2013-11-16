/* ============================================================================
 *  main.c: Autograder entry point.
 *  Copyright (C) 2013, Tyler J. Stachecki.
 *  All rights reserved.
 *
 *  This file is subject to the terms and conditions defined in
 *  file 'LICENSE', which is part of this source code package.
 * ========================================================================= */
#include "common.h"
#include "daemon.h"
#include "inotify.h"
#include "log.h"
#include "notify.h"
#include "request.h"
#include <limits.h>
#include <stdio.h>
#include <unistd.h>

/* Application entry point. */
int main(int argc, const char *argv[]) {
	struct notify_backend notifier = inotify_backend;
	struct request_pool pool;
	unsigned requests = 2;

	if (argc != 2) {
		printf("Usage: %s <path>\n", argv[0]);
		return 0;
	}

	if (daemonize() < 0) {
		fprintf(stderr, "Failed to daemonize.\n");
		return 1;
	}

	if (chdir(argv[1]) < 0) {
		fprintf(stderr, "Failed to set working directory.\n");
		return 2;
	}

	if (notifier.init(&notifier, argv[1]) < 0) {
		fprintf(stderr, "Failed to start notifier.\n");
		return 3;
	}

	if (create_request_pool(&pool, requests) != 0) {
		fprintf(stderr, "Failed to allocate memory.\n");
		notifier.deinit(&notifier);
		return 4;
	}

	log_msg("autograder: started, pid=%d", getpid());

	/* Main loop. */
	while (1) {
		const char *name;

		/* inotify's read can return EINTR. */
		/* For now, we'll just try reading again. */
		if (notifier.next(&notifier, &name))
			continue;

		process_request(&pool, name);
	}

	destroy_request_pool(&pool);
	notifier.deinit(&notifier);
	return 0;
}

