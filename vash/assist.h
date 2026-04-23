/*
 * VASH - visual assistant for shell
 * Copyright (c) 1990-2025 Sergey Vovk <vsv>
 *
 * License: GPL and/or MIT,
 * see files COPYING and LICENSE
 */

#define VASH_JOBS_HACK 1
#define VASH_SIG_POSIX 1

#include "sys/types.h"
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
	/* modern 64/32-bit commputers */
#define ITMMAX  120000     /* Макс. количество пунктов меню */
#define ITMLEN  MAXLICO /*16      // Длина строки пункта меню */
#define ITMBUF  MAXLICO * ITMMAX /* 16384   *//* Размер буфера для главного меню */
				/*NOTE: ITMBUF is controlled via options -b since vash-2.1.0 */
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

/* flags, see description in main.c -- vflag, pa[] */
typedef struct {
	int	scrolf;		/* = 1; // флаг: продвигать рулон, а не гасить экран */
	int	oneitm;		/* = 0; // флаг: разрешено указать только один пункт меню */
	int	panelf; 	/* = 1; // флаг: показывать панель подсказки */
	int	whodirf;	/* = 1; // show whodir panel on screen */

	int	xtermf;		/* = 0; // show whodir panel on window title using xterm escape sequence */
	int	histf;		/* = 0; // флаг: сохранять историю команд при выходе из vash, если histsn != 1 */
	int	histsn;		/* = 0;	// флаг: синхронизировать историю после каждой команды */
	int	clockf;		/* = 1; // флаг: показывать часы */

	int	cmailf;		/* = 1; // флаг: проверять почту */
	int	loginf;		/* = 0; // флаг: главная оболочка, ppid() == 1 */
	int	jobctl;		/* = 1; // JOB Control support */
	int	jobshow;	/* = 1; // Job index show*/

	int	exittrap;	/* trap on exit of command: 0 - modern, 1 - vash canonical */
	int	novice;		/* = 1; novice prompter messages allowed */
	int	shanyway;	/* = 1; shell -c 'cmd' in all cases anyway */
	int	subatrc;	/* = 0; substitute '#@' from rc files before cmd editor */

	int	subshow;	/* = 0; substitute '#@' show position on main menu */
	int predef;  	/* (readonly) rc style selector: 1 - BSD, 0 - other */
} VASHFLAG;

extern int	predump;	/* print on stdout preprocessed profile then exit */
#define RCDUMP 1

/*extern VASHFLAG nu_vashflag; /* defined in main.c */

typedef struct {
	pid_t       ppid;
	pid_t       pid;
	pid_t		pgrp;
	pid_t		sid;
	const char *argv0;
	const char *home;
	const char *shell;
	const char *vapath; /* vash library search path */
	const char *versn;  /* version */
	const char *rc;		/* profile like std.rc */
/*
	int			jobctl;  able job control in vash
	int			ok_sus;  able suspend
*/
} VASH_PROC;

extern VASH_PROC v;
extern VASHFLAG vflag;

typedef struct {
	int letter;
	int  *flag;
	const char *sdescr;
	const char *ldescr;
} PARSARGS;

extern char rcopts[];  /* options from profile */
extern void parsopt(char *);

/*extern char   *envshell;*/
/*extern char   *homedir;*/
extern const char   *vexdir; /* visi/linlib extra files directory */
/*extern const char   *vapath; /* vash library search path */
/*extern const char   *vashrc; /* vash runtime cmdset (rc, profile) */
/*extern char   *cwd;*/
extern char   cwdpath[];
extern int     t_file();

extern int     /*maxli, maxco,*/ y0_top;
extern LFRAME  lfmain;

/*extern int visini();*/
extern int cmdset (const char *);
extern int sup    (const char *);
extern int v_susp (const char *);
extern int ffile  (const char *);
/*extern int w_help (const char *);*/
extern int vashelp(const char *); /*cast of arg to (LINE *) inside function*/
extern int rchelp (const char *);
extern int fmsg   (const char *);
extern int fmsgerr(const char *);
extern int rescan (const char *);
extern int f_ls   (const char *);
extern int f_mark (const char *);
/*extern int itmfnd();*/
extern int itmpos(const char *);
extern int kshow  (const char *);

extern void scrldo();
extern void scrlarea();

extern void cmdhreset();
extern int cmdphist();

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
extern void shprolog(void);
extern void tty_cmd(wchar_t *, u8char_t *);
/*extern void vt_off();*/
/*extern void vt_bak();*/
extern int vreap(int *, int);
extern void onchld(int);
extern void showtime(int);
extern int vin_chk(wchar_t *);
extern int vin_do(wchar_t *, char *);

extern int cvt_vf(LINE *, kbcod, char *, char *);
extern int cvt_s(LINE *, kbcod, char *, char *);
extern int cvt_fd(LINE *, kbcod, char *, char *);

extern int Tpgrp(pid_t, const char *, const char *);

extern void onintr(int);

/*extern  u8sobj_t *Cfill_o;*/
extern  char Cfill_o[/*STR_OVRSZ+4*/];
extern  char *Cfill;
/*extern  char *Cfill;*/
extern  char Crepf[];
extern  char Coutf[];
extern  char Csubs[];

extern const char *pmtsh;

kbcod pmtrstr(const char *, char *, int);
kbcod pmtrobj(char *, u8sobj_t *, int);

extern void vfread(FILE *);
extern void showitem(int);
extern int  cmdvew(wchar_t *);
extern int  cmdprv(wchar_t *);
extern int  cmdnxt(wchar_t *);
extern void onexit(int);
extern int  vchdir(const char *);
extern int  sh_wcesc(wchar_t *, wchar_t *, int);
extern int  sh_esc(char *, char *);

extern void blk_on(void);
extern void blk_off(void);
extern void blk_new(void);
extern void blk_sigchld(int);
extern void conduit(char *);

extern void on_onchld(void);    /*set handler*/
extern void Signal(int, void *);

#define ASH_NOWAIT 001  /* do not wait process just invoked(BG) */
#define ASH_NOFORK 010  /* заменить программу процесса */
#define ASH_NOSH   020  /* не запускать дополнительный процесс sh */

/* job indicator */
#ifdef WSHOW_EDIT
#define WSHOW_ITEM (WSHOW_EDIT  - 8)
#define WSHOW_JOBS (WSHOW_ITEM - 8)
#endif
extern char *jobshow();

#endif
/* assist_h_def */
