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
#define ITMCNM 255
#else
	/* normal 64/32-bit commputers */
#define ITMMAX  120000     /* Макс. количество пунктов меню */
#define ITMLEN  MAXLICO /*16      // Длина строки пункта меню */
#define ITMBUF  MAXLICO * ITMMAX /* 16384   // Размер буфера текста меню по умолчанию */
#define ITMCNM 1024
#endif

#define CFILL_MAX STR_OVRSZ

/*#define BEGLI 11        // ПЕРВАЯ СТРОКА МЕНЮ НА ЭКРАНЕ (???) */

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
LINE	*_line;         /* current line pointed by cursor */
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
int		scrolf;			/* = 1; // флаг: продвигать рулон, а не гасить экран */
int		oneitm;			/* = 0; // флаг: разрешено указать только один пункт меню */
int		panelf; 		/* = 1; // флаг: показывать панель подсказки */
int		whodirf;		/* = 1; // show whodir panel on screen */
int		xtermf;			/* = 0; // show whodir panel on window title using xterm escape sequence */
int		histf;			/* = 0; // флаг: сохранять историю команд при выходе из vash, если histsn != 1 */
int		histsn;			/* = 0;	// флаг: синхронизировать историю после каждой команды */
int		clockf;			/* = 1; // флаг: показывать часы */
int		cmailf;			/* = 1; // флаг: проверять почту */
int		loginf;			/* = 0; // флаг: главная оболочка, ppid() == 1 */
int		exittrap;		/* trap on exit of command: 0 - modern, 1 - vash canonical */
int		novice;			/* = 1; novice prompter messages allowed */
int		shanyway;		/* = 1; shell -c 'cmd' in all cases anyway */
int		subatrc;		/* = 0; substitute '#@' from rc files */
int		subshow;		/* = 0; substitute '#@' show position on main menu */
int     predef;  		/* (readonly) rc style selector: 1 - BSD, 0 - other */
} VASHFLAG;
extern int	predump;	/* print on stdout profile after preprocessing then exit */

extern VASHFLAG vashflag; /* defined in main.c */

extern char   *envshell;
extern char   *homedir;
extern const char   *vexdir; /* vash extra files directory */
extern const char   *vapath; /* vash library search path */
extern const char   *vashrc; /* vash runtime cmdset (rc, profile) */
extern char   *cwd;
extern int     t_file();

extern int     /*maxli, maxco,*/ y0_top;
extern LFRAME  lfmain;
extern int vashelp(LINE *);
extern int rchelp(char *);
extern void scrldo();
extern void scrlarea();

extern void cmdhreset();
extern int cmdphist();

/*extern int visini();*/
extern int cmdset(const char *);

extern int fil_vf(int);

extern void scrlst();
extern void scrlnl();
extern int cmdsub(char *, const char *, int, int, int);
extern void fatal();
extern void clritm();

extern void itmshow();
extern int cmdghist();

extern void u_menu(LINE */*, char **/);

extern int itmadj(kbcod);
extern void itmini();
/*extern int itmfnd();*/
extern int itmpos(char *);
extern int itmsel(int);
extern void pre_vf();
extern int patcmp(const char *, char *);
extern int cntsel();

extern int vincmd(char *);
extern int vexcmd(char *, char *);
extern int fnsplit(char *, char *, int);
extern int vcmd(kbcod);
extern void w_cmd(wchar_t *);
extern void cwdshow();

extern int ok_msg();

extern int gidchk(int);
extern int filetype(char *);
extern int prefix(char *, char *); /*TODO WTF*/

extern int try_compl(wchar_t *, int *, int);
extern int hlp_compl();
extern void hlp_clr();
extern int cmdput(u8char_t *);
extern int vsystem(char *, char *);
extern void showtime(int);

extern int cvt_vf(LINE *, kbcod, char *, char *);
extern int cvt_s(LINE *, kbcod, char *, char *);

/*extern  u8sobj_t *Cfill_o;*/
extern  char Cfill_o[/*STR_OVRSZ+4*/];
extern  char *Cfill;
/*extern  char *Cfill;*/
extern  char Crepf[];
extern  char Coutf[];
extern  char Csubs[];

kbcod pmtrstr(const char *, char *, int);
kbcod pmtrobj(char *, u8sobj_t *, int);

extern void vfread(FILE *);
extern void showitem(int);
extern int  cmdvew(wchar_t *);
extern int  cmdprv(wchar_t *);
extern int  cmdnxt(wchar_t *);
extern void onexit(int);
extern int  vchdir(char *);
extern int  sh_wcesc(wchar_t *, wchar_t *, int);
extern int  sh_esc(char *, char *);

#endif
/* assist_h_def */
