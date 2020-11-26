/*
**      +----------+    БИБЛИОТЕКА ВВОДА-ВЫВОДА
**     (c) linlib  !    ДЛЯ АЛФАВИТНО-ЦИФРОВЫХ
**      +----------+    ВИДЕОТЕРМИНАЛОВ
**/

/*
 *      $Header: lin425.c,v 3.2 89/08/29 14:45:19 vsv Rel $
 *
 *      $Log:	lin425.c,v $
 * Revision 3.2  89/08/29  14:45:19  vsv
 * ВЕРСИЯ LINLIB_3
 * 
 * Revision 3.1  89/01/13  19:12:04  vsv
 * НАЗВАНИЕ КЛАВИШИ ТЕПЕРЬ МОЖЕТ СОДЕРЖАТЬ ПРОБЕЛ.
 * КРОМЕ ТОГО, ПРИМЕНЕНЫ МАКРОСЫ tocod ИЗ line.h
 * 
 * Revision 3.0  88/06/16  18:02:25  vsv
 * ПРЕДВАРИТЕЛЬНЫЙ ВЫПУСК
 * 
 * Revision 3.0  87/12/21  12:21:20  vsv
 * ПРЕДВАРИТЕЛЬНЫЙ ВЫПУСК.
 * 
 */

#include <stdio.h>
#include <ctype.h>
#include "line.h"
#include "line0.h"

cvt_lh(line, cod, mod, str)
/*------------------------*/
/* ВЫВОД НАЗВАНИЯ КЛАВИШИ */
/*------------------------*/
LINE    *line;
int       cod;
char     *mod;
char     *str;
{
	/* НАЙТИ И ПОДСТАВИТЬ НАЗВАНИЕ КЛАВИШИ
	 * ПО ЕЕ СМЫСЛОВОМУ (ЛОГИЧЕСКОМУ) КОДУ
	 * НАПРИМЕР ":NL" ЗАМЕНЯЕТСЯ НА "RETURN"
	 */
	register char   *s;
	extern   KBL    kbl[];  /* ТАБЛИЦА СВЯЗИ КОДОВ И НАЗВАНИЙ КЛАВИШ */
	register KBL   *kblp;
		kbcod   codh;   /* КОД, ДЛЯ КОТОРОГО НАЙТИ НАЗВАНИЕ */
		int i;

	if( *mod == 'w' ) {
		s = line->varl;
		/* ДЛЯ ЭТОГО ЕСТЬ МАКРОСЫ В "line.h" */
		/*
		codh = tocod0(s[1]);
		codh |= tocod1(s[2]);
		*/
		codh = KBCOD(s[1], s[2]);

		for(kblp=kbl; kblp->t_key!=0; kblp++) {
			if(kblp->t_cod == codh) {
				/*strncpy(str, kblp->t_knm, sizeof(kbl[0].t_cod));*/
				strcpy(str, kblp->t_knm);
				goto ret;
			}
		}
		strcpy(str, &line->varl[1]);
ret:
	;	/* trim trailing spaces, otherwise PAD|MID does not working */

		s = str;
		for(i = 0; i < line->size && *s != ' ' && *s !='\0'; i++) {
			s++;
		}
		*s = '\0';

	}
	return( TRUE );
}
