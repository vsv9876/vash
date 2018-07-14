#include <ediag.h>
#include <stdio.h>      /* ЗАГОЛОВОК СТАНДАРТНОЙ БИБЛИОТЕКИ ВВ/ВЫВ */
#include "line.h"       /* ФАЙЛ-ЗАГОЛОВОК LINLIB */

#ifdef JCLsuspend

#include <signal.h>

#if !defined(SIGTSTP)
v_susp()
{
       w_emsg("JCL are not supported, sorry...");
       return(FALSE);
}

#else /* SIGTSTP */

extern int getpgrp();   /* СБОРКА: cc ... -ljobs */

/*
       #ifndef BSD
       #ifdef SYSV
       extern int killpg();
       #endif
       #endif
*/

v_susp()
/*
 * Suspend for ash.
 * Job control (JCL) optionally required.
 *
 *      BSD 2.*, BSD 4.*, ДЕМОС 2.*, ISC UNIX (386/ix) V2.2
 */
{
	char *getenv();
	register char *env;
	int     pid;

	/* проверку на csh можно сделать и по другому */
	if ((env = getenv("SHELL")) != NULL) {
	    if( strcmp("/bin/csh", env) == 0 ) {

		pid = getpid();
		scrlst();       /* курсор в начало свитка */
		io_set(IO_TTYPE);
		putchar('\n');
#ifdef  INFOSUSPEND
		printf(ediag("To continue use 'fg' or %%\n",
			 " Для продолжения используйте команду fg или %%\n"));
#endif
		kill(pid, SIGTSTP);  /* Остановить только ash */
		io_set(IO_VIDEO);
		scrlnl();       /* свиток прокрутить */
		return( TRUE );
	    }
	}
	w_emsg(ediag("This is NOT a subshell of /bin/csh",
		     "Не факт, что Вы работаете с /bin/csh"));
	return(FALSE);
}

#endif /* SIGTSTP */
#endif /* JCLsuspend */

#ifndef JCLsuspend
p_susp()
{
       w_emsg("SUSPEND are NOT implemented...Sorry");
       return(FALSE);
}
#endif
