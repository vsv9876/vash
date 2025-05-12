/*
 * testing pgrp
 */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>

int ttyfd = STDERR_FILENO;
void out_pid(pid)
pid_t pid;
{
	pid_t ppid;
	pid_t sid, psid;
	pid_t pgid, pgrp;

	pid_t tcpgrp, tcsid;

	if (pid < 0) {

	    if ( (tcsid = tcgetsid(ttyfd)) < 0 )
	    	perror("tcgetsid(ttyfd): ");

	    if ( (tcpgrp = tcgetpgrp(ttyfd)) < 0 )
	    	perror("tcgetpgrp(ttyfd)");

	    printf("terminal fg[%-d]  sid/pgrp: %7d/%-7d\r\n", ttyfd, tcsid, tcpgrp);
	    pid = getpid();
	    ppid = getppid();
	    pgrp = getpgrp();
	    printf("pid/ppid: %7d/%-7d  pgrp: %7d\r\n", pid, ppid, pgrp);
	    sid = getsid(pid);
	    psid = getsid(ppid);
	    pgid = getpgid(pid);
	} else {
		sid = getsid(pid);
		psid = getsid(ppid);
		pgid = getpgid(pid);
	}
	printf( "pid/pgid: %7d/%-7d ", pid, pgid);
	printf(" sid/psid: %7d/%-7d\r\n", sid, psid);
}

int main(argc, argv)
int argc;
char **argv;
{
    pid_t pid;
    int ac;
    
    ac = argc;
    while(1) {
    	printf("\r\n");
    	if (ac == 1)
    		out_pid(-1);
    	else {
    		for(ac = 1; ac < argc; ac++) {
    			pid = atoi(argv[ac]);
    			out_pid(pid);
    		}
    	}
    	if (ac > 1)
    		return(1);

    	if (ac == 1) {
    		printf("------------------\n");
    		fflush(stdout);
    		sleep (3);
    	}
    }
    return(0);
}
