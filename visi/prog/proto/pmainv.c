/*
**      +----------+    БИБЛИОТЕКА ВВОДА-ВЫВОДА
**     (c) linlib  !    ДЛЯ АЛФАВИТНО-ЦИФРОВЫХ
**      +----------+    ВИДЕОТЕРМИНАЛОВ
**/

/*
 *      $Header$
 *      $Log$
 */

#include <stdio.h>
#include "line.h"       /* ФАЙЛ-ЗАГОЛОВОК LINLIB */


static  int ex_flg;     /* ФЛАГ: ПОРА ЗАКАНЧИВАТЬ */

mkexit()
{
	cp_set(-1, 0, CMD);
	ex_flg = 1;
	return(TRUE);
}


LINE linem[] = {
{ 80, 0, 0, 0, LHDR, 0,0,0, "ПРОТОТИП ПРИКЛАДНОЙ ПРОГРАММЫ  LINLIB V3.4" },
{ 38, 8,24, 0,	INP|PMT|LMSE,	0,	mkexit,	tst_m,	" КОНЕЦ РАБОТЫ " },
{  4,13, 3, 0, ATT|MID|PAD, 0, cvt_lh, 0, ":? " },
{  8,13, 8, 0, ATT|MID|PAD, 0, cvt_lh, 0, ":HE" },
{ 22,13,18, 0, LTXT, 0,0,0, "ВЫВОД ПОДСКАЗКИ (HELP)" },
{  8,15, 8, 0, ATT|MID|PAD, 0, cvt_lh, 0, ":RE" },
{ 28,15,18, 0, LTXT, 0,0,0, "ПЕРЕРИСОВКА ЭКРАНА (REFRESH)" },
{ 0 }, };



vmain()
/*------------*/
/* VIDEO MAIN */
/*------------*/
{
	kbcod cod;
	LINE *cline;
	cline = 0;

	er_pag();
	w_page(linem);

	for( ;; ) {
		cod = r_page(linem, &cline, 0);
		/* ФЛАГ ВЗВОДИТСЯ ЧЕРЕЗ ФУНКЦИЮ ОБСЛУЖИВАНИЯ МЕНЮ */
		if(ex_flg) return;

		switch(cod) {
		case ' ' :
			/* ПЕРЕРИСОВАТЬ ПОСЛЕ МЕНЮ */
			if((cline->attr & LMSE) == LMSE) {
				er_pag();
				w_page(linem);
			}
			break;
		case '?':
		case KB_HE:
			/* ВЫВОД СПРАВОЧНОЙ ИНФОРМАЦИИ */
			w_help(0); w_page(linem);
			break;
		case KB_EX :
			/* ЗВОНОК, ИЗ ЭТОЙ СТРАНИЦЫ
			 * ПОДНИМАТЬСЯ НЕКУДА
			 */
			bell();
			break;
		default :
			w_emsg("");
			cp_set(-1, 0, CMD);
			printf("<%c%c>", cod1(cod), cod0(cod));
			fflush(vttout);
			break;
		}
	}
}
