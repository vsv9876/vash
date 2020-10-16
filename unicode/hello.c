/*
 * test for compile and run utf8 encoded C binary
 * in different locale environment
 * 
 */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <locale.h>
#include <wchar.h>

#if 8 <= sizeof(char *)
typedef unsigned long linptr_t;
#elif 4 == sizeof(char *)
typedef unsigned int linptr_t;
#else
typedef unsigned linptr_t;
#endif


int main(argc, argv)
int argc;
char *argv[];
{
    int pexit;
    int i;
    wint_t koi8chr = 0xFE; /* illegal utf8 code */
    
    /* there are string constants encoded as UTF-8/Unicode */
    wchar_t *msgs[] = {L" abs-абв", L"/utf8/koi8/", NULL};
    char    *msmb[] = { " abs-абв",  "/utf8/koi8/", NULL};

    printf ("argc=%d\n", argc);
    if (argc == 1) {
	if (!setlocale(LC_CTYPE, "")) {
		fprintf(stderr, "Can't set the specified locale! "
			"Check LANG, LC_CTYPE, LC_ALL.\n");
		return 1;
		}
    }

    for(i = 0; msgs[i] != NULL; i++) {
		pexit = printf("'%s'  -- ", msgs[i]);
		printf("%%s = %d;  ", pexit);
		pexit = printf("'%ls'  -- ", msgs[i]);
		printf("%%ls = %d\n", pexit);
    }
    for(i = 0; msmb[i] != NULL; i++) {
		pexit = printf("'%s'  -- ", msmb[i]);
		printf("%%s = %d;  ", pexit);
    }
    printf("\n");
    printf ("WCHAR_MAX=0x%x\n", WCHAR_MAX);
    printf ("WCHAR_MIN=0x%x\n", WCHAR_MIN);
    printf ("sizeof(wint_t)=%d\n", sizeof(wint_t));
    printf ("sizeof(wchar_t)=%d\n", sizeof(wchar_t));
    printf ("sizeof(linptr_t *)=%d\n", sizeof(linptr_t *));
    printf ("MB_CUR_MAX=%d\n", MB_CUR_MAX);

    return 0;
}
