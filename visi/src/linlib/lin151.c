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

#include <stdio.h>
#include <wchar.h>
#include "line.h"
#include "line0.h"

extern SCRN scrn;

/*TODO все функции w_w*() должны возвращать ошибку в случае
 * невозможности записи в vtttout,
 * либо сама функция w_wchr должна выбрасывать вызвавшую программу
 */

/*
 * выдать символ с перекодировкой в UTF-8 из внутреннего формата
 */
w_wchr(c)
wchar_t c;
{
	/*wint_t ret;*/
	unsigned char s[6];
	unsigned char *sp;
	int  cnt;
	/* 
	 * check if inside lframe(screen) borders, writeout,
	 * then save new position of cursor to be expected
 	 */
	/*if (scrn.sc_li <= lframe->maxli && scrn.sc_co <= lframe->maxco) {*/
	if (scrn.sc_li < (lframe->baseli + lframe->maxli)
	 && scrn.sc_co < (lframe->baseco + lframe->maxco)) {
		cnt = wctomb(s, c);
		sp = s;
		while(cnt-- > 0) {
			w_putc( *sp++ );
			/*fputc(*sp++, vttout);*/
		}
		/*ret = fputwc(c, vttout); fflush(vttout); if (ret == WEOF) { perror(""); exit(8); }*/
		/* putc(oc, stdout); */
	}
	scrn.sc_co += 1;
}

w_wcstrn(s, n)
/* ВЫДАТЬ СТРОКУ ЗАДАННОЙ ДЛИНЫ, ДОПОЛНИТЬ ПРОБЕЛАМИ */
register wchar_t *s;
register int n;
{
	n -= wcslen(s);
	for(; *s; s++) {
		w_wchr(*s);
	}
	while(--n>=0) w_wchr(L' ');
}

w_wcstr(s)
register wchar_t *s;
{
	for(; *s; s++) {
		w_wchr(*s);
	}
}


