/*
 * test for compile and run utf8 encoded C binary
 * in different locale environment
 * 
 */

#include <stdlib.h>
#include <stdio.h>
#include <locale.h>
#include <wchar.h>

int main()
{
    int pexit;
    int i;

    /* there are string constants encoded as UTF-8/Unicode */
    wchar_t *msgs[] = {L"Привет", L" юникод", L"/utf8/koi8/", NULL};

    if (!setlocale(LC_CTYPE, "")) {
      fprintf(stderr, "Can't set the specified locale! "
              "Check LANG, LC_CTYPE, LC_ALL.\n");
      return 1;
    }

    for(i = 0; msgs[i] != NULL; i++) {
	pexit = printf("%ls\n", msgs[i]);
	printf("printf returns '%d'\n\n", pexit);
    }

    return 0;
}
