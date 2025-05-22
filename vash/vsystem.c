#include <sys/ioctl.h>
#include <sys/param.h>
#include <errno.h>
/*#define _POSIX_C_SOURCE 200809L*/
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include <ctype.h>
#include <stdio.h>
#include "line.h"
#include "assist.h"
#include "vjobs.h"

#define ARGVMAX 500
#ifndef NCARGS 
#define NCARGS 4096
#endif 

extern  int     y0_top;
/*extern  char    *pmtsh;*/
extern  char  **environ;
extern  char    Csubs[];

extern  char  *nmsubs();
extern  char  *tmpflnm;

extern  int     mark_i;
extern  int     mark_o;

/*static int n; 	/* vj slot table index, moved to vjobs.c  */
static reapsig = 1;		/* reapchk on SIGCHLD if == 1 */

#ifdef VISI_DEBUG
static int debug = 0;
static int debug_ok = 1;
#endif

/*
 * signals routines
 */
void Signal(sig, hdlr)
int sig;
void *hdlr;
{
	struct sigaction sa;

	sigaction(sig, NULL, &sa);
	if (sa.sa_handler != hdlr) {
		/*sa.sa_flags = 0;*/
		sa.sa_handler = hdlr;
		sigaction(sig, &sa, NULL);
	}
}


void blk_on(void) /*normal video mode */
{
	sigset_t sblock;

#ifdef VISI_DEBUG
	char *tmps;

	/* check environment 1st */
	if (debug_ok && (tmps = getenv("VASH_PGRP")) != 0) {
		debug = atoi(tmps);
		debug_ok = 0;
	}
	if (debug) {
		printf(" !<on");
		fflush(stdout);
	}
#endif
/*	sigemptyset(&sblock);*/
	sigprocmask(SIG_BLOCK, NULL, &sblock);
	sigaddset(&sblock, SIGTERM);
	sigaddset(&sblock, SIGTTOU);
	sigaddset(&sblock, SIGTTIN);
	sigaddset(&sblock, SIGTSTP);
/*	sigaddset(&sblock, SIGCHLD);*/
	sigprocmask(SIG_BLOCK, &sblock, NULL);
#ifdef VISI_DEBUG
	if (debug) {
		printf("! ");
		fflush(stdout);
	}
#endif /*VISI_DEBUG*/
}

void blk_new(void) /*starting a child*/
{
	sigset_t sigs;
	struct sigaction sa;

#ifdef VISI_DEBUG
	if (debug) {
		printf(" !<new");
		fflush(stdout);
	}
#endif
	sigemptyset(&sigs);
/*	sigaction(SIG_DFL, NULL, &sa);*/
	sigprocmask(SIG_SETMASK, NULL, &sigs);
	sigaddset(&sigs, SIGINT);
	sigaddset(&sigs, SIGQUIT);
	sigaddset(&sigs, SIGTSTP);
	sigaddset(&sigs, SIGTTIN);
	sigaddset(&sigs, SIGTTOU);
	sigaddset(&sigs, SIGCHLD);
	sigprocmask(SIG_UNBLOCK, &sigs, NULL);

	Signal(SIGINT,  SIG_DFL);
	Signal(SIGQUIT, SIG_DFL);
	Signal(SIGTSTP, SIG_DFL);
	Signal(SIGTTIN, SIG_DFL);
	Signal(SIGTTOU, SIG_DFL);
	Signal(SIGCHLD, SIG_DFL);
	Signal(SIGWINCH, SIG_DFL);
#ifdef VISI_DEBUG
	if (debug) {
		printf("! ");
		fflush(stdout);
	}
#endif
}

void blk_off(void) /*unblock just before reap / do waiting*/
{
	sigset_t sblock;

#ifdef VISI_DEBUG
	if (debug) {
		printf(" !<off");
		fflush(stdout);
	}
#endif
	sigemptyset(&sblock);
/*	sigprocmask(SIG_UNBLOCK, NULL, &sblock);*/
#if 0
	sigdelset(&sblock, SIGTSTP);
	sigdelset(&sblock, SIGCHLD);
#endif
	sigaddset(&sblock, SIGTTOU);
	sigaddset(&sblock, SIGTTIN);
	sigprocmask(SIG_BLOCK, &sblock, NULL);
#ifdef VISI_DEBUG
	if (debug) {
		printf("! ");
		fflush(stdout);
	}
#endif
}

