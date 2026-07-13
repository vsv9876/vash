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

#include "navi.i"

int p_navi()
{

    kbcod   cod;        /* КОД ПОСЛЕДНЕЙ КЛАВИШИ */
    LINE *cline;        /* УКАЗАТЕЛЬ НА ТЕКУЩУЮ ЛИНИЮ В СТРАНИЦЕ */

    cline = navi;      /* УСТАНОВИТЬ В НАЧАЛО СТРАНИЦЫ */
    er_pag();
    w_page(navi);

    while ( -1 ) {
		cod = r_page(navi, &cline, 0);
		switch ( cod ) {
		case KB_NL:
		case ' ':
			/* ПЕРЕРИСОВАТЬ ПОСЛЕ МЕНЮ */
			if((cline->attr & LMSE) == LMSE) {
				er_pag();
				w_page(navi);
			}
			break;
	/*	case KB_CA: no return from r_page if r_line was cancelled */
		case KB_EX :
			/*NOBREAK*/
			return(1);
		case '?':
			w_page(navi);
			break;
		default:   w_emsg("");     /* clear err msg */
		}
    }
    return(1);
}
