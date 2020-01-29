/*
**      +----------+    БИБЛИОТЕКА ВВОДА-ВЫВОДА
**     (c) linlib  !    ДЛЯ АЛФАВИТНО-ЦИФРОВЫХ
**      +----------+    ВИДЕОТЕРМИНАЛОВ
**/

/*
 *      $Header: lin220.c,v 1.2 94/05/27 22:04:20 vsv Exp $
 *
 *      $Log:	lin220.c,v $
 * Revision 1.2  94/05/27  22:04:20  vsv
 * синхронизация архива RCS
 * 
 * Revision 1.1  90/12/27  16:29:01  vsv
 * ВЕРСИЯ LINLIB_3
 * 
 * Revision 3.2  89/08/29  15:16:00  vsv
 * ВЕРСИЯ LINLIB_3
 * 
 * Revision 3.1  88/06/27  15:19:57  vsv
 * РЕВИЗИЯ АРХИВА RCS
 * 
 */

#include <stdlib.h>
#include <string.h>
#include <ediag.h>
#include <stdio.h>
#include "line.h"
#include "line0.h"

/*
 * НАСТРОЙКА НА ТИП ТЕРМИНАЛА
 *
 * ПРОЦЕДУРЫ ДЛЯ ВЫПОЛНЕНИЯ НАСТРОЙКИ НА ТИП ТЕРМИНАЛА
 */

#include <ctype.h>

/*extern char *malloc();*/

#ifdef DEMOS2
extern  char *getenv();
#endif

#ifdef RT11
static  char ttynm[30] = "VT0:TRMCAP.VT0";

/*-------------------------------*/
/*  ПОЛУЧИТЬ ИМЯ ФАЙЛА НАСТРОЙКИ */
/*-------------------------------*/
char *
gttynm()
{
	FILE    *fopen();
	FILE    *i_fp;
	register char *s;

	for( s = &ttynm[2]; *s < '9'; *s += 1 ) {
		if((i_fp=fopen(ttynm, "r")) == NULL )
			continue;
		else {
			fclose(i_fp);
			ttynm[13] = *s;
			break;
		}
	}
	ttynm[0] = 'T';
	ttynm[1] = 'T';
	ttynm[2] = 'Y';
	return( ttynm );
}
#endif

/*  ЗАПРОСЫ ИЗ БИБЛИОТЕКИ termcap */
extern  int  tgetent(), tgetnum(), tgetflag();
extern  char *tgetstr();

extern  char *UP;       /* ВНЕШНИЕ ПЕРЕМЕННЫЕ ДЛЯ tgoto() */
extern  char *BC;

/* БУФЕР ОПИСАНИЙ linlib, И УКАЗАТЕЛЬ
 * ДЛЯ ОЧЕРЕДНОГО ЗАНЕСЕНИЯ В БУФЕР.
 */
static char *codesp = 0;

/* РАЗМЕРЫ ЭКРАНА */
extern  int     maxli;
extern  int     maxco;


/*---------------------*/
/* ОПИСАНИЕ КЛАВИАТУРЫ */
/*---------------------*/
extern  KBF   kbf[];
extern  char *tcapo[];


static int cannot()
/*-------------*/
/* ДИАГНОСТИКА */
/*-------------*/
{
#ifdef  DEMOS2
	fprintf(stderr, "%s '%s'\n",
	ediag("Bad environ", "НЕ УСТАНОВЛЕНА ПЕРЕМЕННАЯ"), "TERM");

#endif
#ifdef  RT11
	fprintf(stderr,
"=== НЕТ ФАЙЛА НАСТРОЙКИ :\n\r\
	TTY:TRMCAP.VT* (.ASSIGN NL: VT*:)\n\r");
#endif
	exit(1);

}

static int stripd(s1, s2)
/*----------------------------------------*/
/* ИЗБАВИТЬСЯ ОТ ЛИДИРУЮЩИХ ЦИФР ЗАДЕРЖКИ */
/* ЧИТАТЬ ИЗ s2, ПИСАТЬ В s1              */
/*----------------------------------------*/
register char *s1, *s2;
{
	while(isdigit(*s2))
		s2++;
	strcpy(s1, s2);
}