void blk_sigchld(on)
int on;
{
	sigset_t sigset;

	sigemptyset(&sigset);

	/*sigprocmask(SIG_SETMASK, NULL, &sigset);*/
	sigaddset(&sigset, SIGCHLD);

	if(on)
		sigprocmask(SIG_BLOCK, &sigset, NULL);
	else
		sigprocmask(SIG_UNBLOCK, &sigset, NULL);
}

int Tpgrp(pgrp, tmsg, emsg)
const pid_t pgrp;
const char *tmsg;	/*trace message */
const char *emsg;
{
	int ret, err;

#ifdef VISI_DEBUG
	if (debug) {
		printf(" %%%s ", tmsg);
		fflush(stdout);
	}
#endif
	if ((ret = tcsetpgrp(STDOUT_FILENO, pgrp)) < 0) {
		err = errno;
		fprintf(stderr, "vash: ");
		errno = err;
		perror(emsg);
	}
	fflush(stdout);
	return(ret);
}

/*same on SIGTSTP, SIGCHLD */
void onchld(signo)
int signo;
{
	reapchk(0);
	;
#if VISI_SIG_POSIX
#else
	signal(SIGTSTP, &onchld);
	signal(SIGCHLD, &onchld);
#endif
}

void on_onchld(void)
{
#if VISI_SIG_POSIX
	struct sigaction sa;

	sigaction(SIGCHLD, NULL, &sa);
	sa.sa_handler = onchld;
	sa.sa_flags = SA_RESTART /*| SA_NOCLDWAIT*/ /* | SA_SIGINFO*/;
/*	sigemptyset(&sa.sa_mask);*/

	sigaction(SIGCHLD, &sa, NULL);
#else
	signal( SIGCHLD, &onchld );
#endif
}

/*
 * check background tasks for changing state
 * vj[] table cleaned of jobs when status 'done' by vj_notify()
 * version with waitid() - seems to be more predictable vs waitpid()
 * but need more data in vj structure
 *
 * return vj index (NOT si.si_code) if reaped new state from process requested
 * return 0 otherwise (no job)
 */
int reapchk(chkpid)
pid_t chkpid;
{
	siginfo_t si;
	int i;
	pid_t pid, ok, ret;
	int wpid;


	if (chkpid > 0) {
		wpid = P_PID; pid = chkpid;
	} else {
		wpid = P_ALL; pid = 0;
	}
	si.si_pid = si.si_signo = si.si_status = si.si_code = 0;
	ret = 0;
	if ((ok = waitid(wpid, pid, &si,
		/*WNOWAIT|*/WNOHANG|WEXITED|WSTOPPED|WCONTINUED)) < 0) {
		if (errno != ECHILD)
			perror("vash: reapchk() waitid");
	} else {
		if ((pid = si.si_pid) > 0 /*&& si.si_signo == SIGCHLD*/) {
			/*ret = si.si_code;*/
			ret = i = vj_by_pid(pid);
			vj[i].notify = 1;
			memcpy(&(vj[i].si), &si, sizeof(si));
			if  (  si.si_code == CLD_EXITED
				|| si.si_code == CLD_KILLED
				|| si.si_code == CLD_DUMPED
				)
				vj[i].done = 1;
			else
				vj[i].done = 0;
		}
	}
	return (ret);
}

/*
 * check background tasks for changing state;
 * mark vj[] entry to be cleaned if job done;
 * NOTE: vj[] cleanup will perform by vj_notify()
 *
 * return waitstatus if chkpid > 0
 */
int reapchk_NOTUSED(chkpid)
pid_t chkpid;
{
	int i, ws;
	pid_t pid, wpid;

	if (chkpid > 0) wpid = chkpid;
	else            wpid = -1;
	if ((pid =
			waitpid(wpid,
					&ws,
					WNOHANG|WUNTRACED|WSTOPPED|WCONTINUED)) < 0) {
		if (errno != ECHILD)
			perror("reapchk() waitpid");
		return (-1);
	}
	if (pid == 0)
		return (-1);
	i = vj_by_pid(pid);
	i = vj[i].done = 0;
	vj[i].wstatus = ws;

	if (pid > 0) {
		vj[i].notify = 1;
		if (WIFEXITED(ws))
			vj[i].done = 1;
		if (WIFSIGNALED(ws))
			vj[i].done = 1;
		if (WCOREDUMP(ws))
			vj[i].done = 1;
	}
	if (chkpid > 0)
		return(ws);
	return (0);
}

