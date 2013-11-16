/* ============================================================================
 *  inotify.c: inotify backend for autograder.
 *  Copyright (C) 2013, Tyler J. Stachecki.
 *  All rights reserved.
 *
 *  This file is subject to the terms and conditions defined in
 *  file 'LICENSE', which is part of this source code package.
 * ========================================================================= */
#include "common.h"
#include "inotify.h"
#include "notify.h"
#include <stddef.h>
#include <stdlib.h>
#include <sys/inotify.h>
#include <unistd.h>

static int deinit(struct notify_backend *backend);
static int init(struct notify_backend *backend, const char *pathname);

struct inotify_private {
	int fd;
	int wd;
};

const struct notify_backend inotify_backend = {
	deinit,
	init,
	"inotify",
	NULL,
};

/* Releases any resources used for the backend. */
static int deinit(struct notify_backend *backend) {
	struct inotify_private *opaque = (struct inotify_private*) (backend->opaque);

	inotify_rm_watch(opaque->fd, opaque->wd);
	close(opaque->fd);
	free(opaque);
	return 0;
}

/* Initializes an inotify fd, binds it to the backend. */
static int init(struct notify_backend *backend, const char *pathname) {
	struct inotify_private *opaque;
	int fd, wd;

	if ((fd = inotify_init()) < 0) {
		dperror("inotify_init");
		return -1;
	}

	if ((opaque = (struct inotify_private*) malloc(sizeof(*opaque))) == NULL) {
		dperror("malloc");
		close(fd);
		return -2;
	}	

	if ((wd = inotify_add_watch(fd, pathname, IN_CLOSE_WRITE)) < 0) {
		dperror("inotify_add_watch");
		close(fd);
		return -3;
	}

	backend->opaque = opaque;
	opaque->fd = fd;
	opaque->wd = wd;
	return 0;
}

