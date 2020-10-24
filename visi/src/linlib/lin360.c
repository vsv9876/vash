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
/*
 * ВНИМАНИЕ ! В ФУНКЦИЯХ fnd_ar(), fnd_al(), ...
 *      ИСПОЛЬЗУЕТСЯ НЕБОЛЬШОЙ ФОКУС, ОСНОВАННЫЙ НА
 *      РЕАЛИЗАЦИИ КОМПИЛЯТОРОВ СИ ДЛЯ PDP-11 (DECUS, UNIX,
 *      НЕ ПРОВЕРЕНО ДЛЯ Whitesmith) :
 *      АРГУМЕНТЫ МОЖНО ПЕРЕДАВАТЬ ЧЕРЕЗ РЕГИСТРОВЫЕ ОПИСАНИЯ
 *      В ОДИНАКОВОМ ПОРЯДКЕ (ИМЕНА НЕ ИГРАЮТ РОЛИ).
 *      В ЭТОМ СЛУЧАЕ ВЫЗВАННАЯ ФУНКЦИЯ ПОЛУЧИТ ЗНАЧЕНИЯ ИЗ ВЫЗВАВШЕЙ,
 *      НО НЕ ЗАПОРТИТ ИХ ПРИ ВОЗВРАТЕ.
 *      ПРОБЛЕМЫ СВЯЗАННЫЕ С ПЕРЕНОСОМ, РЕШАЮТСЯ ПУТЕМ ВЫЯСНЕНИЯ
 *      ОТСУТСТВИЯ АНАЛОГИЧНЫХ ВОЗМОЖНОСТЕЙ НА ВАШЕМ КОМПИЛЯТОРЕ И
 *      ПЕРЕДЕЛКЕ ВЫЗОВОВ ЭТИХ ФУНКЦИЙ ПОД СТАНДАРТНЫЕ СОГЛАШЕНИЯ.
 *      =========================================================
 *      ЭТО, К СОЖАЛЕНИЮ, АТАВИЗМЫ linlib v2.0 -- ТАМ БЫЛИ ТАКИЕ
 *      КОМАНДЫ, КАК "НАЙТИ ПО СТРЕЛКЕ ДО УПОРА" (ИТЕРАЦИЯМИ), НО
 *      ЭКОНОМИЯ ОТ СКОРОСТИ ПЕРЕДАЧИ АРГУМЕНТОВ
 *      ОКАЗАЛАСЬ СОМНИТЕЛЬНОЙ -- ЭТИМИ КОМАНДАМИ ПОПРОСТУ
 *      НИКТО НЕ ПОЛЬЗОВАЛСЯ.
 */

static LINE *
fnd_home(lni, page)
/* jump to first line of page */
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
/* jump to first line of page */
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

/*-------------------------*/
/* НАЙТИ ПО СТРЕЛКЕ ВПРАВО */
/*-------------------------*/
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
/*------------------------*/
/* НАЙТИ ПО СТРЕЛКЕ ВЛЕВО */
/*------------------------*/
static LINE *
fnd_al (lni, page)
	register LINE *lni;
	register LINE *page ;
{
	register LINE *lnj;

	/* поиск в той же строке */
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
	/* поиск от конца формы */
	for(lnj=lni; lnj->size != 0; lnj++ ) ;
	for(   ; lnj>=page; lnj--) {
		if(0 == (INP & (lnj->attr)) )
			continue;
/*		if((lnj->line == lni->line))*/
		if(lnj->line < lni->line)
			return(lnj);
	}
	return(lni);
}
/*------------------------*/
/* НАЙТИ ПО СТРЕЛКЕ ВВЕРХ */
/*------------------------*/
LINE *
fnd_au (lni, page)
	register LINE *lni;
	register LINE *page ;
{
	register LINE *lnj;
	int     nxt_line ;      /* НОМЕР БЛИЖАЙШЕЙ СТРОКИ */
	int		on_top = 0;

	/*
	 * поиск назад от текущего места
	 */
	for(lnj=lni; lnj>=page; lnj--) {
		if(lnj == lni)
			continue;
		if(0 == (INP & (lnj->attr)) )
			continue;
		/*в той же колонке и выше*/
		if((lnj->line <  lni->line)
		 &&(lnj->colu == lni->colu))
			return(lnj);
		/* ниже и левее (на участке с вертикальной организацией '+' в начале строки в коде .lav) */
		if((lnj->line >  lni->line)
		 &&(lnj->colu <  lni->colu))
			return(lnj);
	}
	/* если находимся в начале формы, искать от конца к текущему месту */
	for(lnj=page; lnj->size != 0; lnj++) {
		if (0 != (INP & (lnj->attr)) && lnj == lni && on_top == 0) {
			on_top = 1;
			break;
		}
	}
	if (on_top) {
		for(lnj=lni; lnj->size != 0; lnj++) ; /* поиск конца формы */
		for(   ; lnj>=page; lnj--) {
			/* первый с конца формы, но не в текущей строке */			
			if((INP & (lnj->attr)) && (lnj->line >= lni->line))
				return(lnj);
		}
	}
	return(lni);
}
/*-----------------------*/
/* НАЙТИ ПО СТРЕЛКЕ ВНИЗ */
/*-----------------------*/
static LINE *
fnd_ad (lni, page)
	register LINE *lni;
	register LINE *page ;
{
	register LINE *lnj;
	int     nxt_line ;

	if((nxt_line = (int)lni->line ) < maxli ) { /*ВПЕРЕД, ЕСЛИ ЕСТЬ КУДА*/
		for(lnj=lni; lnj->size!=0; lnj++) {     /* КОНЕЦ ? */
			if((lnj->line <= lni->line))    /*СТР. НЕ НИЖЕ */
				continue;
			else if( (INP & ~(lnj->attr)) ) /*НЕ ДЛЯ ВВОДА*/
				continue;
			else {                          /*СТР. НИЖЕ */
				if((lnj->colu + lnj->size) >= lni->colu)
					return(lnj);  /*ПОЛЕ ПОД КУРСОРОМ*/
				else
					continue;     /*ВСЕ ПОЛЕ СЛЕВА*/
			}
		}
	}
	/* ПОПЫТКА НАЙТИ ОТ НАЧАЛА СТРАНИЦЫ */
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
/*      register LINE *lnj;     */

	lni++;
	if(lni->size == 0)
		lni = page;          /* cycle to begin */
	return(lni);
}

