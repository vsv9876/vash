#include <sys/types.h>
#include <stdio.h>

/* #include <ndir.h> */      /* BSD 4.2 & DEMOS/P mistake */
#include <dirent.h>

#include "line.h"
#include "assist.h"

extern  char  Crepf[];
extern  char  Cfill[];

vfread(fpread)
/*никогда не вызывается?!!!*/
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
		strcpy(clm._itmbuf, " /.."); /* finsh dummy list with ".." element */
		len = 4;
		clm._itmmax++;
	}
	if ( len > clm._itmlen ) clm._itmlen = len;
	clm._itmlen++;
}

int     cvt_vf(line, cod, mod, str)
/*
 * special version of cvt_sp() used in menu vf -
 * 2nd symbol shown at the end, like ls -F
 */
LINE *line;
kbcod cod;
char *mod;
char *str;
{
	register char **spp;
	/*register int max_co;*/
	size_t lsize;  /* size of significant part of LINE */
	register size_t rsize; /* real size of string, defines place of ist symbol shown at the end of string */
	register char *s;
	register char *v;

	s = str;

	spp = (char **)line->varl;
	v = *spp;
	lsize = line->size - 1; /* magic symbol plased at index 1, after PMT placeholder */
	if (line->attr & PMT) lsize -= 1; /* PMT causes a placeholder for marker '#'*/
	rsize = strlen(v);
	if (rsize >= lsize) {
		rsize = lsize;
	}

	/* max_co = maxco-2; */
	if (str) {
		if(*mod == 'r') { /* never used on input, code may be removed because not used */
			; /*strcpy(*v, str);*/
		}
		if(*mod == 'w') {
			*s++ = *v++; /* PMT itself */
			v++;
			strncpy(s, v, rsize); /* string - file name */
			v = *spp;
			if (v[1] != ' ') {
				s[rsize-2] = v[1];
				s[rsize-1] = '\0';
			} else  {
				s[rsize-2] = '\0';
			}
		}
	}
	return(TRUE);
}
