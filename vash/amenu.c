#include <stdlib.h>
#include <stdio.h>
#include "line.h"
#include "line0.h"
#include "assist.h"

/*
 * РАБОТА С МЕНЮ.
 *
 */

pre_vf()
/*----------------------------------*/
/* построить страницу главного меню */
/*----------------------------------*/
{
	register int i;
	register int j;
	int yyxx, x0x;
	unsigned msize, pagesize;

	if (clm._vf != (LINE *)0)
	    free((char *)clm._vf);

	msize = ((clm._yy * clm._xx) + 1) * sizeof(LINE);
#ifdef BSDpagesize
	pagesize = getpagesize();
	if (msize < pagesize) {
	    msize = pagesize;
	}
#endif
#ifndef RETRO
	if ((clm._vf=(LINE *)malloc(msize)) == (LINE *)0) {
#else
	if ((clm._vf=(LINE *)calloc((clm._yy * clm._xx)+1, sizeof(LINE))) == (LINE *)0) {
#endif
		/*VARARGS*/
		fprintf(stderr, "No memory for vf[]\n");
		fatal();
	}
	yyxx = clm._yy * clm._xx;
	i = x0x = 0;
	while( i < yyxx && i + clm._itmofs < clm._itmmax) {
		for (j = 0; j < clm._yy && i < yyxx && i + clm._itmofs < clm._itmmax; j++) {

			clm._vf[i] = *clm._ltmpl; /* скопировать шаблон */
			clm._vf[i].colu = clm._x0 + x0x; /* ((i / yy) * dx); */
			clm._vf[i].line = clm._y0 + j;   /* (i % yy); */
			clm._vf[i].size = clm._dx;
			/* clm._vf[i].size = 20; /* test for hack */
			/* надо учесть атрибут отмеченых файлов */
			if (*clm._itms[i + clm._itmofs] == MONEY) {
				clm._vf[i].attr = ATT|INP|NED|LFASTR;
			} else if (*clm._itms[i + clm._itmofs] == '<'
					|| *clm._itms[i + clm._itmofs] == '>') {
				clm._vf[i].attr = ATT|VEXT|INP|NED|LFASTR;
			} else {
				clm._vf[i].attr = clm._ltmpl->attr; /* TXT|INP|NED|LFASTR; */
			}
			clm._vf[i].varl = (char *)(&clm._itms[i + clm._itmofs]);
			i++;
		}
		x0x += clm._dx;
	}
	clm._vf[i].size = 0;
}

int
itmadj(cod)
/*
 * НАСТРОИТЬ ПОЛОЖЕНИЕ ОКНА, ПОЛОЖЕНИЕ КУРСОРА
 * И ВЕРНУТЬ ИНДЕКС ЛИНИИ ДЛЯ СТРАНИЦЫ МЕНЮ
 */
kbcod cod;
{
	register int i;
	register int itmr;      /* ДУБЛИКАТ itm */

	itmr = clm._itm;

	/* СНАЧАЛА СМЕСТИТЬ ГЛАВНЫЙ ИНДЕКС */

	switch (cod) {

	case KB_AL:
		itmr -= clm._yy;
		if (itmr < 0) itmr = 0;
		break;
	case KB_AU:
		if (itmr > 0) itmr--;
		break;
	case KB_AD:
		if (itmr < clm._itmmax-1) itmr++;
		break;
	case KB_AR:
		itmr += clm._yy;
		if (itmr >= clm._itmmax) itmr = clm._itmmax - 1;
		break;
	case KB_KH:
		itmr = clm._itmofs;
		break;
	case KB_KE:
		itmr =             clm._itmofs + (clm._yy * clm._xx) -1;
		if (clm._itmmax < (clm._itmofs + (clm._yy * clm._xx))) {
			itmr = clm._itmmax  -1;
		} else {
		}
		break;
	}
	clm._itm = itmr;

	/* поставить в соответствие значение itm
	 * и положение курсора на экране
	 */
	i = itmr - clm._itmofs;
	if (i >= 0 && i < clm._xx * clm._yy)
		;      /* ОКНО НЕ НАДО ДВИГАТЬ */
	else {
		switch (cod) {
		case 0:
			i = (clm._itm / clm._ofsx) * clm._ofsx;
			clm._itmofs = i;
			break;
		case KB_AL:
			clm._itmofs -= clm._ofsx;
			if (clm._itmofs < 0)
				clm._itmofs = 0;
			break;
		case KB_AR:
			clm._itmofs += clm._ofsx;
			if (clm._xx > 1
					&& clm._itmofs
							>= (((clm._itmmax / clm._ofsy)
									- (clm._ofsx / clm._ofsy)) * clm._ofsy))
				clm._itmofs = ((clm._itmmax / clm._ofsy)
						- (clm._ofsx / clm._ofsy)) * clm._ofsy;
			break;
		case KB_AU:
			clm._itmofs -= clm._ofsy;
			break;
		case KB_AD:
		case ' ':
			clm._itmofs += clm._ofsy;
			break;
/*
		case KB_KH:
			clm._itmofs = 0;
			break;
		case KB_KE:
			clm._itmofs = clm._itmmax;
			break;
*/
		}
		clm._itm = itmr;     /* ВОССТАНОВИТЬ ВНЕШ. */
		clritm();
		pre_vf();
		itmshow();
		w_page(clm._vf, 0);
	}
	return(clm._itm - clm._itmofs);
}

itmini()
/*-------------------------------*/
/* НАСТРОИТЬ НАЧАЛЬНЫЕ ПАРАМЕТРЫ */
/*-------------------------------*/
{
	int nxx;        /* КОЛИЧЕСТВО СТОЛБЦОВ НА ЭКРАНЕ */

	clm._yy = clm._yy_max;
	clm._xx = clm._xx1;

	if (clm._xx1 != 1) {
		clm._xx = lframe->maxco/(clm._itmlen + 1);
		if (clm._xx == 0) clm._xx = 1;
	}
	clm._yy = (clm._itmmax + clm._xx - 1)/clm._xx;        /* м.б. нужно меньше строчек... */
	if (clm._yy > clm._yy_max) clm._yy = clm._yy_max;
	nxx = (clm._itmmax + clm._yy - 1)/clm._yy;        /* м.б. нужно меньше колонок... */
	if (nxx < clm._xx) clm._xx = nxx;
	/* если строка только одна */
	if (clm._yy == 1) {
		clm._yy = 2;
		clm._xx = (clm._xx + 1)/2;
	}
	clm._x0 = ((lframe->maxco - ((clm._itmlen + 1) * clm._xx)) / (clm._xx + 1));
	clm._dx = clm._itmlen + 1 + clm._x0;
/*      dx = itmlen + 1 + ((lframe->maxco - ((itmlen + 1) * xx)) / (xx + 1));   */
	if (clm._dx > lframe->maxco) clm._dx = lframe->maxco;

	clm._y0 = lframe->maxli - clm._yy - 2;
	clm._x0 = (lframe->maxco - (clm._dx * clm._xx))/2;

	if (clm._xx == 1) {
		clm._ofsx = clm._yy;
		clm._ofsy = clm._yy/2;
		clm._x0 = (lframe->maxco - clm._itmlen) / 2;
	}
	else  {
		clm._ofsx = clm._yy * (clm._xx - 1);
		clm._ofsy = clm._yy;
	}
	if (clm._x0 < 0) clm._x0 = 0;

}
