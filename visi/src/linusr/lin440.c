/*
**      +----------+    БИБЛИОТЕКА ВВОДА-ВЫВОДА
**     (c) linlib  !    ДЛЯ АЛФАВИТНО-ЦИФРОВЫХ
**      +----------+    ВИДЕОТЕРМИНАЛОВ
**/

#include <stdlib.h>
#include <stdio.h>

/* utility for open file in path with mode specified */

FILE *
dafopen(filen, dirpath, fmode)
char *filen;
char *dirpath;
char *fmode;
{
	FILE *fp;
	char fname[200];
	register char *pathp;
	register int i;

#define FPMAX 8
	int fcnt;
	int srccnt;
	char *fdir[FPMAX];
	char *src;
	char *dst;
	char fpath[400];

	/* convert PATH syntax to array of strings fdir[] */
	fcnt = 0;
	fdir[fcnt] = dst = &fpath[0];
	for (src=dirpath, srccnt=0; srccnt<400; src++,dst++,srccnt++) {
		if (*src != '\0') {
			if (*src == ':') {
				*dst++ = '\0';
				/* advance to next array element */
				fcnt++;
				if (fcnt < FPMAX) {
					fdir[fcnt] = dst;
				} else {
					break;
				}
				src++;
			}
			*dst = *src;
		} else {
			*dst = *src; break;
		}
	}

	fp = NULL;
	for (i = 0; fp == NULL && i <= fcnt; i++) {
		/*VARARGS*/
		sprintf(fname, "%s/%s", fdir[i], filen);
		if ((fp = fopen(fname, /*"r"*/ fmode)) != NULL)
				return(fp);
	}
	return(NULL);
}
