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

#include <stdlib.h>
#include <stdio.h>
#include <locale.h>
#include "line.h"

static  char    devtty[] = "/dev/tty";

FILE *vttout;
#ifdef DEMOS2
int     vtti;
#endif

void
visini() {
    /* FILE *vttout = stderr; /* see line.h */
    /* FILE *vttout = stdout; /* see line.h */
    /* FILE *vttout = fdopen(0, "w+"); /* see line.h */

	if (!setlocale(LC_CTYPE, "")) {
			fprintf(stderr, "Can't set the specified locale! "
					"Check LANG, LC_CTYPE, LC_ALL.\n");
			exit(1);
	}
	setlocale(LC_COLLATE, "");

	if((vttout = fopen(devtty, "w+")) == NULL) {
		fprintf(stderr, "%s: no r/write access\n", devtty);
		exit(1);
	}


    int     vtti = 0;
}
