/*
**      +----------+    БИБЛИОТЕКА ВВОДА-ВЫВОДА
**     (c) linlib  !    ДЛЯ АЛФАВИТНО-ЦИФРОВЫХ
**      +----------+    ВИДЕОТЕРМИНАЛОВ
**/

/*
 *      $Header$
 *      $Log$
 */

#include <wchar.h>
#include <stdlib.h>
#include <stdio.h>
#include "line.h"
#include "line0.h"

#define STRSIZE 128

static char u8s[STRSIZE] = "абвгдежзиклмн  ";
static double v1;
static double vd = 7.68123456789012345678;
static float  vf = 3.14;
static int	  vi = 54321;
static short  vh = 123;
static long   vl = 99999999;
wchar_t wcs[STRSIZE/4] = L"абвгдежзиклмн xyz";

static  int ex_flg;     /* ФЛАГ: ПОРА ЗАКАНЧИВАТЬ */
static  char tmps[STRSIZE] = "";

mkexit()
{
	er_pag();
	cp_set(-1, 0, TXT);
	ex_flg = 1;
	return(TRUE);
}

static int sout()
{
	unsigned char *s;

	cp_set(-2, 0, HDR);
	fprintf(vttout, " s_raw=\"%s\" " , u8s);
	for (s = u8s; *s != '\0'; s++)
		fprintf(vttout, "%2x ", (int)*s);
	er_eol(HDR);
}

static int sout2()
{
	unsigned char *s;

	cp_set(-2, 0, HDR);
	fprintf(vttout, " vd=\"%.10lf\" " , vd);
	fprintf(vttout, " vf=\"%.5f\" " , (double)vf);
	fprintf(vttout, " vi=\"%d\" " , vi);
	fprintf(vttout, " vh=\"%hd\" " , vh);
	fprintf(vttout, " vl=\"%ld\" " , vl);
	er_eol(HDR);
}

static int sout3()
{
	extern SCRN scrn;
	cp_set(-2, 0, HDR);
	//w_wcstr(wcs);
	sprintf(tmps, " wcs=\"%ls\" ", wcs);
	w_str(tmps);
	sprintf(tmps, " scrn.sc_co=%d ", scrn.sc_co);
	w_str(tmps);
	er_eol(HDR);

}

#include "pmainv.i"

vmain()
/*------------*/
/* VIDEO MAIN */
/*------------*/
{
	kbcod cod;
	LINE *cline;
	cline = 0;
	char *sbuf;
	sbuf = calloc(MAXLICO + 1, sizeof(wchar_t));

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
		case '+':
		case '=':
			sout2();
			break;
		case '?':
		case '/':
			sout3();
			break;
		case KB_HE:
			/* ВЫВОД СПРАВОЧНОЙ ИНФОРМАЦИИ */
			w_help("no_help.lb"); w_page(linem);
			break;
		case KB_EX :
			/* ЗВОНОК, ИЗ ЭТОЙ СТРАНИЦЫ
			 * ПОДНИМАТЬСЯ НЕКУДА
			 */
			bell();
			w_msg(ERR, "Please, use menu - close, quit");
			break;
		case KB_KI:
			sout();
		default :
			w_emsg("");
			break;
		}
		cp_set(2, 0, TXT);
		if (cod == L'\0') {
			at_set(ERR);
			sprintf(sbuf, "cod=0");
		} else if (cod < 0x80) {
			sprintf(sbuf, "ASCII(7bit) 0x%2.2x '%c'", cod, cod);
		} else if (cod >= 0x80 && cod < 0x10ffff) {
			at_set(MSE|INP);
			sprintf(sbuf, "UTF-8 0x%x '%lc'", cod, cod);
		} else if (cod0(cod) == '^') {
			sprintf(sbuf, "KBCTL('%c')", cod1(cod));
		} else {
			at_set(ATT|VEXT);
			sprintf(sbuf, "KB_%c%c", cod0(cod), cod1(cod));
		}
		w_str(sbuf);
		er_eol(TXT);
		fflush(vttout);

	}
}
