/*
 * VASH - visual assistant for shell
 * Copyright (c) 1990-2025 Sergey Vovk <vsv>
 *
 * License: GPL and/or MIT,
 * see files COPYING and LICENSE
 */

/*
 * vjobs.h
 *
 *  Created on: 26 мар. 2025 г.
 *      Author: vsv
 */

#include <unistd.h>
#include <termios.h>
#include <line.h>
#include <signal.h>
#include <sys/wait.h>

#ifndef VASH_VJOBS_H
#define VASH_VJOBS_H

extern int v_exit  (wchar_t *, char *);
extern int v_cd    (wchar_t *, char *);
extern int v_jobs  (wchar_t *, char *);
extern int v_fg    (wchar_t *, char *);
extern int v_bg    (wchar_t *, char *);
extern int v_disown(wchar_t *, char *);
extern int v_stop  (wchar_t *, char *);
extern int v_kill  (wchar_t *, char *);

typedef struct
{
	int  order;
	pid_t pid;
	int  wstatus;
	int  notify;         /* cleared when vjob shown, raised on any change */
	int  done;
	siginfo_t  si;
	char cmd[STRBUF];
	char dir[STRBUF];
	struct termios ts;
	int ts_saved;
} VJOB;

extern VJOB vj[];
#define VJOB_MAX 8		/* size of job slots array, slot #0 never used */

extern int reapchk(pid_t);
extern void reapnowt(pid_t);

extern void sistat(char *, int);
extern void wstat(int, char *);

extern int vj_new(int);
extern int vjobs_n();
extern void vj_clr(int);
extern int vj_adv(int);
extern void vj_info(int);
extern pid_t vj_pid(int);
extern int vj_by_pid(pid_t);
extern void vj_notify(int);

extern int  fgn_get(void);
extern void fgn_set(int);


#endif /* VASH_VJOBS_H_ */
