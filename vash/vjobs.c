/*
 * VASH - visual assistant for shell
 * Copyright (c) 1990-2025 Sergey Vovk <vsv>
 *
 * License: GPL and/or MIT,
 * see files COPYING and LICENSE
 */

/*
 * vjobs.c
 *
 *  Created on: 26 мар. 2025 г.
 *      Author: vsv
 */

#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <stdio.h>
#include <termios.h>
#include "line.h"
#include "assist.h"
#include "vjobs.h"

#ifdef DEBUG_VJOB_INIT
VJOB vj[VJOB_MAX] = {
		{ 0 },
		{ 111, 10, 0, "/bin/init -?-", "" },
		{ 1,   20, 0, "sleep 200", "" },
		{ 12,  30, 0, "", "" }
/*		{ 12,  3,  0, "sleep 100", "" },*/
};
#else
VJOB vj[VJOB_MAX] = { 0 };
#endif

static int fgn = 0;
int fgn_get()
{
	return fgn;
}

void fgn_set(x)
int x;
{
	fgn = x;
}

/* get new slot:
 * return free slot index, return -1 if no more free slot;
 * fill order
 */
int vj_new(bg)
int bg;
{
	int n;
	/* 1 free slot for FG job anyway */
	for (n = 1; n < (VJOB_MAX - bg); n++)
		if (vj[n].pid == 0) {
			vj_adv(n);
			fgn = n;
			return n;
		}
	fgn = 0;
	return (-1);
}

int orprev, orlast;

void orenum()
{
	int n, imax;

	imax = 0;
	for (n = 1; n < VJOB_MAX; n++) {
		if (vj[n].order >= imax)
			imax = vj[n].order;
	}
	orlast = imax;
	imax = 0;
	for (n = 1; n < VJOB_MAX; n++) {
		if (vj[n].order >= imax && vj[n].order != orlast)
			imax = vj[n].order;
	}
	orprev = imax;
}

static int vj_by_ord(o)
int o;
{
	int n;
	for (n = 1; n < VJOB_MAX; n++)
		if (vj[n].order == o)
			return (n);
	return (-1);
}

static char* orwhat(n)
int n;
{
	if (vj[n].order == orlast)
		return ("+");
	if (vj[n].order == orprev)
		return ("-");
	return(" ");
}

/*
 * human readable si.si_status for waitid()
 */
void wsi_stat(jn, tmpstr)
int jn; /* index of vj[] */
{

}

static char vjs[VJOB_MAX + 2];
static void vjshow()
{
	int i, zero;
	/* init indicators string */
	if (vjs[0] == '\0') {
		vjs[0] = ' ';
		for (i = 1; i < VJOB_MAX; i++)
			vjs[i] = ' ';
		vjs[i] = '\0';
	}
	zero = 1;
	for (i = 1; i < VJOB_MAX; i++) {
		if (vj[i].pid == 0)
			vjs[i] = '-';
		else {
			zero = 0;
			vjs[i] = 'R';
			switch (vj[i].si.si_code) {
			case CLD_DUMPED:     vjs[i] = 'X'; break;
			case CLD_CONTINUED:  vjs[i] = 'C'; break;
			case CLD_EXITED:
				vjs[i] = 'x';
				if (vj[i].si.si_status)
					vjs[i] = 'E';
				break;
			case CLD_STOPPED:
				switch (vj[i].si.si_status) {
				case SIGTTIN: vjs[i] = 'T'; break;
				case SIGTTOU: vjs[i] = 'O'; break;
				case SIGSTOP: vjs[i] = 'S'; break;
				default:      vjs[i] = 's'; break;
				}
				break;
			}
		}
	}
	if (zero)
		vjs[0] = '\0';
}

/*
 * return string of indicators
 */
char *jobshow()
{
	vjshow();
	return(vjs);
}



void sistat(tmpstr, j)
char *tmpstr;
int j; /* job number */
{
	int code;
	int st;

	code = vj[j].si.si_code;
	st = vj[j].si.si_status;
	if        (code == CLD_EXITED) {
		if (st == 0)
			sprintf(tmpstr, " OK ");
		else
			sprintf(tmpstr, " exit(%-d) ", st);
	} else if (code == CLD_KILLED) {
		sprintf(tmpstr, "%s", strsignal(st));
	} else if (code == CLD_DUMPED) {
		sprintf(tmpstr, "core dumped");
	} else if (code == CLD_STOPPED) {
		sprintf(tmpstr, " %s ", strsignal(st));
	} else if (code == CLD_TRAPPED) {
		sprintf(tmpstr, "Trapped");
	} else if (code == CLD_CONTINUED) {
		sprintf(tmpstr, "Continued");
	} else /*if (code == 0)*/ {
		sprintf(tmpstr, "Running");
	}
}

