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
#include <string.h>
#include "line.h"
#include "line0.h"

extern LPA lpainp[];
extern LPA lpaout[];

extern int allcod;


/*----------------------------*/
/* ВЫВЕСТИ ПО ФОРМАТАМ printf */
/*----------------------------*/
static int
docvts(ou, fo, va)
register char *ou;      /* СТРОКА, КУДА ПОМЕСТИТЬ ВЫВОД */
register char *fo;      /* ФОРМАТ printf */
register char *va;      /* УКАЗАТЕЛЬ НА ПЕРЕМЕННУЮ */
{
#ifdef DOUBLE
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
	register char *s;
	register int  i;
	kbcod   cod;            /* КОД ПОСЛЕДНЕЙ КЛАВИШИ */
	int     attr;           /* СЛОВО АТРИБУТОВ */
	int     cvterror;       /* ФЛАГ ОШИБКИ ФОРМАТА */
	int     tsterror;       /* ФЛАГ ОШИБКИ ТЕСТА */
	int     midcnt;         /* СЧЕТЧИК ДЛЯ ВЫРАВНИВАНИЯ ПО ЦЕНТРУ */
	int     filch;          /* ЗНАК ДЛЯ ЗАПОЛНЕНИЯ */
	int     wksl;           /* ДЛИНА СТРОКИ ПЕРЕД ВЫРАВНИВАНИЕМ */
	int     size;           /* РАЗМЕР ПОЛЯ */
	int     flo;            /* ФЛАГ: ФОРМАТ ЗАБЛОКИРОВАН */
	int     onexit;         /* ФЛАГ: КОНЕЦ РАБОТЫ */
	char   *eds;            /* СТРОКА ДЛЯ РЕДАКТОРА */
	int     base;           /* НАЧАЛО ПОЛЕЗНОЙ ИНФОРМАЦИИ:
				**   СМЕЩЕНИЕ ОТ ПОДСКАЗКИ */
	char    wks[STRLEN];    /* РАБОЧАЯ СТРОКА */

	attr = line->attr;

	/* ВЫЗОВ ЧЕРЕЗ w_line() ? */
	if(onexit = ((posp == (int *)(-1)) ? 1 : 0))
		attr &= ~INP;  /* НЕТ, вызов r_line */
	cod = 0;

inp_retry:
out_string:
	cvterror = tsterror = 0;
	s = eds = wks;
	base = 0; size = line->size;

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
		eds++; base++; size--;
		if(attr & INP)  *s++ = lpainp[i].lpa_p;
		else            *s++ = lpaout[i].lpa_p;
	}
	/*==== ФОРМАТ НА ВЫВОД */
	if( !(flo = ((attr & FLO) == FLO))) { /*=== ЕСЛИ НЕ БЛОКИРОВАН */
		if(line->cvtf) {                /* ЧЕРЕЗ ФУНКЦИЮ */
			(*(line->cvtf))(line, cod, "w", s);
		}
		else if(line->cvts) {           /* В СТИЛЕ printf */
			docvts(s, line->cvts, line->varl);
		}
		else {                          /* ПРОСТО СТРОКА */
			eds = line->varl;
			goto string_simple; }
	} else {
string_simple:
		strncpy(s, line->varl, size); s[size] = 0;
	}

	wksl = strlen(s);

	/*==== ВЫРАВНИВАНИЕ */
	if(attr & MID) {
		if((midcnt=((size-wksl)/2)) > 0) {
			for(i= wksl+= midcnt; i>=midcnt; i--)
				s[i] = s[i-midcnt];
			while(i >= 0) s[i--] = filch;
		}
	}
	/*==== ЗАПОЛНЕНИЕ */
	if(attr & PAD) {
		for(i=wksl; i<size;)    s[i++] = filch; s[size] = 0;
	}
	/*==== КУРСОР, ВИДЕО (НЕ ЗАБЫТЬ ПОДСКАЗКУ) */
	cp_set(line->line, line->colu, attr);

	/*==== ПОКАЗАТЬ */
	if ( (posp != (int *)(-1)) && (attr & (LFASTR|NED)) == (LFASTR|NED) )
		w_chr(*wks);    /* только для r_line */
	else
		w_str(wks);

	if(onexit || posp == (int *)(-1)) return(cod);    /* КОНЕЦ ДЛЯ ВЫЗОВА ЧЕРЕЗ w_line */

	/*==== КУРСОР ПЕРЕД ВВОДОМ ПЕРВОЙ КЛАВИШИ */
	cp_set(line->line, line->colu, attr);

	/*==== ЧИТАТЬ КОД ПЕРВОЙ КЛАВИШИ */
	switch(cod = r_cod(0)) {
	case KB_DE: *eds = '\0';
	case  ' ': if(posp != (int *)(-1) && posp) *posp=0;
		   break;
	default:
		/* ЕСТЬ ТОНКОСТЬ ДЛЯ ПОЛЕЙ С РЕДАКТИРОВАНИЕМ:
		 * ФОРМАТ НА ВВОДЕ НУЖЕН ТОЛЬКО
		 * ПОСЛЕ РЕДАКТОРА СТРОКИ И КОДА KB_NL;
		 */
		if((attr & NED) == 0) {
			if(allcod && cod1(cod) == 0) {
				unr_c(cod);     /* "ПРОЧИТАТЬ" НАЗАД */
				*eds = '\0';    /* ОЧИСТИТЬ СТРОКУ */
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
	cod = e_str(eds, size,
		     /*==== ТЕСТ ДЛЯ РЕДАКТОРА ? */
		    ((attr & EDT) ? (linptr_t)(line->test) : 0), posp);
	/*
	 * ЕСТЬ ТОНКОСТИ С ФОРМАТОМ ПОСЛЕ РЕДАКТОРА:
	 *    НАДО РЕАГИРОВАТЬ ТОЛЬКО НА KB_NL
	 */
	if(cod != KB_NL) goto inp_test;

inp_format:
	/*==== ФОРМАТ НА ВВОДЕ */
	if(flo) goto inp_test;  /* М.БЫТЬ БЛОКИРОВАН... */

	if(line->cvtf) {
		if(cvterror = !(*line->cvtf)(line, cod, "r", eds))
			bell();
	} else if(line->cvts) {
#ifdef DURA
		/*
		 * НЕ СОВСЕМ НАДЕЖНОЕ ПРЕОБРАЗОВАНИЕ НА ВВОДЕ :
		 * У КОМПИЛЯТОРА децус РАБОТАЕТ ОДИН ФОРМАТ - %f
		 * (И НЕ ТОЛЬКО У DECUS  -- vsv, 15/02/87)
		 */
		if((index(line->cvts, 'f') != 0)
		    || (index(line->cvts, 'g') != 0)
		    || (index(line->cvts, 'e') != 0))       {
			if(cvterror= (!sscanf(eds,"%f",line->varl)))
				bell();
		}
		/* ТО, ЧТО ВЫШЕ, МОЖНО УБРАТЬ, ЕСЛИ КОМУ МЕШАЕТ */
		else    {
			if(cvterror= (!sscanf(eds,line->cvts,line->varl)))
				bell();
		}
#endif DURA
		if(cvterror= (!sscanf(eds,line->cvts,line->varl)))
			bell();

	}
	/*==== ОШИБКА ФОРМАТА ? */
	if(cvterror && onexit == 0 ) goto edit_retry;

inp_test:
	/*==== ТЕСТ ПОСЛЕ РЕДАКТОРА ? */
	if( flo || !(attr & EDT) ) {
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