char *
gettcp(key)
/*-----------------------------------*/
/* ПОЛУЧИТЬ ESC-КОД ДЛЯ ЭЛЕМЕНТА key */
/*-----------------------------------*/
char *key;
{
	/* ЗАПИШЕМ НАЙДЕННУЮ ESC-ПОСЛЕДОВАТЕЛЬНОСТЬ
	 * В БУФЕР, ВЕРНЕМ УКАЗАТЕЛЬ НА НАЧАЛО
	 * СТРОКИ В БУФЕРЕ.
	 */
	register char *p, *pp;
	char *ptcap;
	char tbuf[64];          /* БУФЕР ОЧЕРЕДНОГО ОПИСАНИЯ */
	char *x;                /* УКАЗАТЕЛЬ ДЛЯ ОБРАЩЕНИЯ К tgetstr() */

	x = tbuf;
	pp = codesp;    /* ВСПОМНИТЬ, КУДА КЛАСТЬ ОЧЕРЕДНОЕ ОПИСАНИЕ */
	p = tgetstr(key, &x);          /* ОЧЕРЕДНОЕ ОПИСАНИЕ */
	if (p && *p!=0) { ; }
	else {  p = ""; }

	ptcap = pp;
#ifdef USE_W_RAW
	/* УДАЛИТЬ ЗАДЕРЖКИ, СОХРАНИТЬ ОПИСАНИЕ В БУФЕРЕ: */
	stripd(pp, p);
#else
	strcpy(pp, p);
#endif
	pp += (linptr_t)(strlen(pp)+1);

	codesp = pp;    /* ЗАПОМНИТЬ, КУДА КЛАСТЬ ОЧЕРЕДНОЕ ОПИСАНИЕ */
	return(ptcap);
}

hw_set()
/*--------------------------------------*/
/* ВЫПОЛНИТЬ НАСТРОЙКУ НА ТИП ТЕРМИНАЛА */
/*--------------------------------------*/
{
	register char *p;
	register char **pp;
	register KBF *kbfp;
	kbcod    cod;
	char  *buf;             /* УКАЗАТЕЛЬ НА ВРЕМЕННЫЙ БУФЕР termcap */
	char     keys[4];       /* СТРОКА ДЛЯ ПОИСКА В termcap ("k1") */
	int     ok;

	/* ПАМЯТЬ ДЛЯ НАСТРОЙКИ linlib */
	codesp = malloc(TBUFSZ);  /* ДИАГНОСТИКА НИЖЕ... */

	/* ПОЛУЧИТЬ ПАМЯТЬ ДЛЯ ВРЕМЕННОГО БУФЕРА */
	if( (buf = malloc(TBUFSZ)) == 0 ) {
		fprintf(stderr, "\nHWSET:%s\n", ediag("NO Memory\n", "МАЛО ПАМЯТИ"));
		exit(1);
	}
	/* ПОЛУЧИТЬ ОПИСАНИЕ ТЕРМИНАЛА */
#ifdef  DEMOS2
	if( p = getenv("TERM") ) {
#endif
#ifdef  RT11
	if( p = gttynm() ) {
#endif
		ok = tgetent(buf,p);
		if(ok != 1) {
			fprintf(stdout, "%s: '%s'\n",
	   ediag("Unknown terminal type", "ТЕРМИНАЛ НЕИЗВЕСТЕН"), p);
			exit(1);
		}
	}
	else    {
		cannot();
	}

	/*---------------------------------*/
	/* НАСТРОИТЬ УКАЗАТЕЛИ ДЛЯ tgoto() */
	/*---------------------------------*/
/***    p = tgetstr("bc", &x);
	if (p && *p==0) {       BC = 0; }
	else    {
		stripd(bc_str, p); BC = bc_str; }
	p = tgetstr("up", &x);
	if (p && *p==0) {       UP = 0; }
	else    {
		stripd(up_str, p); UP = up_str; }

****/   /* МОЖНО ПРОЩЕ: */
	BC = gettcp("bc");   if(*BC=='\0') BC = 0;
	UP = gettcp("up");   if(*UP=='\0') UP = 0;

	/*-------------------------*/
	/* ПОЛУЧИТЬ РАЗМЕРЫ ЭКРАНА */
	/*-------------------------*/
#ifdef RT11
	maxli = tgetnum("li");
	maxco = tgetnum("co");
#else
	/*maxli = maxco = 0;*/
	if (0 == gtty_sz()) {
		maxli = tty_li;
		maxco = tty_co;
	}
	if (p = getenv("LINES"))    maxli = atoi(p);
	if (maxli == 0)             maxli = tgetnum("li");
	if (maxli > MAXLICO)		maxli = MAXLICO;
	if (p = getenv("COLUMNS"))  maxco = atoi(p);
	if (maxco == 0)             maxco = tgetnum("co");
	if (maxco > MAXLICO)		maxco = MAXLICO;
#endif

	/*--------------------------------*/
	/* НАСТРОИТЬ УКАЗАТЕЛИ ДЛЯ linlib */
	/*--------------------------------*/
	for (pp=tcapo; *pp; pp++) {
		*pp = gettcp(*pp);
	}
	for (kbfp= &kbf[0]; cod=(kbfp->t_key); kbfp++) {
		keys[0] = cod0(cod);
		keys[1] = cod1(cod);
		keys[2] = '\0';
#ifdef DEBUG
		printf("keys = '%s'\n", keys);
#endif
		kbfp->t_cap = gettcp(keys);
	}
	do_kbl();       /* НАСТРОИТЬ ТАБЛ. ЛОГ. КОДОВ */
	free(buf);
}
