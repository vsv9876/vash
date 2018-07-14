/*
**      +----------+    библиотека ввода-вывода
**     (c) linlib  !    для алфавитно-цифровых
**      +----------+    видеотерминалов
**/

/*
 *      $Header: line.h,v 3.7 90/01/11 16:19:10 vsv Exp $
 *
 *      $Log:	line.h,v $
 * Revision 3.7  90/01/11  16:19:10  vsv
 * добавлено описание некоторых функций linusr
 * 
 * Revision 3.6  90/01/11  10:18:00  vsv
 * версия V32
 * 
 * Revision 3.5  89/08/29  16:21:26  vsv
 * версия LINLIB_3
 * 
 * Revision 3.4  88/08/02  10:50:28  vsv
 * добавлен флаг режима перехода к редактированию поля (allcod).
 * 
 * Revision 3.3  88/05/17  11:09:34  vsv
 * исправлены константы флагов задержек.
 * 
 * Revision 3.2  88/04/27  11:08:55  vsv
 * maxli, maxco в файле line.h, так более удобно.
 * 
 * Revision 3.1  88/04/27  08:38:28  vsv
 * переделано для поддержки 
 * системы VISI
 * 
 * Revision 3.0  87/12/21  12:23:14  vsv
 * предварительный выпуск.
 * 
 */
#ifndef line_h_def
#define line_h_def
#include <stdio.h>

/******* эти константы определяются через Makefile, CFLAGS_VISI */
/* #define USE_TERMIO    /* must be enabled on SYSTEM V */
/* #define USE_SGTTYH    /* must be enabled on DEMOS 2.x */
/* #define USE_TERMIOS_386BSD  /* 386bsd, old versions BSDI,FreeBSD */
/* #define USE_TERMIOS   /* POSIX */

#ifndef NOUSE_STRCHAR
#define index strchr
#define rindex strrchr
#endif

/* требуется для разных констант, RT11 более не поддерживается */
#define DEMOS2

/* конец настраиваемых параметров */
/*-------------------------------*/

/* стандартный вывод и ввод для экрана */
#if defined(FILE)
extern  FILE *vttout;
extern  int   vtti;      /* дескриптор файла для ввода */
#else           /* temporary for first debug SYSTEM V */
extern  FILE *vttout;
extern  int   vtti;
#endif

typedef int   kbcod;    /* код клавиши, например '^M', 'P1', 'k1' */
typedef char  bool;     /* короткий формат для целых чисел, флагов */

/* манипуляции с кодом клавиши (переменная типа kbcod) */
/* проверено для pdp11, i8086, i80286, m68000 */

#define cod0(c) ((c) & 0xff)            /* выделить первый символ */
#define cod1(c) (((c)>>8) & 0xff)       /* --"---   второй --"--- */
#define tocod0(c) ((c) & 0xff)          /* сформировать первый --"--- */
#define tocod1(c) (((c)<<8) & 0xff00)   /* ---"---- второй --"--- */

/* сформировать код клавиши (kbcod из двух типа char) */
#define KBCOD(c0, c1)   (tocod0(c0)|tocod1(c1))
#define KBUSR( c )      (tocod0( 'U' )|tocod1(c))
#define KBCTL( c )      (tocod0( '^' )|tocod1(c))
#define ISCTL( c )      (cod1(c) != 0 ? 1 : 0)

#define STRLEN 82       /* длина строки для ввода с клавиатуры */
#ifndef FALSE           /* определение м.быть в др. месте */
#define TRUE    1
#define FALSE   0
#endif


/*--------------------------------------*/
/* универсальное описание линий (полей) */
/*--------------------------------------*/
typedef struct  {
	bool    size;           /* общий размер поля, вкл. промптер     */
	bool    line;           /* номер строки на экране               */
	bool    colu;           /* номер колонки (позиция промптера)    */
	bool    flag;           /* флаги задержек                       */
	short   attr;           /* слово атрибутов линии (тип,видео,..) */
	char    *cvts;          /* строка формат. преобр. (sprintf)     */
	char    (*cvtf)();      /* функция нестандарт. форматн. преобр. */
	int     (*test)();      /* адрес функции проверки               */
	char    *varl;          /* адрес переменной, связанной с линией */
} LINE ;

/*---------------------------------------*/
/* определения битов для слова атрибутов */
/*---------------------------------------*/

