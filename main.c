/* ============================================================================
 *	main.c: Autograder entry point.
 *	Copyright (C) 2013, Tyler J. Stachecki.
 *	All rights reserved.
 *
 *	This file is subject to the terms and conditions defined in
 *	file 'LICENSE', which is part of this source code package.
 * ========================================================================= */
#include "common.h"
#include "daemon.h"
#include <stdio.h>

/* Application entry point. */
int main(int argc, const char *argv[]) {
	if (argc != 1) {
		printf("Usage: %s\n", argv[0]);
		return 0;
	}

	if (daemonize() < 0) {
		fprintf(stderr, "Failed to daemonize.\n");
		return 1;
	}

	return 0;
}

