/* ============================================================================
 *  notify.h: Definitions for notifiers.
 *  Copyright (C) 2013, Tyler J. Stachecki.
 *  All rights reserved.
 *
 *  This file is subject to the terms and conditions defined in
 *  file 'LICENSE', which is part of this source code package.
 * ========================================================================= */
#ifndef __NOTIFY_H__
#define __NOTIFY_H__
#include "common.h"

struct notify_backend {
	int (*const deinit)(struct notify_backend *);
	int (*const init)(struct notify_backend *, const char *pathname);
	int (*const next)(struct notify_backend *, const char **name);
	const char *backend_name;
	void *opaque;
};

#endif

