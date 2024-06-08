#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(argc, argv)
int argc;
char *argv[];
{
	pid_t pid, ppid;
	pid_t sid, psid;
	pid_t pgid, pgrp;

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
	return(0);
}
