/*
 * Определяется через маке,
 * может быть   "/usr/new/lib/???/", или
 *              "/usr/lib/???/", или
 *              "/usr/local/lib/???/"
 */
/*NOXSTR*/

#include "assist.h"

char   *vexdir = DESTLIB;
char   *versn  = VERSN;
char   *vapath  = VASH_PATH;
char   *vashrc = NULL; /* defined in runtime by default */
