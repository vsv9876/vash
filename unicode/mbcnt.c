/*
 * test for 
 * mbrtowc
 * 
 */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <locale.h>
#include <string.h>
#include <wchar.h>
    
int main(argc, argv)
int argc;
char *argv[];
{
    int pexit;
    int n;
    size_t len;
    mbstate_t ps = { 0 };
    wchar_t inp[120+1];
    char    ins[120*4 + 4];
    char *p;
    
    printf ("argc=%d\n", argc);
    if (argc == 1) {
	if (!setlocale(LC_CTYPE, "")) {
		fprintf(stderr, "Can't set the specified locale! "
			"Check LANG, LC_CTYPE, LC_ALL.\n");
		return 1;
		}
    }

    while(1) {
	printf("\n-->");
	fgets(ins, 120, stdin);
	n = strlen(ins);
// 	len = wcslen(inp);
	p = ins;
	wcscpy(inp, L"--------------------------------");
	len = mbsrtowcs(inp, &p, 11, &ps);
	if (inp[0] == L'\n') break;
	printf("%2d: %3d %3d >%ls", mbsinit(&ps), n, len, inp);
    }
    exit(0);
    return 0;
}
