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
char   *cwd;            /* текущий (рабочий) каталог */

const char *pmtsh;

VASHFLAG *vflag;
VASH_PROC v = {
		{ 1, 0, 1, 1, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 0, 0 },
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

usage()
{
	fprintf(stderr, "Usage: vash [-1] [-bN] [-c] [-h] [-S] [-s] [-w] [-x] [-m] [-p]\n");
	fprintf(stderr, "Usage: vash [any flags] -- command\n");
	exit(1);
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
	if (ok == 0 && v.flag.histf && v.home != (char *)0 && v.flag.histsn == 0) {
		cmdphist();
	}

	unlink(tmpflnm);


/*	exit(ok);*/
}

/*ARGSUSED*/
int onintr(signo)
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
	extern int rescan();

	if (0 != gtty_sz()) {
		return;
	}
	lfmain.maxli  =  24; /* it is restriction for classic 24 lines */
	lfmain.baseli = hwframe.maxli - lfmain.maxli;
	if (lfmain.baseli < 0) {
		lfmain.maxli = hwframe.maxli;
		lfmain.baseli = 0;
	}
	lfmain.maxco  = hwframe.maxco;

	lframe = &lfmain;

	rescan(NULL);

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
	if (v.sid == v.pid)
		printf("session leader;\n");
	if (v.pgrp == v.pid)
		printf("process group leader;\n");
	if ((v.shell = getenv("SHELL")) == NULL /*(char *)0*/)
   		v.shell = "/bin/sh";
   	if ((v.home = getenv("HOME")) == NULL /*(char *)0*/)
   		v.flag.histf = 0;
}

main(argc, argv)
int argc;
char **argv;
{
	char *envsup;   /* строка флагов из окружения */
	int c;
	char *s;

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

	vash_ini();

	v.argv0 = argv0 = *argv;

	/* setup extra directory for all working files library, vashrc will be found in that place */
	if ((s = getenv(VEXDIR)) != (char *)0)
		vexdir = s;
	if ((s = getenv(VAPATH)) != (char *)0)
		v.vapath = s;

	/* main menu init. */
	clm._itms   = itms1;
	clm._ltmpl  = &tmplate;
	clm._itmbsz = ITMBUF;
	clm._yy_max = 10;

#ifdef  VTTY
	vtty();
#endif
	visini();
	hw_set();

	sigwinch(0); /* get initial sizes of screen */

	if (getuid() == 0) {
		pmtsh = " # ";
	} else {
		pmtsh = " $ ";
	}

	/*
	 * environment setup parsed before command line args
	 */
	v.flag.exittrap = 0;
	if ((envsup = getenv("VASH")) != (char *) 0) {
		char linenoa[4];
		char *p;

		/* reset defaults in case environment setup is in use */
		v.flag.scrolf =
				v.flag.histf = v.flag.histsn = v.flag.panelf =
						v.flag.whodirf = v.flag.xtermf = v.flag.clockf =
								v.flag.cmailf = v.flag.exittrap = v.flag.novice =
										v.flag.subatrc = v.flag.subshow = 0;
		if(strcmp("BSD", VASHLIB)==0)
			v.flag.predef = 1;
#if 1
		else
			v.flag.predef = 0;
#endif
		/*** yy_max = 10; */

		while ((c = *envsup++)) {
			switch (c) {
			case 'l':
				p = linenoa;
				while (isdigit(*envsup)) {
					*p++ = *envsup++;
				}
				*p = '\0';
				clm._yy_max = atoi(linenoa);
				if (clm._yy_max < 2)
					clm._yy_max = 2;
				if (clm._yy_max > (lframe->maxli - 4))
					clm._yy_max = lframe->maxli - 4;
				break;
			case 'p':
				v.flag.panelf++;
				break;
			case 'x':
				v.flag.xtermf++;
				break;
			case 'w':
				v.flag.whodirf++;
				break;
			case 's':
				v.flag.scrolf++;
				break;
			case 'h':
				v.flag.histf++;
				break;
			case 'S':
				v.flag.histsn++;
				break;
			case 'c':
				v.flag.clockf++;
				break;
			case 'm':
				v.flag.cmailf++;
				break;
			case 'T':
				v.flag.exittrap++;
				break;
			case 'N':
				v.flag.novice++;
				break;
			case 'A':
				v.flag.shanyway++;
				break;
			}
		}
	}

	/*Cfill_o = */u8o_init((u8sobj_t *)Cfill_o, CFILL_MAX); /*malloc*/
	if (Cfill_o == NULL) {
		fprintf(stderr, "Can't get extra memory");
		exit (1);
	} else
		Cfill = ((u8sobj_t *)Cfill_o)->u8s;

   	for (argc--, argv++; argc > 0; argc--, argv++) {
		if (*argv[0] == '-') {
			switch (argv[0][1]) {
			default:
				usage();
				break;
			case 'b':
				clm._itmbsz = atoi(&argv[0][2]) * 1024;
				if (clm._itmbsz <= 0) {
					fprintf(stderr, "-b flag bad usage...");
					usage();
				}
				continue;
			case '1':
				clm._xx1 = 1;
				continue;
			case 'p':
				v.flag.panelf = 0;
				continue;
			case 'x':
				v.flag.xtermf = 0;
				continue;
			case 'w':
				v.flag.whodirf = 0;
				continue;
			case 's':
				v.flag.scrolf = 0;
				continue;
			case 'h':
				v.flag.histf = 0;
				continue;
			case 'S':
				v.flag.histsn = 0;
				continue;
			case 'c':
				v.flag.clockf = 0;
				continue;
			case 'm':
				v.flag.cmailf = 0;
				continue;
			case 'T':
				v.flag.exittrap = 0;
				continue;
			case 'A':
				v.flag.shanyway = 1;
				continue;
			case 'P':
				predump = 1;
				continue;

			case '-':
/*				cfill(argc, argv);  tail of agruments is fill command replaced one from vashrc */
				goto args_done;
				break;
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
		cmdghist(v.home);
	}

    /*  tmpflnm = "/tmp/ash.tmp";        /* получить имя временного файла */
    tmpfd = mkstemp(tmpflnm);      /* получить имя временного файла */
    if (tmpfd >= 0) {
    	close(tmpfd);
    	unlink(tmpflnm);
    }

#ifdef DEBUG
	printf("   Cfill=\"%s\" vashrc=\"%s\"\r\n", Cfill, vashrc);
#endif
/*NOXSTR*/
	if ( cmdset(v.rc) ) {

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
	} else {
		/* ошибки (плохо установлен ash, не читается реперный файл */
		/* scrlnl(); */
		printf("\n");
		exit(1);
	}
}
