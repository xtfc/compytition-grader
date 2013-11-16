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
#include <stdio.h>
#include <unistd.h>

/* Application entry point. */
int main(int argc, const char *argv[]) {
	struct notify_backend notifier = inotify_backend;

	if (argc != 2) {
		printf("Usage: %s <path>\n", argv[0]);
		return 0;
	}

	if (daemonize() < 0) {
		fprintf(stderr, "Failed to daemonize.\n");
		return 1;
	}

	if (notifier.init(&notifier, argv[1]) < 0) {
		fprintf(stderr, "Failed to start notifier.\n");
		return 2;
	}

	log_msg("autograder started; pid=%d", getpid());

	notifier.deinit(&notifier);
	return 0;
}

