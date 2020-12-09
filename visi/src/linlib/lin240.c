/*
**      +----------+    БИБЛИОТЕКА ВВОДА-ВЫВОДА
**     (c) linlib  !    ДЛЯ АЛФАВИТНО-ЦИФРОВЫХ
**      +----------+    ВИДЕОТЕРМИНАЛОВ
**/

/*
 *      $Header: lin240.c,v 1.1 90/12/27 16:29:07 vsv Rel $
 *
 *      $Log:	lin240.c,v $
 * Revision 1.1  90/12/27  16:29:07  vsv
 * ВЕРСИЯ LINLIB_3
 * 
 * Revision 3.9  89/08/29  15:16:14  vsv
 * ВЕРСИЯ LINLIB_3
 * 
 * Revision 3.8  89/08/29  10:32:44  vsv
 * НЕУДАЧНАЯ ПОПЫТКА МОДИФИЦИРОВАТЬ
 * АЛГОРИТМ НАСТРОЙКИ НА КЛАВИШИ (ДЛЯ uas)
 * СМОТРИ #ifdef UASDEBUG
 * 
 * Revision 3.7  89/08/14  16:37:16  vsv
 * ПОДПРАВЛЕНА РАБОТА С ИМЕНЕМ ФАЙЛА НАСТРОЙКИ,
 * ВСЯКАЯ МЕЛОЧЬ.
 * 
 * Revision 3.6  89/05/05  11:45:05  vsv
 * vhset :lh: works without :lh: termcap key
 * 
 * Revision 3.5  89/01/16  19:29:12  vsv
 * ПРАВЛЕНА НЕБОЛЬШАЯ ЧУШЬ,
 * СВЯЗАННАЯ С ЗАГРУЗКОЙ ИМЕН КЛАВИШ
 * ИЗ ОПИСАНИЯ vhset.
 * 
 * Revision 3.4  89/01/05  14:41:58  vsv
 * ИСПРАВЛЕНА ПЛЮХА: НАДО БЫЛО В КОНЦЕ
 * ЗАКРЫВАТЬ ФАЙЛ.
 * 
 * Revision 3.3  88/08/18  10:36:59  vsv
 * ИСПРАВЛЕНО ДУБЛИРОВАНИЕ МЕТКИ cont:
 * 
 * Revision 3.2  88/07/27  16:32:14  vsv
 * НАКОНЕЦ ИСПРАВЛЕНО ЧТЕНИЕ НАСТРОЙКИ КЛАВИШ
 * 
 * Revision 3.1  88/06/27  15:20:15  vsv
 * РЕВИЗИЯ АРХИВА RCS
 * 
 * Revision 3.0  88/06/16  17:55:17  vsv
 * ИСПРАВЛЕН .vhelp НА .vhset
 * 
 */

#include <stdlib.h>
#include <string.h>
#include <ediag.h>
#include <ctype.h>
#include <stdio.h>
#include "line.h"
#include "line0.h"

/*
 * НАСТРОЙКА ЛОГИЧЕСКИХ КОДОВ ТЕРМИНАЛА
 */

extern  LPA     lpainp[];
extern  LPA     lpaout[];
extern  KBL     kbl[];      /* ОПИСАНИЕ ВОЗМОЖНОСТЕЙ ТЕРМИНАЛА */
extern  char   *filelh;    /* УКАЗАТЕЛЬ НА ИМЯ ФАЙЛА С ОПИСАНИЕМ */
extern  int     kpadon;    /* ФЛАГ: ДОПОЛНИТЕЛЬНАЯ КЛАВИАТУРА ВКЛЮЧЕНА */
extern int		sgrmode;   /* initial monochrome */

#ifdef DEMOS2
extern  char *getenv();
#endif DEMOS2


