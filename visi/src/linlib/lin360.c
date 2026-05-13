/*
**      +----------+    БИБЛИОТЕКА ВВОДА-ВЫВОДА
**     (c) linlib  !    ДЛЯ АЛФАВИТНО-ЦИФРОВЫХ
**      +----------+    ВИДЕОТЕРМИНАЛОВ
**/

/*
 *      $Header: lin360.c,v 3.2 89/08/29 15:17:36 vsv Rel $
 *
#define W_PAGE_TAB
#ifndef W_PAGE_TAB
 *      $Log:	lin360.c,v $
 * Revision 3.2  89/08/29  15:17:36  vsv
 * ВЕРСИЯ LINLIB_3
 * 
 * Revision 3.1  88/06/27  15:21:45  vsv
 * РЕВИЗИЯ АРХИВА RCS
 * 
 * Revision 3.0  87/12/21  12:20:49  vsv
 * ПРЕДВАРИТЕЛЬНЫЙ ВЫПУСК.
 * 
#else W_PAGE_TAB
 *      Специальная версия с упрощениями.
#endif W_PAGE_TAB
 */

#include <stdio.h>
#include "line.h"
#include "line0.h"

/*----------------------------------------------------------------------*/
/* retro comment in russian. Please, don't use any tricks such described.
 *
 * ВНИМАНИЕ ! в функциях fnd_ar(), fnd_al(), ...
 *      используется небольшой фокус, основанный на
 *      реализации компиляторов C для PDP-11 (DECUS, unix,
 *      не проверено для Whitesmith) :
 *      аргументы можно передавать через регистровые описания
 *      в одинаковом порядке (имена не играют роли).
 *      В этом случае вызванная функция получит значения из вызвавшей,
 *      но не запортит их при возврате.
 *      Проблемы связанные с переносом, решаются путем выяснения
 *      отсутствия аналогичных возможностей на вашем компиляторе и
 *      переделке вызовов этих функций под стандартные соглашения.
 *      =========================================================
 *      Это, к сожалению, атавизмы linlib v2.0 -- там были такие
 *      команды, как "найти по стрелке до упора" (итерациями), но
 *      экономия от скорости передачи аргументов
 *      оказалась сомнительной -- этими командами попросту
 *      никто не пользовался.
 */

static LINE *
fnd_home(lni, page)
/* 
 * jump to first line of page
 */
register LINE *lni;
register LINE *page ;
{
	register LINE *lnj;

	for(lnj=page; lnj->size!=0; lnj++) {
		if(0 != (INP & (lnj->attr)) )
			return(lnj);
	}
	return(lni);
}

static LINE *
fnd_end(lni, page)
/*
/* jump to last line of page
 */
register LINE *lni;
register LINE *page ;
{
	register LINE *lnj;

	/* поиск от конца формы */
	for(lnj=lni; lnj->size != 0; lnj++ ) ;
	for(   ; lnj>=page; lnj--) {
		if(0 != (INP & (lnj->attr)) )
			return(lnj);
	}
	return(lni);
}

/*
 * find on arrow right
 */
static LINE *
fnd_ar (lni, page)
	register LINE *lni;
	register LINE *page ;
{
	register LINE *lnj;

	for(lnj=lni; lnj->size != 0; lnj++) {
		if( (INP & ~(lnj->attr)) )
			continue;
		if(((int)lnj->line == (int)lni->line)
		&& ((int)lnj->colu > (int)lni->colu))
			return(lnj);
	}
	/* ЗАЦИКЛИТЬ В ЭТОЙ ЖЕ СТРОКЕ */
	for(lnj=page; lnj->size != 0; lnj++) {
		if( (INP & ~(lnj->attr)) )
			continue;
		if(((int)lnj->line == (int)lni->line))
			return(lnj);
	}
	return(lni);
}
/*
 * find on arrow left
 */
