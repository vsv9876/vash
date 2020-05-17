/*
**      +----------+    БИБЛИОТЕКА ВВОДА-ВЫВОДА
**     (c) linlib  !    ДЛЯ АЛФАВИТНО-ЦИФРОВЫХ
**      +----------+    ВИДЕОТЕРМИНАЛОВ
**/

/*
 *      $Header: lin330.c,v 1.1 90/08/24 08:08:11 vsv Exp $
 *
 *      $Log:	lin330.c,v $
 * Revision 1.1  90/08/24  08:08:11  vsv
 * Initial revision
 * 
 * Revision 3.1  89/08/29  15:17:03  vsv
 * ВЕРСИЯ LINLIB_3
 * 
 * Revision 3.0  88/06/16  17:50:23  vsv
 * НАВЕДЕН НЕКОТОРЫЙ ГЛЯНЕЦ НА КОМАНДЫ РЕДАКТИРОВАНИЯ
 * 
 */

#include <alloca.h>
#include <stdio.h>
#include <wchar.h>
#include <ctype.h>
#include "line.h"
#include "line0.h"

extern SCRN scrn;

int     edinsm = 1;     /* флаг: режим вставки 1, замены 0 */
/* extern  int edinff;  */
int     edinff = 1;     /* флаг: показывать состояние редактора строки */
int     edshow = 1;     /* флаг: показывать строку до редактирования */

/*static int edbak = 0;	/* флаг: копировать буфер редактирования обратно при выходе из e_str */


re_str(str_l, size, ctst, ofsp)
/*
 * Редактор строки без показа старого содержимого
 */
char   *str_l;      /* строка для редактирования*/
int     size;       /* размер поля редактирования */
kbcod (*ctst)();    /* тест для ввода печатаемых кодов */
int    *ofsp;       /* указатель на величину смещения от нач. поля */
{
	kbcod cod;
	int     _edshow, _edinff, _edinsm;
	wchar_t *wcsbuf;	/* буфер для строки во внутренней кодировке, снаружи e_str */
	int len, len2;		/* занятая символами длина строки, до и после редактирования */
	mbstate_t ps = { 0 };

	wcsbuf = alloca(2*4*(MAXLICO)+1);

	_edshow = edshow; edshow = 0;
	_edinff = edinff; edinff = 0;
	_edinsm = edinsm; edinsm = 1;

	len = u8wcs(wcsbuf, str_l);
	cod = e_str(wcsbuf, size, ctst, ofsp);
	len = wcsrtombs(str_l, &wcsbuf, 2*4*(MAXLICO)+1, &ps);

	edshow = _edshow;
	edinff = _edinff;
	edinsm = _edinsm;
	return cod;
}

static edinfo( infflg )
register int infflg;
/*
 * Показать режим редактирования, см. ниже
 */
{
/*	if ( !edinff )  return;*/
#ifndef CP_SAV
	cp_sav();
#endif
	cp_set(maxli-1, maxco-6, TXT);
	er_eol(TXT);
	cp_set(maxli-1, maxco-5, TXT|INP);
	if ( infflg ) {
		w_str(":");
		if ( edinsm ) {
			w_str("Ins");
		} else {
			w_str("Ovr");
		}
	} else {
		cp_set(maxli-1, maxco-8, TXT);
		er_eol(TXT);
	}
#ifndef CP_SAV
	cp_fet();
#endif

}

/*
static showed()
{
	if ( !edinff )  return;
	if ( infflg ) {
		w_msg(scrn.sc_at, "ed:");
		if ( edinsm )   w_str("Ins") else w_str("Ovr");

	} else {
		w_emsg("");
	}
}
*/

/* ВЫПОЛНИТЬ ИЗМЕНЕНИЯ И ПОКАЗАТЬ */
static int chgstr(s, size, ofs, cod)
register wchar_t *s;
int  size;
int ofs;
kbcod cod;
{
	register int j;
	register int lend;      /* ПОЗИЦИЯ ПОСЛЕДНЕГО НЕПУСТОГО СИМВОЛА */
		 int chg;       /* ПОЗИЦИЯ ИЗМЕНЕНИЯ */

	/* НАЙТИ ЛОГ. КОНЕЦ СТРОКИ */
	if(edinsm) {
		for(lend=size; lend>0 && s[--lend]==' ';) ;
		if(lend<ofs) lend=ofs;
	}
	chg = ofs;
	if(cod==KB_DE) {
		cod = ' ';
		ofs = --chg;
		if(edinsm) {
			for(j=chg; j<lend; j++) s[j]=s[j+1];
			s[j++]=' ';
		} else {
			s[lend = chg] = cod;
		}
		/******* <--(KB_DE)--- ******/
		cp_set(scrn.sc_li, scrn.sc_co - 1, scrn.sc_at);
	} else {
		if(edinsm) {
			for(j=size; --j>chg; ) s[j]=s[j-1]; /* ВСТАВИТЬ */
			lend += (lend<(size-1) ? 1 : 0);
		} else {
			lend = chg;
		}
		s[chg] = cod;
		ofs += 1;
	}

	for(j=chg; j<=lend; j++) w_wchr(s[j]);   /* НА ЭКРАН!!! */
	return (ofs);
}


