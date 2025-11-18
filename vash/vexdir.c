/*
 * VASH - visual assistant for shell
 * Copyright (c) 1990-2025 Sergey Vovk <vsv>
 *
 * License: GPL and/or MIT,
 * see files COPYING and LICENSE
 */

#include "assist.h"

/*
 * defined via Makefile
 * can be   "/usr/new/lib/???/",
 *     or   "/usr/lib/???/",
 *     or   "/usr/local/lib/???/"
 */
const char   *vexdir = DESTLIB;
/* members of struct v (type VASH_PROC)
const char   *vapath  = VASH_PATH;
const char   *versn  = VERSN;
*/