/*---- основные логические типы видеоатрибутов */
#define TXT 1       /* текст комментария (TEXT) */
#define HDR 2       /* заголовок (HEADER)  */
#define VAR 3       /* поле для ввода (VARIABLE) */
#define ALT 4       /* переключатель (ALTERNATE) */
#define MSE 5       /* селектор меню (MENU SELECTOR) */
#define ERR 6       /* сообщение об ошибке (ERROR) */
#define ATT 7       /* "внимание !"   (ATTENTION !) */

/*---- общие битовые маски атрибутов */
#define PMT  0100000    /* впереди подсказка (PROMPTER) */
#define PAD   040000    /* поле для вывода заполняется справа пробелами */
#define MID   020000    /* текст в поле выравнивается по центру */
#define INP   010000    /* поле доступно для ввода */
#define NED    04000    /* не редактировать строку линии */
#define EDT    02000    /* тест-функция подкл. для редактирования */
#define FLO    06000    /* вызов форм. преобр. и редактор блокировать */
#define VCOLOR 01000    /* ?!!... использовать цветную таблицу атрибутов */
#define LFASTR  0400    /* r_line перерисовывает только первый символ поля */

/* маска индекса таблицы видеоатрибутов и подсказок: */
#define VIDEO  00177
#define VIDEOM (VIDEO|INP|VCOLOR)       /* маска видеоатрибутов */

/*---- атрибуты часто используемых типов линий: */
#define LTXT TXT
#define LHDR (HDR|MID|PAD)
#define LVAR (VAR|PAD)
#define LALT (ALT|MID|PAD|NED)
#define LMSE (MSE|FLO)

/*----------------------*/
/* биты флагов задержек */
/*----------------------*/
#define SUST    0100    /* элемент таблицы: базу искать по флагу */
	/* нет перехода: */
#define SUSNL   0003    /* к следующему полю по <CR><LF>   */
#define SUSAR   0034    /* по стрелкам                     */
#define SUSUD   0014    /* вверх и вниз                    */
#define SUSLR   0060    /* вправо и влево                  */
#define SUSU    0004    /* вверх                           */
#define SUSD    0010    /* вниз                            */
#define SUSL    0020    /* влево                           */
#define SUSR    0040    /* вправо                          */

/*---------------------*/
/* стандартные команды */
/*---------------------*/
#define KB_AU     KBCOD('A','U')
#define KB_AD     KBCOD('A','D')
#define KB_AL     KBCOD('A','L')
#define KB_AR     KBCOD('A','R')
#define KB_EX     KBCOD('E','X')
#define KB_HE     KBCOD('H','E')
#define KB_KP     KBCOD('K','P')
#define KB_PR     KBCOD('P','R')
#define KB_NL     KBCOD('N','L')
#define KB_DE     KBCOD('D','E')
#define KB_RE     KBCOD('R','E')
#define KB_TA     KBCOD('T','A')

/*----------------------*/
/* флаги функции io_set */
/*----------------------*/
#define IO_VIDEO  0000001       /* без эхопечати, вывод в кодах терминала */
#define IO_TTYPE  0000002       /* обычный телетайпный режим */
#define IO_WAIT   0000004       /* next_j() ожидает нажатие клавиши */
#define IO_NOWAIT 0000010       /* next_j() в непрерывном цикле */

/*--------------------*/
/* функции библиотеки */
/*--------------------*/

/* lin(1) - интерфейс терминала с операционной системой */
extern  int     io_set(), ttyinp(), w_chr(), w_raw(), w_str(), w_strn();

/* lin(2) - управление терминалом: физический уровень */
extern  int     at_set();
extern  kbcod   r_cod(), r_key();
extern  int     hw_set(), do_kbl();

/* lin(3) - управление терминалом: логический уровень */

extern  int    maxli;   /* размер экрана по горизонтали */
extern  int    maxco;   /* размер по вертикали */

extern  kbcod   k_pars();
extern  int     bell(), cp_fet(), cp_sav(), cp_set();
extern  int     er_eol(), er_eop(), er_pag(), er_scr();
extern  kbcod   e_str(), r_line(), r_page();
extern  int     allcod, edinff;
extern  int     w_line(), w_page();

/* lin(4) - функции уровня пользователя */
extern  int     cvt_a(), cvt_sp(), cvt_lh(), cvt_hl(), tst_m();

extern  int     w_msg(), w_emsg(), w_help(), u_page();
extern  LINE	*bpage();

#define er_page er_pag
#define r_str   e_str

#endif /* line_h_def */
