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
#include <wchar.h>

/******* эти константы определяются через Makefile, CFLAGS_VISI */
/* #define USE_TERMIO    // must be enabled on SYSTEM V */
/* #define USE_SGTTYH    // must be enabled on DEMOS 2.x */
/* #define USE_TERMIOS_386BSD  // 386bsd, old versions BSDI,FreeBSD */
/* #define USE_TERMIOS   // POSIX */

/* тип для арифметики над указателями */
/*(sizeof(char *) != sizeof(int))*/
#if !defined LINEPTR_L
#define LINEPTR_L
#if defined __LP64__
typedef unsigned long linptr_t;
#elif defined __LP32__
typedef unsigned int linptr_t;
#else
typedef unsigned linptr_t;
#endif
#endif

#ifndef NOUSE_STRCHAR
#include <string.h>
#define index strchr
#define rindex strrchr
#endif

#if 0
#define NOUSE_STRCP
#ifndef NOUSE_STRCP
extern char *strcp(char *, char *);
extern char *strncp(char *, char *, size_t);
#define strcpy strcp
#define strncpy strncp
#endif
#endif

/* требуется для разных констант, RT11 более не поддерживается */
#define DEMOS2

/*---- конец настраиваемых параметров */

/* стандартный вывод и ввод для экрана */
#if defined(FILE)
/* extern  FILE *vttout; very bad idea, macro used as workaround... */
#define vttout stderr
/*#define vttout stdout*/
extern  int   vtti;      /* дескриптор файла для ввода */
#else           /* temporary for first debug SYSTEM V */
/* remix, so try again vttout */
extern  FILE *vttout; /* ex, no matter: very bad idea, macro used as workaround... */
/* #define vttout stderr
/* #define vttout stdout */
extern  int   vtti;
#endif

/*typedef unsigned int kbcod;  /* код клавиши, например '^M', 'P1', 'k1' */
typedef int   kbcod;  /* код клавиши, например '^M', 'P1', 'k1' */
typedef short  bool;     /* короткий формат для целых чисел, флагов */

#define KBCOD_INT32

#ifdef KBCOD_INT16
/* манипуляции с кодом клавиши (переменная типа kbcod) (sizeof(int)>=16)*/
/* проверено для pdp11, i8086, i80286, m68000 */

#define cod0(c) ((c) & 0xff)            /* выделить первый символ */
#define cod1(c) (((c)>>8) & 0xff)       /* --"---   второй --"--- */
#define tocod0(c) ((c) & 0xff)          /* сформировать первый --"--- */
#define tocod1(c) (((c)<<8) & 0xff00)   /* ---"---- второй --"--- */

/* сформировать код клавиши (kbcod из двух типа char) */
#define KBCOD(c0, c1)   tocod0(c0)|tocod1(c1)
#define KBUSR( c )      (tocod0( 'U' )|tocod1(c))
#define KBCTL( c )      (tocod0( '^' )|tocod1(c))
#define ISCTL( c )      (cod1(c) != 0 ? 1 : 0)
#endif /*KBCOD_INT16*/

#ifdef KBCOD_INT32
/* Unicode support on 32/64 bit architectures (sizeof(int)>=32)
 * kbcod contains mixed value:
 * mask for control logical code does not cover byte 0 (is printable ASCII),
 * and control logical code is outside Unicode/utf8 space in upper byte 3;
 * Unicode character is not masked/confused by control code mask.
 */

/* kbcod control prefix, outside Unicode space below 0x10ffffff */
#define KBPRE 0x1f000000

#define cod0(c)       ((c)  & 0xff)       /* extract 1st symbol */
#define cod1(c)   (((c)>>8) & 0xff)       /* extract 2nd symbol */
/* prepare 1st, 2nd symbol */

#define tocod0(c)  (c       & 0xff)
#define tocod1(c) ((c << 8) & 0xff00)

/* сформировать код клавиши (kbcod из двух типа char) */
#define KBCOD( c1 , c2 )     ((tocod0 (c1) | tocod1 (c2)) | KBPRE)
#define KBUSR( c )      KBCOD('U',c)
#define KBCTL( c )      KBCOD('^',c)

