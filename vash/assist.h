/*
 * visual assistant shell
 */
#include "line.h"

#ifndef assist_h_def
#define assist_h_def
#define VEXDIR "VASH_EXDIR"
#define VAPATH "VASH_PATH"

/* Основные константы внутренних таблиц меню */

#ifdef TINYSMALL
	/* pdp11 & xenix286 small model */
#define ITMMAX  400     /* Макс. количество пунктов меню */
#define ITMLEN  16      /* Длина строки пункта меню */
#define ITMBUF  2048    /* Размер буфера текста меню по умолчанию */

#else
	/* normal 64/32-bit commputers */
#define ITMMAX  120000     /* Макс. количество пунктов меню */
#define ITMLEN  MAXLICO /*16      /* Длина строки пункта меню */
#define ITMBUF  MAXLICO * ITMMAX /* 16384   /* Размер буфера текста меню по умолчанию */

#endif
/*#define BEGLI 11        /* ПЕРВАЯ СТРОКА МЕНЮ НА ЭКРАНЕ (???) */

#define MONEY '#'       /* знак для подстановок в командах */

/*extern  char *malloc();*/
/*extern  char *calloc();*/
extern  char *getenv();

/*
 * СТРУКТУРА ДАННЫХ МЕНЮ ИМЕН ФАЙЛОВ И ДР. МЕНЮ.
 *
 */
typedef struct {

char   **_itms;         /* АДРЕС ТАБЛИЦЫ УКАЗАТЕЛЕЙ НА ПУНКТЫ МЕНЮ */
char    *_itmbuf;       /* БУФЕР СТРОК ПУНКТОВ МЕНЮ */
int      _itmbsz;       /* РАЗМЕР БУФЕРА СТРОК */

LINE    *_vf;            /* УКАЗАТЕЛЬ НА СТРАНИЦУ МЕНЮ */
LINE    *_ltmpl;         /* шаблон для построения страницы */
int     _xx1;            /* начальное количество столбцов (xx1 >= 1) */

int     _itm;            /* индекс указанного курсором пункта меню */
int     _itmofs;         /* смещение окна (индекс) */
/* следующие два параметра определяются при чтении списка пунктов меню */
int     _itmlen;         /* макс. длина строки пункта меню */
int     _itmmax;         /* количество пунктов меню */

int     _yy_max;         /* максимально разрешенное количество строк меню */
int     _yy;    /* количество пунктов меню в столбце */
int     _xx;    /* количество столбцов */
int     _dx;    /* расстояние между первыми позициями соседних столбцов */
int     _x0;             /* позиция первого столбца */
int     _y0;             /* позиция первой строки */
		/* смещение при переходе через кадр: */
int     _ofsx;           /* влево/вправо */
int     _ofsy;           /* вверх/вниз */
} LINEMENU;

extern LINEMENU clm;    /* ТЕКУЩЕЕ МЕНЮ (current lines menu) */

/*#define itms        clm._itms*/
/*#define itmbuf      clm._itmbuf*/
/*#define itmbsz      clm._itmbsz*/
/*#define vf          clm._vf*/
/*#define ltmpl       clm._ltmpl*/
/*#define xx1         clm._xx1*/
/*#define itm         clm._itm*/
/*#define itmofs      clm._itmofs*/
/*#define itmlen      clm._itmlen*/
/*#define itmmax      clm._itmmax*/
/*#define yy_max      clm._yy_max*/
/*#define yy          clm._yy*/
/*#define xx          clm._xx*/
/*#define dx          clm._dx*/
/*#define x0          clm._x0*/
/*#define y0          clm._y0*/
/*#define ofsx        clm._ofsx*/
/*#define ofsy        clm._ofsy*/

/*extern  char   *yesno[];*/

/*extern int     scrolf;*/
/*extern int     oneitm;*/
/*extern int     panelf;*/
/*extern int     whodirf;*/
/*extern int     xtermf;*/
/*extern int     histf;*/
/*extern int     histsn;*/
/*extern int     clockf;*/
/*extern int     cmailf;*/
/*extern int     loginf;*/

/* flags */
typedef struct {
int		scrolf;			/* = 1; /* флаг: продвигать рулон, а не гасить экран */
int		oneitm;			/* = 0; /* флаг: разрешено указать только один пункт меню */
int		panelf; 		/* = 1; /* флаг: показывать панель подсказки */
int		whodirf;		/* = 1; /* show whodir panel on screen */
int		xtermf;			/* = 0; /* show whodir panel on window title using xterm escape sequence */
int		histf;			/* = 0; /* флаг: сохранять историю команд при выходе из vash, если histsn != 1 */
int		histsn;			/* = 0;	/* флаг: синхронизировать историю после каждой команды */
int		clockf;			/* = 1; /* флаг: показывать часы */
int		cmailf;			/* = 1; /* флаг: проверять почту */
int		loginf;			/* = 0; /* флаг: главная оболочка, ppid() == 1 */
int		exittrap;		/* trap on exit of command: 0 - modern, 1 - vash canonical */
} VASHFLAG;

extern VASHFLAG vashflag; /* defined in main.c */

extern char   *envshell;
extern char   *homedir;
extern char   *vexdir; /* vash extra files directory */
extern char   *vapath; /* vash library search path */
extern char   *vashrc; /* vash runtime cmdset (rc, profile) */
extern char   *cwd;
extern int     t_file();

extern int     /*maxli, maxco,*/ y0_top;
extern LFRAME  lfmain;
extern int    vashelp(), scrldo(), scrlarea();

extern int cmdhreset();
extern int cmdphist(),visini(), cmdset(), fil_vf(), scrlst(), scrlnl(), cmdsub(), fatal(), clritm();
extern int itmshow(), cmdghist(), u_menu(), itmadj(), itmini(), pre_vf(), patcmp();
extern int vincmd(), vexcmd(), fnsplit(), vcmd(), cwdshow();
extern int patcmp(), ok_msg();
extern int gidchk(), filetype(), prefix();
extern int try_compl(), hlp_compl(), hlp_clr(), cmdput(), vsystem(), showtime();
extern int sl_find();

extern int cvt_vf(), cvt_s();

extern  char *Cfill;
extern  char Crepf[];
extern  char Coutf[];
extern  char Csubs[];


#endif /* assist_h_def */