/*
 * wait fg process and reap waitstatus if
 *   process terminated or stoped;
 *
 * version with waitid() -
 * return si.si_code;
 */
int reapw(jn)
int jn;
{
	siginfo_t si;
	int i;
	pid_t pid, ok, ret;
	int wpid;
	struct termios ts;

	blk_sigchld(1);

	errno = 0;
	pid = vj[jn].pid;
	si.si_pid = si.si_signo = si.si_status = si.si_code = 0;
	ret = vj[jn].done = 0;
	if ((ok = waitid(P_PID, pid, &si, WEXITED|WUNTRACED|WSTOPPED/*|WCONTINUED*/)) < 0) {
		if (errno != ECHILD) {
			perror("vash: reapw waitid()");
/*			Tpgrp(v.pid, "reapw(),Tpgrp()", "reapw Tpgrp waitid()");*/
		}
	} else {
		ret = jn;/*si.si_code;*/
		vj[jn].notify = 1;
		memcpy(&(vj[jn].si), &si, sizeof(si));
		if        (si.si_code == CLD_EXITED
				|| si.si_code == CLD_KILLED
				|| si.si_code == CLD_DUMPED) {
			/*vj_clr(jn);*/
			vj[jn].done = 1;
		}
		if (si.si_code == CLD_STOPPED && vj[jn].ts_saved == 0) {
			/*
			 * save tty mode of FG job -- will be restored by fg command
			 */
			fflush(stdout);
			tcgetattr(STDOUT_FILENO, &ts);
			memcpy(&(vj[jn].ts), &ts, sizeof(struct termios));
			vj[jn].ts_saved = 1;
		}
/*		Tpgrp(v.pid, "reapw() return", "reapw return");*/
	}
	Tpgrp(v.pid, "reapw(),Tpgrp()", "reapw Tpgrp waitid()");

	blk_sigchld(0);
	return (ret);
}

/*
 * wait fg process and reap waitstatus if
 *   process terminated or stoped;
 * return waitstatus;
 */
int reapw_NOUSED(jn)
int jn;
{
	pid_t pid;
	int ws;
	int ok;
	struct termios ts;

	/*blk_sigchld(1);*/

	ws = 0;
	errno = 0;
	pid = vj[jn].pid;

	if (waitpid(pid, &ws, WUNTRACED|WSTOPPED/*|WCONTINUED*/) < 0) {
		if (errno != ECHILD) {
			perror("vash: reap waitpid");
			Tpgrp(v.pid, "reapw()", "reap waitpid()");
		}
	}
	vj[jn].notify = 1;
	vj[jn].wstatus = ws;
	if (WIFEXITED(ws) /*|| WIFSIGNALED(ws) */|| WCOREDUMP(ws)) {
		vj_clr(jn);
		/*vj[jn].done = 1;*/
	} else if (/*vj[jn].ts_saved == 0
			&& */WIFSTOPPED(ws)/* || WIFCONTINUED(ws)*/) {
		/*
		 * save tty mode -- will be restored by fg command
		 */
		fflush(stdout);
		tcgetattr(STDOUT_FILENO, &ts);
		memcpy(&(vj[jn].ts), &ts, sizeof(struct termios));
		/*sleep (1);*/
		vj[jn].ts_saved = 1;
	}

	Tpgrp(v.pid, "reapw() return", "reap return");

	/*blk_sigchld(0);*/
	return (ws);
}

/*
 * vash launcher
 *
 * return vj[] index if launched FG (set FG job number)
 * else return 0 (launched BG) ;
 * return -1 in case of errors: no lauch, etc..
 */
