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
	A_SO,   A_US,   A_VS,   A_MD,	A_MR,   A_MB,   A_MK,	0,      0,      0,
	};

LPA     *lpa_p[2] = {
	lpaout,         lpainp
	};

/* Color support for attr.cv page */
char   *sgrms[] = {
		"[0] BW(mono) ",
		"[1] BW+color ",
		0 };
extern int		sgrmode;

extern  LINE linem[];
LINE *linesgr = (LINE *)0;
/*char *sgra = "";*/
char *sgra = lpaout[0].lpa_sgr;
extern int cvt_sg();

LINE *getlsgr(line)
register LINE *line;
/*вернуть указатель на линию редактирования атрибутов цвета (SGR editor) */
{
	register LINE *lsgr;

	if (linesgr != (LINE *)0)
		return (linesgr);
	for (lsgr=linem; lsgr->size != 0; lsgr++) {
		if (lsgr->varl != &sgra || lsgr->cvtf != cvt_sg)
			continue;
		linesgr = lsgr;
		return(lsgr);
	}
	return( (LINE *)(0) );
}

LINE *getl4(line)
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

		cp_set(line->line + 1, line->colu, attr|INP);
		w_str(line->varl);
	}
	return(TRUE);
}

/* копипаста из cvt_va... */
cvt_sgr(line, cod, mod, str)
/*------------------*/
/* формат для цвета */
/*------------------*/
LINE *line;
kbcod cod;
char *mod;
char *str;
{
	char outstr[20];	/* строка для формирования вывода */
	int i;
//	int     va;             /* видеоатрибуты ( флаги ) */
	int posp;
	register LINE *line4;	/* указатель на базовую линию в 4-й строке */
	register LPA *lpap;
	char *sgr_v;

//	va = *(int *)line->cvts;
	i = (int) line->varl;
	if (*mod == 'w') {
		strcpy(outstr, " . .");
		strcpy(str, outstr);
	} else {
		/*hack. TODO cleanup */
		strcpy(outstr, " . .");
		strcpy(str, outstr);

		if (cod == ' ' || cod == KB_DE) {
			line4 = getl4(line); /* тут будет показан результат */
			/*выбрать, где будет настроен результат */
			switch (cod) {
			case ' ':
				outstr[1] = '%'; /*lpap = lpa_p[0];*/
				sgr_v = &lpaout[i].lpa_sgr[0];
				break;
			case KB_DE:
				outstr[3] = '%'; /*lpap = lpa_p[1];*/
				sgr_v = &lpainp[i].lpa_sgr[0];
				break;
			}
			if (sgr_v != (char*) 0) {

				sgra = sgr_v;
				/*w_line(line);*/
				cp_set(line->line, line->colu, line->attr);
				w_str(outstr);
				sgr_ed(line, cod);
			}
			/*w_line(line4); - вызов перенесен в sgr_ed() */
		}
	}
	return (TRUE);
}


sgr_ed(line, cod)
/*редактор атрибутов цвета (SGR composer)*/
LINE *line;
kbcod cod;
{
	kbcod ed_cod;
	register LINE *line4;	/* указатель на базовую линию в 4-й строке */
	int posp;
//	if (cod == ' ' || cod == KB_DE) {
	if ((/*linesgr=*/getlsgr(line)) != (LINE *)0) {
		linesgr->attr |= INP;
		posp = 0;
		w_line(linesgr, &posp);
		ed_cod = r_line(linesgr, &posp);
		linesgr->attr &= (~INP);

		line4 = getl4(line); /* тут будет показан результат с учетом новых параметров SGR */
		w_line(line4);
	}
//	}
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

cvt_sg(line, cod, mod, str) /*TODO*/ /*основа - копипаста из cvt_sp*/
/*---------------------*/
/* формат для атрибута */
/*---------------------*/
LINE   *line;
kbcod   cod;
char   *mod;
char   *str;
{
	register char **spp;
	register int max_co;
	int size;
	register char *si;
	register char *so;
	register int i;

	spp = (char **)line->varl;
	size = line->size;
	if (line->attr & PMT) size -= 1;
	max_co = maxco-2;
	if (str) {
		if(*mod == 'r') {
			strcpy(*spp, str);
		}
		else    {
			/*str[0] = '\0';*/  /* TODO: make line hidden */
			strncpy(str, *spp, size);
			if (strlen(*spp) >= size) str[size] = '\0';
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
extern  int     sgrtst();

#include "attr.i"

sgrtst(line, cod)
LINE *line;
kbcod cod;
{
	at_set(0);
//	return(TRUE);
	er_pag();
	w_page(linem);
	return(TRUE);
}

static  char    helpf[] = "vhseta.lb";

pag_a()
/*---------------------*/
/* настройка атрибутов */
/*---------------------*/
{
	u_page(linem, helpf);
	return(TRUE);
}
