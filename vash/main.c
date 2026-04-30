/*
 * VASH - visual assistant for shell
 * Copyright (c) 1990-2025 Sergey Vovk <vsv>
 *
 * License: GPL and/or MIT,
 * see files COPYING and LICENSE
 */

#include <stdlib.h>
#include <unistd.h>
#include <stdlib.h>
#include <locale.h>
#include <wchar.h>
#include <signal.h>
#include <ctype.h>
#include <stdio.h>
#include "line.h"
#include "assist.h"

/*#define DEBUG*/
/*NOXSTR*/
char     tmpflss[] = "/tmp/ashXXXXXX";
char    *tmpflnm = &tmpflss[0];
int		tmpfd = -1;
FILE   *tmpfp = NULL;
/*YESXSTR*/
/*extern  char *mkstemp();*/

/*extern  char *getenv();*/

int     y0_top = 0;   /* begin of scroll area // Начало свитка на экране */

/*VASHFLAG nu_vashflag = { 1, 0, 1, 1, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 0, 0 } ;*/
int predump = 0;

/*char   *envshell;		/* env SHELL= */
/*char   *homedir;        /* домашний каталог */
/*char   *cwd;            /* текущий (рабочий) каталог */

/*VASHFLAG *vflag;*/
#if defined(PREDEF)
#define V_PREDEF PREDEF
#else
#define V_PREDEF 1
#endif
/*static const VASHFLAG vf = { 1, 0, 1, 1, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 0, 0 };*/
VASHFLAG vflag = {
		0, 0, 0, 0,
		0, 0, 0, 0,
		0, 0, 0, 0,
		0, 0, 0, 0,
		0, 0, V_PREDEF };

/*PARSARGS pargs[];*/

VASH_PROC v = {
/*		{ 1, 0, 1, 1, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 0, 0 },*/
/*		vf,*/
		0,
		0,
		0,
		0,
		NULL, 	    /* .argv0 */
		NULL,		/* $HOME */
		"/bin/sh",	/* $SHELL */
		VASH_PATH,	/* .vapath */
		VERSN,
		"std.rc",
		};

/*const VASHFLAG vf = (v.flag);*/
static int vdummy = 0; /* fake not used flag stub */

PARSARGS pa[] = {
	{ '1', &vdummy,         "1column",        "list main menu in 1 column" },
	{ 's', &vflag.scrolf,   "s,scroll",       "scrool vs clear screen" },			/* = 1; // флаг: продвигать рулон, а не гасить экран */
	{ 'o', &vflag.oneitm,   "o,one item",     "main menu allowed one item only" },			/* = 0; // флаг: разрешено указать только один пункт меню */
	{ 'c', &vflag.clockf,   "c,clock",        "show clock" },			/* = 1; // флаг: показывать часы */
	{ 'm', &vflag.cmailf,   "m,mail",         "notify about incoming mail" },			/* = 1; // флаг: проверять почту */
	{ 'w', &vflag.whodirf,  "w,title",        "show title panel" },			/* = 1; // show whodir panel on screen */
	{ 'x', &vflag.xtermf,   "x,X11 title",    "show title panel with window manager" },			/* = 0; // show whodir panel on window title using xterm escape sequence */
	{ 'p', &vflag.panelf,   "p,panel/key",    "show panel with 10 keys help" },			/* = 1; // флаг: показывать панель подсказки */
	{ 'H', &vflag.histf,    "H,history",      "save history cache on exit" },			/* = 0; // флаг: сохранять историю команд при выходе из vash, если histsn != 1 */
	{ 'S', &vflag.histsn,   "S,hist.sync",    "sync(save) history cache on every command immediately" },	/* = 0;	// флаг: синхронизировать историю после каждой команды */
	{ 'd', &vflag.histcd,   "d,hist.home",    "save/sync history cache in current directory" },			/* = 0; */
	{ 'T', &vflag.exittrap, "T,trap",         "trap \"[ ok ]\" on command reap" },			/* trap on exit of command: 0 - modern, 1 - vash canonical */
	{ 'N', &vflag.novice,   "N,trap msg",     "display help message on trap" },			/* = 1; novice prompter messages allowed */
	{ 'A', &vflag.shanyway, "A,sh -c",        "exec $SHELL -c 'command' anyway" },			/* = 1; shell -c 'cmd' in all cases anyway */
	{ 'j', &vflag.jobctl,   "j,job control",  "job control support" },			/* = 0; // флаг: главная оболочка, ppid() == 1 */
	{ 'J', &vflag.jobshow,  "J,job show",     "show job activity" },			/* = 0; // флаг: главная оболочка, ppid() == 1 */
	{ 'R', &vflag.subatrc,  "R,subs in rc",   "substite #@ before command editing" },			/* = 0; substitute '#@' from rc files before cmd editor */
	{ '@', &vflag.subshow,  "@,show #@",      "show mark '@' on item when command editing" },			/* = 0; substitute '#@' show position on main menu */
	{ ' ', &vflag.loginf,   " login sh",       "(readonly) vash is login shell" },			/* = 0; // флаг: главная оболочка, ppid() == 1 */
	{ ' ', &vflag.predef,   " rc predef",      "(readonly) rc predef (rc-style)" },			/* (readonly) rc style selector: 1 - BSD, 0 - other */
	{ 0 },
	};