static LINE *
fnd_al (lni, page)
	register LINE *lni;
	register LINE *page ;
{
	register LINE *lnj;

	/* find on the same row */
	for(lnj=lni; lnj>=page; lnj--) {
		if(lnj == lni)
			continue;
		if(0 == (INP & (lnj->attr)) )
			continue;
		if((lnj->line == lni->line)
		&& (lnj->colu < lni->colu))
			return(lnj);
		/*if((lnj->line < lni->line)
		&& (lnj->colu != lnj->colu))
				return(lnj);*/
	}
	/* find from end of page */
	for(lnj=lni; lnj->size != 0; lnj++ ) ;
	for(   ; lnj>=page; lnj--) {
		if(0 == (INP & (lnj->attr)) )
			continue;
		if((lnj->line == lni->line)) /* cycle on the same row */
/*		if(lnj->line < lni->line)*/ /* or shift to the upper line */
			return(lnj);
	}
	return(lni);
}
/*
 * find on arrow UP
 */
LINE *
fnd_au (lni, page)
	register LINE *lni;
	register LINE *page ;
{
	register LINE *lnj;
	int     nxt_line ;
	int		on_top = 0;

	/*
	 * find to back from current place
	 */
	for(lnj=lni; lnj>=page; lnj--) {
		if(lnj == lni)
			continue;
		if(0 == (INP & (lnj->attr)) )
			continue;
		/* on the same column and upper */
		if((lnj->line <  lni->line)
		 &&(lnj->colu == lni->colu))
			return(lnj);
		/*
		 * lower and left - on the vertically organized page part -
		 * defined with '+' sign an the begin of strings
		 * in .lav/.cv code
		 */
		if((lnj->line >  lni->line)
		 &&(lnj->colu <  lni->colu))
			return(lnj);
	}
	/*
	 * if the current place is on the beginning,
	 * find to current place from the end of page
	 */
	for(lnj=page; lnj->size != 0; lnj++) {
		if (0 != (INP & (lnj->attr)) && lnj == lni && on_top == 0) {
			on_top = 1;
			break;
		}
	}
	if (on_top) {
		for(lnj=lni; lnj->size != 0; lnj++) ; /* find the tail */
		for(   ; lnj>=page; lnj--) {
			/* nearest from the tail but not in current row
			 */			
			if((INP & (lnj->attr))
					&& (lnj->line != lni->line)
						&& (lnj->colu == lni->colu))
					/* && (lnj->line >= lni->line)) /* cycle from the end */
				return(lnj);
		}
	}
	return(lni);
}
/*
 * find on arrow down
 */
static LINE *
fnd_ad (lni, page)
	register LINE *lni;
	register LINE *page ;
{
	register LINE *lnj;
	int     nxt_line ;

	if((nxt_line = (int)lni->line ) < lframe->maxli ) { /* forward if possible */
		for(lnj=lni; lnj->size!=0; lnj++) {     
			if((lnj->line <= lni->line))
				continue;
			else if(0 == (INP & (lnj->attr)) )
				continue;
			else {                       /* row is below */
				if((lnj->colu + lnj->size) >= lni->colu)
					return(lnj); /* field under cursor */
				else
					continue; /* all field is on the left */
			}
		}
	}
	/* try to find from page begin */
	for(lnj=page; lnj->size!=0; lnj++) {
		if( (INP & ~(lnj->attr)) )
			continue;
		else if(lnj->colu >= lni->colu)
			return(lnj);
	}
	return(lni);      /* last resort */
}

/*------------------------------------------*/
/* simplest search: next line in page order */
/*------------------------------------------*/
static LINE *
fnd_nxt(lni, page)
	register LINE *lni;
	register LINE *page ;
{
	lni++;
	if(lni->size == 0)
		lni = page;          /* cycle to begin */
	return(lni);
}

/*----------------------------------------------------*/
/* simple reverse search: previous line in page order */
/*----------------------------------------------------*/
static LINE *
fnd_prv(lni, page)
	register LINE *lni;
	register LINE *page ;
{
	while(lni > page) {
		lni--;
		if (0 != (INP & lni->attr))
			return(lni);
	}
	/* no cycle to begin */
	return(page);
}

#ifndef W_PAGE_TAB
void
aj_tbl(scnd, line_e)
/*
 * setup table element referring to the base
 */
