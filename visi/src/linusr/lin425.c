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
void
get_lh(str, hlpcod)
const char * hlpcod; /* string pointer for help code, eg. ":HE" */
const char *str;
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
				return /*0*/;
			}
		}
		strcpy(str, hlpcod + 1); /* if no code found */
}

/*
 * write key label string
 */
void w_lh_lbl(hlpcod)
const char *hlpcod;
{
	char out[10]; /* key label is no more than 8 ascii symbols */
	get_lh(out, hlpcod);
	at_set(HDR|VEXT);
	w_str(out);
}

/*
 * write help message with key label incorporated into
 */
void w_lh_msg(msg)
const char *msg;
{
	char out[200]; /* short printable string */
	char *p;
	char *op;
	int newlbl = 1;

	op = out;
	p = msg;
	while ( *p ) {
		if (*p == ':' && newlbl) { /* label */
			newlbl = 0;
			/*op = out;*/
			while(*p && *p != ' ')
				*op++ = *p++;
			*op = '\0';
			w_lh_lbl(out);
			op = out;
		} else if (*p == ' ') { /* continue simple string */
			newlbl = 1;
			*op++ = *p++;
			*op = '\0';
			at_set(TXT);
			w_str(out);
			op = out;	/* start collect next word */
		} else {
			newlbl = 0;
			*op++ = *p++;
		}
	}
}

int cvt_lh(line, cod, mod, str)
/*------------------------*/
/* ВЫВОД НАЗВАНИЯ КЛАВИШИ */
/*------------------------*/
LINE    *line;
kbcod     cod;
char     *mod;
const char     *str;
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
