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

#ifdef VTTOUT
static  char    devtty[] = "/dev/tty";
#endif

/*FILE *vttout;*/

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
#ifdef VTTOUT
	if((vttout = fopen(devtty, "w")) == NULL) {
		fprintf(stderr, "%s: no write access\n", devtty);
		exit(1);
	}
#else
/*	vttout = stdout;*/

#if 0
	if((vttout = fdopen(dup(2), "w")) == NULL) {
		fprintf(stderr, "%s: no write access\n", "fd[1]");
		exit(1);
	}
#endif
#endif

    int     vtti = 0;
}

