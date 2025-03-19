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

VASHFLAG vashflag = { 1, 0, 1, 1, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 0, 0 } ;
int predump = 0;

char   *envshell;		/* env SHELL= */
char   *homedir;        /* домашний каталог */
char   *cwd;            /* текущий (рабочий) каталог */

const char *pmtsh;

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
	io_set(IO_TTYPE);
#ifdef RETRO
	putchar('\n');
#endif
	if (ok == 0 && vashflag.histf && homedir != (char *)0 && vashflag.histsn == 0) {
		cmdphist();
	}

	unlink(tmpflnm);


/*	exit(ok);*/
}

/*ARGSUSED*/
void onintr(signo)
int signo;
{
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	onexit(1); exit(1);
}

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

	rescan();

	if (signo)
		jkb_re();

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


       /* инициализация главного меню */
       clm._itms   = itms1;
       clm._ltmpl  = &tmplate;
       clm._itmbsz = ITMBUF;
       clm._yy_max = 10;

       vashrc = "std.rc";

       /* setup extra directory for all working files library, vashrc will be found in that place */
       if ((s = getenv(VEXDIR)) != (char *)0) vexdir = s;
       if ((s = getenv(VAPATH)) != (char *)0) vapath = s;

#ifdef  VTTY
	vtty();
#endif
	visini();
	hw_set();

	sigwinch(0);

	if (getuid() == 0) {
		pmtsh = " # ";
	} else {
		pmtsh = " $ ";
	}

	/*
	 * environment setup parsed before command line args
	 */
	vashflag.exittrap = 0;
	if ((envsup = getenv("VASH")) != (char *) 0) {
		char linenoa[4];
		char *p;

		/* reset defaults in case environment setup is in use */
		vashflag.scrolf =
				vashflag.histf = vashflag.histsn = vashflag.panelf =
						vashflag.whodirf = vashflag.xtermf = vashflag.clockf =
								vashflag.cmailf = vashflag.exittrap = vashflag.novice =
										vashflag.subatrc = vashflag.subshow = 0;
		if(strcmp("BSD", VASHLIB)==0)
			vashflag.predef = 1;
#if 1
		else
			vashflag.predef = 0;
#endif
		/*** yy_max = 10; */

		while (c = *envsup++) {
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
				vashflag.panelf++;
				break;
			case 'x':
				vashflag.xtermf++;
				break;
			case 'w':
				vashflag.whodirf++;
				break;
			case 's':
				vashflag.scrolf++;
				break;
			case 'h':
				vashflag.histf++;
				break;
			case 'S':
				vashflag.histsn++;
				break;
			case 'c':
				vashflag.clockf++;
				break;
			case 'm':
				vashflag.cmailf++;
				break;
			case 'T':
				vashflag.exittrap++;
				break;
			case 'N':
				vashflag.novice++;
				break;
			case 'A':
				vashflag.shanyway++;
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
				vashflag.panelf = 0;
				continue;
			case 'x':
				vashflag.xtermf = 0;
				continue;
			case 'w':
				vashflag.whodirf = 0;
				continue;
			case 's':
				vashflag.scrolf = 0;
				continue;
			case 'h':
				vashflag.histf = 0;
				continue;
			case 'S':
				vashflag.histsn = 0;
				continue;
			case 'c':
				vashflag.clockf = 0;
				continue;
			case 'm':
				vashflag.cmailf = 0;
				continue;
			case 'T':
				vashflag.exittrap = 0;
				continue;
			case 'A':
				vashflag.shanyway = 1;
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
			vashrc = *argv;
		}
	}
args_done:
#ifdef DEBUG
	printf("args_done; Cfill=\"%s\" vashrc=\"%s\"\r\n", Cfill, vashrc);
#endif

   	if ((envshell=getenv("SHELL")) == (char *)0) {
   		envshell = "/bin/sh";
   	}

   	if ((homedir=getenv("HOME")) == (char *)0) {
   		vashflag.histf = 0;
   	}

    if (homedir != (char *)0) {
		cmdghist(homedir);
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
	if ( cmdset(vashrc) ) {

		if (predump)
			exit(0);
		io_set(IO_VIDEO);

		cfill(argc, argv); /* tail of agruments is fill command replaced one from vashrc */
		signal(SIGINT, onintr);

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

			signal( SIGINT, SIG_IGN );
			signal( SIGQUIT, SIG_IGN );
			signal(SIGWINCH, sigwinch);

			u_menu(clm._vf);

			onexit(0);

			io_set(IO_TTYPE);

			exit(0);
		}
	} else {
		/* ошибки (плохо установлен ash, не читается реперный файл */
		/* scrlnl(); */
		printf("\n");
		exit(1);
	}
}
