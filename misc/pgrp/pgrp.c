#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>

void out_pid(pid)
pid_t pid;
{
	pid_t ppid;
	pid_t sid, psid;
	pid_t pgid, pgrp;

	pid_t tcpgrp, tcsid;

	if (pid > 0) {
	    printf("pid=%-d: ", pid);
	    sid = getsid(pid);
	    psid = getsid(pid);
	    pgid = getpgid(pid);
	}
	else {
	    tcsid = tcgetsid(0);
	    tcpgrp = tcgetpgrp(0);
	    printf("pcsid=%-d, tcpgrp=%-d\n", tcsid, tcpgrp);
	    pid = getpid();
	    ppid = getppid();
	    pgrp = getpgrp();
	    printf("pid=%-d, ppid=%-d, pgrp=%-d\n", pid, ppid, pgrp);
	    sid = getsid(pid);
	    psid = getsid(ppid);
	    pgid = getpgid(pid);
	}

	printf("sid=%-d, psid=%-d, pgid=%-d\n", sid, psid, pgid);
}

int main(argc, argv)
int argc;
char **argv;
{
    pid_t pid;
    int ac;
    
    ac = argc;
    if (ac == 1)
	out_pid(-1);
    else
    for(ac = 1; ac < argc; ac++) {
        pid = atoi(argv[ac]);
		out_pid(pid);
    }
    return(0);
}