static void usage(opt)
int opt;
{
	PARSARGS *p;
	fprintf(stderr, "\n"
		"Usage:\n"
		"  vash [-bN] [-lN] [[+-]"
		"["
		);
		for (p = &pa[0]; p->letter != 0; p++)
			if (p->sdescr[0] != ' ')
				fprintf(stderr, "%1.1s", p->sdescr/*p->letter*/);
		fprintf(stderr,
		"]"
		"] [profile] [-- command]\n"
/*		"or\n"*/
		"  vash -P [profile]\n"
		"  vash -h\n"
		);
	fprintf(stderr, "\n"
		"    -h  - get help for +/- options\n"
		"    -lN - get screen space for main menu: N lines, 0 if get maximum\n"
		"    -bN - get storage for main menu: N in KiB (64 by default)\n"
		"    -P - print profile than exit\n"
		);
	if (opt) {
		fprintf(stderr, "\n"
			"+/- options:\n");
		for (p = &pa[0]; p->letter != 0; p++)
			if (p->sdescr[0] != ' ')
				fprintf(stderr, "\t%1.1s : %s\n", p->sdescr, p->ldescr);
	}
	exit(1);
}

int cvt_fd(LINE *line,
		kbcod cod,
		char *mod,
		char *str)
{
	int size;
	int *flag;   /* &vflag.{flag} */
	PARSARGS *p;

	if (*mod == 'w') {
		size = line->size;
		if (line->attr & PMT) size -= 1;
		for (p = &pa[0]; p->letter != 0; p++) {
			flag = (int *)line->varl;
			if (flag == p->flag) {
				u8snu8s(str, p->sdescr, size);
				return(TRUE);
			}
		}
	}
	return(FALSE);
}

int     allcod = 1;

void
onexit(ok)
int ok;
{
#ifdef RETRO
	cp_set(-1, 0, TXT);
#else
	cp_set(clm._y0-1, 0, CMD);
	er_eop(0);
	cp_set(clm._y0-1, 0, CMD);
#endif
	io_set(VT_OFF);
#ifdef RETRO
	putchar('\n');
#endif
	if (ok == 0 && vflag.histf && v.home != (char *)0 && vflag.histsn == 0) {
		cmdhput();
	}

	unlink(tmpflnm);

/*	exit(ok);*/
}

/*ARGSUSED*/
void onintr(signo)
{
	onexit(1);
	printf("\n(%-d)bye\n", signo);
	fflush(stdout);
	exit(1);
}

#if 0 /*on_onintr()*/
/*
 * finish vash jobs handler
 */
static void on_onintr()
{
#if VASH_SIG_POSIX
	struct sigaction sa;
	sa.sa_handler = onintr;
	sa.sa_flags = 0;

/*	sigfillset(&sa.sa_mask);*/
	sigemptyset(&sa.sa_mask);

	sigaction(SIGHUP, &sa, NULL);
	sigaction(SIGINT, &sa, NULL);
	sigaction(SIGQUIT, &sa, NULL);
	sigaction(SIGTERM, &sa, NULL);
#else
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
#endif
}
#endif
#if 0
static void Signal(sig, hdlr)
int sig;
void *hdlr;
{
	struct sigaction sa;

/*	sigemptyset(sa.sa_mask);*/

	sigaction(sig, NULL, &sa);
	if (sa.sa_handler != hdlr) {
		sa.sa_flags = 0;
		sa.sa_handler = hdlr;
		sigaction(sig, &sa, NULL);
	}
}
#endif

/*disabling any linlib signal processing*/
void vsignal(ontty)
int ontty;
{
	fflush(stdout);
}

#if 0 /*vsignal()*/
/*
 * in linlib exists standard handler-dispatcher with the same name
 * there is a replacement for it, adding mode ontty=2
 *
 */
