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
#include <wchar.h>
#include "line.h"
#include "line0.h"

allcod = 1;	/* linlib behaviour flag: open to edit with any key; default: space only */
/* should be big anouth */
#define STRSIZE 128

static double vd = 7.68123456789012345678;
static float  vf = 3.14;
static int	  vi = 54321;
static short  vh = 123;
static long   vl = 99999999;

/* wide unicode char support */
static /*const*/ wcsobj_t wcso =
    const_wcsobj( 30, L"1234567中文素养АБВгдё7890abcdefghij1234567" );

volatile static /*const*/ u8sobj_t u8so =
    const_u8sobj( 22, "-123456789-123456789-" );
u8sobj_t *u8optr = &u8so;

/*0123456789-123456789-123456789-123456789-1 */

static int dummy0=-1,dummy1=-1,dummy2=-1,dummy3=-1;
static /*const*/ char u8s[STRSIZE] = "а国际站бвгдежзиклмн...  ";

static  u8char_t tmps[4 * STRSIZE] = "";

static  int ex_flg = 0;     /* exit flag */
mkexit()
{
	ex_flg = 1;

	er_pag();
	cp_set(-1, 0, TXT);
	return(TRUE);
}

void keydump(li, co, cod)
int li;
int co;
kbcod cod;
{
	char *sbuf;
	sbuf = calloc(STRBUF, sizeof(wchar_t));

	cp_set(li, co, TXT);
	if (cod == L'\0') {
		at_set(ERR);
		sprintf(sbuf, "cod=0");
	} else if (cod < 0x80) {
		sprintf(sbuf, "ASCII(7bit) 0x%2.2x '%c'", cod, cod);
	} else if (cod >= 0x80 && cod <= 0x10ffff) {
		at_set(MSE|INP);
		sprintf(sbuf, "UTF-8 0x%x '%lc'", cod, cod);
	} else if (cod0(cod) == '^') {
		sprintf(sbuf, "KBCTL('%c')", cod1(cod));
	} else {
		at_set(HDR|VEXT);
		sprintf(sbuf, "KB_%c%c", cod0(cod), cod1(cod));
	}
	w_str(sbuf);
	er_eol(TXT);
	fflush(vttout);

}

#define TXT_CO 20

static sout()
{
	unsigned char *s;

	cp_sav();
	cp_set(-6, TXT_CO, HDR/*ATT|VEXT*/);
	fprintf(vttout, " s_raw=\"%s\" " , u8s);
	for (s = u8s; *s != '\0'; s++)
		fprintf(vttout, "%2x ", (int)*s);
	er_eol(TXT);
	cp_fet();
}

static sout2()
{
	unsigned char *s;

	cp_sav();
	cp_set(-6, 0, HDR/*ATT|VEXT*/);
	fprintf(vttout, " vd=\"%.10lf\" " , vd);
	fprintf(vttout, " vf=\"%.5f\" " , (double)vf);
	fprintf(vttout, " vi=\"%d\" " , vi);
	fprintf(vttout, " vh=\"%hd\" " , vh);
	fprintf(vttout, " vl=\"%ld\" " , vl);
	er_eol(TXT);
	cp_fet();
}

static sout3()
{
	extern SCRN scrn;
	char *wcs;

	cp_sav();

	cp_set(-3, TXT_CO, TXT);
	w_str("0123456789-123456789-123456789-123456789-12345");
	wcs = &wcso.wcs;

	cp_set(-6, TXT_CO-1,  HDR/*ATT*/);
	sprintf(tmps, "'%ls'", wcs);
	w_str(tmps);
/*
	sprintf(tmps, " scrn.sc_co=%d ", scrn.sc_co);
	w_str(tmps);
*/
	er_eol(TXT);
	cp_fet();

}

static e_tst(size)
int size;
{
	static int curpos = 0;
	int refresh = 1;
	kbcod cod;
	int i;

	while(1) {
		cp_set(-4, TXT_CO-10, TXT);

		sprintf(tmps, "e_str(%2d)", size);
		w_str(tmps);
		cp_set(-4, TXT_CO, VAR|VEXT);
		if (refresh) {
			refresh = 0;
			sout3();
			cp_set(-4, TXT_CO, ERR);
			er_eol(ERR);
		}
		cod = e_str(&wcso, size, 0, &curpos);

		switch (cod) {
		case 0:
		case KB_AD:
		case KB_AU:
		case KB_HE:
		case KB_CA:
		case KB_EX:
			for (i = -6; i <= -3; i++) {
				cp_set(i, 0, TXT); er_eol(TXT);
			}
			return cod;
			break;
		case KB_RE:
		case KB_NL:
			refresh = 1;
			break;
		default:
			continue;
			break;
		}
	}
	return cod;
}

int m_test(line, cod)
register LINE *line;
	 kbcod cod;
{
	int size;
	size = atoi(line->cvts);

	if(cod == ' ' || cod == KB_NL) {
/*
		if(line->cvtf) {
			(*line->cvtf)(size);
		}
*/
		e_tst(size);
	}
	return(TRUE);   /* last resort */
}

#include "pmainv.i"

int vmain()
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
		keydump(1, 2/*40*/, cod);

		if(ex_flg) return 0;

		switch(cod) {
/*
		case ' ' :
			if((cline->attr & LMSE) == LMSE) {
				er_pag();
				w_page(linem);
			}
			break;
*/
		case '"': case '\'': cod = e_tst(3); break;
		case ':': case ';':  cod = e_tst(5); break;
		case '|': case '\\': sout(); break;
		case '+': case '=':  sout2(); break;
		case '?': case '/':  sout3(); break;
		case KB_HE:
			w_help("no_help.lb"); w_page(linem);
			break;
		case KB_EX :
			bell();
			w_msg(ERR, "Please, use menu - quit");
			break;
		default :
			w_emsg("");
			break;
		}

	}
}
