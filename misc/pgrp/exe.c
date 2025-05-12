/*
 * man 2 fork
 */
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int
main(void)
{
   pid_t pid;
#if 0
   if (signal(SIGCHLD, SIG_IGN) == SIG_ERR) {
       perror("signal");
       exit(EXIT_FAILURE);
   }
#endif
   pid = fork();
   switch (pid) {
   case -1:
       perror("fork");
       exit(EXIT_FAILURE);
   case 0:
       puts("Child exiting.");
       exit(EXIT_SUCCESS);
   default:
       printf("Child is PID %jd\n", (intmax_t) pid);
       puts("Parent exiting.");
       exit(EXIT_SUCCESS);
   }
}