void vsignal(ontty)
int ontty;
{
	int debug = 0;
#if VASH_SIG_POSIX
	sigset_t sblock, snorm;
	sigemptyset(&sblock);
	sigaddset(&sblock, SIGTERM);
	sigaddset(&sblock, SIGTTOU);
	sigaddset(&sblock, SIGTTIN);
	sigaddset(&sblock, SIGTSTP);
/*	sigaddset(&sblock, SIGCHLD);*/

	if(ontty == 0)	{
		if (debug)
			printf("\n-VIDEO-\n");
		Signal(SIGHUP,  &onintr);
		Signal(SIGINT,  SIG_IGN);
		Signal(SIGQUIT, &onintr/*SIG_DFL*/);
		Signal(SIGCHLD,  &onchld);
/*		Signal(SIGTSTP,  SIG_IGN);
		Signal(SIGTTIN,  SIG_IGN);
		Signal(SIGTTOU,  SIG_IGN);*/
/*		sigaddset(&sblock, SIGCHLD);*/
		sigprocmask(SIG_BLOCK, &sblock, &snorm);
	} else if(ontty == 1) {
		if (debug)
			printf("\n-TTY-");
		Signal(SIGHUP,  SIG_DFL);
		Signal(SIGINT,  SIG_IGN);
		Signal(SIGQUIT, SIG_IGN);

		Signal(SIGTERM, SIG_IGN);
		Signal(SIGCHLD,  &onchld);
/*		Signal(SIGTSTP,  SIG_IGN);
		Signal(SIGTTIN,  SIG_IGN);
		Signal(SIGTTOU,  SIG_IGN);*/
		sigprocmask(SIG_BLOCK, &snorm, NULL);
	} else if(ontty == 2) {
		/* after fork() before exec() */
		if (debug)
			printf("\t\t\t-child-\r\n");
		Signal(SIGHUP,  SIG_DFL);
		Signal(SIGINT,  SIG_DFL);
		Signal(SIGQUIT, SIG_DFL);

		Signal(SIGTERM, SIG_DFL);
		Signal(SIGCHLD, &onchld);
/*		Signal(SIGTSTP, SIG_DFL);
		Signal(SIGTTIN,  SIG_IGN);
		Signal(SIGTTOU,  SIG_IGN);*/
		/*Don't forget a handler after */
	}
#else
	if(ontty == 0)	{
		if (debug)
			printf("\n--VIDEO--\n");
		signal(SIGHUP,  &onintr); /* TODO clean of children */
		signal(SIGINT,  SIG_IGN);
		signal(SIGQUIT, &onintr/*SIG_DFL*/);
		signal(SIGCHLD, &onchld);
		signal(SIGTSTP, &onchld);
	} else if(ontty == 1) {
		if (debug)
			printf("\n---TTY---");
		signal(SIGHUP,  SIG_DFL);
		signal(SIGINT,  SIG_IGN);
		signal(SIGQUIT, SIG_IGN);

		signal(SIGTERM, SIG_IGN);
		signal(SIGTTIN, SIG_IGN);
		signal(SIGTTOU, SIG_IGN);
		signal(SIGCHLD, &onchld);
		signal(SIGTSTP, &onchld);
	} else if(ontty == 2) {
		/* after fork() before exec() */
		if (debug)
			printf("\t\t\t--child--\r\n");
		signal(SIGHUP,  SIG_DFL);
		signal(SIGINT,  SIG_DFL);
		signal(SIGQUIT, SIG_DFL);

		signal(SIGTERM, SIG_DFL);
		signal(SIGCHLD, SIG_DFL);
		signal(SIGTTIN, SIG_DFL);
		signal(SIGTTOU, SIG_DFL);
		signal(SIGTSTP, SIG_DFL);
		/*Don't forget a handler after */
	}
#endif
	fflush(stdout);
}
#endif


LFRAME lfmain = { 0 };

/*ARGSUSED*/
void sigwinch(signo)
{
	if (0 != gtty_sz()) {
		return;
	}
	if (clm._yy_max == 0)
		clm._yy_max = hwframe.maxli;
	if (clm._yy_max < 2)
		clm._yy_max = 2;
	if (clm._yy_max > (lframe->maxli - 4))
		clm._yy_max = lframe->maxli - 4;
	lfmain.maxli  =  24; /* it is restriction for classic VDT hardware */
	/*lfmain.maxli  = hwframe.maxli;*/ /*TODO: separate frames... */
	lfmain.baseli = hwframe.maxli - lfmain.maxli;
	if (lfmain.baseli < 0) {
		lfmain.maxli = hwframe.maxli;
		lfmain.baseli = 0;
	}
	lfmain.maxco  = hwframe.maxco;

	lframe = &lfmain;

/*	rescan(NULL);*/

	if (signo)
		jkb_re();

}

