/*
**      +----------+    גיגליןפוכב קקןהב-קשקןהב
**     (c) linlib  !    הלס בלזבקיפמן-דיזעןקשט
**      +----------+    קיהוןפועםימבלןק
**/

/*
 *      $Header$
 *
 *      $Log$
 */

#include <stdio.h>
#include "line.h"
#include "line0.h"

static char *keynam[] = {

"KB_KP  Keypad toggle (on/off)",
"KB_AU  cursor up",
"KB_AD  cursor down",
"KB_AL  cursor left",
"KB_AR  cursor right",
"KB_NL  Enter, end of line",
"KB_DE  Delete left from cursor",
"KB_EX  Exit, end of frame, cancel",
"KB_HE  Help call",
"KB_PR  Prefix for complex keyboard command",
"KB_RE  Refresh (redraw) screen",
"KB_TA  Tab key (for editing)",
"KB_KH  Home key",
"KB_KE  End key",
"KB_PU  PgUp",
"KB_PD  PgDown",
"KB_KD  del->  ",
"KB_IN  Insert",
0 };

extern  int cvt_sp();
extern  KBL kbl[];

cv_kbl(line, cod, mod, str)
/*----------------------------------------*/
/* "ÆÏÒÍÁÔ" ÄÌÑ ××ÏÄÁ ÎÏ×ÏÇÏ ËÏÄÁ ËÌÁ×ÉÛÉ */
/*----------------------------------------*/
LINE *line;
kbcod cod;
char *mod;
char *str;
{
	/*
	 * üÔÁ ÆÕÎËÃÉÑ ÚÁ×ÉÓÉÔ ÏÔ termcap
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
		w_strn("??????", line->size);

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
/* מבףפעןךכב כלבקיû*/
/*-----------------*/
{
	u_page(mainm, helpf);
	return(TRUE);
}
