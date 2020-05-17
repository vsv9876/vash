/*
**      +----------+    БИБЛИОТЕКА ВВОДА-ВЫВОДА
**     (c) linlib  !    ДЛЯ АЛФАВИТНО-ЦИФРОВЫХ
**      +----------+    ВИДЕОТЕРМИНАЛОВ
**/

/*
 *      $Header: lin340.c,v 3.5 89/08/29 15:17:13 vsv Rel $
 *
 *      $Log:	lin340.c,v $
 * Revision 3.5  89/08/29  15:17:13  vsv
 * ВЕРСИЯ LINLIB_3
 * 
 * Revision 3.4  89/08/29  10:36:03  vsv
 * СИНТАКСИС. СМ. rcsdiff
 * 
 * Revision 3.3  88/08/10  10:40:28  vsv
 * ТОНКОСТИ ФОРМАТНЫХ ПРЕОБРАЗОВАНИЙ
 * ДЛЯ ЛИНИЙ С РЕДАКТИРОВАНИЕМ
 * 
 * Revision 3.2  88/08/02  10:48:55  vsv
 * ДОБАВЛЕН ФЛАГ РЕЖИМА ПЕРЕХОДА К РЕДАКТИРОВАНИЮ ПОЛЯ (allcod).
 * 
 * Revision 3.1  88/06/27  15:21:26  vsv
 * РЕВИЗИЯ АРХИВА RCS
 * 
 * Revision 3.0  87/12/21  12:19:56  vsv
 * ПРЕДВАРИТЕЛЬНЫЙ ВЫПУСК.
 * 
 */

#include <stdio.h>
#include <wchar.h>
#include <string.h>
#include "line.h"
#include "line0.h"

extern LPA lpainp[];
extern LPA lpaout[];

extern int allcod;

/*#define FLOAT_CVT /*not supported, use line->cvtf instead, please*/

/*----------------------------*/
/* ВЫВЕСТИ ПО ФОРМАТАМ printf */
/*----------------------------*/
static int
docvts(ou, fo, va)
register char *ou;      /* СТРОКА, КУДА ПОМЕСТИТЬ ВЫВОД */
register char *fo;      /* ФОРМАТ printf */
register char *va;      /* УКАЗАТЕЛЬ НА ПЕРЕМЕННУЮ */
{
#ifdef FLOAT_CVT
	if(index(fo, 'f') || index(fo, 'e') || index(fo, 'g'))
		/* ФОРМАТЫ С ПЛАВАЮЩЕЙ ТОЧКОЙ */
		return( sprintf(ou, fo, *(double *)va) );
	else
		/* ДЛЯ ОСТАЛЬНЫХ ДОСТАТОЧНО И ТАК */
#endif
		return( sprintf(ou, fo, *(long *)va) );
}

w_line(line)
register LINE *line;
{
/*      next_j();     */

	r_line(line, (int *)(-1)); /* preset to call in write mode (only output, no input) */
}

