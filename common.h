/* ============================================================================
 *	common.h: Macros, typedefs, etc.
 *	Copyright (C) 2013, Tyler J. Stachecki.
 *	All rights reserved.
 *
 *	This file is subject to the terms and conditions defined in
 *	file 'LICENSE', which is part of this source code package.
 * ========================================================================= */
#ifndef __COMMON_H__
#define __COMMON_H__

#ifndef NDEBUG
#include <stdio.h>
#define dperror(s) perror(s)
#else
#define dperror(s)
#endif

#endif

