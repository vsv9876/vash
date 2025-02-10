/*
**      +----------+    БИБЛИОТЕКА ВВОДА-ВЫВОДА
**     (c) linlib  !    ДЛЯ АЛФАВИТНО-ЦИФРОВЫХ
**      +----------+    ВИДЕОТЕРМИНАЛОВ
**/

/*
 *      $Header: lin150.c,v 1.1 90/12/27 16:28:49 vsv Rel $
 *
 *      $Log:	lin150.c,v $
 * Revision 1.1  90/12/27  16:28:49  vsv
 * ВЕРСИЯ LINLIB_3
 * 
 * Revision 3.3  89/08/29  15:14:37  vsv
 * ВЕРСИЯ LINLIB_3
 *
 * Revision 3.2  88/07/28  09:25:47  vsv
 * ДОБАВЛЕНА ФУНКЦИЯ w_strn
 *
 * Revision 3.1  88/05/23  10:24:45  vsv
 * ИСПРАВЛЕНЫ МЕЛКИЕ ПЛЮХИ ВАРИАНТА ДЛЯ RT-11
 *
 * Revision 3.0  87/12/21  12:16:07  vsv
 * ПРЕДВАРИТЕЛЬНЫЙ ВЫПУСК.
 *
 */

#include <stdio.h>
#include "line.h"
#include "line0.h"

#ifdef VTTOUT_DEBUG
#include <stdlib.h>
#include <time.h>
#endif

/*
**      ВЫВОД НА ЭКРАН СТРОК В КОДАХ ТЕРМИНАЛА
**      И С ПЕРЕКОДИРОВКОЙ (ПО РЕЖИМАМ ДРАЙВЕРА)
**/

#ifdef RT11
/*
 * ДЛЯ RT-11 НЕ ИМЕЕТ СМЫСЛА ДЕЛАТЬ ВСЮ ЭТУ ОБРАБОТКУ,
 * ЗДЕСЬ ПРОСТО ЗАГЛУШКИ.
 */

w_raw(s)
register char *s;
{
	fprintf(vttout, "%s", s);
}

w_str(s)
register char *s;
{
	fprintf(vttout, "%s", s);
}

w_strn(s, n)
register char *s;
register int n;
{
	n -= strlen(s);
	fprintf(vttout, "%s", s);
	while(--n>=0) w_chr(' ');
}

w_chr(c)
register int c;
{
	putc(c, vttout);
}


int escseq(c)
register int c;
{
	return(c);
}

#endif /* RT11 */
#ifdef DEMOS2
#ifdef DEMOS2CYR_OBSOLETE
#include <sgtty.h>

extern  int osgflg;             /* ФЛАГИ ДРАЙВЕРА (old.sg_flags) */
#endif

int w_putc(c)
int c;
{
#ifdef VTTOUT_DEBUG
	int nanosec = 0;
	struct timespec req = { 0 };
	struct timespec rem = { 0 };
	char *sleepenv;

	if ((sleepenv = getenv("VTTOUT_SLEEP")) != NULL) {
		req.tv_sec  = 0;
		req.tv_nsec = 1000 * atoi(sleepenv); /* in microseconds, not nanoseconds */
	}
#endif

	fputc(c, vttout);
#ifdef VTTOUT_DEBUG
	if (sleepenv != NULL) {
		fflush(vttout);
		nanosleep(&req, &rem);
	}
#endif
}
/*--------------------------------*/
/* ВЫВОД СТРОКИ В КОДАХ ТЕРМИНАЛА */
/*--------------------------------*/
/* на самом деле с предкомпенсацией,
 * так, чтобы драйвер снова перекодировал, a
 * терминал понял правильно...
 */
void w_raw(s)
const char *s;
{
#ifndef USE_W_RAW
	extern int tputs();
	/* modern documented way to use terminfo/termcap string capabilities */
	tputs(s, 1, w_putc);
#else
	int c;
	for(; s!=(char*)NULL && *s!=0; s++) {
		c = *s;
#ifdef DEMOS2CYR_OBSOLETE
		/* этого достаточно, потому что
		 * UCASE все равно подавлен :
		 */
		if(((osgflg & LCASE)!=0) && (c >= 0140) && (c < 0177))
			c |= 0200;
#endif
		w_putc( c );
		/*fputc( c, vttout );*/
		/* fputc( c, stderr ); */
		/* fputc( c, stdout ); */
	}
#endif
}

