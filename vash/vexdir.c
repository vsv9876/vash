/*
 * Определяется через маке,
 * может быть   "/usr/new/lib/???/", или
 *              "/usr/lib/???/", или
 *              "/usr/local/lib/???/"
 */
/*NOXSTR*/

#include "assist.h"

const char   *vexdir = DESTLIB;
const char   *versn  = VERSN;
const char   *vapath  = VASH_PATH;
const char   *vashrc = NULL; /* defined in runtime by default */
