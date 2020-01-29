/*
**      +----------+    БИБЛИОТЕКА ВВОДА-ВЫВОДА
**     (c) linlib  !    ДЛЯ АЛФАВИТНО-ЦИФРОВЫХ
**      +----------+    ВИДЕОТЕРМИНАЛОВ
**/

/*
 *      $Header: lin426.c,v 1.1 90/12/27 16:33:17 vsv Rel $
 *
 *      $Log:	lin426.c,v $
 * Revision 1.1  90/12/27  16:33:17  vsv
 * ВЕРСИЯ LINLIB_3
 * 
 */

/* оформление в стиле К.Уейна Ретлиффа */

#include <stdio.h>
#include "line.h"

int cvt_hl(line, cod, mod, str)
/*
 * Провести горизонтальную линию,
 * символ для рисования линии указывается в форме, первый символ строки varl.
 *
 */
	LINE    *line;
	int       cod;
	char     *mod;
	char     *str;
	{
	register char *s;
	register int   i;

	if( *mod == 'w' ) {
		s = line->varl;
		for(i = line->size; i>0; i--)
			*str++ = *s;
		*str = '\0';
		}
	return( TRUE );
}
