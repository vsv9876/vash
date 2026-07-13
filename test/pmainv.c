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

/*allcod = 1;	/* linlib behaviour flag: open to edit with any key; default: space only */

static  int ex_flg = 0;     /* exit flag */

static double vd = 7.68123456789012345678;
static float  vf = 3.14;
static int	  vi = 54321;
static short  vh = 123;
static long   vl = 99999999;

extern int p_navi();
extern int p_suputf8();

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
	} else if (cod <= 0177) {
		sprintf(sbuf, "ASCII(7bit) 0x%2.2x '%c'", cod, cod);
	} else if (cod <= 0377 && mb_cur_max == 1) {
		at_set(ALT|INP);
		sprintf(sbuf, "legacy(8bit) 0x%2.2x '%c'", cod, cod);
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

LFRAME lfmain = { 0 };

/*ARGSUSED*/
void sigwinch(signo)
int signo;
{
	if (0 != gtty_sz()) {
		return;
	}
	/* this is restriction for main frame to use classic 24 lines */
	lfmain.maxli  =  24;
	lfmain.baseli = hwframe.maxli - lfmain.maxli;
	if (lfmain.baseli < 0) {
		lfmain.maxli = hwframe.maxli;
		lfmain.baseli = 0;
	}
	lfmain.maxco  = hwframe.maxco;

	lframe = &lfmain;

	if (signo)
		jkb_re();
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

	sigwinch(0);
	er_pag();
	w_page(linem);
	signal(SIGWINCH, sigwinch);

	for( ;; ) {
		cod = r_page(linem, &cline, 0);
		keydump(1, 2/*40*/, cod);

		if(ex_flg) return 0;

		switch(cod) {

		case KB_NL:
		case ' ':
			if((cline->attr & LMSE) == LMSE) {
				er_pag();
				w_page(linem);
			}
			break;

		case '+': case '=':  sout2(); break;
		case KB_HE:
/*			w_help("no_help.lb"); w_page(linem);*/
			w_emsg("no_help.lb"); w_page(linem);
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
