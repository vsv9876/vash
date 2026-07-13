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
#include <signal.h>
#include <wchar.h>
#include "line.h"
#include "line0.h"

/* redefine linlib: should be big enough for those tests */
#define STRSIZE 128

static char dim1[4] = "18";
static char dim2[4] = " 7";

/*
 * an origin for renewal on start of testing
 */
/*volatile*/
static wcsobj_t wcso =
		/*const_wcsobj( 30, L"1234567中文素养АБВгдё7890abcdefghij1234567" );*/
		const_wcsobj( 30, L"123456789 ---- ---x ---- ---x" );
volatile
static wcsobj_t *wcoptr = &wcso;
static wchar_t wcs_save[STRSIZE];

/*volatile*/
static u8sobj_t u8so =
		const_u8sobj( 22, "---- ---- ---- ---- -" );
volatile
static u8sobj_t *u8optr = &u8so;
static u8char_t u8s_save[STRSIZE];

/*0123456789-123456789-123456789-123456789-1 */

static char u8s[STRSIZE] = "абвг中文дежзиклмн...  ";
static char u8s_0[STRSIZE];


static  u8char_t tmps[4 * STRSIZE] = "";

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

static sout3()
{
	extern SCRN scrn;
/*	char *wcs;*/
	char *wcs;

	cp_sav();

	cp_set(-3, TXT_CO, TXT);
	w_str("0123456789-123456789-123456789-123456789-12345");
	wcs = (void *)&wcso.wcs;

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
		e_tst(size);
	}
	return(TRUE);   /* last resort */
}


#include "suputf8.i"

static int once = 1;
int p_suputf8()
{
    kbcod   cod;        /* КОД ПОСЛЕДНЕЙ КЛАВИШИ */
    static LINE *cline;        /* УКАЗАТЕЛЬ НА ТЕКУЩУЮ ЛИНИЮ В СТРАНИЦЕ */

    if (once) {
    	once = 0;
        cline = suputf8;      /* УСТАНОВИТЬ В НАЧАЛО СТРАНИЦЫ */
    	wcscpy(wcs_save, wcso.wcs);
    	strcpy(u8s_save, u8so.u8s);
    	strcpy(u8s_0, u8s);
    }
	/* renewal of constants on every call */
	wcscpy(wcso.wcs, wcs_save);
	strcpy(u8so.u8s, u8s_save);
	strcpy(u8s, u8s_0);

    er_pag();
    w_page(suputf8);

    while ( -1 ) {
		cod = r_page(suputf8, &cline, 0);
		switch ( cod ) {
		case KB_NL:
		case ' ':
			/* ПЕРЕРИСОВАТЬ ПОСЛЕ МЕНЮ */
			if((cline->attr & LMSE) == LMSE) {
				er_pag();
				w_page(suputf8);
			}
			break;
		case '"': case '\'':
			cod = e_tst(3); break;
		case ':': case ';':
			cod = e_tst(5); break;
		case '|': case '\\':
			sout(); break;
		case '?': case '/':
			sout3(); break;
	/*	case KB_CA: no return from r_page if r_line was cancelled */
		case KB_EX :
			/*NOBREAK*/
			return(1);
			break;
		default:   w_emsg("");     /* clear err msg */
		}
    }
    return(1);
}
