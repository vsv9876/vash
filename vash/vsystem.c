#include <sys/ioctl.h>
#include <sys/param.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <ctype.h>
#include <stdio.h>
#include "line.h"
#include "assist.h"

#define ARGVMAX 500
#ifndef NCARGS 
#define NCARGS 4096
#endif 

extern  int     y0_top;
extern  char    *pmtsh;
/*extern  char  **environ;*/
extern  char    Csubs[];

extern  char  *nmsubs();
extern  char  *tmpflnm;


#define ASH_NOWAIT 001  /* не ждать завершения запущенного процесса */
#define ASH_NOFORK 010  /* заменить программу процесса */
#define ASH_NOSH   020  /* не запускать дополнительный процесс sh */

/*
 * Выполнить программу.
 *
 * Запуск нового процесса, ожидание.
 */
int vexec(argv0, argv, execmode)
char  *argv0;
char **argv;
int execmode;
{
	extern int errno;
	int pid, piddone;       /* номера процессов */
	int forked;             /* флаг: есть порожденный процесс */
	int syscod;
	void (*sigint)();
	int i;

	/* реакция на сигнал(ы) уже была установлена SIG_IGN,
	   здесь повторяется для надежности, после окончательной
	   отладки надо убрать */
	sigint = signal( SIGINT, SIG_IGN);

	pid = 0;
	if (execmode & ASH_NOFORK) {
		forked = 0;
	}
	else {
		forked = 1;
		pid = fork();  /* vfork требует точно указать exec */
	}
	if (pid < 0) {
		fprintf(stderr, "Can't fork\n");
		return(-1);
	}
	if (pid == 0) {
		/* процесс-ребенок */
		if (execmode & ASH_NOWAIT) {
			signal( SIGINT, SIG_IGN);
			signal( SIGQUIT, SIG_DFL);
		}
		else {
			signal( SIGINT, SIG_DFL );
			signal( SIGQUIT, SIG_DFL );
		}
		/* закрыть файлы */
		for (i = 20; i > 2; i--) {
			close(i);
		}
		/* здесь переопределить станд. файлы, -- man credentials(7) ?*/
#if 0
		setsid();
		setpgrp();
#endif
		fclose(vttout);
		/*...*/

#ifndef LUNIX
		execvp(argv0, argv, environ);
#else
		execvp(argv0, argv);
#endif
		fprintf(stderr, "%s: command not found\n", argv0);
		if (forked)
			exit(1);
		else    {
			signal( SIGINT, sigint);
			return(-1);
		}
	}
	if (pid > 0) {
		/* процесс-родитель */
		syscod = 0;
		if (execmode & ASH_NOWAIT)
			/*VARARGS*/
			printf("[%d] Started\n", pid);
		else
			do {
				piddone = wait( &syscod );
				if ( pid != piddone)
					printf("[%d] done\n", piddone);
			} while ( pid != piddone);

		unlink(tmpflnm); /* временный файл menu2 удаляется после каждого запуска */

		signal( SIGINT, sigint );
		signal( SIGQUIT, SIG_IGN );
		/*signal (SIGTSTP, SIG_IGN);*/
		return(syscod);
	}
#ifdef lint
	/*NOTREACHED*/
	return(0);
#endif
}

extern int showtime();
/*
 * Экранная преамбула запуска команды.
 */
void shstart()
{
	showtime( 0 );                  /* погасить часы */
	/* это старый мусор, теперь самая верхняя строка ни при каких режимах не занята;
	 * можно даже не гасить часы - их погасит er_eop() после показа команды.
	 * TODO: режим IO_TTYPE перенести поближе к execve, здесь рановато...
	 */
	/*cp_set(0, 0, TXT); er_eol();    /* погасить cwd */

	if (clm._y0 > y0_top)
		cp_set(y0_top - 1, 0, CMD);
	else
		cp_set(clm._y0 - 1, 0, CMD);

	y0_top = clm._y0;    /* установить границу свитка */
	at_set(0); /* er_eop(); /*HACK*/

	/* io_set(IO_TTYPE); */
}

