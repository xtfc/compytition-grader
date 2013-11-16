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
#include <limits.h>
#include <stddef.h>
#include <stdlib.h>
#include <sys/inotify.h>
#include <unistd.h>

#define BUF_LEN (sizeof(struct inotify_event) + NAME_MAX + 1)

static int deinit(struct notify_backend *backend);
static int init(struct notify_backend *backend, const char *pathname);
static int next(struct notify_backend *backend, const char **name);

struct inotify_private {
	char buf[BUF_LEN];
	int size;
	int fd;
	int wd;
};

const struct notify_backend inotify_backend = {
	deinit,
	init,
	next,
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

	if ((wd = inotify_add_watch(fd, pathname, IN_CLOSE_WRITE | IN_ATTRIB)) < 0) {
		dperror("inotify_add_watch");
		close(fd);
		return -3;
	}

	backend->opaque = opaque;
	opaque->fd = fd;
	opaque->wd = wd;
	return 0;
}

/* Checks if inotify has any events for us. Blocks on input. */
static int next(struct notify_backend *backend, const char **name) {
	struct inotify_private *opaque = (struct inotify_private*) (backend->opaque);
	struct inotify_event *event = (struct inotify_event*) (opaque->buf);
	int ret;


	/* TODO: Handle short reads from inotify. */
	/* TODO: It shouldn't ever happen, but... */
	if ((ret = read(opaque->fd, opaque->buf, sizeof(opaque->buf))) < 0) {
		dperror("read");
		*name = NULL;
		return -1;
	}

	*name = opaque->buf + sizeof(struct inotify_event);
	opaque->size = ret;
	return 0;
}

