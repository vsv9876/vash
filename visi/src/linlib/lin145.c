/*
**      +----------+    БИБЛИОТЕКА ВВОДА-ВЫВОДА
**     (c) linlib  !    ДЛЯ АЛФАВИТНО-ЦИФРОВЫХ
**      +----------+    ВИДЕОТЕРМИНАЛОВ
**/

/*
 *      $Header: lin145.c,v 1.1 90/12/27 16:28:43 vsv Rel $
 *      $Log:	lin145.c,v $
 * Revision 1.1  90/12/27  16:28:43  vsv
 * ВЕРСИЯ LINLIB_3
 * 
 */

#include <stdio.h>
#include "line.h"

static  char    devtty[] = "/dev/tty";

FILE *vttout;
#ifdef DEMOS2
int     vtti;
#endif

visini() {
    /* FILE *vttout = stderr; /* see line.h */
    /* FILE *vttout = stdout; /* see line.h */
    /* FILE *vttout = fdopen(0, "w+"); /* see line.h */

	if((vttout = fopen(devtty, "w+")) == NULL) {
		fprintf(stderr, "%s: no r/write access\n", devtty);
		exit(1);
	}


    int     vtti = 0;
}
