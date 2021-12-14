/*
**      +----------+    библиотека ввода-вывода
**     (c) linlib  !    для алфавитно-цифровых
**      +----------+    видеотерминалов
**/

/*
 *      $Header: lin210.c,v 1.1 90/12/27 16:28:54 vsv Rel $
 *
 *      $Log:	lin210.c,v $
 * Revision 1.1  90/12/27  16:28:54  vsv
 * версия LINLIB_3
 * 
 * Revision 3.4  89/08/29  15:14:53  vsv
 * версия LINLIB_3
 * 
 * Revision 3.3  88/07/28  09:23:43  vsv
 * работа с клавишей 'DEL' приведена к общему виду
 * 
 * Revision 3.2  88/06/27  15:19:23  vsv
 * ревизия архива RCS
 * 
 * Revision 3.1  88/04/29  12:24:13  vsv
 * разделены таблицы и процедуры,
 * имеется два разных модуля
 * 
 */

#include <stdio.h>
#include "line.h"
#include "line0.h"
#include "lineva.h"

/*
 * Настройка на тип терминала
 *
 * termcap не совсем удобен, так как
 * требует пухлого кода, который
 * используется только один раз.
 * Возможно, лучше настройку из termcap
 * готовить отдельно, затем в уже упакованном виде
 * считывать за одно обращение к файлу.
 * ПРИМЕЧАНИЕ.  работаем
 * с termcap, только help вытаскиваем по ключу :lh= ... :
 *
 * В этом файле объвлены данные, требуемые для
 * описания возможностей терминала.
 * Можно объявить все строки esc-последовательностей
 * константами, и функцию hw_set как пустую -
 * размер кода исполняющей библиотеки
 * уменьшится за счет потери независимости от
 * типа терминала.
 */


/* внешние переменные для tgoto() из termcap */
char *UP = NULL;        /* переместить курсор вверх на 1 позицию */
char *BC = NULL;        /* ---------""------- влево -----""----- */

/* размеры экрана, устанавливаются через hw_ini() */
/*int     maxli = 0;/*24;     /* размер по вертикали */
/*int     maxco = 0;/*80;     /* --"-- по горизонтали */
LFRAME hwframe = { 0 };

/*---------------------*/
/* ОПИСАНИЕ КЛАВИАТУРЫ */
/*---------------------*/

KBF   kbf[] = {

/*      код возвращается по запросу r_key();
 *      вторые элементы заполняются hw_set(),
 *      или вручную для описания единственного
 *      дисплея; в последнем случае можно
 *      изменить количество клавиш, и/или занести
 *      константные описания в таблицу kbl (см. ниже).
 */
{ KBCOD('k','u'), 0 },
{ KBCOD('k','d'), 0 },
{ KBCOD('k','l'), 0 },
{ KBCOD('k','r'), 0 },
{ KBCOD('I','C'), 0 },
{ KBCOD('D','C'), 0 },
{ KBCOD('I','L'), 0 },
{ KBCOD('D','L'), 0 },
{ KBCOD('k','h'), 0 },
/*
{ KBCOD('i','c'), 0 },
{ KBCOD('i','m'), 0 },
{ KBCOD('k','A'), 0 },
{ KBCOD('k','A'), 0 },
*/
{ KBCOD('k','D'), 0 },
{ KBCOD('k','I'), 0 },
{ KBCOD('@','7'), 0 },
{ KBCOD('k','P'), 0 },
{ KBCOD('k','N'), 0 },

{ KBCOD('k','0'), 0 }, /* probably no such key in termcap/terminfo */
{ KBCOD('k','1'), 0 }, /* PC F1 / DEC VT100 PF1 */
{ KBCOD('k','2'), 0 }, 
{ KBCOD('k','3'), 0 },
{ KBCOD('k','4'), 0 }, /* PC F4 / DEC VT100 PF4 */
{ KBCOD('k','5'), 0 }, /* PC F5 */
{ KBCOD('k','6'), 0 },
{ KBCOD('k','7'), 0 },
{ KBCOD('k','8'), 0 },
{ KBCOD('k','9'), 0 },
{ KBCOD('k',';'), 0 }, /* PC F10*/
{ KBCOD('F','1'), 0 }, /* PC F11*/
{ KBCOD('F','2'), 0 }, /* PC F12*/

{ KBCOD('f','.'), 0 }, /* probably, keypad DEC VT100 in function mode */
{ KBCOD('f','-'), 0 },
{ KBCOD('f',','), 0 },
{ KBCOD('f','0'), 0 },
{ KBCOD('f','1'), 0 },
{ KBCOD('f','2'), 0 },
{ KBCOD('f','3'), 0 },
{ KBCOD('f','4'), 0 },
{ KBCOD('f','5'), 0 },
{ KBCOD('f','6'), 0 },
{ KBCOD('f','7'), 0 },
{ KBCOD('f','8'), 0 },
{ KBCOD('f','9'), 0 },
{ 0, 0 },
};

/* ОПИСАНИЕ ВОЗМОЖНОСТЕЙ ВЫВОДА НА ЭКРАН */
/*
 * функция hw_set заменяет строки, которые являются
 * ключами возможностей терминала, см. termcap(5)
 * на строки значений этих ключей.
 * Для описания единственного терминала замените ключи на значения
 * (т.е. на соответствующие esc-последовательности)
 * и поместите в этом файле описание пустой функции "hw_set() {}"
 */
char *tcapo[] = {
	"cm", "cl", "cd", "ce", "ks", "ke",
	"cs", "sr", "sf", "al", "dl",
	"so", "se", "us", "ue",
	"md", "mr", "mb", "mk", "me",
	"bl", "vb",
	"lh",   /* ИМЯ КЛАВИАТУРЫ */
	0 };