static void on_sigwinch()
{
#if VISI_SIG_POSIX
	struct sigaction sa, osa;

	sigaction(SIGWINCH, NULL, &sa);
/*	sigemptyset(&sa.sa_mask);*/
	sa.sa_handler = sigwinch;
	sa.sa_flags = 0;

	sigaction(SIGWINCH, &sa, &osa);
#else
	/*fsig = */
	signal(SIGWINCH, sigwinch);
#endif
}

static  LINE tmplate =
/*NOSTRICT*/
       { 16, 0, 0, 0,
	       TXT|INP|NED|LFASTR,
		       (char *)0,
			       cvt_vf /*cvt_sp*/,
			       0 /*t_file*/,
				       (char*)0 };

char    *itms1[ITMMAX+1];       /* УКАЗАТЕЛИ НА ПУНКТЫ ГЛАВНОГО МЕНЮ */

/**/
cfill(argc, argv)
int argc;
char **argv;
{
	int i = 0;
	for (argc--, argv++; argc > 0; argc--, argv++, i++) {
		if (i == 0) {
			strcpy(Cfill, argv[0]);
		} else {
			strcat(Cfill, " ");
			strcat(Cfill, argv[0]);
		}
	}
}

static void vash_ini()
{
	/*
	 * passport constants
	 */
	v.ppid = getppid();
	v.pid = getpid();
	v.pgrp = getpgrp();
	v.sid = getsid(v.pid);
#if 0
	if (v.sid == v.pid)
		printf("session leader;\n");
	if (v.pgrp == v.pid)
		printf("process group leader;\n");
#endif
	if ((v.shell = getenv("SHELL")) == NULL /*(char *)0*/)
   		v.shell = "/bin/sh";
   	if ((v.home = getenv("HOME")) == NULL /*(char *)0*/)
   		vflag.histf = 0;
}

static int parsargs(mode, inp)
int mode;
char **inp;
{
	int i;
	char *s;
	PARSARGS *p;
	char tmps[4];
	char *arg;

	arg = *inp;
	if (*arg == '1') {
		clm._xx1 = 1;
		return(1);
	}
#if 1
	if (*arg == 'l') {
		/*** yy_max = 10; */
		s = tmps;
		arg++;
		while (isdigit(*arg))
			*s++ = *arg++;
		*s = '\0';
		clm._yy_max = atoi(tmps);
		*inp = --arg;
		return (1);
	}
#endif
	if (*arg == 'b') {
		s = tmps;
		arg++;
		while (isdigit(*arg))
			*s++ = *arg++;
		*s = '\0';
		clm._itmbsz = atoi(tmps) * 1024;
		if (clm._itmbsz <= 0) {
			fprintf(stderr, "-b detected %d\n", clm._itmbsz);
			usage(0);
		}
		*inp = --arg;
		return(1);
	}
	for (p = &pa[0]; p->letter != 0; p++) {
		if (*arg == p->letter) {
			*p->flag = mode;
			*inp = arg;
			return(1);
		}
	}
	return 0;
}

/* parse args: both precompiled and from environment */
void parsopt(s)
char *s;
{
	char linenoa[4];
	int cmode, c;
	char *envash;

	envash = s;
	cmode = 1;
	while ((c = *s)) {
		switch (c) {
		case '-':
			cmode = 0; break;
		case '+':
			cmode = 1; break;
		case ' ':
			break;
		default:
			if(parsargs(cmode, &s) == 0)
				printf("option parse error: '%c' in \"%s\"\n",
						c, envash);
			break;
		}
		s++;
	}
	/*fix yy_max*/
/*	if (clm._yy_max == 0)
		clm._yy_max = lfmain.maxli - 4;
/*	lfmain.maxli  =  24; /* it is restriction for classic VDT hardware */
	sigwinch(0);
}