int vexec(argv0, argv, execmode)
char  *argv0;
char **argv;
int execmode;
{
	/*extern int errno;*/
	pid_t /*piddone, */chld;       /* номера процессов */
	int forked;             /* _to be_ forked flag: есть(будет) порожденный процесс */
	int nowait;
	/*int wstatus;*/
/*	int options;*/
/*	void (*sigint)();*/
	int i;
	int ok;
	struct termios ts;

	forked = nowait = 0;

	nowait = ((execmode & ASH_NOWAIT)==0) ? 0 : 1;
	forked = ((execmode & ASH_NOFORK)==0) ? 1 : 0;

	blk_off();
	blk_sigchld(1);
	if (forked)
		chld = fork();
	else
		chld = 0;

	if (chld < 0) {
		fprintf(stderr, "vash: can't fork\n");
		return(-1);
	}

	if (chld == 0) {	/* child */

		blk_new();

		chld = getpid();
		/*blk_new();*/
		if ((ok = setpgid(chld, 0 /*chld*/)) < 0)
			perror("vash: child setpgid");

		if (forked) {
			if (0 == nowait) {
				if ((Tpgrp(chld, "new.chld", "fg tcsetpgrp") >= 0))
					/*io_set(VT_OFF)*/;
			}
		}
		io_set(VT_OFF);
		/* canonical close files; 20 is a retro -- TODO a modernize */
		for (i = 20; i > 2; i--) {
			close(i);
		}
		/**/

		execve(argv0, argv, environ);
		execvp(argv0, argv);
		fprintf(stderr, "vash: launch failed: %s\n", argv0);
		if ( ! forked)
			return(-1);
		exit(1);
	}
	/* parent (the vash itself) */
	/*else {*/
	blk_on();
	if (forked) {
		/*repeat setting for child in parent process */
		if ((ok = setpgid(chld, chld)) < 0)
			perror("vash: child setpgid");
		if ((ok >= 0) && (0 == nowait)) {
			Tpgrp(chld, "new..chld", "fg tcsetpgrp");
		}
	}
	/* complete the launch: set setpgid+tcsetpgrp back to parent */
	if ((ok = setpgid(v.pid, v.pgrp/*0*/)) < 0)
		perror("vash: VASH setpgid");
	if(v.pid != getpgrp() /*0 == nowait*/) {
		Tpgrp(v.pid, "new.chld", "fg tcsetpgrp");
	}
	/*}*/
	/**/
	/* fill child job registry record */
	i = fgn_get();
	vj[i].pid = chld;
	vj_adv(i);

	if(nowait) {
		/*notify operator*/
		printf("[%-d]  %6d\r\n", fgn_get(), chld);
		vj[i].ts_saved = 0; /* no save tty mode: use io_set(VT_OFF) in fg */
		io_set(VT_ON);
		fflush(stdout);
	}
	/* временный файл menu2 удаляется после каждого запуска:
	 *  TODO: убрать этот бред */
	/*unlink(tmpflnm); */

	if (nowait) {	/* bg & */
		/*kill(chld, SIGCONT);*/
		return (0);
	}

	return(i);  /* fg */
}


/*
 * Screen preamble of command before io_set(VT_OFF) while io_set(VT_ON)
 */
void shprolog()
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

	at_set(CMD|INP);
	w_str(pmtsh);
	at_set(CMD);
	er_eop(0);

	/*io_set(IO_TTYPE);*/
}

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
	return( vexec (argv[0], argv, execmode) );
}

/*extern int cmdsub();  TODO: cleanup*/

/*
 * Формирование команды для /bin/sh.
 * Подстановка помеченных файлов.
 * Показ команды перед выполнением.
 *
 * returns:
 *  0 if FG launched
 *  1 if BG launched
 * -1 if failed
 * wait status get by vwreap(reap())
 */

static char out_str[MAXLICO/*800*/] = "";