/*#define  KB_EMPTY  KBCTL( 0 )/*((kbcod)0)*/
#define KB_EMPTY		(kbcod)0
/* check if kbcod is linlib control code - TODO more accurate, see KBCOD_INT16 */
#define ISCTL( c )      (((c) & KBPRE) == KBPRE)

#endif /*KBCOD_INT32*/

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
	const void    *cvts;    /* строка формат. преобр. (sprintf)     */
	int     (*cvtf)();      /* функция нестандарт. форматн. преобр. */
	int     (*test)();      /* адрес функции проверки               */
	const void    *varl;    /* адрес переменной, связанной с линией */
} LINE ;

/*---------------------------------------*/
/* определения битов для слова атрибутов */
/*---------------------------------------*/

/*---- основные логические типы видеоатрибутов */
#define LTYPE 0xf	/* маска логического типа, пример (line->attr == (LTYPE & ATT)) */
#define CMD 0x0     /* команды терминала, режим за пределами visi */
#define TXT 01       /* текст комментария (TEXT) */
#define HDR 02       /* заголовок (HEADER)  */
#define VAR 03       /* поле для ввода (VARIABLE) */
#define ALT 04       /* переключатель (ALTERNATE) */
#define MSE 05       /* селектор меню (MENU SELECTOR) */
#define ERR 06       /* сообщение об ошибке (ERROR) */
#define ATT 07       /* "внимание !"   (ATTENTION !) */

/*---- общие битовые маски атрибутов */
#define PMT  0100000    /* впереди подсказка (PROMPTER) */
#define PAD   040000    /* поле для вывода заполняется справа пробелами */
#define MID   020000    /* текст в поле выравнивается по центру */
#define INP   010000    /* поле доступно для ввода */
#define NED    04000    /* не редактировать строку линии */
#define EDT    02000    /* тест-функция подкл. для редактирования */
#define FLO    06000    /* вызов форм. преобр. и редактор блокировать */
#define VEXT   01000    /* видео атрибут для вида "как на вводе", но при выводе  */
#define LFASTR  0400    /* r_line перерисовывает только первый символ поля */
/*define U8SOBJ 0 // no room for... */

/* маска индекса таблицы видеоатрибутов и подсказок: */
#define VIDEO  0177
#define VIDEOM (VIDEO|INP|VEXT)       /* маска видеоатрибутов */

/*---- атрибуты часто используемых типов линий: */
#define LTXT TXT
#define LHDR (HDR|MID|PAD)
#define LVAR (VAR|PAD)
#define LALT (ALT|MID|PAD|NED)
#define LMSE (MSE|FLO)
#define LKEY (VEXT|LHDR)

/*----------------------*/
/* биты флагов задержек */
/*----------------------*/
	/* нет перехода: */
#define SUSNL   0001    /* к следующему полю по <CR><LF>   */
/*#define SUSAR   0034    // по стрелкам                     */
/*#define SUSUD   0014    // вверх и вниз                    */
/*#define SUSLR   0060    // вправо и влево                  */
#define SUSU    0002    /* вверх                           */
#define SUSD    0004    /* вниз                            */
#define SUSL    0010    /* влево                           */
#define SUSR    0020    /* вправо                          */
	/* base of table signature */
#define SUST    0040    /* элемент таблицы: базу искать по флагу */
	/* oversize string object signature (no room in line-flag) */
#define U8SOBJ  0100    /* line->varl points to u8sobj_t */
#define WCSOBJ  0200    /* line->varl points to wcsobj_t */

