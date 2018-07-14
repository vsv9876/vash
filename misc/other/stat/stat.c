#include <sys/types.h>
#include <sys/stat.h>

#include <stdio.h>
#include <ctype.h>
#include <time.h>

/*
 * Распечатать st_mtime, st_atime, st_ctime для указанного файла
 */

main(argc, argv)
int argc;
char **argv;
{
    struct stat statbuf;

    long atimer, ctimer, mtimer;

    if (argc != 2) {
	fprintf(stderr, "Usage: %s file\n", *argv[0]);
    }
    else {
	if (stat(argv[1], &statbuf) < 0) {
		perror(argv[1]);
		exit(1);
	}
	mtimer = statbuf.st_mtime;
	atimer = statbuf.st_atime;
	ctimer = statbuf.st_ctime;
#ifndef VERBOSE
	printf("mtime=%9ld\natime=%9ld\nctime=%9ld %s\n",
	mtimer, atimer, ctimer, argv[1]
	);
#else
	printf("mtime = %s", ctime(&mtimer));
	printf("atime = %s", ctime(&atimer));
	printf("ctime = %s", ctime(&ctimer));
	printf("----------------- %s\n", argv[1]
	);
#endif
    }
    exit (0);
}
