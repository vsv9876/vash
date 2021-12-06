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


/*
 * get label for key, for help text
 */
get_lh(str, hlpcod)
char *hlpcod; /* string pointer for help code, eg. ":HE" */
char *str;
{
	extern   KBL    kbl[];  /* ТАБЛИЦА СВЯЗИ КОДОВ И НАЗВАНИЙ КЛАВИШ */
	register KBL   *kblp;
		kbcod   codh;   /* КОД, ДЛЯ КОТОРОГО НАЙТИ НАЗВАНИЕ */
		int i;

		char *s;
		char *p;

		s = str;

		codh = KBCOD(hlpcod[1], hlpcod[2]);

		for(kblp=kbl; kblp->t_key1!=0; kblp++) {
			if(kblp->t_cod == codh) {
				/*strncpy(str, kblp->t_knm, sizeof(kbl[0].t_cod));*/
				/*strcpy(str, kblp->t_knm);*/
				s = str; p = kblp->t_knm;
				/* 1st space terminates key label */
				while (*p != ' ' && *p != '\0') {
					*s = *p;
					s++; p++;
				}
				*s = '\0';
				return;
			}
		}
		strcpy(str, hlpcod + 1); /* if no code found */
}

w_lh_str(hlpcod)
char *hlpcod;
{
	char out[10]; /* label is no more than 8 ascii symbols */
	get_lh(out, hlpcod);
	at_set(HDR|VEXT);
	w_str(out);
}

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
		int i;

	if( *mod == 'w' ) {
		get_lh(str, line->varl);
	}
	return( TRUE );
}
