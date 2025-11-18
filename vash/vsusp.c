/*
 * VASH - visual assistant for shell
 * Copyright (c) 1990-2025 Sergey Vovk <vsv>
 *
 * License: GPL and/or MIT,
 * see files COPYING and LICENSE
 */

#include <sys/types.h>
#include <unistd.h>

#include <ediag.h>
#include <stdio.h>      /* ЗАГОЛОВОК СТАНДАРТНОЙ БИБЛИОТЕКИ ВВ/ВЫВ */
#include "line.h"       /* ФАЙЛ-ЗАГОЛОВОК LINLIB */


#include <signal.h>

#include "assist.h"


#if !defined(SIGTSTP)
v_susp()
{
       w_emsg("JCL are not supported");
       return(FALSE);
}

#else /* SIGTSTP */

#ifndef JCLsuspend
v_susp()
{
       w_emsg("JCLsuspend are NOT configured");
       return(FALSE);
}
#else

/*
 * command 'suspend' for vash
 * allowed when vash process is not session leader
 * POSIX version
 */
v_susp(cmd)
const char *cmd;
{
	if (v.sid && v.sid == v.pid) {
		w_emsg("session leader cannot be suspended");
		return(FALSE);
	}
	else {
		scrlst();
		io_set(VT_OFF);
		putchar('\n');

/* 		kill(v.pid, SIGSTOP);  /* SIGTSTP only vash process signalled */
		raise(SIGSTOP);

		io_set(VT_ON);
		scrlnl();
	}
	return(TRUE);
}

#endif /* JCLsuspend */

#endif /* SIGTSTP */
