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
static KBDESCR keynm[] = {
/*static char const *keynam[] = {}*/

	{ KB_KP, "Keypad toggle (on/off)" },
	{ KB_AU, "cursor up" },
	{ KB_AD, "cursor down" },
	{ KB_AL, "cursor left" },
	{ KB_AR, "cursor right" },
	{ KB_SP, "select/toggle (space bar)" },
	{ KB_TA, "advance next line (Tab)"},

	{ KB_NL, "Return, complete input" },
	{ KB_DE, "erase left from cursor" },

	{ KB_EX, "Exit" },
	{ KB_CA, "Cancel" },
	{ KB_HE, "Help" },
	{ KB_RE, "Refresh screen" },
	{ KB_PR, "Prefix, 1st of 2-keys command" },

	{ KB_KI, "Insert" },
	{ KB_KD, "delete right from cursor" },
               
	{ KB_KH, "jump to 1st position" },
	{ KB_KE, "jump to last position" },
	{ KB_PU, "page back" },
	{ KB_PD, "page forward" },
	{ 0 }
};

/*
 * show label of logical code (kbcod), index is line->lvar
 */
cvt_keynm (line, cod, mod, str)
LINE *line;
kbcod cod;
char *mod;
char *str;
{
	int x;			/* indexes in KBL, KBDESCR*/
	kbcod codix;	/* t_cod from kbl */
	kbcod *codixp;

	if ( *mod == 'w' ) {
		codixp = line->varl;
		codix = *codixp;
		for (x=0; keynm[x].t_cod != 0; x++) {
			if (keynm[x].t_cod == codix) {
				strcpy(str, keynm[x].t_descr);
				return(TRUE);
			}
		}
	}
	return(FALSE);
}

/*
 * show description of logical code, index of keynm
 */
cvt_keycod (line, cod, mod, str)
LINE *line;
kbcod cod;
char *mod;
char *str;
{
	int x;			/* indexes in KBL, KBDESCR */
	kbcod codix;	/* t_cod from kbl */
	kbcod *codixp;
	char buf[8];

	if ( *mod == 'w' ) {
		codixp = line->varl;
		codix = *codixp;
		buf[0] = 'K';
		buf[1] = 'B';
		buf[2] = '_';
		buf[3] = cod0(codix);
		buf[4] = cod1(codix);
		buf[5] = '\0';
		strcpy(str, buf);
	}
	return(FALSE);
}

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
	char skey = 0;	/* cvts field used in keys.cv as descriptor for primary/secondary */
	kbcod keyptr;

	if (line->cvts) {
		skey = line->cvts[0];
	}

	kblp = (KBL *)line->varl;
	if ( *mod == 'w' ) {
		if (skey == '2')
			tmpcod = kblp->t_key2;
		else
			tmpcod = kblp->t_key1;

		str[0] = cod0(tmpcod);
		str[1] = cod1(tmpcod);
		str[2] = '\0';
	}
	else if ( *mod == 'r' && cod == ' ' ) {
		cp_set(line->line, line->colu, ATT|INP);
		w_strn(">>>", line->size);

		w_msg(ATT|VEXT, "Please, press key to be assigned...");
		tmpcod = r_key();
		if (tmpcod == 0)
			w_msg(ERR, "Unknown key");
		else if (cod1(tmpcod) == 0)
			w_msg(ERR,
"Printable char is disallowed. Please, use function or control keys");
		else if ((skey == '2' && kblp->t_key2 == tmpcod)
			  || (skey == '1' && kblp->t_key1 == tmpcod))
		{
			w_msg(ATT, "Key code was not changed!");
		} else {
			if (skey == '2') {
				kblp->t_key2 = tmpcod;
			} else {
				kblp->t_key1 = tmpcod;
			}
			w_msg(TXT, "");
		}
	} else if ( *mod == 'r' && cod == KB_DE ) {
		if (skey == '2') {
			kblp->t_key2 = KB_EMPTY;
		} else {
			kblp->t_key1 = KB_EMPTY;
		}
		w_msg(TXT, "Key code was erased.");
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
