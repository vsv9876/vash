/*
**      +----------+    БИБЛИОТЕКА ВВОДА-ВЫВОДА
**     (c) linlib  !    ДЛЯ АЛФАВИТНО-ЦИФРОВЫХ
**      +----------+    ВИДЕОТЕРМИНАЛОВ
**/

/*
 *      $Header$
 *
 *      $Log$
 */

#include <stdio.h>
#include "line.h"
#include "line0.h"

/* order must be the same as declared in lin210.c: KBL kbl[KBSIZE] */
static char const *keynam[] = {

"KB_KP  Keypad toggle (on/off)",
"KB_AU  cursor up",
"KB_AD  cursor down",
"KB_AL  cursor left",
"KB_AR  cursor right",
"KB_TA  Tab, advance next line",
"KB_SP  space bar, menu/selector",

"KB_NL  enter/return - complete input",
"KB_DE  erase left from cursor",

"KB_EX  Exit, end of frame, cancel",
"KB_HE  Help page call",
"KB_RE  Refresh/redraw screen",
"KB_PR  Prefix for complex keyboard command",

"KB_IN  Insert",
"KB_KD  delete under cursor",
"KB_KH  jump to 1st position",
"KB_KE  jump to last position",
"KB_PU  scroll page back",
"KB_PD  scroll page forward",
0 };

extern  int cvt_sp();
extern  KBL kbl[];

cv_kbl(line, cod, mod, str)
/*----------------------------------------*/
/* "формат" для ввода нового кода клавиши */
/*----------------------------------------*/
LINE *line;
kbcod cod;
char *mod;
char *str;
{
	/*
	 * Эта функция зависит от termcap
	 */
	register KBL *kblp;
	kbcod tmpcod;

	kblp = (KBL *)line->varl;
	if ( *mod == 'w' ) {
		tmpcod = kblp->t_key;
		str[0] = cod0(tmpcod);
		str[1] = cod1(tmpcod);
		str[2] = '\0';
	}
	else if ( *mod == 'r' && cod == ' ' ) {
		cp_set(line->line, line->colu, ATT|INP);
		w_strn("  <?> ", line->size);

		w_msg(ATT, "Press new key...");
		tmpcod = r_key();
		if (tmpcod == 0)
			w_msg(ERR, "Unknown key");
		else if (cod1(tmpcod) == 0)
			w_msg(ERR,
"Printable char invalid, please use function or control keys");
		else if (kblp->t_key == tmpcod)
			w_msg(ATT, "Key code was not changed...");
		else    {
			kblp->t_key = tmpcod;
			w_msg(TXT, "");
		}
	}
	return(TRUE);
}

#include "keys.i"

static  char    helpf[] = "vhsetk.lb";

pag_k()
/*-----------------*/
/* НАСТРОЙКА КЛАВИШ*/
/*-----------------*/
{
	u_page(mainm, helpf);
	return(TRUE);
}
