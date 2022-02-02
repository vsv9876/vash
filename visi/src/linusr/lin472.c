/*
**      +----------+    БИБЛИОТЕКА ВВОДА-ВЫВОДА
**     (c) linlib  !    ДЛЯ АЛФАВИТНО-ЦИФРОВЫХ
**      +----------+    ВИДЕОТЕРМИНАЛОВ
**/

/*
 *      ПОЛЬЗОВАТЕЛЬСКИЙ УРОВЕНЬ БИБЛИОТЕКИ LINLIB
 *
 *      cvt_sp()   ВВОД СТРОКИ ПО УКАЗАТЕЛЮ;
 *
 */

/*
 *      $Header: lin472.c,v 1.1 89/08/29 14:50:42 vsv Rel $
 *      $Log:	lin472.c,v $
 * Revision 1.1  89/08/29  14:50:42  vsv
 * ВЕРСИЯ LINLIB_3
 * 
 */

#include <stdio.h>
#include <string.h>
#include "line.h"


int     cvt_sp(line, cod, mod, str)
/*-----------------------------------*/
/* ВВОД СТРОКИ ПО ДВОЙНОМУ УКАЗАТЕЛЮ */
/*-----------------------------------*/
LINE *line;
kbcod cod;
char *mod;
char *str; /*utf8 buffer*/
{
	char **spp;

	int size;

	spp = (char **)(line->varl);

	size = line->size;
	if (line->attr & PMT) size -= 1;

	if (str != NULL /*&& *str != '\0'*/ && *spp != NULL ) {
		/* trimming to line->size will be done in [wr]_line context */
		if(*mod == 'r') {
			strcpy(*spp, str);
		}
		else    {
			strcpy(str, *spp);
		}
/*
	} else {
		*str = '\0';
*/
	}
	return(TRUE);
}

int     cvt_s(line, cod, mod, str)
/*--------------------------*/
/*ВВОД СТРОКИ ПО УКАЗАТЕЛЮ */
/*--------------------------*/
LINE *line;
kbcod cod;
char *mod;
char *str;
{
	char *sp;

	/*int max_co;//TODO cleanup*/
	int size;
	register char *si;
	register char *so;
	register int i;

	sp = (char *)line->varl;
	size = line->size;
	if (line->attr & PMT) size -= 1;
	/*max_co = maxco-2;*/
	if (str) {
		if(*mod == 'r') {
			strcpy(sp, str);
		}
		else    {
			u8snu8s(str, sp, size);
		}
	}
	return(TRUE);
}