do_kbl()
/*--------------------------------*/
/* ВЫПОЛНИТЬ НАСТРОЙКУ КЛАВИАТУРЫ */
/*--------------------------------*/
{
	char    name[100]; /* ИМЯ КЛАВИАТУРЫ ДЛЯ ПОИСКА HELP */
	char    str[100];  /* СТРОКА ДЛЯ ХРАНЕНИЯ ОДНОЙ ЗАПИСИ НАСТРОЙКИ */
	FILE    *lhfp;     /* ФАЙЛ С НАСТРОЙКОЙ */

	kbcod   t_key1;     /* ФИЗ. КОД КЛАВИШИ */
	kbcod   t_key2;
	kbcod   t_cod;     /* ЛОГ. КОД */
	kbcod   t_cods;    /* ЛОГ. КОД КЛАВИШИ ДЛЯ ПОИСКА */
	char    c1,c2;		/* 2 символа для получения кода через макрос */

	register LPA *lpap;
	LPA *lpapset[2]; /*0-lpaout, 1-lpainp*/
	register char *s;
	register int c;
	register int   i;
		int lpa_o;
		int lpa_i;
		 int ibeg;
		 char *os;
		 int  oscnt;

#ifndef DEMOS2
	/* ЕСЛИ НЕТ ИНФОРМАЦИИ, ГДЕ ИСКАТЬ НАСТРОЙКУ, КОНЕЦ */
	if ( *t_lh == '\0' ) return;
#endif
#ifdef  DEMOS2
	/* ОПРЕДЕЛИТЬ МЕСТО, ГДЕ ЕЕ ИСКАТЬ */
	if ( (s=getenv("VHSET")) != NULL ) {
		strcpy(name, s);
	} else {
		strcpy(name, "/etc/vhset");
	}
	/* strcat(name, "/.vhset/"); */
	strcat(name, "/");
	if(*t_lh) {
		strcat(name, t_lh);
	} else {
		if((s=getenv("TERM")) != NULL)
			strcat(name, s);
	}

	/* ОТКРЫВАЕМ ФАЙЛ */
	if( (lhfp=fopen(name, "r")) == NULL ) {
		/* НИКАКОЙ ДИАГНОСТИКИ... ЕСТЬ УМОЛЧАНИЕ */
		return;
	}
#endif  DEMOS2

#ifdef  RT11
	/* НАДО НАЙТИ ФАЙЛ ВИДА 'HOM:VT52.VHS' */
	/*              ЛИБО    'SY:VT52.VHS' */
	strcpy (name, "VHS:");
	strcat (name, t_lh );
	strcat (name, ".VHS");

	/* ОТКРЫВАЕМ ФАЙЛ */
	if( (lhfp=fopen(name, "r")) != NULL ) {
		goto cont1;
	}
	strcpy (name, "SY:");
	strcat (name, t_lh );
	strcat (name, ".VHS");

	/* ОТКРЫВАЕМ ФАЙЛ */
	if( (lhfp=fopen(name, "r")) == NULL ) {
		return;
	}
cont1:
	for(s=name; *s; s++) {
		if(islower(*s)) toupper(*s);
	}
#endif

	/* СОХРАНИМ ИМЯ ФАЙЛА, ЕСЛИ ЕСТЬ ГДЕ */
	if (filelh) strcpy(filelh, name);

	ibeg = 0;
	/* ЧИТАЕМ НАСТРОЙКУ... */
	while( fgets(s=str, 98, lhfp), feof(lhfp) == 0) {
		if(isspace(*s))
			continue;

		/* keypad mode (+/-), SGR mode (0,1,2,3) */
		if (*s == '+' || *s == '-') {
			if (*s == '+')      { w_raw(t_ks); kpadon = 1;      }
			if (*s == '-')      { w_raw(t_ke); kpadon = 0;      }
			if (isdigit(str[1]))	{		sgrmode = s[1] - '0'; }
		}
		/* logical code */
		else if(*s == ':') {
			s++;
			/* new format, 1st field is to key for insert into runtime table
			 * example: ':HE:F2    :k2:^G' */
			/* fields number 1st, 2nd and 3rd cannot be empty,
			 * 4th may be omitted (filled with spaces), but colon ':' */
			c1 = *s++;
			c2 = *s++;
			t_cod = KBCOD(c1, c2);
			if (*s == ':') {
				s++; /* skip colon */
			} else {
				continue;	/* skip this record in old format */
			}

#ifndef UASDEBUG
			/* verify place of insertion */
			for(i=ibeg, ibeg++; t_cods = kbl[i].t_cod; i++ ) {
				if(t_cods == t_cod) {
					/* label scan */
					/*kbl[i].t_knm = malloc(KBLSIZE);*/
					os = kbl[i].t_knm;
					for(oscnt=8; oscnt > 0; s++, oscnt--) {
						if (*s != '\0') { /*if (isprint(*s)) {*/
							*os++ = *s;
						} else {
							*os++ = '\0';
						}
						*os = '\0';
					}
					/*
					 * setup phisical keys:
					 * get primary key (and secondary key if exist)
					 */
					s++; /* skip colon */
					if ( isspace(*s) ) {
						t_key1 = KBCOD(' ', ' ');
					} else {
						c1 = *s++;
						c2 = *s++;
						kbl[i].t_key1 = KBCOD(c1, c2);
						s++; /* skip colon */
						c1 = *s++;
						c2 = *s++;
						kbl[i].t_key2 = KBCOD(c1, c2);
					}

					goto cont;
				}
			}
#else
			/* вариант при старом формате файлов vhset */
			/* НИКАКИХ ПРОВЕРОК */
			i=ibeg; ibeg++;

			/* СКОПИРОВАТЬ ФИЗ. КОД */
			kbl[i].t_key = t_key;
			/* СКОПИРОВАТЬ ЛОГ. КОД */
			kbl[i].t_cod = t_cod;
			/* СКОПИРОВАТЬ НАЗВАНИЕ */
			if(os = kbl[i].t_knm) {
			    kbl[i].t_knm = malloc(KBLSIZE);
			    os = kbl[i].t_knm;
			    while(*s && isprint(*s))
				*os++ = *s++;
			    *os = 0;
			}
			goto cont;
#endif
		} else
		if(isdigit(*s)) {     /* АТРИБУТ И ПОДСКАЗКА */
			lpap = &lpaout[*s++ & 07];  /* ASCII-КОИ7-КОИ8-depend! */
			lpap->lpa_p  =  *s++;
			lpap->lpa_a  = (*s++ & 07) << 6;
			lpap->lpa_a |= (*s++ & 07) << 3;
			lpap->lpa_a |= (*s++ & 07);
			lpap = &lpainp[*str & 07];
			lpap->lpa_p  =  *s++;
			lpap->lpa_a  = (*s++ & 07) << 6;
			lpap->lpa_a |= (*s++ & 07) << 3;
			lpap->lpa_a |= (*s++ & 07);
			/* ANSI Color GSR, with <TAB> separator(s)*/
			if (*s++ == '\t') {
				lpapset[0] = &lpaout[*str & 07];
				lpapset[1] = &lpainp[*str & 07];
				for(i = 0; i < 2; i++) {
					lpap = lpapset[i];
					for(os = lpap->lpa_sgr; *s != '\0' && *s != '\n' && *s != '\t'; *os++,*s++) {
						*os = *s;
					}
					*os = '\0'; *s++; /* skip <TAB> before next GSR*/
				}
			}

		}
cont:
		;
	}
	fclose(lhfp);
	return;
}
