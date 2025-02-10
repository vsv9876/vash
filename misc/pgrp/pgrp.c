#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>

int main(argc, argv)
int argc;
char *argv[];
{
	pid_t pid, ppid;
	pid_t sid, psid;
	pid_t pgid, pgrp;

	pid_t tcpgrp, tcsid;

	pid = getpid();
	ppid = getppid();
	sid = getsid(pid);
	psid = getsid(ppid);
	pgid = getpgid(pid);
	pgrp = getpgrp();

	printf("pid=%-d, ppid=%-d pgid=%-d\n", pid, ppid, pgid);
	printf("sid=%-d, psid=%-d pgrp=%-d\n", sid, psid, pgrp);
/*
	printf("pgid=%-d, pgrp=%-d\n", pgid, pgrp);
*/
	tcpgrp = tcgetpgrp(0);
	tcsid = tcgetsid(0);
	printf("tcpgrp=%-d, pcsid=%-d\n", tcpgrp, tcsid);
	return(0);
}