kbcod
r_line(line, posp)
/*-------------------------------------------------*/
/* ВЫВОД/ВВОД ЛИНИИ, ВЕРНУТЬ КОД ПОСЛЕДНЕЙ КЛАВИШИ */
/*-------------------------------------------------*/
register LINE    *line; /* УКАЗАТЕЛЬ НА ЛИНИЮ */
	 int     *posp; /* ПОЗИЦИЯ, С КОТОРОЙ НАЧАТЬ РЕДАКТИРОВАТЬ */
{
	kbcod   cod;            /* КОД ПОСЛЕДНЕЙ КЛАВИШИ */
	int     attr;           /* СЛОВО АТРИБУТОВ */
	int     cvterror;       /* ФЛАГ ОШИБКИ ФОРМАТА */
	int     tsterror;       /* ФЛАГ ОШИБКИ ТЕСТА */
	int     midcnt;         /* СЧЕТЧИК ДЛЯ ВЫРАВНИВАНИЯ ПО ЦЕНТРУ */
	int     filch;          /* ЗНАК ДЛЯ ЗАПОЛНЕНИЯ */
	int     slen;           /* ДЛИНА СТРОКИ ПЕРЕД ВЫРАВНИВАНИЕМ */
	int     size;           /* РАЗМЕР ПОЛЯ */
	int     fmtlock;            /* ФЛАГ: ФОРМАТ ЗАБЛОКИРОВАН */
	int     onexit;         /* ФЛАГ: КОНЕЦ РАБОТЫ */
	int     base;           /* НАЧАЛО ПОЛЕЗНОЙ ИНФОРМАЦИИ: СМЕЩЕНИЕ ОТ ПОДСКАЗКИ */

 register
    int  i;
	wchar_t *editptr;         /* начало строки для редактора, после промптера */
 register
    wchar_t *wcsptr;
	wchar_t  wcsbuf[STRLEN];  /* рабочая строка во внутренней кодировке */

	char    u8buf[4*STRLEN + 2]; /*промежуточная строка для в кодировке UTF-8*/
	int		u8size;			     /* размер в символах в промежуточной строке*/

	attr = line->attr;

	/* ВЫЗОВ ЧЕРЕЗ w_line() ? */
	if(onexit = ((posp == (int *)(-1)) ? 1 : 0))
		attr &= ~INP;  /* НЕТ, вызов r_line */
	cod = 0;

inp_retry:
out_string:
	cvterror = tsterror = 0;
	wcsptr = wcsbuf;
	editptr = wcsbuf;
	base = 0;
	size = line->size;

	i = attr & VIDEO;

#ifdef RETRO_FILCH
	/*==== ЯВНОЕ ЗАПОЛНЕНИЕ ПРИ ОТСУТСТВИИ АТРИБУТОВ */
	if (((line->attr & INP) == 0 ) && (lpaout[i].lpa_a == 0))
			filch = '_';
	else            filch = ' ';
#else
	filch = ' ';
	if ((line->attr & VIDEO) == HDR) {
		filch = lpaout[HDR].lpa_p;
	}
#endif
	/*==== ПОДСКАЗКА */
	if(attr & PMT) {
		editptr++; base++; size--;
		if(attr & INP)  *wcsptr++ = lpainp[i].lpa_p;
		else            *wcsptr++ = lpaout[i].lpa_p;
	}
	/*==== ФОРМАТ НА ВЫВОД */
	if( !(fmtlock = ((attr & FLO) == FLO))) { /*=== ЕСЛИ НЕ БЛОКИРОВАН */
		if(line->cvtf) {                /* ЧЕРЕЗ ФУНКЦИЮ */
			(*(line->cvtf))(line, cod, "w", u8buf); u8size = u8wcs(wcsptr, u8buf);
		}
		else if(line->cvts) {           /* В СТИЛЕ printf */
			docvts(u8buf, line->cvts, line->varl); u8size = u8wcs(wcsptr, u8buf);
		}
		else {                          /* ПРОСТО СТРОКА */
			/* editptr = line->varl; /*varl показывает на UTF-8, а нужно wchar_t*/
			/*u8size = u8wcs(wcsptr, line->varl);/* ниже, string_simple: */
			editptr = wcsptr;
			goto string_simple;
		}
	} else {
string_simple:
		//strncpy(s, line->varl, size); s[size] = 0;
		u8size = u8wcsn(wcsptr, line->varl, size); wcsptr[size] = 0;
	}

	/* slen = strlen(s);*/
	slen = wcslen(wcsptr);

	/*==== ВЫРАВНИВАНИЕ */
	if(attr & MID) {
		if((midcnt=((size-slen)/2)) > 0) {
			for(i= slen+= midcnt; i>=midcnt; i--)
				wcsptr[i] = wcsptr[i-midcnt];
			while(i >= 0) wcsptr[i--] = filch;
		}
	}
	/*==== ЗАПОЛНЕНИЕ */
	if(attr & PAD) {
		for(i=slen; i<size;)    wcsptr[i++] = filch; wcsptr[size] = 0;
	}
	/*==== КУРСОР, ВИДЕО (НЕ ЗАБЫТЬ ПОДСКАЗКУ) */
	cp_set(line->line, line->colu, attr);

	/*==== ПОКАЗАТЬ */
	if ( (posp != (int *)(-1)) && (attr & (LFASTR|NED)) == (LFASTR|NED) )
		w_wchr(*wcsbuf);    /* только для r_line */
	else
		w_wcstr(wcsbuf);

	if(onexit || posp == (int *)(-1)) return(cod);    /* КОНЕЦ ДЛЯ ВЫЗОВА ЧЕРЕЗ w_line */

	/*==== КУРСОР ПЕРЕД ВВОДОМ ПЕРВОЙ КЛАВИШИ */
	cp_set(line->line, line->colu, attr);

	/*==== ЧИТАТЬ КОД ПЕРВОЙ КЛАВИШИ */
	switch(cod = r_cod(0)) {
	case KB_DE: *editptr = '\0';
	case  ' ': if(posp != (int *)(-1) && posp) *posp=0;
		   break;
	default:
		/* ЕСТЬ ТОНКОСТЬ ДЛЯ ПОЛЕЙ С РЕДАКТИРОВАНИЕМ:
		 * ФОРМАТ НА ВВОДЕ НУЖЕН ТОЛЬКО
		 * ПОСЛЕ РЕДАКТОРА СТРОКИ И КОДА KB_NL;
		 */
		if((attr & NED) == 0) {
			if(allcod && ISCTL(cod) == 0) { /*&& cod1(cod) == 0) {*/
				unr_c(cod);     /* "ПРОЧИТАТЬ" НАЗАД */
				*editptr = L'\0';    /* ОЧИСТИТЬ СТРОКУ */
			} else
	/*                if(cod1(cod) != 0)    *******/
				goto inp_test;
		} else
			goto inp_format;
		break;
	}

edit_retry:
	/*==== НЕ РЕДАКТИРОВАТЬ ? */
	if(attr & NED) goto inp_format;

	cp_set(line->line, base + line->colu, attr);

	/*==== РЕДАКТОР, ХРАНИТЬ КОД */
	cod = e_str(editptr, size,
		     /*==== ТЕСТ ДЛЯ РЕДАКТОРА ? */
		    ((attr & EDT) ? (linptr_t)(line->test) : 0), posp);
	/*после редактора вернуть все в UTF-8*/
	u8size = wcstombs(u8buf, editptr);
	/*
	 * ЕСТЬ ТОНКОСТИ С ФОРМАТОМ ПОСЛЕ РЕДАКТОРА:
	 *    НАДО РЕАГИРОВАТЬ ТОЛЬКО НА KB_NL
	 */
	if(cod != KB_NL) goto inp_test;

inp_format:
	/*==== ФОРМАТ НА ВВОДЕ */
	if(fmtlock) goto inp_test;  /* М.БЫТЬ БЛОКИРОВАН... */

	if(line->cvtf) {
		if(cvterror = !(*line->cvtf)(line, cod, "r", /*editptr*/u8buf))
			bell();
	} else if(line->cvts) {
#ifdef FLOAT_CVT
		/*
		 * НЕ СОВСЕМ НАДЕЖНОЕ ПРЕОБРАЗОВАНИЕ НА ВВОДЕ :
		 * У КОМПИЛЯТОРА DECUS РАБОТАЕТ ОДИН ФОРМАТ - %f
		 * (И НЕ ТОЛЬКО У DECUS  -- vsv, 15/02/87)
		 */
		if((index(line->cvts, 'f') != 0)
		    || (index(line->cvts, 'g') != 0)
		    || (index(line->cvts, 'e') != 0))       {
			if(cvterror= (!sscanf(/*editptr*/u8buf, "%f", ((double *)line->varl))))
				bell();
		}
		else
#endif
		if(cvterror= (!sscanf(/*editptr*/u8buf, line->cvts, line->varl))) {
			bell();
		}
	} else {
		/*просто строка - вернуть содержимое после редактирования*/
		strcpy(line->varl, u8buf);
	}
	/*==== ОШИБКА ФОРМАТА ? */
	if(cvterror && onexit == 0 ) goto edit_retry;

inp_test:
	/*==== ТЕСТ ПОСЛЕ РЕДАКТОРА ? */
	if( fmtlock || !(attr & EDT) ) {
		tsterror = !(line->test ? (*line->test)(line, cod) : TRUE);
	}
	/*==== ЕСТЬ ОШИБКА ? */
	if(tsterror)           goto inp_retry;

	switch(cod) {
	case KB_AU:      /* ПОСЛЕ ЭТИХ КОДОВ ПЕРЕХОД К ДР. ПОЛЯМ */
	case KB_AD:
	case KB_AL:
	case KB_AR:
	case KB_NL:      break;
	default :       return(cod);
	}

	onexit = 1; attr &= ~INP;       /* АТРИБУТ ДЛЯ ПОКАЗА */
	goto    out_string;
}
