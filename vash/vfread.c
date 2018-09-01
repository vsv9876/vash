#include <sys/types.h>
#include <stdio.h>

/* #include <ndir.h> */      /* BSD 4.2 & DEMOS/P mistake */
#include <dirent.h>

#include "line.h"
#include "assist.h"

extern  char  Crepf[];
extern  char  Cfill[];

vfread(fpread)
FILE *fpread;
/*
 * Заполнить буфер пунктов меню через внешнюю команду:
 * посчитать пункты, определить макс. длину пункта
 */
{
	register char *itmbp;
	short len;
	int c;

	len = clm._itmlen = clm._itmmax = 0;
	clm._itms[clm._itmmax] = itmbp = clm._itmbuf;
	*itmbp++ = ' ';
	*itmbp++ = ' ';
	while ((c = getc(fpread)) != EOF) {
		if (&clm._itmbuf[clm._itmbsz] == itmbp)
			break;

		if (c == '\n') {
			/* конец очередной строки */
			*itmbp++ = '\0';
			if ( len > clm._itmlen ) clm._itmlen = len;
			len = 0 ;
			if (clm._itmmax >= ITMMAX)
				break;
			if ((clm._itmmax % 10) == 0) {
				w_chr('#'); fflush(vttout);
			}
			clm._itmmax++;
			clm._itms[clm._itmmax] = itmbp;
			*itmbp++ = ' ';
			*itmbp++ = ' ';
		}
		else {
			len++;
			*itmbp++ = c;
		}
	}
	*itmbp++ = '\0';
	if (clm._itmmax == 0) {
		strcpy(clm._itmbuf, " /..");
		len = 4;
		clm._itmmax++;
	}
	if ( len > clm._itmlen ) clm._itmlen = len;
	clm._itmlen++;
}