kbcod e_str(str_l, size, ctst, ofsp)
/*-----------------*/
/* РЕДАКТОР СТРОКИ */
/*-----------------*/
register wchar_t    *str_l;  /* СТРОКА ДЛЯ РЕДАКТИРОВАНИЯ*/
int     size;           /* РАЗМЕР ПОЛЯ РЕДАКТИРОВАНИЯ */
kbcod   (*ctst)();      /* ТЕСТ ДЛЯ ВВОДА ПЕЧАТАЕМЫХ КОДОВ */
int     *ofsp;          /* УКАЗАТЕЛЬ НА ВЕЛИЧИНУ СМЕЩЕНИЯ ОТ НАЧ. ПОЛЯ */
{
	/* НАЧАЛО ПОЛЯ И VIDEO ЗАДАЮТСЯ ЧЕРЕЗ cp_set();
	 * ctst ВОЗВРАЩАЕТ 0, ЕСЛИ КОД НЕ ИЗМЕНЯЕТ СТРОКУ;
	 *                -1, ЕСЛИ НАДО ЗАКОНЧИТЬ РЕДАКТИРОВАНИЕ;
	 *              cod,  ЕСЛИ КОД ИЗМЕНЯЕТ СОДЕРЖИМОЕ СТРОКИ.
	 */
	register int i;
	register int j;
	register int  column;      /* ПОЗИЦИЯ И СТРОКА НА ЭКРАНЕ */
			 int  linenu;
			 int  attrib; /* логический видеоатрибут */
		 kbcod    cod;
		 kbcod    ok;
	/*wchar_t  *str_l;	 /* СТРОКА ДЛЯ РЕДАКТИРОВАНИЯ, внутри этой функции */

	column = scrn.sc_co;
	linenu = scrn.sc_li;
	attrib = scrn.sc_at;

	edinfo(1);

/*
	str_l = alloca(sizeof(wchar_t) * (size+1));
	u8wcs(str_l, ext_l);
*/
	/* заполнить пробелами конец строки */
	j = 0;
	while(j<size && str_l[j]) j++;
	while(j<size)       str_l[j++] = ' ';
	str_l[size] = 0;
	if (edshow) {
		/* показать на экране перед редактированием */
		cp_set(linenu, column, attrib);
		j = 0;
		while(j<size) w_wchr(str_l[j++]);
	}
	i = ofsp ? *ofsp : 0;

	/* ЦИКЛ РЕДАКТИРОВАНИЯ */
	for( ;; ) {
		if(i >= size) i = size-1;       /* ВСЕГДА ПРОВЕРЯТЬ!!! */
		cp_set(linenu, column+i, attrib);

		cod = r_cod(0);
		/* ПРОТЕСТИРУЕМ ВВОД */
		ok = ctst ? (*ctst)(cod, size, i) : cod;
		if(ok == -1) {
			goto ret;
		} else if(ok == 0) {
			bell();
		} else {
			switch(cod = ok) {
			case KB_PR: /* ДОП. КОМАНДЫ РЕДАКТОРА СТРОКИ */
				switch(r_cod(0)) {
				case KB_AR: /* КОНЕЦ ЗНАЧАЩЕЙ ИНФ. */
					   for(i=size-1; isspace(str_l[i]);	i--); i++; break;
				case KB_AL: i = 0; break;
				case ' ':
					   for(j=i; j<size; j++) {
						w_wchr(str_l[j] = ' ');
					   }; break;
				case KB_DE:
					   for(j=i; j<size-1; j++) {
					       w_wchr(str_l[j] = str_l[j+1]);
					   };
					   w_wchr(str_l[j] = ' ');
					   str_l[size] = '\0';
					   break;
				case KB_PR: edinsm=(edinsm ? 0 : 1);
					   edinfo(1);
					   break;
				default: bell(); break;
				}; break;

			/* linlib 4 since 2017-05 */
			case KB_KE: /* КОНЕЦ ЗНАЧАЩЕЙ ИНФ. */
			   for(i=size-1; isspace(str_l[i]);	i--); i++; break;
			case KB_KH: i = 0; break;
			case KB_IN: edinsm=(edinsm ? 0 : 1);
				   edinfo(1);
				   break;
			case KB_KD:
				   for(j=i; j<size-1; j++) {
				       w_wchr(str_l[j] = str_l[j+1]);
				   };
				   w_wchr(str_l[j] = ' ');
				   str_l[size] = '\0';
				   break;
			/*----------------------------*/

			case KB_AL: if(i>0)  i--;
				   else     goto ret;
				   break;
			case KB_TA: /*bell(); break;*/
				/*cp_sav(); cpa(-2, 1, ATT); w_str("ta"); cp_fet();*/
				goto ret; break;
#ifdef OLD_USE_TAB
			case KB_TA: if(i == size-1) goto ret;
				   i += 8-(i%8); break;
#endif
			case KB_AR: if(i == size-1) goto ret;
				   i++;
				   break;
			case KB_NL: goto ret;
			case KB_DE:
				   if(i==0)      goto ret;
				   i = chgstr(str_l, size, i, cod);
				   break;
			default:
				/*if(cod1(cod))   goto ret;/*НО МОЖНО И ЛУЧШЕ*/
				if(ISCTL(cod))   goto ret;
				else            i=chgstr(str_l, size, i, cod);

			}
		}
		/* КОНЕЦ ЦИКЛА РЕДАКТОРА */
	}
ret:
	str_l[size] = 0;
	if(ofsp) *ofsp = i;

	/* ПОДЧИСТИТЬ ПРОБЕЛЫ В КОНЦЕ СТРОКИ */
	for (i=size; --i>=0 && (str_l[i]==' ');) ;
	str_l[++i] = 0;
	edinfo(0);
/*	wctomb(ext_l, str_l);*/
	return(cod);
}