extern  int     mark_i;
extern  int     mark_o;

/*ARGSUSED*/
int avexec(cmd, cmdlbl, execmode)
char *cmd;      /* собственно команда, которую надо выполнить */
char *cmdlbl;   /* строка для индикации, как правило == cmd */
int  execmode;
{
	char *argv[ARGVMAX+1];
	register char *p;
	register int i;

	p = cmd; while(isspace(*p)) p++;
	i = 0;
	argv[i] = p;
	for (; *p; p++) {
		if(isspace(*p)) {
			/* закрыть очередной аргумент */
			*p++ = '\0';
			while(*p && isspace(*p)) p++;
			i++; argv[i] = p;
		}
		if (i < ARGVMAX) continue;
		else break;
	}
	i++; argv[i] = (char *)0;
	return(vexec(argv[0], argv, execmode));
}

extern int cmdsub();
/*
 * Формирование команды для /bin/sh.
 * Подстановка помеченных файлов.
 * Показ команды перед выполнением.
 */
static char out_str[800] = "";
int shexec(cmd, cmdlbl, execmode, execapnd)
char *cmd;      /* собственно команда, которую надо выполнить */
char *cmdlbl;   /* строка для индикации, как правило == cmd */
int  execmode;
int  execapnd;
{
	/* строка для подстановки:
	 * макс. длина аргументов + длина префикса "exec "
	 */
	char cmd2[NCARGS + 6];
	char *argv[7];
	int syscod;         /* код возврата команды */
	int i;
	register int j;
	register char *p;
	char psep;  /* сепаратор аргументов при подстановке, м. быть ' ' или ',' */
	char *nm_ptr;
	int ttysane = 0; /* container for IO_TTYSANE flag */

	p = cmd;
	i = 0;
	if ( execapnd ) {
		/*VARARGS*/
		sprintf(cmd2, "exec ");
		i += 5;
	}
	/* подставить пометку станд. ввода */
	if (mark_i >= 0) {
		strcat(&cmd2[i], "<");
		while(cmd2[++i]);
		cmdsub(&cmd2[i], "#@", mark_i, 1);
		strcat(&cmd2[i], " ");
		while(cmd2[++i]);
		clm._itms[mark_i][0] = ' '; mark_i = -1;
/*                while(cmd2[++i]);     */
	}
	/* подставить аргументы */
	while( *p ) {
		if (p[1] == MONEY || p[1] == ',') psep = p[1];
		else 							psep = '\0';
		if (*p == MONEY && psep) {
			p++; p++;
			/* подставить */
			for (j = 0; j < clm._itmmax; j++) {
	  /* проверка переполнения самая приблизительная */
				if (i >= (NCARGS-MAXLICO)) {
					fprintf(stderr, " *** %s ***\n",
					"name substitution overflow");
					return(-1);
				}
				if(clm._itms[j][0] == MONEY) {
				   /* вставить очередное имя */
				   nm_ptr = nmsubs(&clm._itms[j][2], Csubs);
				   if (sh_esc(out_str, nm_ptr)) execmode = execmode & (~ASH_NOSH);
				   sprintf(&cmd2[i], "%s ", out_str);

				   while(cmd2[++i]) ;

				   i--; cmd2[i++] = psep==MONEY?' ':psep;
				}
			}
#ifndef RETRO
			/* убрать последний пробел после подстановки - он лишний */
			if (cmd2[i - 1] == ' ' || cmd2[i - 1] == ',') cmd2[--i] = '\0';

#endif
			/*cmd2[i] = '\0';*/
		}
		else    cmd2[i++] = *p++;
	}
	cmd2[i] = 0;
	/* подставить пометку станд. вывода */
	if (mark_o >= 0) {
/*                sprintf(&cmd2[i], " >%s", &itms[mark_o][2]);  */
		strcat(&cmd2[i], " >");
		while(cmd2[++i]);
		cmdsub(&cmd2[i], "#@", mark_o, 1);

		clm._itms[mark_o][0] = ' '; mark_o = -1;
	}

	/* показать выполняемую команду: */
	if (cmdlbl == (char *)0) {
		cmdlbl = cmd2;  /* саму команду и показать */
	}
	/*at_set(TXT);*/
	at_set(CMD); er_eop(CMD);
	at_set(CMD|INP);
	w_str(pmtsh);
	er_eop(0);
	fflush(vttout);
	io_set(IO_TTYPE);

	/*Warning! no IO_TTYPE mode below this point, stdio only */

	printf("%s", cmdlbl);
	if (execmode & ASH_NOWAIT)
		putchar('&');
	putchar('\n');

	fflush(stdout);

	/* выполнить команду */
	if (execmode & ASH_NOSH)
		syscod = avexec(cmd2, cmdlbl, execmode);
	else {
		argv[0] = envshell; /* "sh";*/
		argv[1] = "-c"; /* please, do not hack -cli for bash :) */
		argv[2] = cmd2;
		argv[3] = (char *)0;

		syscod = vexec(argv[0], argv, execmode);
	}

	/*moved from vsystem*/
	/*fprintf(stdout, " ");*/ fflush(stdout);
	if (syscod != 0)
		ttysane = IO_TTYSANE;
	io_set(IO_VIDEO | ttysane);

	return(syscod);
}

