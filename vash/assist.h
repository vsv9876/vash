
#include "line.h"

#ifndef assist_h_def
#define assist_h_def
#define VEXDIR "VASH_EXDIR"
#define VPATH  "VASH_PATH"

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

extern  char   *yesno[];
extern  int     scrolf, oneitm, panelf, whodirf, xtermf, histf, histsn, clockf, cmailf, loginf;
extern  char   *envshell;
extern  char   *homedir;
extern  char   *vexdir; /* vash extra files directory */
extern  char   *vpath; /* vash library search path */
extern  char   *cwd;
extern  int     t_file();

extern  int     maxli, maxco, y0_top;

extern int cmdphist(),visini(), cmdset(), fil_vf(), scrlst(), scrlnl(), cmdsub(), fatal(), clritm();
extern int itmshow(), cmdghist(), u_menu(), itmadj(), itmini(), pre_vf(), patcmp();
extern int vincmd(), vexcmd(), fnsplit(), vcmd(), cwdshow();
extern int patcmp(), ok_msg();
extern int gidchk(), filetype(), prefix();
extern int try_compl(), hlp_compl(), hlp_clr(), cmdput(), vsystem(), showtime();
extern int sl_chkdup();

extern int cvt_vf(), cvt_s();

#endif /* assist_h_def */
