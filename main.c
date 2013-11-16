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
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

/* Continually fetches requests. */
static void process(struct notify_backend *
	notifier, struct request_pool *pool) {
	const char *name;

	while (1) {
		/* inotify's read can return EINTR. */
		/* For now, we'll just try reading again. */
		if (notifier->next(notifier, &name))
			continue;

		process_request(pool, name);
	}
}

/* Prints the usage to stdout. */
static void usage(const char *argv0) {
	printf("Usage: %s ... <path>\n\n"
		"Options:\n"
		"  -threads [num]\n",
		argv0);
}

/* Application entry point. */
int main(int argc, const char *argv[]) {
	struct notify_backend notifier = inotify_backend;
	const char *path = argv[argc - 1];
	struct request_pool pool;
	char fullpath[PATH_MAX];

	struct stat sb;
	unsigned requests = 2;
	int i;

	if (argc < 2) {
		usage(argv[0]);
		return 0;
	}

	/* Check to ensure that it's a valid directory, */
	/* and that we have permissions to access it. */
	if (stat(path, &sb) != 0) {
		printf("Failed to stat: %s\n", path);
		return 255;
	}

	if (!S_ISDIR(sb.st_mode)) {
		printf("%s is not a directory.\n", path);
		return 255;
	}

	if (access(path, R_OK | X_OK) != 0) {
		printf("Insufficient permissions on %s.\n", path);
		return 255;
	}

	/* Not POSIX, but a useful function... */
	if (realpath(path, fullpath) == NULL) {
		printf("Unable to determine the canonicalized absolute pathname.\n");
		return 255;
	}

	/* Parse command line arguments. */
	/* TODO: Convert this to getops? */
	for (i = 1; i < (argc - 1); i++) {
		const char *arg = argv[i];

		while (*arg == ' ');

		/* Accept -, --, and / */
		if (*arg == '-') {
			arg++;

			if (*arg == '-')
				arg++;
		}

		else if (*arg == '/')
			arg++;

		/* Set number of threads to use. */
		if (!strcmp(arg, "threads")) {
			char *endptr;

			requests = strtol(argv[++i], &endptr, 10);

			if (*endptr != '\0') {
				printf("-threads requires a numeric argument.\n");
				return 255;
			}
		}
	}

	if (daemonize() < 0 || chdir(fullpath) < 0) {
		fprintf(stderr, "Failed to daemonize.\n");
		return 1;
	}

	if (notifier.init(&notifier, fullpath) < 0) {
		fprintf(stderr, "Failed to start notifier.\n");
		return 2;
	}

	if (create_request_pool(&pool, requests) != 0) {
		fprintf(stderr, "Failed to allocate memory.\n");
		notifier.deinit(&notifier);
		return 3;
	}

	log_msg("autograder: started, pid=%d", getpid());
	process(&notifier, &pool);

	destroy_request_pool(&pool);
	notifier.deinit(&notifier);
	return 0;
}