/*
 * Главная запускалка команд.
 *
 * Анализ командной строки, выбор режима и способа запуска.
 */
int vsystem(cmd, cmdlbl)
char *cmd;      /* собственно команда, которую надо выполнить */
char *cmdlbl;   /* строка для индикации, как правило == cmd */
{
	int execmode;   /* флаги запуска команды */
	int execapnd;   /* флаг: дописать впереди команды "exec " */
	int execargv;   /* флаг: выполнить команду без sh */
	register char *p;
	register char *cmdp;
	int syscod;

	execmode = 0;
	execargv = execapnd = 1;
	if (strncmp(cmd, "exec ", 5) == 0) {
		execmode |= ASH_NOFORK;
		p = cmdp = &cmd[5];
	}
	else
		p = cmdp = cmd;

	for (; *p; p++) {
		switch(*p) {
		default:
			break;
		case '&':
			if (p[1] == '\0') {
				/* фоновый запуск, подчистить пробелы */
				execmode |= ASH_NOWAIT;
				*p = '\0'; --p;
				while (p > cmdp && isspace(*p)) {
					*p = '\0'; --p;
				}
				continue;
			}
			/*NO BREAK*/
		/* разделители процессов как метасимволы sh */
		case '|': case ';':
		case '(': case ')':
		case '{': case '}':
			execapnd = 0;
			/*NO BREAK*/
		/* другие метасимволы sh */
		case '[': case ']':
		case '<': case '>':
		case '*': case '@': case '?':
		case '!': case '$': case '^':
		case '\'': case '"': case '`': case '~': case '\\':
			execargv = 0;
			break;
		/*подстановки, обрабатываемые на следующем шаге*/
		case MONEY:
			if (p[1] == MONEY || p[1] == '\'') {
				p++; p++;
				continue;
			}
			execargv = 0;
			break;
		}
	}
	if (mark_i >= 0 || mark_o >= 0)
		execargv = 0;
	if (execargv)
		execapnd = 0;
	execmode |= (execargv ? ASH_NOSH : 0);
	shstart();
	syscod = shexec(cmdp, cmdlbl, execmode, execapnd);
#if 0
    moved to shexec() for readability
	fprintf(stdout, " "); fflush(stdout);
	io_set(IO_VIDEO);
#endif
	return(syscod);
}
