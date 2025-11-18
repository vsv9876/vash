/*
 * VASH - visual assistant for shell
 * Copyright (c) 1990-2025 Sergey Vovk <vsv>
 *
 * License: GPL and/or MIT,
 * see files COPYING and LICENSE
 */

#include <stddef.h>
#include <stdlib.h>

#include <sys/types.h>
#include <stdio.h>
#include <signal.h>

/* #include <ndir.h> */      /* BSD 4.2 & DEMOS/P mistake */
#include <dirent.h>

#include "line.h"
#include "assist.h"

static stopvfread = 0;
static FILE *vfr_fp;

/*ARGSUSED*/
void sig_vfread(signo)
{
		stopvfread = 1;
		ungetc('\0', vfr_fp);
}
/*
signal(SIGINT, SIG_DFL);
signal(SIGQUIT, SIG_DFL);
 */
void
vfread(fpread)
/* -- calling from vndir.c:fil_vf() */
FILE *fpread;
/*
 * fill item buffer vith extra (shell) command
 * посчитать пункты, определить макс. длину пункта
 */
{
    char *itmbp;  /* item buffer pointer */
    int   itmbsz;
    char *ibp_new; /* new item buffer pointer */

    ptrdiff_t ibp_ofs; /* ofset for stored items */
    int noext = 0;
    int  itmbextn = 1;  /* extent total number */
    int  i;
	short len;
	int c;

	/*signal(SIGINT, sig_vfread);*/
	/* initial allocation */
	if (clm._itmbuf == NULL) {
		itmbsz = 1 + clm._itmbsz;
		if ((clm._itmbuf = malloc(itmbsz)) == NULL) {
			w_emsg("malloc for main buffer: NO MEM... fatal");
			onintr(1);
		}
		clm._itms[0] = itmbp = clm._itmbuf;
	    /**itmbp++ = ' '; /* 1st placeholder */
	}

	vfr_fp = fpread;
	/*signal(SIGINT, SIG_DFL);*/
	io_set(IO_TTYPE);
	len = clm._itmlen = clm._itmmax = 0;
	clm._itms[clm._itmmax] = itmbp = clm._itmbuf;
	*itmbp++ = ' ';
	*itmbp++ = ' ';
	while ((c = getc(fpread)) != EOF && stopvfread == 0) {
/*
		if (&clm._itmbuf[clm._itmbsz] == itmbp)
			break;
*/
		/* try to extent item buffer if no room for current entry */
		if (&clm._itmbuf[itmbsz] <= &itmbp[1]) {
			itmbextn += 1;
			itmbsz = 1 + (clm._itmbsz * itmbextn);
			if ((ibp_new = realloc(clm._itmbuf, itmbsz)) == NULL) {
				w_emsg("No mem for all menu items");
				break;
			}
			ibp_ofs = ibp_new - clm._itmbuf;
			clm._itmbuf = ibp_new;
			/* fix stored pointers */
			if (ibp_ofs != 0)
				for (i = 0; i < clm._itmmax; i++)
					clm._itms[i] += ibp_ofs;
			itmbp += ibp_ofs;
		}

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
		strcpy(clm._itmbuf, " .."); /* finish dummy list with ".." element */
		len = 4;
		clm._itmmax++;
	}
	if ( len > clm._itmlen ) clm._itmlen = len;
	clm._itmlen++;
	io_set(IO_VIDEO);
	/*signal(SIGINT, SIG_IGN);*/
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
	size_t lsize;  				/* size of significant part of LINE */
	register size_t rsize; 		/* real size of string will be shown */
	register char *s;
	register char *v;

	s = str;

	spp = (char **)line->varl;
	v = *spp;
	lsize = line->size; /* - 1; /* magic symbol plased at index 1, after PMT placeholder */
	if (line->attr & PMT) lsize -= 1; /* PMT causes a placeholder for marker '#'*/
	rsize = /*strlen(v)*/u8slen(v);
	if (rsize >= lsize) {
		rsize = lsize;
	}

	/* max_co = lframe->maxco - 2; */
	if (str) {
		if(*mod == 'r') { /* never used on input, this part of code may be removed because not used */
			; /*strcpy(*v, str);*/
		}
		if(*mod == 'w') {
			clm._line = line;
			*s++ = *v++; /* PMT itself */
			v++;		/* skip marker placeholder */
			/*strncpy*/u8snu8s(s, v, rsize); /* string - file name */
			v = *spp;
			rsize = strlen(v);
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



