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

static
LINE mainm[] = {
{ 80, 0, 0, 0, LHDR, 0,0,0, "Keys setup" },
{  3, 2, 9, 0, LTXT, 0,0,0, "Key" },
{  4, 2,16, 0, LTXT, 0,0,0, "Name" },
{ 11, 2,27, 0, LTXT, 0,0,0, "Description" },
{  6, 3, 8, 0,	INP|LALT,	0,	cv_kbl,	0,	&kbl[0] },
{  8, 3,16, 0,	INP|LVAR,	0,	cvt_sp,	0,	&kbl[0].t_knm },
{ 45, 3,26, 0,	0|LTXT,	0,	cvt_sp,	0,	&keynam[0] },
{  6, 4, 8, SUST|SUSU, 0,0,0,0, sizeof(KBL) },
{  8, 4,16, SUST|SUSU, 0,0,0,0, sizeof(KBL) },
{ 45, 4,26, SUST|SUSU, 0,0,0,0, sizeof(keynam[0]) },
{  6, 5, 8, SUST|SUSU, 0,0,0,0, sizeof(KBL) },
{  8, 5,16, SUST|SUSU, 0,0,0,0, sizeof(KBL) },
{ 45, 5,26, SUST|SUSU, 0,0,0,0, sizeof(keynam[0]) },
{  6, 6, 8, SUST|SUSU, 0,0,0,0, sizeof(KBL) },
{  8, 6,16, SUST|SUSU, 0,0,0,0, sizeof(KBL) },
{ 45, 6,26, SUST|SUSU, 0,0,0,0, sizeof(keynam[0]) },
{  6, 7, 8, SUST|SUSU, 0,0,0,0, sizeof(KBL) },
{  8, 7,16, SUST|SUSU, 0,0,0,0, sizeof(KBL) },
{ 45, 7,26, SUST|SUSU, 0,0,0,0, sizeof(keynam[0]) },
{  6, 8, 8, SUST|SUSU, 0,0,0,0, sizeof(KBL) },
{  8, 8,16, SUST|SUSU, 0,0,0,0, sizeof(KBL) },
{ 45, 8,26, SUST|SUSU, 0,0,0,0, sizeof(keynam[0]) },
{  6, 9, 8, SUST|SUSU, 0,0,0,0, sizeof(KBL) },
{  8, 9,16, SUST|SUSU, 0,0,0,0, sizeof(KBL) },
{ 45, 9,26, SUST|SUSU, 0,0,0,0, sizeof(keynam[0]) },
{  6,10, 8, SUST|SUSU, 0,0,0,0, sizeof(KBL) },
{  8,10,16, SUST|SUSU, 0,0,0,0, sizeof(KBL) },
{ 45,10,26, SUST|SUSU, 0,0,0,0, sizeof(keynam[0]) },
{  6,11, 8, SUST|SUSU, 0,0,0,0, sizeof(KBL) },
{  8,11,16, SUST|SUSU, 0,0,0,0, sizeof(KBL) },
{ 45,11,26, SUST|SUSU, 0,0,0,0, sizeof(keynam[0]) },
{  6,12, 8, SUST|SUSU, 0,0,0,0, sizeof(KBL) },
{  8,12,16, SUST|SUSU, 0,0,0,0, sizeof(KBL) },
{ 45,12,26, SUST|SUSU, 0,0,0,0, sizeof(keynam[0]) },
{  6,13, 8, SUST|SUSU, 0,0,0,0, sizeof(KBL) },
{  8,13,16, SUST|SUSU, 0,0,0,0, sizeof(KBL) },
{ 45,13,26, SUST|SUSU, 0,0,0,0, sizeof(keynam[0]) },
{  6,14, 8, SUST|SUSU, 0,0,0,0, sizeof(KBL) },
{  8,14,16, SUST|SUSU, 0,0,0,0, sizeof(KBL) },
{ 45,14,26, SUST|SUSU, 0,0,0,0, sizeof(keynam[0]) },
{  6,15, 8, SUST|SUSU, 0,0,0,0, sizeof(KBL) },
{  8,15,16, SUST|SUSU, 0,0,0,0, sizeof(KBL) },
{ 45,15,26, SUST|SUSU, 0,0,0,0, sizeof(keynam[0]) },
{  6,16, 8, SUST|SUSU, 0,0,0,0, sizeof(KBL) },
{  8,16,16, SUST|SUSU, 0,0,0,0, sizeof(KBL) },
{ 45,16,26, SUST|SUSU, 0,0,0,0, sizeof(keynam[0]) },
{  6,17, 8, SUST|SUSU, 0,0,0,0, sizeof(KBL) },
{  8,17,16, SUST|SUSU, 0,0,0,0, sizeof(KBL) },
{ 45,17,26, SUST|SUSU, 0,0,0,0, sizeof(keynam[0]) },
{  6,18, 8, SUST|SUSU, 0,0,0,0, sizeof(KBL) },
{  8,18,16, SUST|SUSU, 0,0,0,0, sizeof(KBL) },
{ 45,18,26, SUST|SUSU, 0,0,0,0, sizeof(keynam[0]) },
{  6,19, 8, SUST|SUSU, 0,0,0,0, sizeof(KBL) },
{  8,19,16, SUST|SUSU, 0,0,0,0, sizeof(KBL) },
{ 45,19,26, SUST|SUSU, 0,0,0,0, sizeof(keynam[0]) },
{  6,20, 8, SUST|SUSU, 0,0,0,0, sizeof(KBL) },
{  8,20,16, SUST|SUSU, 0,0,0,0, sizeof(KBL) },
{ 45,20,26, SUST|SUSU, 0,0,0,0, sizeof(keynam[0]) },
{ 0 }, };


static  char    helpf[] = "vhsetk.lb";

pag_k()
/*-----------------*/
/* מבףפעןךכב כלבקיû*/
/*-----------------*/
{
	u_page(mainm, helpf);
	return(TRUE);
}
