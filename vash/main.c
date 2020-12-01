#include <stdlib.h>
#include <unistd.h>
/*#include <stdlib.h>*/
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

extern  char *getenv();

int     y0_top = 0;   /* Начало свитка на экране */

/*
 * flags:
 */
int     scrolf = 1;     /* флаг: продвигать рулон, а не гасить экран */
int     oneitm = 0;     /* флаг: разрешено указать только один пункт меню */
int     panelf = 1;     /* флаг: показывать панель подсказки */
int     whodirf = 1;    /* show whodir panel on screen */
int     xtermf = 0;     /* show whodir panel on window title using xterm escape sequence */
int     histf  = 0;     /* флаг: сохранять историю команд при выходе из vash, если histsn != 1 */
int		histsn = 0;		/* флаг: синхронизировать историю после каждой команды */
int     clockf = 1;     /* флаг: показывать часы */
int     cmailf = 1;     /* флаг: проверять почту */
int     loginf = 0;     /* флаг: главная оболочка, ppid() == 1 */

char   *envshell;		/* env SHELL= */
char   *homedir;        /* домашний каталог */
char   *cwd;            /* текущий (рабочий) каталог */

char *pmtsh;

usage()
{
	fprintf(stderr, "Usage: vash [-1] [-bN] [-c] [-h] [-S] [-s] [-w] [-x] [-m] [-p]\n");
	fprintf(stderr, "Usage: vash [any flags] -- command\n");
	exit(1);
}

int     allcod = 1;

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
	if (ok == 0 && histf && homedir != (char *)0 && histsn == 0) {
		cmdphist(homedir);
	}
	unlink(tmpflnm);

/*	exit(ok);*/
}

/*ARGSUSED*/
void onintr(signo)
{
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	onexit(1); exit(1);
}

static  LINE tmplate =
/*NOSTRICT*/
       { 16, 0, 0, 0,
	       TXT|INP|NED|LFASTR,
		       (char *)0,
			       cvt_vf
			   	   /*cvt_sp*/ ,
			       t_file,
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

	if (getuid() == 0) {
		pmtsh = "# ";
	} else {
		pmtsh = "$ ";
	}

	/*
	 * environment setup parsed before command line args
	 */
	if ((envsup = getenv("VASH")) != (char *) 0) {
		char linenoa[4];
		char *p;

		/* reset defaults in case environment setup is in use */
		scrolf = histf = histsn = panelf = whodirf = xtermf = clockf = cmailf = 0;
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
				if (clm._yy_max > (maxli - 4))
					clm._yy_max = maxli - 4;
				break;
			case 'p':
				panelf++;
				break;
			case 'x':
				xtermf++;
				break;
			case 'w':
				whodirf++;
				break;
			case 's':
				scrolf++;
				break;
			case 'h':
				histf++;
				break;
			case 'S':
				histsn++;
				break;
			case 'c':
				clockf++;
				break;
			case 'm':
				cmailf++;
				break;
			}
		}
	}

	Cfill = malloc(4096); /* TODO constant in assist.h */
	if (Cfill != NULL) {
		*Cfill = ':';
		*Cfill = '\0';
	} else {
		printf(stderr, "Can't get extra memory");
		exit (1);
	}

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
				panelf = 0;
				continue;
			case 'x':
				xtermf = 0;
				continue;
			case 'w':
				whodirf = 0;
				continue;
			case 's':
				scrolf = 0;
				continue;
			case 'h':
				histf = 0;
				continue;
			case 'S':
				histsn = 0;
				continue;
			case 'c':
				clockf = 0;
				continue;
			case 'm':
				cmailf = 0;
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
	       histf = 0;
   	}

    if (homedir != (char *)0) {
		cmdghist(homedir);
	}
    tmpfd = mkstemp(tmpflnm);      /* получить имя временного файла */
/*  tmpflnm = "/tmp/ash.tmp";        /* получить имя временного файла */

	io_set(IO_VIDEO);
	signal(SIGINT, onintr);
#ifdef DEBUG
	printf("   Cfill=\"%s\" vashrc=\"%s\"\r\n", Cfill, vashrc);
#endif
/*NOXSTR*/
	if ( cmdset(vashrc) ) {

		cfill(argc, argv); /* tail of agruments is fill command replaced one from vashrc */

#ifdef DEBUG
		printf("cmdset; Cfill=\"%s\" vashrc=\"%s\"\r\n", Cfill, vashrc);
#endif
		if ( fil_vf(1) ) {
			/* Настроить нач. состояние области свитка */
			/* y0_top = maxli - clm._yy_max;/* - 1;*/

			/*y0_top = 0;*/
			y0_top = clm._y0;/* - 1;*/
			scrlnl();
			/*y0_top = clm._y0;*/

			signal( SIGINT, SIG_IGN );
			signal( SIGQUIT, SIG_IGN );

			u_menu(clm._vf, "mainh.lb");
			onexit(0); exit(0);
		}
	} else {
		/* ошибки (плохо установлен ash, не читается реперный файл */
		/* scrlnl(); */
		io_set(IO_TTYPE);
		printf("\n");
		exit(1);
	}
}
