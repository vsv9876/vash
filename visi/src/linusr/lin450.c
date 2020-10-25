/*
**      +----------+    БИБЛИОТЕКА ВВОДА-ВЫВОДА
**     (c) linlib  !    ДЛЯ АЛФАВИТНО-ЦИФРОВЫХ
**      +----------+    ВИДЕОТЕРМИНАЛОВ
**/

/*
 *      $Header: lin450.c,v 3.1 89/08/29 14:50:18 vsv Rel $
 *
 *      $Log:	lin450.c,v $
 * Revision 3.1  89/08/29  14:50:18  vsv
 * ВЕРСИЯ LINLIB_3
 * 
 * Revision 3.0  88/06/16  18:04:04  vsv
 * ПРЕДВАРИТЕЛЬНЫЙ ВЫПУСК
 * 
 */

#include "line.h"

/*
 * this function used as simple menu callback connector
 */


/*
 * "тестирующая" функция для меню.
 * если line->cvtf, то вызывается указанная функция;
 * если line->cvts, то вызывается команда unix (не реализовано)
 *
 * функция возвращает TRUE или FALSE, в зависимости от
 * того, что вернули ей.
 *
 */

int
tst_m(line, cod)
register LINE *line;
	 kbcod cod;
{
	if(cod == ' ' || cod == KB_NL) {
		if(line->cvtf) {
			return( (*line->cvtf)() );
		}
	}
	return(TRUE);   /* last resort */
}
