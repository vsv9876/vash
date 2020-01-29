/*
**      +----------+    БИБЛИОТЕКА ВВОДА-ВЫВОДА
**     (c) linlib  !    ДЛЯ АЛФАВИТНО-ЦИФРОВЫХ
**      +----------+    ВИДЕОТЕРМИНАЛОВ
**/

/*
 *      $Header: /home/vsv/proj/proj20010321/visi/src/linusr/RCS/lin440.c,v 3.4 1989/08/29 14:50:05 vsv Rel vsv $
 *
 *      $Log: lin440.c,v $
 *      Revision 3.4  1989/08/29 14:50:05  vsv
 *      ВЕРСИЯ LINLIB_3
 *
 * Revision 3.3  88/12/28  12:04:35  vsv
 * ОЧЕНЬ ПЕРЕДЕЛАННАЯ ВЕРСИЯ.
 * РАБОТАЕТ, КАК БЫЛО ЗАДУМАНО.
 * 
 * Revision 3.2  88/12/25  13:48:30  vsv
 * НЕМНОГО ВЫЛЕЧЕНА БОЛЕЗНЬ ОТНОСИТЕЛЬНО
 * malloc() & free() ИЖЕ С НИМИ fopen() & fclose(),
 * НО МОЖНО СДЕЛАТЬ И ЛУЧШЕ, НАДО СДЕЛАТЬ НОВУЮ ВЕРСИЮ
 * lbp & b_page().
 * 
 * Revision 3.1  88/06/27  15:30:39  vsv
 * РЕВИЗИЯ АРХИВА RCS
 * 
 * Revision 3.0  87/12/21  12:22:20  vsv
 * ПРЕДВАРИТЕЛЬНЫЙ ВЫПУСК.
 * 
 */

/*
 * РАБОТА С ВНЕШНИМИ ОПИСАНИЯМИ СТРАНИЦ
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "line.h"
#include "linebp.h"

LINE_H  lhd = { 0 };    /* МЕСТО ДЛЯ СЧИТЫВАНИЯ ЗАГОЛОВКА */
static  char **atabp = NULL;   /* ТАБЛИЦА АДРЕСОВ ДЛЯ НАСТРОЙКИ line[] */
static  LINE *lineb = NULL;     /* УКАЗАТЕЛЬ НА СТРАНИЦУ, РЕЗ-ТАТ РАБОТЫ */
	char exname[8] = { 0 };       /* ИМЯ ИЗ ВНЕШНЕГО ОПИСАНИЯ */
static  linptr_t maxid = 0;     /* МАКС. НОМЕР ИМЕНИ ДЛЯ ПОДСТАНОВКИ */

static int errflg = 0;     /* ФЛАГ ОШИБКИ НАСТРОЙКИ АДРЕСА */
static err_l(line)
LINE *line;
{
	if( errflg ) {
		line->flag = 0;
		line->attr = LHDR;
		line->cvts = 0;
		line->cvtf = 0;
		line->test = 0;
		line->varl = "<?>";
/*
	} else {
		return ;
*/
	}
}

d_page(llll)
/*---------------------*/
/* УНИЧТОЖИТЬ СТРАНИЦУ */
/*---------------------*/
LINE *llll;
{
	free( llll );
}


