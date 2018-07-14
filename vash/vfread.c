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

	len = itmlen = itmmax = 0;
	itms[itmmax] = itmbp = itmbuf;
	*itmbp++ = ' ';
	*itmbp++ = ' ';
	while ((c = getc(fpread)) != EOF) {
		if (&itmbuf[itmbsz] == itmbp)
			break;

		if (c == '\n') {
			/* конец очередной строки */
			*itmbp++ = '\0';
			if ( len > itmlen ) itmlen = len;
			len = 0 ;
			if (itmmax >= ITMMAX)
				break;
			if ((itmmax % 10) == 0) {
				w_chr('#'); fflush(vttout);
			}
			itmmax++;
			itms[itmmax] = itmbp;
			*itmbp++ = ' ';
			*itmbp++ = ' ';
		}
		else {
			len++;
			*itmbp++ = c;
		}
	}
	*itmbp++ = '\0';
	if (itmmax == 0) {
		strcpy(itmbuf, " /..");
		len = 4;
		itmmax++;
	}
	if ( len > itmlen ) itmlen = len;
	itmlen++;
}
