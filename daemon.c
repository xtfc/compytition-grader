/* ============================================================================
 *  daemon.c: Converts process into a daemon.
 *  Copyright (C) 2013, Tyler J. Stachecki.
 *  All rights reserved.
 *
 *  This file is subject to the terms and conditions defined in
 *  file 'LICENSE', which is part of this source code package.
 * ========================================================================= */
#include "common.h"
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

/* Location of the logfile; assume system complies with FHS. */
static const char *logfile = "/var/log/autograder.log";

/* Reparent ourselves onto init, drop controlling terminal, etc.
 * On success, return 0. Otherwise, leave errno set and return
 * a negative value to indicate an error. */
int daemonize(void) {
	static const int logflags = O_WRONLY | O_APPEND | O_CREAT;
	int logfd, nullfd;
	pid_t pid;

	/* Make ourselves autonomous and */
	/* allow parent shell to proceed. */
	if ((pid = fork()) < 0) {
		dperror("fork");
		return -1;
	}

	else if (pid > 0) {
		exit(EXIT_SUCCESS);
	}

	/* Start a new session so to ensure that */
	/* we don't have a controlling terminal. */
	if (setsid() < 0) {
		dperror("setsid");
		return -2;
	}

	/* Double fork again to ensure that we */
	/* can't regain a controlling terminal. */
	if ((pid = fork()) < 0) {
		dperror("fork");
		return -3;
	}

	else if (pid > 0) {
		fprintf(stderr, "Autograder PID: %d\n", pid);
		exit(EXIT_SUCCESS);
	}

	/* Make sure we don't hold onto an otherwise */
	/* unused directory and cause issues later. */
	if (chdir("/") < 0) {
		dperror("chdir");
		return -4;
	}

	/* Allow us to have control of what we write. */
	/* Our umask may be undefined until now. */
	umask(0);

	/* Open log file(s) so we can record our status and progress. */
	if ((logfd = open(logfile, logflags, S_IRUSR | S_IWUSR)) < 0) {
		dperror("open");
		return -5;
	}

	/* Tie off any loose ends. */
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);

	/* Redirect stderr/stdout to a log file. */
	if (dup2(logfd, STDERR_FILENO) < 0) {
		static const char *error = "Failed to establish stderr.\n";
		write(logfd, error, sizeof(error));
		return -6;
	}

	if (dup2(logfd, STDOUT_FILENO) < 0) {
		static const char *error = "Failed to establish stdout.\n";
		write(logfd, error, sizeof(error));
		return -7;
	}

	close(logfd);

	/* Open /dev/null as stdin. */
	if ((nullfd = open("/dev/null", O_RDONLY, 0)) < 0) {
		static const char *error = "Failed to open /dev/null.\n";
		write(logfd, error, sizeof(error));
		return -8;
	}

	if (dup2(nullfd, STDIN_FILENO) < 0) {
		static const char *error = "Failed to establish stdin.\n";
		write(logfd, error, sizeof(error));
		return -9;
	}

	close(nullfd);
	return 0;
}