/*-----------------------------------*/
/* стандартные однокнопочные команды */
/*-----------------------------------*/
/* arrow: up, down, left, right */
#define KB_AU     KBCOD('A','U')
#define KB_AD     KBCOD('A','D')
#define KB_AL     KBCOD('A','L')
#define KB_AR     KBCOD('A','R')
/* exit, help, keypad on/off, print, newline/Enter */
#define KB_EX     KBCOD('E','X')
#define KB_CA     KBCOD('C','A')
#define KB_HE     KBCOD('H','E')
#define KB_KP     KBCOD('K','P')
#define KB_PR     KBCOD('P','R')
#define KB_NL     KBCOD('N','L')
/* delete(backspace), refresh, tab */
#define KB_DE     KBCOD('D','E')
#define KB_RE     KBCOD('R','E')
#define KB_TA     KBCOD('T','A')
/* linlib 4 since 2017-05 */
/* insert, delete(forward), home, end, PageUp, PageDown */
#define KB_KI     KBCOD('K','I')
#define KB_KD     KBCOD('K','D')
#define KB_KH     KBCOD('K','H')
#define KB_KE     KBCOD('K','E')
#define KB_PU     KBCOD('P','U')
#define KB_PD     KBCOD('P','D')
#define KB_SP     KBCOD('S','P') /* never detected, used in help pages */
/*#define KB_EMPTY  KBCOD(' ' , ' ')*/
/*#define  KB_EMPTY  KBCTL( 0 )/*((kbcod)0)*/

/*----------------------*/
/* флаги функции io_set */
/*----------------------*/
#define IO_VIDEO  0000001       /* без эхопечати, вывод в кодах терминала */
#define IO_TTYPE  0000002       /* обычный телетайпный режим */
#define IO_WAIT   0000004       /* next_j() ожидает нажатие клавиши */
#define IO_NOWAIT 0000010       /* next_j() в непрерывном цикле */
#define IO_TTYSANE 000040		/* do not save old tty state which probably broken */

/*
 * hardware and logical data for cursor addressing
 */
#ifdef MAXLI_OBSOLETED
extern  int    maxli;   /* размер экрана по горизонтали */
extern  int    maxco;   /* размер по вертикали */
#endif
/*
 * cursor position frame, in logical coordinates of screen:
 * for whole screen = { 0, maxli, 0, maxco };
 * ofsli,ofsco values may be negative - means distance from (frame) border
 */
typedef struct {
	int baseli; /* offset to 1st line */
	int maxli; /* last line */
	int baseco; /* offset to 1st column */
	int maxco; /* last column */
} LFRAME;

extern LFRAME hwframe; /* hardware frame, initialized in hw_ini() */
extern LFRAME *lframe; /* logical current frame, by default is &hwframe */

/*
 * string object limited itself
 *
union wco {
	wchar_t s[];
	struct o {
		wchar_t wco_sig;
		wchar_t wco_size;
		wchar_t wcs[];
	};
};
 *
 * can be represented as wchar_t[], where [0] and [1] elements initialised before usage
 */

#define WCO_SIG ((wchar_t)-1)
/*#define WCO_SIG ((wchar_t)0xffffffff)*/
/*#define WCO_SIG ((wchar_t)0x8)*/
typedef struct {
	wchar_t wco_sig;    /* allways (wchar_t)-1 */
	wchar_t wco_size;   /* size of string container */
	wchar_t wcs[];      /* string container */
} wcsobj_t;
#define const_wcsobj(z,s) { WCO_SIG,z,s }

typedef unsigned char u8char_t;

/* DLE Data Link Escape - any which not used in 1st position of text */
/*#define U8O_SIG (0x10)*/
/*#define U8O_SIG (0x1)*/
#define U8O_SIG (1)

typedef struct {
	u8char_t u8o_sig;	/* allways (u8char_t)0xFF */
	u8char_t u8o_sizeh;	/* size of string container */
	u8char_t u8o_sizel;	/* size of string container */
	u8char_t u8s[];		/* string container */
} u8sobj_t;
#define const_u8sobj(z,s) { U8O_SIG,(z/256),(z%256),s }
extern u8sobj_t * u8o_init(u8sobj_t *, int);
/*extern int cvt_u8o();*/

extern int wco_size(wcsobj_t *); /*extern int wcsobj();*/
extern int u8o_size(const u8sobj_t *); /*extern int u8sobj();*/
/* convert string objects couple functions */
extern int u8owco(wcsobj_t *, u8sobj_t *);
extern int wcou8o(u8sobj_t *, wcsobj_t *);

