/*
**      +----------+    БИБЛИОТЕКА ВВОДА-ВЫВОДА
**     (c) linlib  !    ДЛЯ АЛФАВИТНО-ЦИФРОВЫХ
**      +----------+    ВИДЕОТЕРМИНАЛОВ
**/

/*
 *      $Header$
 *
 *      $Log$
 */

/*#include <stdio.h>*/
#include <wchar.h>
#include "line.h"


/*
 * поддержка UTF8
 */

int u8wcsn(dst, src, n)
char    *src;
wchar_t *dst;
int      n;
{
	int len;
	mbstate_t ps = { 0 };

    len = mbsrtowcs(dst, &src, n, &ps);

	return len;
}

int u8wcs(dst, src)
char    *src;
wchar_t *dst;
{
	return(u8wcsn(dst, &src, MAXLICO*4));
}

int u8len(str)
char *str;	/* multibyte string */
{
	int len;

    len = u8wcs(NULL, str);

    return len;
}