int shexec(cmd, cmdlbl, execmode, execpref)
char *cmd;      /* собственно команда, которую надо выполнить */
char *cmdlbl;   /* строка для индикации, как правило == cmd */
int  execmode;
int  execpref;
{
	/* строка для подстановки:
	 * макс. длина аргументов + длина префикса "exec "
	 */
	char cmd2[NCARGS + 6];
	char *argv[7];
	int jobnum;         /* starting job number */
	int i;
	register int j;
	register char *p;
	char psep;  /* сепаратор аргументов при подстановке, м. быть ' ' или ',' */
	char *nm_ptr;
	int ttysane = 0; /* container for IO_TTYSANE flag */
	int n; /* new job slot number  */
	int bg;

	bg = ((execmode & ASH_NOWAIT) == 0) ? 0 : 1;
	p = cmd;
	i = 0;
	cmd2[i] = '\0';
	if ( execpref ) {
		/*VARARGS*/
		sprintf(cmd2, "exec ");
		i += 5;
	}
	/* подставить пометку станд. ввода */
	if (mark_i >= 0) {
		strcat(&cmd2[i], "<");
		while(cmd2[++i]);
		cmdsub(&cmd2[i], "#@", mark_i, 1, 1);
		strcat(&cmd2[i], " ");
		while(cmd2[++i]);
		clm._itms[mark_i][0] = ' '; mark_i = -1;
/*                while(cmd2[++i]);     */
	}
	/* подставить аргументы */
	while( *p ) {
		if (p[1] == MONEY || p[1] == ',')
			psep = p[1];
		else
			psep = '\0';
		if (p[0] == MONEY && p[1] == '@') {
			p++; p++;
			/* experiment: pointed by cursor substitution */
			nm_ptr = nmsubs(&clm._itms[clm._itm][2], Csubs);
			if (sh_esc(out_str, nm_ptr)) {
				execmode = execmode & (~ASH_NOSH);
			}
			sprintf(&cmd2[i], "%s", out_str);
			while (cmd2[++i]) ;
#if 0
			cmd2[i++] = ' ';
#endif
		}
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
				   if (sh_esc(out_str, nm_ptr)) {
					   execmode = execmode & (~ASH_NOSH);
				   }
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
		cmdsub(&cmd2[i], "#@", mark_o, 1, 1);

		clm._itms[mark_o][0] = ' '; mark_o = -1;
	}

	/* показать выполняемую команду: */
	if (cmdlbl == (char *)0) {
		cmdlbl = cmd2;  /* саму команду и показать */
	}
#if 0
	at_set(CMD); er_eop(CMD);
	at_set(CMD|INP);
	w_str(pmtsh);
	er_eop(0);
	fflush(vttout);

	shprolog();

/*	printf("%s", cmdlbl);*/
	tty_cmd(NULL, cmdlbl);

	/*vt_off();/*io_set(IO_TTYPE);*/
#endif
	/*Warning! no IO_VIDEO mode reasonable below this point */
/*
	if (execmode & ASH_NOWAIT) {
		putchar(' ');
		putchar('&');
	}
*/
	putchar('\r');
	putchar('\n');

	fflush(stdout);

	n = vj_new(bg); /* prevent vj table overflow */
	if (n < 0) {
		w_emsg("no more jobs configured, sorry...");
		return(0/*-1*/);
	}
	strncpy(vj[n].cmd, cmd2/*cmdlbl*/, MAXLICO);

	/* выполнить команду */
	if (execmode & ASH_NOSH) {
		jobnum = avexec(cmd2, cmdlbl, execmode);
	} else {
		argv[0] = v.shell; /* "/bin/sh";*/
		argv[1] = "-c"; /* please, do not hack -ci for bash :) */
		argv[2] = cmd2;
		argv[3] = (char *)0;

		jobnum = vexec(argv[0], argv, execmode);
	}
	/*vj[n].wstatus = ret;*/

	fflush(stdout);

	return(jobnum);
}

/*
 * Главная запускалка команд.
 *
 * Анализ командной строки, выбор режима и способа запуска.
 *
 *
 */
int vsystem(cmd, cmdlbl/*, execmp*/)
char *cmd;      /* собственно команда, которую надо выполнить */
char *cmdlbl;   /* строка для индикации, как правило == cmd */
/* int  *execmp;	/* pointer to execmode */
{
	int execmode;   /* флаги запуска команды */
	int execpref;   /* флаг: дописать впереди команды префикс "exec " */
	int execargv;   /* флаг: выполнить команду без sh */
	register char *p;
	register char *cmdp;
	int jobnum;

	execmode = 0;
	execargv = execpref = 1;
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
			execpref = 0;
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
	if (vflag.shanyway) {
		execargv = 0; execpref = 0;
	} else {
		if (mark_i >= 0 || mark_o >= 0)
			execargv = 0;
		if (execargv)
			execpref = 0;
		execmode |= (execargv ? ASH_NOSH : 0);
	}
	/*shprolog();*/
	jobnum = shexec(cmdp, cmdlbl, execmode, execpref);
	fflush(stdout);
#if 0
    /*was moved to shexec() for readability, return back in place*/
	fprintf(stdout, " "); fflush(stdout);
	io_set(IO_VIDEO);
#endif
#if 0
	/**execmp = execmode;*/
	if (execmode & ASH_NOWAIT)
		return(1);
#endif
	return(/*0*/ jobnum);
}