main(argc, argv)
int argc;
char **argv;
{
	static
	const char *stdopts =  /* default compiled options */
			"+spwc HS J l8 b1";
	/*char *envsup;   /* environment VASH= options */
	char *envopts;
	int c;
	char *s;
	int cmode;
	char *args;

#define VASH_DEBUG
#ifdef VASH_DEBUG
	int ch;
	/*char *s;*/
	if ((s = getenv("VASH_DEBUG")) != NULL) {
		fprintf(stdout, "--> [%d] ready to debug, Please, press <Enter> to continue ", getpid());
		fscanf(stdin, "%c", &ch);
	}
#endif
	if (!setlocale(LC_CTYPE, "")) {
		fprintf(stderr, "Can't set the specified locale! "
			"Check LANG, LC_CTYPE, LC_ALL.\n");
		return 1;
	} else {
		mb_cur_max = MB_CUR_MAX;
	}

	envopts = NULL;

	vash_ini();

	v.argv0 = argv0 = *argv;

	/* setup extra directory for all working files library, vashrc will be found in that place */
	if ((s = getenv(VEXDIR)) != (char *)0)
		vexdir = s;
	if ((s = getenv(VAPATH)) != (char *)0)
		v.vapath = s;

#ifdef  VTTY
	vtty();
#endif
	visini();
	hw_set();

	parsopt(stdopts);
	/*
	 * environment setup parsing before command line args
	 * NOTE: starting with precompiled defaults
	 */
	envopts = getenv("VASH");
	if (envopts != NULL)
		parsopt(envopts);

	/*Cfill_o = */u8o_init((u8sobj_t *)Cfill_o, CFILL_MAX); /*malloc*/
	if (Cfill_o == NULL) {
		fprintf(stderr, "Can't get extra memory");
		exit (1);
	} else
		Cfill = ((u8sobj_t *)Cfill_o)->u8s;

   	for (argc--, argv++; argc > 0; argc--, argv++) {
   		c = *argv[0];
		if (c == '-' || c == '+') {
			cmode = ((c == '+') ? 1 : 0);
			for (args = &argv[0][1]; *args != '\0'; args++) {
				/*c = *p;*/
				switch (*args) {
				case '-':	/*second one*/
					/*				cfill(argc, argv); */
					if ((args[1]) == '\0')
						/* tail of agruments is fill command
						 * replaced one from vashrc */
						goto args_done;
					break;
				case 'h': usage(1);
					break;
				/*case '1':
					clm._xx1 = 1;
					break;*/
				case 'P':
					predump = 1;
					break;
				default:
					if(parsargs(cmode, &args) == 0) {
						fprintf(stderr,
								"\nerror: unknown option '%c' in \"%s\"\n",
								*args, argv[0]);
						usage(0);
					}
					break;
				}
			}
		} else {
			/* имя файла для интерпретации cmdset() */
			v.rc = *argv;
		}
	}
args_done:
#ifdef DEBUG
	printf("args_done; Cfill=\"%s\" vashrc=\"%s\"\r\n", Cfill, vashrc);
#endif

    if (v.home != NULL /*(char *)0*/) {
		cmdhget(1);
	}

    /*TODO: убрать все про tmpflnm*/
    /*  tmpflnm = "/tmp/ash.tmp";        /* получить имя временного файла */
    tmpfd = mkstemp(tmpflnm);      /* получить имя временного файла */
    if (tmpfd >= 0) {
    	close(tmpfd);
    	unlink(tmpflnm);
    }

	/* main menu init. */
	clm._itms   = &itms1[0];
	clm._ltmpl  = &tmplate;
	 /* get this from: stdopts, envopts */
	/*clm._itmbsz = ITMBUF;
	clm._yy_max = 10;*/

	sigwinch(0); /* get initial sizes of screen */

#ifdef DEBUG
	printf("   Cfill=\"%s\" vashrc=\"%s\"\r\n", Cfill, vashrc);
#endif
/*NOXSTR*/
	if ( ! cmdset(v.rc) ) {
		/* fatal error detected */
		/* scrlnl(); */
		printf("\n");
		exit(123);
	} else {

/*		pars1(rcopts);*/ /* call it from cmdset */

		if (predump)
			exit(0);

		/*vsignal(1);*/
		io_set(IO_SAVE | VT_OFF);
		io_set(VT_ON);

		cfill(argc, argv); /* tail of agruments is fill command replaced one from vashrc */
		/*signal(SIGINT, onintr);*/

#ifdef DEBUG
		printf("cmdset; Cfill=\"%s\" vashrc=\"%s\"\r\n", Cfill, vashrc);
#endif
		if ( fil_vf(1) ) {
			/* initial setup of scroll area occupied by vash */
			/* y0_top = lframe->maxli - clm._yy_max;/* - 1;*/

			/*y0_top = 0;*/
			y0_top = clm._y0;/* - 1;*/
			scrlnl();
			/*y0_top = clm._y0;*/

			on_sigwinch();

			cp_cret();
			on_onchld();

			blk_on();

			u_menu(clm._vf);

			onexit(0);

			io_set(VT_OFF);
			/*vsignal(0);*/

			exit(0);
		}
	}
}