void wstat(wstatus, tmpstr)
int wstatus;
char *tmpstr;
{
	int  sign;
	char sbuf[40];
	const char *sigmsg;

	if (WIFSIGNALED(wstatus)) {
		sign = (WTERMSIG(wstatus));
		if      (sign == SIGHUP)    sigmsg = "Hungup";
		else if (sign == SIGINT)    sigmsg = "Cancel";
		else if (sign == SIGQUIT)   sigmsg = "Quit";
		else if (sign == SIGTERM)   sigmsg = "Terminate";
		else if (sign == SIGKILL)   sigmsg = "Kill";
		else    {
			sprintf(sbuf, "signal:%-d", sign);
			sigmsg = sbuf;
		}
        sprintf(tmpstr, "[ %-s ]", sigmsg);
	} else
	if (WIFSTOPPED(wstatus)) {
		sign = (WSTOPSIG(wstatus));
		if      (sign == (SIGSTOP)) sigmsg = "signal";
		else if (sign == (SIGTSTP)) sigmsg = "tty";
		else if (sign == (SIGTTIN)) sigmsg = "input";
		else if (sign == (SIGTTOU)) sigmsg = "output";
		else    {
			sprintf(sbuf, "signal:%-d", sign);
			sigmsg = sbuf;
		}
        sprintf(tmpstr, "[ Stopped:%-s ]", sigmsg);
    } else
    if (WIFCONTINUED(wstatus)) {
        sprintf(tmpstr, "[ Continued ]");
    } else
	if (WIFEXITED(wstatus)) {
		sprintf(tmpstr, "[ exit=%-d ]", WEXITSTATUS(wstatus));
	} else /*if (wstatus == 0)*/ {
		sprintf(tmpstr, "[ok]");
	}
}

/*
 * advance (increment) orlast for every new job activate (move foreground)
 */
int vj_adv(n)
int n;
{
	if (orlast == 0)
		orlast += 1;
	if (vj[n].order != orlast) {
		orlast += 1;
		vj[n].order = orlast;
		orenum();
	}
	return (vj[n].pid);
}

/* find an occupied jobs slot by process number */
int vj_by_pid(pid)
pid_t pid;
{
	int n;
	for (n = 1; n < VJOB_MAX; n++)
		if (vj[n].pid == pid) {
			return (n);
		}
	return(-1);
}

/* clear the jobs slot */
void vj_clr(n)
int n;
{
	if(n >= 0 && n < VJOB_MAX) {
		vj[n].order = 0;
		vj[n].pid = 0;
		vj[n].wstatus = 0;
		vj[n].notify = 0;
		vj[n].done = 0;
		vj[n].ts_saved = 0;
		/*vj[n].si.si_pid = 0;*/
		memset(&(vj[n].si), 0, sizeof(vj[0].si));
		/*memset(vj[n].cmd, 0, sizeof(vj[0].cmd));
		memset(vj[n].dir, 0, sizeof(vj[0].dir));
		memcpy(vj[n].ts), 0, sizeof(vj[0].ts));*/
	}
}

/* check if job slot occupied */
pid_t vj_pid(n)
int n;
{
	if(n > 0 && n < VJOB_MAX) {
		return(vj[n].pid);
	}
	return(0);
}

/*
 * skip spaces until next expr; stop on end of string
 * copy&paste from skipsp()
 *
 */
wchar_t *wcnext(p)
wchar_t *p;
{
	while(*p != L'\0' && *p != L' '/* && isspace(*p)*/)
		p++;
	return p;
}

/* parsing from cmd0/tmpstr: command, separator, parameter */
static u8char_t *cmd;
static u8char_t *sep;
static u8char_t *prm;

