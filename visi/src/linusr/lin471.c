/*
**      +----------+    БИБЛИОТЕКА ВВОДА-ВЫВОДА
**     (c) linlib  !    ДЛЯ АЛФАВИТНО-ЦИФРОВЫХ
**      +----------+    ВИДЕОТЕРМИНАЛОВ
**/

/*
 *      ПОЛЬЗОВАТЕЛЬСКИЙ УРОВЕНЬ БИБЛИОТЕКИ LINLIB
 *
 *      u_page(page, phline)    РАБОТА СО СТРАНИЦЕЙ;
 */

/*
 *      $Header: lin471.c,v 1.1 89/08/29 14:50:41 vsv Rel $
 *      $Log:	lin471.c,v $
 * Revision 1.1  89/08/29  14:50:41  vsv
 * ВЕРСИЯ LINLIB_3
 * 
 */

#include <stdio.h>
#include "line.h"

u_page(page, phline)
/*-----------------------------*/
/* ФУНКЦИЯ РАБОТЫ СО СТРАНИЦЕЙ */
/*-----------------------------*/
LINE *page;             /* ОСНОВНАЯ СТРАНИЦА */
LINE *phline;           /* ИМЯ ФАЙЛА СТРАНИЦЫ HELP */
{
    kbcod   cod;        /* КОД ПОСЛЕДНЕЙ КЛАВИШИ */
    LINE *cline;        /* УКАЗАТЕЛЬ НА ТЕКУЩУЮ ЛИНИЮ В СТРАНИЦЕ */

    cline = page;       /* УСТАНОВИТЬ В НАЧАЛО СТРАНИЦЫ */
    er_pag();
    w_page(page);

    while ( -1 ) {
		cod = r_page( page, &cline, 0);
		switch ( cod ) {
		case KB_NL:
		case ' ':
			/* ПЕРЕРИСОВАТЬ ПОСЛЕ МЕНЮ */
			if((cline->attr & LMSE) == LMSE) {
				er_pag();
				w_page(page);
			}
			break;
	/*	case KB_CA: no return from page if r_line cancelled */
		case KB_EX :
			/*NOBREAK*/
			return;
		case '?':
		case KB_HE:
			/* ПОКАЗАТЬ СПРАВОЧНУЮ ИНФОРМАЦИЮ */
			w_help(phline);
			w_page(page);
			break;
		default:   w_emsg("");     /* ПОГАСИТЬ СООБЩЕНИЕ ОБ ОШИБКЕ */
		}
    }
}