#ifndef W_PAGE_TAB
aj_tbl(scnd, line_e)
/*-----------------------------------*/
/* НАСТРОИТЬ ЭЛЕМЕНТ ТАБЛИЦЫ ПО БАЗЕ */
/*-----------------------------------*/
register LINE *scnd;
LINE *line_e;
{
	register LINE *base;
	int     isleft;
	LINE saved;

	saved = *scnd;
	isleft = (saved.flag & SUSL) ? 1 : 0;
	base = scnd;
	while(base != line_e) {
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
	/* СЮДА ВЫХОДА НЕТ, ЕСЛИ СТРАНИЦА ПРАВИЛЬНО СОСТАВЛЕНА */
	w_chr('\007');
	return;

on_base:
	*scnd = *base;
	scnd->line = saved.line;
	scnd->colu = saved.colu;
	scnd->varl += (linptr_t)(saved.varl);
}

#endif W_PAGE_TAB
/*----------------*/
/* write the page */
/*----------------*/
w_page ( line_e, cod )
LINE    *line_e;
register kbcod cod;
{
	register LINE *line;

	/* СНАЧАЛА ВСЕ, ЧТО НЕ ДЛЯ ВВОДА */
	for(line=line_e; line->size != 0; line++) {
#ifndef W_PAGE_TAB
		/* ВТОРИЧНЫЙ ЭЛЕМЕНТ ТАБЛИЦЫ НАДО СНАЧАЛА НАСТРОИТЬ... */
		if(line->flag & SUST){
			aj_tbl(line, line_e);
		}
#endif W_PAGE_TAB
		if( (INP & ~(line->attr)) )
		    /*    w_line(line, cod);    */
			w_line(line);
	}
	/* ЛИНИИ ДЛЯ ВВОДА В ПОСЛЕДНЮЮ ОЧЕРЕДЬ */
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
	int     cod;             /* code returned from r_line() */

	register LINE *lni;      /* line index in pointer form */
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

	cod = r_line(lni, posp);

	/* НАЙТИ УКАЗАТЕЛЬ ДЛЯ СЛЕДУЮЩЕГО ЧТЕНИЯ */
	switch( cod ) {
	case KB_KH :
		lni = fnd_home(lni, page);
		break;
	case KB_KE :
		lni = fnd_end(lni, page);
		break;
	case KB_AR :
		if ( (lni->flag & SUSR) == FALSE )
			lni = fnd_ar(lni, page) ;
		break ;
	case KB_AL :
		if ( (lni->flag & SUSL) == FALSE )
			lni = fnd_al(lni, page) ;
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
	case KB_NL :
		if ( (lni->flag & SUSNL) == FALSE )
			lni = fnd_nxt(lni, page) ;
		break ;
	case KB_RE :     /* ОСВЕЖИТЬ ИЗОБРАЖЕНИЕ */
		er_pag();
		w_page(page, 0);
		break ;
	default :
		break;
	}

	*curline = lni;    /* save pointer to current line!!! */
	return(cod);
}
