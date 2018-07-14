/*
 * Запускалка для ash.
 *
 * Выполняются команды sh из файла $HOME/.ashrc;
 * Для этого стартуется sh, в рамках его процесса
 * выполняются команды из .ashrc,
 * затем процесс sh заменяется на программу ash
 * вызовом exec.
 */

#include <strings.h>

extern char *getenv();
extern char **environ;

char   *info = "Check installation...Sorry...\n";

main()
{
	char rccmd[100];
	char rcfile[100];
	char argvex[100];
	char *home;

	rcfile[0] = '\0';
	if ((home = getenv("HOME")) != (char *)0) {
		strcpy(rcfile, home);
		strcat(rcfile, "/.ashrc");
	}
	if (rcfile[0] == '\0' || access(rcfile, 4) < 0) {
		strcpy(rcfile, DESTLIB);
		strcat(rcfile, "/.ashrc");
	}
	if (access(rcfile, 4) == 0) {
		strcpy(rccmd, ". ");
		strcat(rccmd, rcfile);
		strcat(rccmd, "; exec ash");
		execl("/bin/sh", "ashrc", "-c", rccmd, 0 );
	}
	else    {
		strcpy(argvex, DESTBIN);
		strcat(argvex, "/ash");
		execl(argvex, "-ash", 0);
	}
	write(2, info, strlen(info));
	exit(1);
}