/*---------------------------------*/
/* ВЫДАТЬ СТРОКУ СИМВОЛОВ НА ЭКРАН */
/* С ПЕРЕКОДИРОВКОЙ                */
/*---------------------------------*/
/*
 * Исправляются символы, которых нет на
 * терминалах с двумя регистрами,
 * так, чтобы они занимали одну позицию,
 * и были на что-то похожи.
 */
void w_str(s)
const char *s;
{

	for(; *s; s++) {
		w_chr(*s);
	}
}

w_strn(s, n)
/*---------------------------------------------------*/
/* ВЫДАТЬ СТРОКУ ЗАДАННОЙ ДЛИНЫ, ДОПОЛНИТЬ ПРОБЕЛАМИ */
/*---------------------------------------------------*/
register const char *s;
register int n;
{
	n -= strlen(s);
	for(; *s; s++) {
		w_chr(*s);
	}
	while(--n>=0) w_chr(' ');
}

/*------------------------------------------*/
/* выдать символ с частичной перекодировкой */
/*------------------------------------------*/
w_chr(c)
int c;
{
	extern SCRN scrn;
	extern int osgflg;
	register int oc;        /* символ для вывода */

#ifdef DEMOS2CYR_OBSOLETE
	if(osgflg & LCASE) {
		switch(c) {
		case '`' :
			oc = '\'';
			break;
		case '{' :
			oc = '(';
			break;
		case '|' :
			oc = '!';
			break;
		case '}' :
			oc = ')';
			break;
		case '~' :
			oc = '^';
			break;
		default :
			oc = c;
			break;
		}
	} else {
		oc = c;
	}
#else
	oc = c;
#endif /* DEMOS2CYR */
#ifdef  ASCII7
	oc &= 0177;
#else
	oc &= 0xFF;
#endif
	/* if byte is a part of UTF-8, advance cursor position only on 1st one */
	if (mb_cur_max == 1) {
		/* single-byte encoding is active */
		scrn.sc_co += 1;
	} else {
		/* 1st byte of UTF-8 symbol or ASCII symbol */
		if (oc <= 0177 || (oc & 0xC0) != 0x80)
			scrn.sc_co += 1;
	}
	/* prevent write_out(printing) outside of logical frame */
	if (
			scrn.sc_li <= (lframe->baseli + lframe->maxli)
			&& scrn.sc_li > lframe->baseli
			&& scrn.sc_co <= (lframe->baseco + lframe->maxco)
			&& scrn.sc_co > lframe->baseco
	   )
	{
		w_putc( oc );
		/*putc(oc, vttout);*/
		/* putc(oc, stdout); */
	}
}
/*---------------------------------------------*/
/* Перекодировть символ ESC-последовательности */
/*---------------------------------------------*/
/*
 * Примечание:
 *      когда появится драйвер, умеющий по таймауту
 *      определять, что символы входят в
 *      последовательность функциональной
 *      клавиши, отпадет необходимость в
 *      этой функции.
 */

int escseq(cod)
int cod;
{
	register int c;

	if((c=cod) <= 0177)
		return( c );

	c = 0177 & c;  /* избавиться от знакового разряда */

#ifndef DEMOS2CYR
	return( c );
#else
	if(c >= 0100 && c <= 0176) {
		if(osgflg & LCASE)  goto rev_cod;
		if(osgflg & CYRILL) {
		    switch(osgflg & CSTYLE) {
					/* КОД НЕ ПЕРЕВОРАЧИВАЕТСЯ */
		    case CBITS8   :
		    case CS_TYPE0 :
		    case CS_TYPE3 :
			break;
					/* ПРИХОДИТ ПЕРЕВЕРНУТЫЙ КОД */
		    case CS_TYPE2 :
		    case CS_TYPE1 :
			goto rev_cod;
			break;
		    };
		goto return_cod;
		}
	}
rev_cod:        /* перевернуть код */
	if(c >= 0100 && c <= 0137)
		c += 040;
	else
		c -= 040;
return_cod:
	return( c );

#endif /* DEMOS2CYR */
}
#endif /* DEMOS2 */