register LINE *scnd;
LINE *line_e;
{
	register LINE *base;
	int     isleft;
	LINE saved;

	saved = *scnd;
	isleft = (saved.flag & SUSL) ? 1 : 0;
	base = scnd;
	while(base >= line_e) {
		base--;
		if(isleft == 1) {
			if (base->colu <  saved.colu) {
				if (base->line == saved.line) {
					goto on_base;
				}
			}
		} else
		if(isleft == 0) {
			if (base->line <  saved.line) {
				if (base->colu == saved.colu) {
					goto on_base;
				}
			}
		} else {
			continue;
		}
	}
	/* сюда выхода нет, если страница правильно составлена */
	w_chr('\007');
	return;

on_base:
	*scnd = *base;
	scnd->line = saved.line;
	scnd->colu = saved.colu;
	scnd->varl += (linptr_t)(saved.varl);
}

#endif /*W_PAGE_TAB*/
/*----------------*/
/* write the page */
/*----------------*/
void w_page ( line_e )
LINE    *line_e;
{
	register LINE *line;

	/* сначала все, что не для ввода */
	for(line=line_e; line->size != 0; line++) {
#ifndef W_PAGE_TAB
		/* вторичный элемент таблицы надо сначала настроить... */
		if(line->flag & SUST){
			aj_tbl(line, line_e);
		}
#endif /*W_PAGE_TAB*/
		if( (INP & ~(line->attr)) )
		    /*    w_line(line, cod);    */
			w_line(line);
	}
	/* линии для ввода в последнюю очередь */
	for(line=line_e; line->size != 0; line++) {
		if( (INP & ~(line->attr)) == 0 )
		    /*    w_line(line, cod);    */
			w_line(line);
	}
}

kbcod
r_page(line_e, curline, posp)
/*---------------*/
/* read the page */
/*---------------*/
LINE    *line_e;             /* page to be edited */
LINE   **curline;             /* current line (status) */
int    *posp;                /* cursor position during edit process */
{
	static kbcod cod;           /* code returned from r_line() */
	static kbcod prvcod;		/* code returned last time */

	register LINE *lni;      /* line index in pointer form */
	LINE *cmplni;			 /* the lni before change retry */
	register LINE *page;     /* pointer to page at all (array of lines) */

	page = line_e;
	if(*curline != (LINE *)NULL)
		lni = *curline;
	else
		lni = line_e;

	/* ПРОПУСТИТЬ ТО, ЧТО НЕЛЬЗЯ РЕДАКТИРОВАТЬ */
	while( INP & ~(lni->attr)) {
		lni++;
		/* ЗАЦИКЛИТЬСЯ ЧЕРЕЗ НАЧАЛО */
		if(lni->size == 0)
			lni = line_e;
	}

	prvcod = cod;	/* 1st time zero is OK */
	cod = r_line(lni, posp);

	/* navigate to next read position on the page */
	switch( cod ) {
	case KB_PU :
	case KB_KH :
		lni = fnd_home(lni, page);
		break;
	case KB_PD :
	case KB_KE :
		lni = fnd_end(lni, page);
		break;
	case KB_AR :
		if ( (lni->flag & SUSR) == FALSE )
			lni = fnd_ar(lni, page) ;
		break ;
	case KB_AL :
		cmplni = lni;
		if ( (lni->flag & SUSL) == FALSE )
			lni = fnd_al(lni, page) ;
		if (cmplni == lni && prvcod == KB_AL)
			lni = fnd_prv(lni, page);
		break ;
	case KB_AU :
		if ( (lni->flag & SUSU) == FALSE )
			lni = fnd_au(lni, page) ;
		break ;
	case KB_AD :
		if ( (lni->flag & SUSD) == FALSE )
			lni = fnd_ad(lni, page) ;
		break ;
	case KB_TA :
		lni = fnd_nxt(lni, page) ;
		break ;
	case KB_NL :
		if ( (lni->flag & SUSNL) == FALSE )
			lni = fnd_nxt(lni, page) ;
		break ;
	case KB_RE :		/*TODO more sofisticated solution*/
		er_pag();
		w_page(page);
		break ;
	default :
		break;
	}

	*curline = lni;    /* save pointer to current line!!! */
	return(cod);
}