LINE *
b_page(libnam, fnam, ports)
/*-----------------------------------------*/
/* ПОСТРОИТЬ СТРАНИЦУ ПО ВНЕШНЕМУ ОПИСАНИЮ */
/*-----------------------------------------*/
char  *libnam;          /* ИМЯ БИБЛИОТЕКИ С ОПИСАНИЯМИ */
char  *fnam;            /* ИМЯ ФОРМЫ (ОПИСАНИЯ) */
IN_PORTS *ports;        /* ТАБЛИЦА ИМЕН УКАЗАТЕЛЕЙ */
{
	/*char    *malloc();*/
	register LINE *line;
	int      j;
	linptr_t i;
	register char *s;
	int      c;
	linptr_t count;
	FILE   *fp;
	IN_PORTS *in_pp;
	char    b_pgnm[100];
		/* ИМЯ ЗАГРУЗОЧНОГО ФАЙЛА ОПИСАНИЯ СТРАНИЦЫ */

	/* НАЙТИ ФОРМУ В БИБЛИОТЕКЕ */
	if( libnam == NULL )
		strcpy(b_pgnm, "");
	else
		strcpy(b_pgnm, libnam);
	strcat(b_pgnm, fnam);

	/* ОТКРЫТЬ ФАЙЛ ОПИСАНИЯ НА ЧТЕНИЕ */
	if((fp=fopen(b_pgnm, "rn")) == NULL) {
		return(NULL);
	}

	/* ПРОЧИТАТЬ ЗАГОЛОВОК */
	s = (char *) &lhd;
	for(i=0; i<sizeof(lhd); i++) {
		if ((c=getc(fp)) == EOF)  break;
		else                      *s++ = c;
	}
	fclose(fp);     /* БУФЕР ОСВОБОЖДАЕТСЯ */

	/*  ТЕПЕРЬ ВСЕ ПО ПОРЯДКУ (НАИЛУЧШАЯ СТРАТЕГИЯ malloc & free) */

	/* ЗАПРОСИТЬ ПАМЯТЬ ДЛЯ line[] И heaps */
	count = lhd.lh_lines + lhd.lh_heaps;
	lineb = (LINE *)malloc(count);
	if ( lineb == NULL ) {
		fclose(fp);
		return(NULL);
	}

	/* СНОВА ОТКРЫТЬ ФАЙЛ ОПИСАНИЯ НА ЧТЕНИЕ */
	if((fp=fopen(b_pgnm, "rn")) == NULL) {
		free((char *)lineb);
		return(NULL);
	}
	/* ПРОПУСТИТЬ ЗАГОЛОВОК */
	for(i=0; i<sizeof(lhd); i++) {
		if ((c=getc(fp)) == EOF)  break;
	}

	/* ПРОЧИТАТЬ line И heaps */
	s = (char *)lineb;
	for(i=0; i<count; i++) {
		if ((c=getc(fp)) == EOF)  break;
		else                      *s++ = c;
	}

	/* ЗАПРОСИТЬ ПАМЯТЬ НА ТАБЛИЦУ АДРЕСОВ */
	atabp = (linptr_t *)malloc( lhd.lh_names);
	if ( atabp == NULL ) {
		fclose(fp);
		free(lineb);
		return(NULL);
	}

	/* ПОСТРОИТЬ ТАБЛИЦУ АДРЕСОВ */
		/* МАГИЧЕСКОЕ 8 - ДЛИНА ИМЕНИ С НУЛЯМИ ВО ВНЕШ. ОПИСАНИИ */
	maxid = ((linptr_t)(lhd.lh_names)/8);
	for(i=0; i < maxid; i++) {

		/* ЧИТАТЬ ОЧЕРЕДНОЕ ИМЯ */
		s = exname;
		for(j=0; j<8; j++) {
			if ((c=getc(fp)) == EOF)  break;
			else                      *s++ = c;
		}
		/* НАЙТИ ДЛЯ НЕГО АДРЕС ПО ШЛЮЗ-ТАБЛИЦЕ */
		atabp[i] = (char *)(~0);
		for(in_pp=ports; in_pp->in_name; in_pp++) {
			if(strcmp(in_pp->in_name, exname) == 0) {
				atabp[i] = in_pp->in_addr;
				break;
			}
		}
	}
	/* НАСТРОИТЬ УКАЗАТЕЛИ В ПРЕДЕЛАХ lineb[] */

	for(line=lineb; line->size; line++) {
		errflg = 0;
		adj_lin(line);
		err_l(line);
	}

	/* ВЕРНУТЬ СИСТЕМЕ ПАМЯТЬ ОТ ТАБЛИЦЫ АДРЕСОВ */
	free( atabp );

	/* --- ЗАКРЫТЬ ФАЙЛ --- */
	fclose(fp);

	/* --- ВЕРНУТЬ УКАЗАТЕЛЬ НА СТРАНИЦУ --- */
	return(lineb);
}

/*---------------------------------------*/
/* НАСТРОЙКА УКАЗАТЕЛЕЙ В ПРЕДЕЛАХ ЛИНИИ */
/*---------------------------------------*/
adj_lin(line)
LINE *line;
{
	char    *adj_ptr();

	line->cvts = adj_ptr( (char *)line->cvts );
	line->cvtf = adj_ptr( (char *)line->cvtf );
	line->test = adj_ptr( (char *)line->test );
	line->varl = adj_ptr( (char *)line->varl );
}

char *
adj_ptr( s )
/*---------------------*/
/* НАСТРОИТЬ УКАЗАТЕЛЬ */
/*---------------------*/
char  *s;
{
	register linptr_t u;
	char   *rets;

	u = (linptr_t)s;

	if(u == 0)
		return( NULL );
	if(u<maxid) {
		if( atabp[u] == (char *)(~0) ) {
			errflg = 1;
		} else {
			/* АДРЕС ПОДСТАВЛЯЕТСЯ ИЗ ТАБЛИЦЫ */
			rets = atabp[u];
		}
	} else {
		/* АДРЕС СТРОКИ ИЗ ВНЕШНЕГО ОПИСАНИЯ */
		u -= 2048;             /* УБРАТЬ КОМПЕНСАЦИЮ */
		u += (lhd.lh_lines);   /* СМЕЩ. ОТ line */
		u += (linptr_t)(lineb); /* БАЗА */
		rets = (char *)u;
	}
	return( rets );
}
