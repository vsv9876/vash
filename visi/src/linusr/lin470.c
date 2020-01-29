/*
**      +----------+    БИБЛИОТЕКА ВВОДА-ВЫВОДА
**     (c) linlib  !    ДЛЯ АЛФАВИТНО-ЦИФРОВЫХ
**      +----------+    ВИДЕОТЕРМИНАЛОВ
**/

/*
 *      ПОЛЬЗОВАТЕЛЬСКИЙ УРОВЕНЬ БИБЛИОТЕКИ LINLIB
 *      w_help()  ПОКАЗ СПРАВОЧНОЙ ИНФОРМАЦИИ
 */

/*
 *      $Header: lin470.cv,v 1.1 89/08/29 14:50:38 vsv Rel $
 *      $Log:	lin470.cv,v $
Revision 1.1  89/08/29  14:50:38  vsv
ВЕРСИЯ LINLIB_3

 */

#include <stdio.h>
#include "line.h"

static
LINE help0[] = {
{ 80, 0, 0, 0, LHDR, 0,0,0, "LINLIB == Main keyboard commands" },
{ 13, 2, 3, 0, LTXT, 0,0,0, "close screen:" },
{  8, 2,27, 0, ATT|MID|PAD, 0, cvt_lh, 0, ":GL" },
{ 25, 2,40, 0, LTXT, 0,0,0, "navigation across fields:" },
{ 15, 3, 3, 0, LTXT, 0,0,0, "screen refresh:" },
{  8, 3,27, 0, ATT|MID|PAD, 0, cvt_lh, 0, ":RE" },
{  8, 3,40, 0, ATT|MID|PAD, 0, cvt_lh, 0, ":AU" },
{  8, 3,49, 0, ATT|MID|PAD, 0, cvt_lh, 0, ":AD" },
{ 14, 4, 3, 0, LTXT, 0,0,0, "keypad on/off:" },
{  8, 4,27, 0, ATT|MID|PAD, 0, cvt_lh, 0, ":KO" },
{  8, 4,40, 0, ATT|MID|PAD, 0, cvt_lh, 0, ":AL" },
{  8, 4,49, 0, ATT|MID|PAD, 0, cvt_lh, 0, ":AR" },
{  8, 4,58, 0, ATT|MID|PAD, 0, cvt_lh, 0, ":NL" },
{ 17, 6, 3, 0, LTXT, 0,0,0, "selector control:" },
{ 24, 6,40, 0, LTXT, 0,0,0, "How to select menu item:" },
{  8, 7, 3, 0, LTXT, 0,0,0, "forward:" },
{  4, 7,13, 0, ATT|MID|PAD, 0, cvt_lh, 0, ":> " },
{  4, 7,19, 0, ATT|MID|PAD, 0, cvt_lh, 0, ":. " },
{ 10, 7,25, 0, LHDR, 0,0,0, "SPACE " },
{ 20, 7,40, 0, LTXT, 0,0,0, "navigate, then press" },
{ 10, 7,61, 0, LHDR, 0,0,0, "SPACE " },
{  9, 8, 3, 0, LTXT, 0,0,0, "backward:" },
{  4, 8,13, 0, ATT|MID|PAD, 0, cvt_lh, 0, ":< " },
{  4, 8,19, 0, ATT|MID|PAD, 0, cvt_lh, 0, ":, " },
{  8, 8,25, 0, ATT|MID|PAD, 0, cvt_lh, 0, ":DE" },
{ 24,10, 3, 0, LTXT, 0,0,0, "switch to field editing:" },
{ 15,11, 6, 0, LTXT, 0,0,0, "- keep content:" },
{ 10,11,40, 0, LHDR, 0,0,0, "SPACE " },
{ 16,12, 6, 0, LTXT, 0,0,0, "- erase content:" },
{  8,12,40, 0, ATT|MID|PAD, 0, cvt_lh, 0, ":DE" },
{ 21,12,51, 0, LTXT, 0,0,0, "(eny printing simbol)" },
{ 80,14, 0, 0, LHDR, 0,0,0, "Field editing commands (string editor)" },
{ 16,16, 1, 0, LTXT, 0,0,0, "cursor to begin:" },
{  8,16,25, 0, ATT|MID|PAD, 0, cvt_lh, 0, ":GL" },
{  8,16,34, 0, ATT|MID|PAD, 0, cvt_lh, 0, ":AL" },
{ 15,16,47, 0, LTXT, 0,0,0, "symbol erasing:" },
{ 20,17, 1, 0, LTXT, 0,0,0, "cursor to text tail:" },
{  8,17,25, 0, ATT|MID|PAD, 0, cvt_lh, 0, ":GL" },
{  8,17,34, 0, ATT|MID|PAD, 0, cvt_lh, 0, ":AR" },
{ 17,17,47, 0, LTXT, 0,0,0, "left from cursor:" },
{  8,17,70, 0, ATT|MID|PAD, 0, cvt_lh, 0, ":DE" },
{ 20,18, 1, 0, LTXT, 0,0,0, "mode insert/replace:" },
{  8,18,25, 0, ATT|MID|PAD, 0, cvt_lh, 0, ":GL" },
{  8,18,34, 0, ATT|MID|PAD, 0, cvt_lh, 0, ":GL" },
{ 13,18,47, 0, LTXT, 0,0,0, "under cursor:" },
{  8,18,61, 0, ATT|MID|PAD, 0, cvt_lh, 0, ":GL" },
{  8,18,70, 0, ATT|MID|PAD, 0, cvt_lh, 0, ":DE" },
{ 16,19, 1, 0, LTXT, 0,0,0, "input new value:" },
{  8,19,25, 0, ATT|MID|PAD, 0, cvt_lh, 0, ":NL" },
{ 0 }, };



w_help(phelp)
/*--------------------------------*/
/* ПОКАЗАТЬ СПРАВОЧНУЮ ИНФОРМАЦИЮ */
/*--------------------------------*/
LINE    *phelp;         /* СТРАНИЦА ПОДСКАЗКИ */
{
help_1:
	if(phelp) {
		er_pag();
		w_page(phelp);
		switch(r_cod(0)) {
		case KB_RE:      goto help_1;
		case KB_HE:      break;
		default:        goto help_0;
		}
	} else bell();
	;
help_2:
	er_pag();
	w_page(help0);
	switch(r_cod(0)) {
	case KB_RE:      goto help_2;
	case KB_HE:      goto help_1;
	default:        break;
	}
help_0:
	er_pag();
}
