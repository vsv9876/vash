/*
**      +----------+    БИБЛИОТЕКА ВВОДА-ВЫВОДА
**     (c) linlib  !    ДЛЯ АЛФАВИТНО-ЦИФРОВЫХ
**      +----------+    ВИДЕОТЕРМИНАЛОВ
**/

#include <stdio.h>
#include "line.h"
#include "line0.h"
#include "lineva.h"

extern LPA lpainp[];
extern LPA lpaout[];

int     lpa_pi = 0;     /* РЕЖИМ ИЗМЕНЕНИЯ АТРИБУТОВ (НА ВВОДЕ/НА ВЫВОДЕ) */

int     wamask[10] = {
	A_SO,   A_US,   A_VS,   A_MD,
	A_MR,   A_MB,   A_MK,
	0,      0,      0,
	};

LPA     *lpa_p[2] = {
	lpaout,         lpainp
	};

/* Color support for attr.cv page */
char   *gsrms[] = {
		" 0/BW only    ",
		" 1/BW + color ",
		0 };
extern int		gsrmode;

extern  LINE linem[];
LINE *
getl4(line)
/*-------------------------------------------------------*/
/* вернуть указатель на базовую линию в 4й строке экрана */
/*-------------------------------------------------------*/
register LINE *line;
{
	register LINE *l;

	for (l=linem; l->size != 0; l++) {
		if (l->line != 4)
			continue;
		if (l->colu == line->colu)
			return( l );
	}
	return( (LINE *)(0) );
}

cvt_atr(line, cod, mod, str)
/*---------------------------------*/
/* формат для изображения лог.типа */
/*---------------------------------*/
LINE   *line;
kbcod   cod;
char   *mod;
char   *str;
{
	int attr;

	if(*mod == 'w') {
		attr = line->attr & VIDEO;
		strcpy(str, line->varl);

		cp_set(line->line + 2, line->colu, attr|INP);
		w_str(line->varl);
	}
	return(TRUE);
}

cvt_va(line, cod, mod, str)
/*---------------------*/
/* формат для атрибута */
/*---------------------*/
LINE   *line;
kbcod   cod;
char   *mod;
char   *str;
{
	char    outstr[6];      /* строка для формирования вывода */
	int     i;
	int     va;             /* видеоатрибуты ( флаги ) */
	register int *ap;       /* указатель на атрибут */
	register LINE *line4;   /* указатель на базовую линию в 5-й строке */
	register LPA *lpap;

	va = *(int *)line->cvts;
	i = (int)line->varl;

	if(*mod == 'w') {
		strcpy(outstr, " . .");
		if(lpainp[i].lpa_a & va) outstr[3] = 'x';
		if(lpaout[i].lpa_a & va) outstr[1] = 'x';
		strcpy(str, outstr);
	} else {
		if(cod == ' ' || cod == KB_DE) {
			line4 = getl4(line);

			switch(cod) {
			case ' ':
				lpap = lpa_p[0];
				break;
			case KB_DE:
				lpap = lpa_p[1];
				break;
			}
			ap = &(lpap[ i ].lpa_a);

			if((*ap) & va) { (*ap) = (*ap) & (~va); }
			else           { (*ap) = (*ap) | ( va); }

			w_line(line4);
		}
	}
	return(TRUE);
}

cvt_pmt(line, cod, mod, str)
/*----------------------*/
/* формат для подсказки */
/*----------------------*/
LINE   *line;
kbcod   cod;
char   *mod;
char   *str;
{
	int     i;
	kbcod tmpcod;

	i = (int)line->varl;

	if(*mod == 'w') {
		sprintf(str, " %c %c",
		lpaout[i].lpa_p, lpainp[i].lpa_p);
	} else {
		if (cod == ' ' || cod == KB_DE) {
			switch(cod) {
			case ' ':
				  lpa_pi = 0; break;
			case KB_DE:
				  lpa_pi = 1; break;
			}
			w_msg(ATT, "Please, type a prompter char ");
			if (lpa_pi)
				w_str("on input: ");
			else
				w_str("on output: ");

			tmpcod = r_key();
			if (tmpcod == 0 || cod1(tmpcod) != 0)
				w_msg(ERR, "Control key pressed");
			else    {
				if(lpa_pi) {    /* подсказка для вывода */
					lpainp[i].lpa_p = cod0(tmpcod);
				} else {        /* для ввода */
					lpaout[i].lpa_p = cod0(tmpcod);
				}
				w_msg(TXT, "");
			}
		}
	}
	return(TRUE);
}

extern  int     cvt_hl();
extern  int     cvt_s();

#include "attr.i"

static  char    helpf[] = "vhseta.lb";

pag_a()
/*---------------------*/
/* настройка атрибутов */
/*---------------------*/
{
	u_page(linem, helpf);
	return(TRUE);
}
