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
int u8snwcs(dst, s, n)
wchar_t *dst;
const char    *s;
int      n;
{
	int len;
	mbstate_t ps = { 0 };
	mbsinit(&ps);
    len = mbsrtowcs(dst, &s, n, &ps);

	return len;
}

/* копировать строку UTF-8 в строку wchar_t */
int u8swcs(dst, src)
char    *src;
wchar_t *dst;
{
	return(u8snwcs(dst, src, MAXLICO*4));
}

/* копировать n символов в строку UTF-8 из строки wchar_t */
int wcsnu8s(dst, s, n)
wchar_t *dst;
const char    *s;
int      n;
{
	int len;
	mbstate_t ps = { 0 };
	mbsinit(&ps);
    len = wcsrtombs(dst, &s, n, &ps);
    		/*mbsrtowcs(dst, &s, n, &ps);*/

	return len;
}

/* копировать в строку UTF-8 из строки wchar_t */
int wcsu8s(dst, src)
char    *src;
wchar_t *dst;
{
	return(wcsnu8s(dst, src, MAXLICO*4));
}

/* вернуть количество символов (не байт) в строке UTF-8 */
int u8slen(str)
char *str;	/* multibyte string */
{
	int len;

    len = u8swcs(NULL, str);

    return len;
}

/* копировать n символов UTF-8, из строки в строку, завершить нулем */
int u8snu8s(dst, s, n)
char *dst;
char *s;
size_t n;
{
	wchar_t *tmp = alloca((n+1)*sizeof(wchar_t));;
	const wchar_t *from = tmp;
	size_t len;

	u8snwcs(tmp, s, n);
	tmp[n] = L'\0';
	len = wcstombs(dst, from, 4*MAXLICO);
	if (len < 0) *dst = '\0'; /* all string to be dropped */
	return(len);
}


/* convert string objects couple functions */
int u8owco(dst, s)
wcsobj_t *dst;
u8sobj_t *s;
{
	int size;
	size = u8o_size(s);
	dst->wco_sig = WCO_SIG;
	dst->wco_size = size;
	return (u8snwcs(dst->wcs, s->u8s, size));
}

int wcou8o(dst, so)
u8sobj_t *dst;
wcsobj_t *so;
{
	int size;
	size = wco_size(so);
	dst->u8o_sig = U8O_SIG;
	dst->u8o_sizeh = size / 256;
	dst->u8o_sizel = size % 256;
	return (wcsnu8s(&(dst->u8s), &(so->wcs), size));
}

/* cannot be implemented as a macro */
int u8o_size(u8o)
u8sobj_t *u8o;
{
	 return ((256*(u8o->u8o_sizeh))+(u8o->u8o_sizel));
}

int wco_size(wco)
wcsobj_t *wco;
{
	return( wco->wco_size);
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
