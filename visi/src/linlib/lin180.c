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

#include <stdlib.h>
#include <alloca.h>
#include <stdio.h>
#include <wchar.h>
#include "line.h"


/*
 * поддержка UTF8
 */

/* копировать n символов UTF-8 в строку wchar_t */
int u8wcsn(dst, s, n)
wchar_t *dst;
const char    *s;
int      n;
{
	int len;
	mbstate_t ps = { 0 };

    len = mbsrtowcs(dst, &s, n, &ps);

	return len;
}

/* копировать строку UTF-8 в строку wchar_t */
int u8wcs(dst, src)
char    *src;
wchar_t *dst;
{
	return(u8wcsn(dst, src, MAXLICO*4));
}

/* вернуть количество символов (не байт) в строке UTF-8 */
int u8len(str)
char *str;	/* multibyte string */
{
	int len;

    len = u8wcs(NULL, str);

    return len;
}

/* копировать n символов UTF-8, из строки в строку, завершить нулем */
int u8mbsn(dst, s, n)
char *dst;
char *s;
int n;
{
	wchar_t *tmp = alloca((n+1)*sizeof(wchar_t));;
	const wchar_t *from = tmp;
	mbstate_t ps = { 0 };
	int len;

	/*tmp = alloca((n+1)*sizeof(wchar_t));*/
	u8wcsn(tmp, s, n);
	//tmp[n] = L'\0';
	len = wcsnrtombs(dst, &from, n, 4*MAXLICO, &ps);
	return(len);
}


/* p++ -- указатель на следующий UTF-8 символ в строке, вернуть NULL если указывает на '\0' */
char *u8pxx(p, wc)
char *p;
wchar_t *wc;
{
	char stmp[8];
	char *s;
	int cod;

	unsigned int cc;      /* current byte from input string */
	unsigned int cbytes;  /* bytes count in codepoint */
	unsigned int cbits;   /* bits encoded */
	unsigned int cmask;   /* significant bits in current byte */

	cod = *p++;
	if (cod == '\0') {
		p = NULL;
	} else {
		if (cod <= 0b01111111) return p; /* ASCII - 1 byte plus*/
		cc = cod;
		if         ((0b11111000 & cc) == 0b11110000) {
			cmask = 0b00000111; cbits = 3; cbytes = 4;
		} else if  ((0b11110000 & cc) == 0b11100000) {
			cmask = 0b00001111; cbits = 4; cbytes = 3;
		} else if  ((0b11100000 & cc) == 0b11000000) {
			cmask = 0b00011111; cbits = 5; cbytes = 2;
		}
		cod = (cc & cmask);/* << cbits;*/
		while (cbytes > 1) {
			cbytes -= 1;
			cc = *p++;
			if ((0b11000000 & cc) != 0b10000000) {
				/* Error: premature end of multibyte sequence */
				return (NULL); /*(0xffffffff);*/
			}
			cod = cod << 6;
			cod |= (cc & 0b00111111);
		}
	}
	if (wc != (wchar_t *)NULL) *wc = cod;
	return p;
}