extern int wcsu8s(u8char_t *, const wchar_t *); /*TODO WTF opposite parameters!!!*/
extern int wcsnu8s(const u8char_t *, const wchar_t *, int); /*TODO check above */

extern int vsize(wchar_t *);
extern int u8vsize(char *);
/* unicode utf-8 limited support */
extern  int mb_cur_max;
extern  int u8nopass;
extern  int u8slen(char *);
extern  int u8swcs(wchar_t *, char *);
extern  int u8snwcs(wchar_t *, const char *, int);
extern  int u8snu8s(char *, char *, size_t);
extern u8char_t *u8pxx(u8char_t *, wchar_t *);

extern 	int w_wchr(wchar_t c);
extern  int w_wcstrn(wchar_t *s, int n);
extern  int w_wcstrv(wchar_t *s, int n);
extern  int w_wcstr(wchar_t *s);


/*--------------------*/
/* функции библиотеки */
/*--------------------*/

/* lin(1) - интерфейс терминала с операционной системой */
extern  int     io_set(int);
extern  int	    ttyinp();
extern  int     w_chr(int);
extern  void     w_raw(const char *);
extern  void     w_str(const char *);
extern  int     w_strn(const char *, int);
extern  int     w_putc(int);
/*TODO: extend list by functions in fact used */


/* lin(2) - управление терминалом: физический уровень */
extern  void    at_set(int p);
extern  kbcod   r_cod(kbcod cod);
extern  kbcod   r_key();
extern  int     hw_set();
extern  int     do_kbl();
extern  void    visini();

/* lin(3) - управление терминалом: логический уровень */

extern  kbcod   k_pars(int);
extern  void    bell();
extern  void    cp_fet(), cp_sav();
extern  void    cp_set(int, int, int), cp_abset(int, int, int), cp_cret();
extern  void    er_eol(int), er_eop(int), er_pag(), er_scr(int, int, int);
extern  kbcod   r_line(LINE *, int *);
extern  kbcod   r_page(LINE *, LINE**, int *);
extern  void    w_line(LINE */*, int **/);
extern  void    w_page(LINE */*, kbcod*/);
extern  kbcod   e_str(wcsobj_t *, int vsize, kbcod (*)(), int *);
extern  kbcod   re_str(wcsobj_t *, int vsize, kbcod (*)(), int *);
extern  int     allcod, edinff;

/* lin(4) - функции уровня пользователя */
extern  int     cvt_a(LINE *, kbcod, char *, char *);
extern  int     cvt_s(LINE *, kbcod, char *, char*);
extern  int     cvt_sp(LINE *, kbcod, char *, char*);
extern  int     cvt_lh(LINE *, kbcod, char *, const char *);
extern  void    get_lh(const char *, const char *);
extern  void    w_lh_str(const char *);
extern  int     cvt_hl(LINE *, int, char *, char *);
extern  int     tst_m(LINE *, kbcod);

extern  void    w_lbl(int, const char *);
extern  void    w_msg(int, const char *);
extern  void    w_emsg(const char *);
extern  void    w_help(const LINE *);


extern void     u_page(LINE *, LINE*), d_page(LINE *);
extern FILE    *dafopen(char *, const char*, const char*);

#define er_page er_pag
#define r_str   e_str

/*
 * limit screen by linlib+termcap limits
 */
#define MAXLICO 512		/* 255 max ? */
#define STRBUF (MAXLICO + 2) /*82 // wcschar_t размер строки ввода */
#define U8_STRBUF (4*STRBUF)       /* UTF-8 размер строки */
#define STR_OVRSZ 2048	/* applicable for wcsobj_t */

/* internal function, called from hw_set only */
extern int	gtty_sz();
extern int tty_li, tty_co;

/* watch/clock and e_str indicator shares common place on the screen */
#define WSHOW_LI (lframe->maxli - 2)
#define WSHOW_CO (lframe->maxco - 9)
#define WSHOW_EDIT (WSHOW_CO    - 10)
#define WSHOW_ITEM (WSHOW_EDIT  - 8)
#define WSHOW_AT TXT|VEXT

#endif /* line_h_def */
