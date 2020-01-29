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

/* размеры экрана */
int     maxli = 0;/*24;     /* размер по вертикали */
int     maxco = 0;/*80;     /* --"-- по горизонтали */


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
{ KBCOD('k','1'), 0 },
{ KBCOD('k','2'), 0 },
{ KBCOD('k','3'), 0 },
{ KBCOD('k','4'), 0 },
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

{ KBCOD('f','.'), 0 },
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
KBL kbl[KBLSIZE] = {
		/*==== main set */
      { KBCTL(    'K'),   KB_KP,   "Ctrl-K " },    /* KEYPAD ON */
      { KBCOD('k','u'),   KB_AU,   "UP     " },		/* arrow keys */
      { KBCOD('k','d'),   KB_AD,   "DOWN   " },
      { KBCOD('k','l'),   KB_AL,   "LEFT   " },
      { KBCOD('k','r'),   KB_AR,   "RIGHT  " },
      { KBCTL(    'J'),   KB_NL,   "Enter  " },
      { KBCOD('d','e'),   KB_DE,   "<-del  " }, /* delete left from cursor */
      { KBCTL(    'C'),   KB_EX,   "Ctrl-C " },	/* exit */
      { KBCOD('k','1'),   KB_HE,   "F1     " }, /* help screen */
      { KBCTL(    'V'),   KB_PR,   "Ctrl-V " }, /* prefix for complex keyboard commands */
      { KBCTL(    'L'),   KB_RE,   "Ctrl-L " }, /* refresh/redraw page content on screen */
      { KBCTL(    'I'),   KB_TA,   "TAB    " },
	  { KBCTL(    'A'), KB_KH,    "Home  " },
	  { KBCTL(    'E'), KB_KE,    "End   " },
	  { KBCTL(    'B'), KB_PU,    "PgUp  " },
	  { KBCTL(    'F'), KB_PD,    "PgDown" },
	  /* extended for linlib 4 since 2017-05 */
	  /* delUnderCursor, insert, home, end, PageUp, PageDown */
      { KBCOD('k','D'),   KB_KD,   "del->  " },
	  { KBCOD('k','I'), KB_IN,    "Insert" },
	  { KBCOD('k','h'), KB_KH,    "Home  " },
	  { KBCOD('@','7'), KB_KE,    "End   " },
	  { KBCOD('k','P'), KB_PU,    "PgUp  " },
	  { KBCOD('k','N'), KB_PD,    "PgDown" },
	  /**/
      { KBCOD('k','4'),   KB_PR,   "PF4    " },
		/*==== extended set */
      { KBCOD('f','0'),   KBUSR('0'),   0         },
      { KBCOD('f','1'),   KBUSR('1'),   0         },
      { KBCOD('f','2'),   KBUSR('2'),   0         },
      { KBCOD('f','3'),   KBUSR('3'),   0         },
      { KBCOD('f','4'),   KBUSR('4'),   0         },
      { KBCOD('f','5'),   KBUSR('5'),   0         },
      { KBCOD('f','6'),   KBUSR('6'),   0         },
      { KBCOD('f','7'),   KBUSR('7'),   0         },
      { KBCOD('f','8'),   KBUSR('8'),   0         },
      { KBCOD('f','9'),   KBUSR('9'),   0         },
      { KBCOD('f','.'),   KBUSR('.'),   0         },
      { KBCOD('f','-'),   KBUSR('-'),   0         },
      { KBCOD('f',','),   KBUSR(','),   0         },
      { KBCTL(    'M'),   KB_NL,   0         },    /*==== aliases */
      { KBCTL(    'H'),   KB_DE,   0         },
      { KBCTL(    'X'),   KB_EX,   0         },
/*    { KBCOD('k','h'),   KB_EX,   0         }, */
      { KBCTL(    'R'),   KB_RE,   0         },
      { 0 },
	};


/*
 * TODO: line0.h
 *
 * ANSI color logical tables for SGR
 * note: there is no defined constant to index FGBG - it is simpy 0 (zero)
 *
 * Note: active code is in w_sgr() from lin320.c and vhset/attr.c
 *
 * now a member of LPA strcture:
 * char *acout [LPASIZE];
 * char *acinp [LPASIZE];
 *
 */
LPA lpaout[LPASIZE] = {
	{       ' ',    0,          "0"     },              /* CMD  */
	{       ' ',    0,          "39;49" },              /* TXT  */
	{       ' ',    A_SO,       "36;49" },              /* HDR  */
	{       ' ',    A_MD,       "39"    },              /* VAR  */
	{       ' ',    A_MD,       "35"    },              /* ALT  */
	{       '-',    A_MD,       "32"    },              /* MSE  */
	{       ' ',    A_MD,       "33;41" },              /* ERR  */
	{       '!',    A_SO,       "33;44" },              /* ATT  */
};

LPA lpainp[LPASIZE] = {
	{       ' ',    A_MD,       "32;49", },             /* CMD */
	{       ':',    A_MD,       "39;44" },              /* TXT  */
	{       ' ',    A_SO,       "36;44" },              /* HDR  */
	{       '"',    A_SO|A_MD,  "39"    },              /* VAR  */
	{      '\'',    A_SO,       "35,47" },              /* ALT  */
	{       '>',    A_SO,       "32"    },              /* MSE  */
	{       ' ',    A_MD,       "36;41" },              /* ERR  */
	{       ' ',    A_MR,       "37;41" },              /* ATT  */
};

int		sgrmode = 1; /* initial monochrome, not a dumb :) */
