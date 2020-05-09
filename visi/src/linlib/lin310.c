/*
**      +----------+    БИБЛИОТЕКА ВВОДА-ВЫВОДА
**     (c) linlib  !    ДЛЯ АЛФАВИТНО-ЦИФРОВЫХ
**      +----------+    ВИДЕОТЕРМИНАЛОВ
**/

/*
 *      $Header: lin310.c,v 1.1 90/12/27 16:29:12 vsv Rel $
 *
 *      $Log:	lin310.c,v $
 * Revision 1.1  90/12/27  16:29:12  vsv
 * ВЕРСИЯ LINLIB_3
 * 
 * Revision 3.3  89/08/29  15:16:44  vsv
 * ВЕРСИЯ LINLIB_3
 * 
 * Revision 3.2  88/07/27  16:34:40  vsv
 * РАБОТА С КЛАВИШЕЙ 'DEL' ПРИВЕДЕНА К ОБЩЕМУ ВИДУ
 * 
 * Revision 3.1  88/06/27  15:20:48  vsv
 * РЕВИЗИЯ АРХИВА RCS
 * 
 * Revision 3.0  87/12/21  12:19:24  vsv
 * ПРЕДВАРИТЕЛЬНЫЙ ВЫПУСК.
 * 
 */


#include <stdio.h>
#include "line.h"
#include "line0.h"

extern  int     ttyinp();
extern  KBF     kbf[];

extern int mb_cur_max;

/*------------------------------------------------------*/
/* ВЕРНУТЬ cod, ЕСЛИ НИЧЕГО НЕ СОВПАЛО, ЛИБО КОД linlib */
/*------------------------------------------------------*/
kbcod  k_pars(cod)
int cod;
{
	register int n;
	register KBF *kbfp;
	register char *esccod;
	int  like;               /* ФЛАГ: ЕСТЬ ПОХОЖИЕ ШАБЛОНЫ */
	char kbuf[10];

	kbuf[0] = cod & 0177;
	kbuf[1] = 0;
	like = 1;               /* НАДО С ЧЕГО-ТО НАЧАТЬ */
	for(n=1; like!=0; ) {

		like = 0;
		for(kbfp = kbf; kbfp->t_key; kbfp++) {
			if(strncmp(kbuf, esccod=kbfp->t_cap, n) == 0) {
				like = 1;
				if(strcmp(kbuf, esccod) == 0)
					return(kbfp->t_key);
				else
					continue;
			}
		}
		if(like == 0) {
			if(n == 1)
				return((kbcod)cod);
			else
				return( 0);     /* НИЧЕГО ПОХОЖЕГО */
		}
		else {
			/* получить код кодовой последовательности -
			 * здесь есть трудности,
			 * связанные с перекодировкой на вводе
			 * в драйвере и в самой клавиатуре,
			 * если вводятся русские буквы.
			 */
/*                      cod=ttyinp();           */
			kbuf[n++] = escseq(   ttyinp() );
			kbuf[n  ] = 0;
		}
	}
	return(KBCOD('O','O'));   /* НА ВСЯКИЙ СЛУЧАЙ ДЛЯ ОТЛАДКИ */
}

/*
 * собрать Unicode codepoint
 */
kbcod r_codep(cod)
kbcod cod;
{
	unsigned int cc;      /* current byte from input stream */
	unsigned int cbytes;  /* bytes count in codepoint */
	unsigned int cbits;   /* bits encoded */
	unsigned int cmask;   /* significant bits in current byte */

	if (cod <= 0b01111111) return cod; /* ASCII */
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
		cc = ttyinp();
		if ((0b11000000 & cc) != 0b10000000) {
		    /* Error: premature end of multibyte sequence */
		    return (0x0); /*(0xffffffff);*/
		}
		cod = cod << 6;
		cod |= (cc & 0b00111111);
	}
	return cod;
}

int u8nopass = 0; /* pass ascii-only */

/*---------------------*/
/* ВЕРНУТЬ КОД КЛАВИШИ */
/*---------------------*/
kbcod r_key()
{
	register kbcod cod ; /* next byte from input stream */
	register pars ;         /* k_pars() = */

	cod = ttyinp();
	if(cod == -1)
		return( -1);    /* АСИНХРОННЫЙ РЕЖИМ - БЕЗ ОЖИДАНИЯ */
#ifdef KOI8_RETRO
	else
		cod &= 0377;    /* ПОДАВИТЬ ЗНАКОВОЕ РАСШИРЕНИЕ int=char */

	if(cod > 0200 && cod < 0300)
		cod &= 0177;    /* А ВДРУГ "РУССКИЙ" #,%,?, И Т.Д. */
#endif
	/* ТЕРМИНАЛО-ЗАВИСИМЫЙ КОД */
	pars = k_pars(cod);
	if(cod != pars)
		return(pars); /* OK, key recognized from termcap/terminfo database */

#if 0
	/* ОБРАБОТКА РУССКИХ И АНГЛИЙСКИХ ПЕЧАТНЫХ КОДОВ *//*TODO : utf8 parsing will be there*/
	else if(((cod < 0377)&&(cod > 0277))
	|| ((cod > 037)&&(cod < 0177)))
		return(cod);
#endif
	if (cod > 0177 && mb_cur_max > 1) {
		cod = r_codep(cod);
		if (u8nopass) {	/* suppress upper codes from utf8/unicode table */
			cod = L'\0';
		}
	}
	/* ASCII DEL */
	if (cod == 0177) {
		return(KBCOD('d','e'));
	}
	else if(cod > 0 && cod < 040) {  /* Nonprintable ASCII */
		return( KBCTL(cod + ('A'-'\001')));
	}
	else {
		return(cod);   /* finish, parsed */
	}
}