/* СВЯЗЬ ФИЗИЧЕСКИХ И ЛОГИЧЕСКИХ КОДОВ, А ТАКЖЕ ИМЕН КЛАВИШ */
/* vanilla setup, may be changed via 'vhset' utility */
KBL kbl[KBLSIZE] = {

/* KEYPAD ON, it is possible to redefine as usual */
{ KB_KP, "Ctrl-K ", KBCTL(    'K'), KB_EMPTY }, /* kbl[0] */

/* default values, cannot be redefined vith vhset */
{ KB_AU, "<up>   ", KBCOD('k','u'), KB_EMPTY       },
{ KB_AD, "<down> ", KBCOD('k','d'), KB_EMPTY       },
{ KB_AL, "<left> ", KBCOD('k','l'), KB_EMPTY       },
{ KB_AR, "<right>", KBCOD('k','r'), KB_EMPTY       },

/* limited customizing possible (labels only) */
{ KB_SP, "Space  ", KBCTL(    'S'), KB_EMPTY       },
{ KB_TA, "^I,Tab ", KBCTL(    'I'), KB_EMPTY       },

/* full customizing supported with vhset */
{ KB_NL, "Enter  ", KBCTL(    'M'), KBCTL(    'J') },
{ KB_DE, "^H,<-- ", KBCOD('d','e'), KBCTL(    'H') },
                                                   
{ KB_HE, "^G,F1  ", KBCOD('k','1'), KBCTL(    'G') },
{ KB_EX, "^X,F2  ", KBCOD('k','2'), KBCTL(    'X') },
{ KB_CA, "^C     ", KBCTL(    'C'), KB_EMPTY       },
{ KB_RE, "^L     ", KBCTL(    'L'), KB_EMPTY       },
{ KB_PR, "^\\     ", KBCTL(    '\\'), KB_EMPTY     },
                                                   
{ KB_KI, "^O,Ins ", KBCOD('k','I'), KBCTL(    'O') },
{ KB_KD, "Delete ", KBCOD('k','D'), KB_EMPTY       },
{ KB_KH, "^A,Home", KBCOD('k','h'), KBCTL(    'A') },
{ KB_KE, "^E,End ", KBCOD('@','7'), KBCTL(    'E') },
{ KB_PU, "^B,PgUp", KBCOD('k','P'), KBCTL(    'B') },
{ KB_PD, "^F,PgDn", KBCOD('k','N'), KBCTL(    'F') },

/* extended set with limited support, not supported by vhset yet */
{ KBUSR('0'), "       ", KBCOD('f','0'), KBUSR('0'), KB_EMPTY },
{ KBUSR('1'), "       ", KBCOD('f','1'), KBUSR('1'), KB_EMPTY },
{ KBUSR('2'), "       ", KBCOD('f','2'), KBUSR('2'), KB_EMPTY },
{ KBUSR('3'), "       ", KBCOD('f','3'), KBUSR('3'), KB_EMPTY },
{ KBUSR('4'), "       ", KBCOD('f','4'), KBUSR('4'), KB_EMPTY },
{ KBUSR('5'), "       ", KBCOD('f','5'), KBUSR('5'), KB_EMPTY },
{ KBUSR('6'), "       ", KBCOD('f','6'), KBUSR('6'), KB_EMPTY },
{ KBUSR('7'), "       ", KBCOD('f','7'), KBUSR('7'), KB_EMPTY },
{ KBUSR('8'), "       ", KBCOD('f','8'), KBUSR('8'), KB_EMPTY },
{ KBUSR('9'), "       ", KBCOD('f','9'), KBUSR('9'), KB_EMPTY },
{ KBUSR('.'), "       ", KBCOD('f','.'), KBUSR('.'), KB_EMPTY },
{ KBUSR('-'), "       ", KBCOD('f','-'), KBUSR('-'), KB_EMPTY },
{ KBUSR(','), "       ", KBCOD('f',','), KBUSR(','), KB_EMPTY },

{ 0 },
};

/*
 * TODO: line0.h
 *
 * ANSI color logical tables for SGR
 *
 * Note: active code is in w_sgr() from lin320.c and vhset/attr.c
 *
 * now a member of LPA structure:
 * char *acout [LPASIZE];
 * char *acinp [LPASIZE];
 *
 */
LPA lpaout[LPASIZE] = {
	{       '.',    0,          "39;49" },              /* CMD  0*/
	{       ' ',    0,          "39;49" },              /* TXT  1*/
	{       '_',    A_MR,       "36;40" },              /* HDR  2*/
	{       ' ',    A_MD,       ""      },              /* VAR  3*/
	{       ' ',    0,          "33"    },              /* ALT  4*/
	{       '*',    0,          "32"    },              /* MSE  5*/
	{       ' ',    A_MR,       "31;47" },              /* ERR  6*/
	{       ' ',    A_MR,       ""      },              /* ATT  7*/
};

LPA lpainp[LPASIZE] = {
	{       ' ',    A_MD,           "31;49" },          /* CMD  0*/
	{       '|',    0,              "33"    },          /* TXT  1*/
	{       '\'',   A_MD,           ""      },          /* HDR  2*/
	{       '"',    A_MR|A_MD,      "34;47" },          /* VAR  3*/
	{      '\'',    A_MR|A_MD,      "34;47" },          /* ALT  4*/
	{       '>',    A_MR,           "32"    },          /* MSE  5*/
	{       ' ',    A_MR|A_US,      "35;47" },          /* ERR  6*/
	{       '!',    A_MR,           "33;40" },          /* ATT  7*/
};

int		sgrmode = 1; /* initial monochrome, not a dumb :) */
