/*
**      +----------+    БИБЛИОТЕКА ВВОДА-ВЫВОДА
**     (c) linlib  !    ДЛЯ АЛФАВИТНО-ЦИФРОВЫХ
**      +----------+    ВИДЕОТЕРМИНАЛОВ
**/

/*
 *      $Header$
 *      $Log$
 */

#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <wchar.h>
#include "line.h"
#include "line0.h"


static void w_cell(int mode, int li, int co)
{
	if (mode == 1) {
		cp_set(li, co, TXT|VEXT);
		w_chr('.');
	} else {
		cp_set(li, co, TXT);
		w_chr(' ');
	}
}

static void w_cross(int mode, LINE *line, LINE *page);
static void w_cross(mode, ln, page)
int mode;
LINE *ln;
LINE *page;
{
	int li;
	int co;
	LINE *pg;
	int oncell; /* cell free/allocate status */

	/* horizontal */
	li = ln->line;
	for (co = 0; co < 80; co++) {
		oncell = 0;
		/* if cell is free then draw a dot */
		for (pg = page; pg->size != 0; pg++) {
			if (pg->line != li)
				continue;
			if ((co >= pg->colu)
				&& (co < (pg->colu + pg->size))) {
				oncell = 1;	break;
			}
		}
		if (oncell == 0)
			w_cell(mode, li, co);
	}
#if 1
	/* vertical */
	for (co = ln->colu, li = 0; li < 20; li++) {
		oncell = 0;
		/* if cell is free then draw a dot */
		for (pg = page; pg->size != 0; pg++) {
			if (pg->line != li)
				continue;
			if ((co >= pg->colu)
				&& (co < (pg->colu + pg->size))) {
				oncell = 1;	break;
			}
		}
		if (oncell == 0)
			w_cell(mode, li, co);
	}
#endif
}

#include "navi.i"

#include "navi2.i"

#include "navi3.i"


void keydump(int li, int co, kbcod cod);

int p_navi()
{

    kbcod   cod;        /* КОД ПОСЛЕДНЕЙ КЛАВИШИ */
    LINE *cline;        /* УКАЗАТЕЛЬ НА ТЕКУЩУЮ ЛИНИЮ В СТРАНИЦЕ */
    LINE *cpage;	/* page may be vary */

    er_pag();

    cpage = navi2;
    cline = cpage;      /* УСТАНОВИТЬ В НАЧАЛО СТРАНИЦЫ */
    w_page(cpage);

    while ( -1 ) {
		cod = r_page(cpage, &cline, 0);

		keydump(1, 2/*40*/, cod);

		switch ( cod ) {

		case KB_HE:
			w_help(NULL); w_page(cpage);
			break;
		case KB_KI:
			w_cross(1, cline, cpage);
			break;
		case KB_KD:
			w_cross(0, cline, cpage);
			break;
		case KB_PU:
			if(cpage == navi2) cpage = navi;
			else              cpage = navi2;
			cline = cpage;
			er_pag();
			w_page(cpage);
			break;
		case KB_PD:
			if(cpage == navi3) cpage = navi;
			else              cpage = navi3;
			cline = cpage;
			er_pag();
			w_page(cpage);
			break;
		case KB_NL:
		case ' ':
			/* ПЕРЕРИСОВАТЬ ПОСЛЕ МЕНЮ */
			if((cline->attr & MSE) == MSE) {
				er_pag();
				w_page(cpage);
			}
			break;
	/*	case KB_CA: no return from r_page if r_line was cancelled */
		case KB_EX :
			/*NOBREAK*/
			return(1);
		case '?':
			w_page(cpage);
			break;
		default:   /* w_emsg("");     /* clear err msg */
			/*NO BREAK*/
		}
    }
    return(1);
}
