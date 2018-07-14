#include <stdio.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>

main(int argc, char **argv)
{
        struct stat buf;

        if (argc != 2) {
                fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
                exit(1);
        }

        if (stat(argv[1], &buf) < 0) {
                fprintf(stderr, "stat() failed on \"%s\" (%s)\n",
                        argv[1],
                        strerror(errno)
                        );
                exit(1);
        }

        printf("Access time for \"%s\" was %lu secs -- %s",
                        argv[1],
                        (unsigned long) buf.st_atime,
                        ctime(&buf.st_atime)
                );
        return 0;
}