static void getparms(cm, p)
wchar_t *cm;
u8char_t *p;
{
	extern char *skipsp();
	u8char_t *ret;
	wcstombs(p, cm, STRBUF);
	cmd = p;
	do {
		p++;
	} while (*p != '\0' && !isspace(*p));
	sep = p;
	p = skipsp(p);
	prm = p;
}

void vj_info(n)
int n;
{
	char tmps[MAXLICO];

	if ( TRUE /*vj[n].show*/) {
		/*wstat(vj[n].wstatus, tmps);*/
		sistat(tmps, n);
		/*printf("[-d] %1d/%s  %-25.25s  %-40.40s\n",*/
		printf("%%%-d %s %-25.25s  %-45.45s\n",
				n, /*vj[n].done,*/ orwhat(n), tmps, vj[n].cmd);
		/*vj[n].show = 0;*/
	}
}

/*
 * notify about changed status in points of interest
 */
void vj_notify(omit)
int omit; /*do not notify about this job*/
{
	char wsbuf[STRBUF];
	char tmpstr[STRBUF];
	char *wsmsg;
	int i, ok, wstatus;
	int once = 1;

	/*reapchk(0); /*have a sense if reapchk never called from SIGCHLD handler*/
	for(i = 1; i < VJOB_MAX; i++) {
		/*j = &vj[i];*/
		if (vj[i].pid != 0 && vj[i].notify) {
			wsmsg = wsbuf;
			/*if (vj[i].wstatus == 0) {*/
			if (vj[i].si.si_status == 0) {
				if (vj[i].si.si_code == CLD_EXITED)
					strcpy(wsmsg, " Done: OK ");
			} else {
				sistat(wsmsg, i);
			}
			if (i != omit) { /* do not notify about fg (current) job */
				if (once) {
					printf("\r\n");
					once = 0;
				}
/* debug
				sprintf(tmpstr, "=(%-d) [%-d]%s  %-25.25s  %-45.45s",
						omit, i, orwhat(i), wsbuf, vj[i].cmd);
*/
				sprintf(tmpstr, "%%%-d %s  %-25.25s  %-45.45s",
						i, orwhat(i), wsbuf, vj[i].cmd);
				printf(tmpstr);
				if (tmpstr[strlen(tmpstr) - 1] != '\n')
					printf("\n\r");
				vj[i].notify = 0;
				if (vj[i].done)
					vj_clr(i);
			}
		}
	}
}

int v_jobs(cmd0, tmpstr)
wchar_t *cmd0;
char *tmpstr;
{
	char tmps[STRBUF];
	int i;
	/*VJOB *j;*/
	bool mode_l = 0;
	bool mode_d = 0;

	orenum();

	getparms(cmd0, tmpstr);
	if (prm != sep && *prm) {
		if (strcmp(prm, "-l") == 0)			mode_l = 1;
/*		if (strcmp(prm, "-d") == 0)			mode_d = 1;*/
	}
	/*reapchk(0);*/
	for(i = 1; i < VJOB_MAX; i++) {
		if (vj[i].pid != 0) {
			sistat(tmps, i);
			if (mode_l) {
				printf("%%%-d %s  %6d %-25.25s  %-40.40s\n",
						i, orwhat(i), vj[i].pid, tmps, vj[i].cmd);
				/*TODO if j->dir != wdir
				 *  print j->dir*/
			} else
				vj_info(i);
			vj[i].notify = 0;
			if (vj[i].si.si_status == CLD_EXITED)
				vj_clr(i);
		}
	}
	printf("\r");
	return -4;
}

/*
 * parse args for command from cmdv[]
 */
static int fgbg_n(cmd0, tmpstr)
wchar_t *cmd0;
char *tmpstr;
{
	int   n;
	int   id = 0;
	char *s;

	getparms(cmd0, tmpstr);
	orenum();
	s = NULL;
	n= 0;
	if (prm == sep)
		n = vj_by_ord(orlast);
	else {
		if (*prm == '+'
				|| (strcmp(prm, "%+") == 0))
			n = vj_by_ord(orlast);
		else
		if (*prm == '-'
				|| (strcmp(prm, "%-") == 0))
			n = vj_by_ord(orprev);
		else
		if (*prm == '%'
				&& (prm[1] != '\0'))
			s = prm + 1;
		else
		if (*prm)
			s = prm;
		if (s != NULL)
			id = atoi(s);
		if (id > 0)
			n = id;
	}
	if (n) {
		if (n > 0 && n < VJOB_MAX) {
			if (vj[n].pid) {
				return (n);
			}
		}
	} else {
		if (prm == sep)
			printf("no jobs\n");
		else
			printf("no such job id: %s\n", prm);
		}

	return (-1);
}

