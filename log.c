/* ============================================================================
 *  log.c: Daemon logging facilities.
 *  Copyright (C) 2013, Tyler J. Stachecki.
 *  All rights reserved.
 *
 *  This file is subject to the terms and conditions defined in
 *  file 'LICENSE', which is part of this source code package.
 * ========================================================================= */
#include "common.h"
#include "log.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

/* Writes a formatted, timestamped string to the log. */
/* Return the number of characters printed to the log. */
int log_msg(const char *fmt, ...) {
	time_t ltime;
	va_list ap;
	int ret = 0;

	/* Timestamp the log message, if possible. */
	if ((ltime = time(NULL)) != (time_t)-1) {
		char *time = ctime(&ltime), *end;
		char timestr[32];

		/* Remove the trailing '\n' from ctime's  return. */
		strncpy(timestr, ctime(&ltime), sizeof(timestr));

		timestr[sizeof(timestr) - 1] = '\0';
		if (end = strchr(timestr, '\n'))
			*end = '\0';

		ret = fprintf(stderr, "%s: ", timestr);
	}

	va_start(ap, fmt);
	ret += vfprintf(stderr, fmt, ap);
	va_end(ap);

	if (fputc('\n', stderr) != EOF)
		ret += 1;

	return ret;
}