int v_fgbg(cmd0, tmpstr, bg)
wchar_t *cmd0;
char *tmpstr;
int bg;
{
	pid_t pid;
	int n, ws;
	char *o;
	struct termios ts;
	/*int vt_backpass = 0;*/

	n = fgbg_n(cmd0, tmpstr);
	if (n < 0)
		return(-1);
	fgn_set(n);
	if ( bg) {
		o = " & ";
	} else {
		vj_adv(n);
		o = orwhat(n);
	}
	/*like vj_info*/
	printf("%%%-d %s  %-60.60s\n", n, o, vj[n].cmd);

/*	blk_on();*/
	pid = vj[n].pid;

	if (kill(0 - pid, 0) >= 0) {
		/*vj[n].wstatus = 0; /* reset shown status */
		if (bg) {
			setpgid(pid, v.pgrp);
		} else {
			setpgid(pid, 0);
			if (vj[n].ts_saved) {
				/*restore saved tty mode on suspend*/
				memcpy(&ts, &(vj[n].ts), sizeof(struct termios));
				fflush(stdout);
				tcsetattr(STDOUT_FILENO, TCSADRAIN, &ts);
			} else {
				io_set(VT_OFF);
			}
			/* give terminal */
			if (Tpgrp(pid, "fgbg+", "FG/BG tcsetpgrp") < 0) {
				return(-4);
			}
		}
		if (kill(0 - pid, SIGCONT) < 0) {
			perror("vash: fg/bg continue");
			/*vt_backpass = 1;*/
		}
	}/* else {
		w_emsg("vash: job done!");
		v_disown(cmd0, tmpstr);
	}*/
	if (bg) {
		/* back terminal to vash */
		io_set(VT_ON);
		Tpgrp(v.pgrp, "fgbg-", "back v.pgrp");
		return(-4);
	}

	return(0);
}

int v_fg(cmd0, tmpstr)
wchar_t *cmd0;
char *tmpstr;
{
	int ret;
	if((ret = v_fgbg(cmd0, tmpstr, 0)) == 0) {
		/* wait the continued job in foreground */
		return(-7 /*-7 -5*/);
	} else
		return( ret );
}

int v_bg(cmd0, tmpstr)
wchar_t *cmd0;
char *tmpstr;
{
	int ret;
	if((ret = v_fgbg(cmd0, tmpstr, 1)) == 0) {
		/* release to run in background */
		return (-6);
	} else
		return (ret);
}

/* clear job slot, forget job id*/
int v_disown(cmd0, tmpstr)
wchar_t *cmd0;
char *tmpstr;
{
	int n;

	n = fgbg_n(cmd0, tmpstr);
	if (n < 0)
		return(-1);
	/*TODO really forget (to be not the parent of)*/
	printf("\r\nforget job:\r\n"); vj_info(n);
	vj_clr(n);
	return (-4);
}

/* stop a job sending TSTP signal then get vash prompt*/
int v_stop(cmd0, tmpstr)
wchar_t *cmd0;
char *tmpstr;
{
	int n, ws, ok;

	n = fgbg_n(cmd0, tmpstr);
	if (n < 0)
		return(-5);

	if (vj[n].pid) {
			if ((ok = kill( 0 - vj[n].pid, SIGSTOP)) < 0) {
				perror("vash: stop");
			}
		/*printf("(%-d) %-45.45s\n", n, vj[n].cmd); /*debug*/
	}
	if (ok == 0) {
		return(-6 /*-7*/);
	}
	return (-6);
}

/* send kill signal to job slot, or
 * TODO try /bin/kill if not match syntax */
int v_kill(cmd0, tmpstr)
wchar_t *cmd0;
char *tmpstr;
{
	int n, pid;

	n = fgbg_n(cmd0, tmpstr);
	if (n < 0)
		return(-5);

/*	vj_info(n);*/
	if ((pid = kill( 0 - vj[n].pid, SIGKILL)) < 0)
		perror("vash: kill");
	else
		vj_clr(n);
	if (pid == 0) {
		return(-6 /*-7*/);
	}
	return (-4);
}



