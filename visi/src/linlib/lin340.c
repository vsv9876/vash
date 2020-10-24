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

#define FLOAT_CVT /*not supported, use line->cvtf instead, please*/

/*#define DEBUG_R_LINE*/
#ifdef DEBUG_R_LINE
static int sout(lipos, ban, u8s)
char *u8s;
char *ban;
int lipos;
{
	unsigned char *s;

	cp_sav();
	cp_set(lipos, 8, HDR);
	fprintf(vttout, " %s=\"%s\" " , ban, u8s);
	for (s = u8s; *s != '\0'; s++)
		fprintf(vttout, "%2x ", (int)*s);
	er_eol(HDR);
	cp_fet();
}
static int dout(lipos, ban, u8s)
char *u8s;
char *ban;
int lipos;
{
	unsigned char *s;

	s = u8s;
	cp_sav();
	cp_set(lipos, 8, HDR);
	fprintf(vttout, " %s: " , ban);
	fprintf(vttout, " int=\"%d\" " , *(int *)s);
	fprintf(vttout, " long=\"%ld\" " , *(long *)s);
	fprintf(vttout, " short=\"%hd\" " , *(short *)s);
	fprintf(vttout, " float=\"%f\" " , *(float *)s);
	fprintf(vttout, " double=\"%lf\" " , *(double *)s);
	er_eol(HDR);
	cp_fet();
}
#else
/*
static int sout(colu, ban, s)
char *s;
char *ban;
int colu;
{}
static int dout(colu, ban, s)
char *s;
char *ban;
int colu;
{}
*/
#endif

/*----------------------------*/
/* ВЫВЕСТИ ПО ФОРМАТАМ sprintf */
/*----------------------------*/
static int
cvts_out(line, buf)
LINE *line;
char *buf;      /* СТРОКА, КУДА ПОМЕСТИТЬ ВЫВОД */
{
	char *fo;      /* ФОРМАТ printf */
	char *va;      /* УКАЗАТЕЛЬ НА ПЕРЕМЕННУЮ */
	int cvt_ok = 0;
	long vl;
	short vh;
	int vi;

	fo = line->cvts;
	va = line->varl;

#ifdef FLOAT_CVT
	if(index(fo, 'f') || index(fo, 'e') || index(fo, 'g'))
		/* ФОРМАТЫ С ПЛАВАЮЩЕЙ ТОЧКОЙ */
		if (index(fo, 'l')) {
			return( sprintf(buf, fo, (double)*((double *)va)) );
		} else {
			return( sprintf(buf, fo, (float)*((float *)va)) );
		}
	else
		/* int, long, short */
#endif
	if(index(line->cvts, 's') == 0) { /* not a string conversion */
		if(index(line->cvts, 'l') != 0) {
			vl = (long)*((long *)va);
			cvt_ok = sprintf(/*editptr*/buf, fo, vl);
		} else if(index(line->cvts, 'h') != 0) {
			vh = (short)*((short *)va);
			cvt_ok = sprintf(/*editptr*/buf, fo, vh);
		} else {
			vi = (int)*((int *)va);
			cvt_ok = sprintf(/*editptr*/buf, fo, vi);
		}
	} else {
		/* string "%s" as last resort conversion */
		cvt_ok = sprintf(/*editptr*/buf, fo, va);
	}
	return (cvt_ok != 0);
}

/*
 * ввод по форматам sscanf
 * возвращает код sscanf
 */
static int
cvts_in(line, u8buf)
LINE *line;
register char *u8buf;
{
	int cvt_ok;

#ifdef FLOAT_CVT
	/*
	 * НЕ СОВСЕМ НАДЕЖНОЕ ПРЕОБРАЗОВАНИЕ НА ВВОДЕ :
	 * У КОМПИЛЯТОРА DECUS РАБОТАЕТ ОДИН ФОРМАТ - %f
	 * (И НЕ ТОЛЬКО У DECUS  -- vsv, 15/02/87)
	 */
	if((index(line->cvts, 'f') != 0)
	|| (index(line->cvts, 'g') != 0)
	|| (index(line->cvts, 'e') != 0))
	{
		if(index(line->cvts, 'l') != 0) {
		    cvt_ok = sscanf(/*editptr*/u8buf, "%lf", ((double *)line->varl));
		} else {
		    cvt_ok = sscanf(/*editptr*/u8buf, "%f", ((float *)line->varl));
		}
	}
	else
#endif
	if(index(line->cvts, 's') == 0) { /* not a string conversion */
		if(index(line->cvts, 'l') != 0) {
			cvt_ok = sscanf(/*editptr*/u8buf, line->cvts, ((long *)line->varl));
		} else if(index(line->cvts, 'h') != 0) {
			cvt_ok = sscanf(/*editptr*/u8buf, line->cvts, ((short *)line->varl));
		} else {
			cvt_ok = sscanf(/*editptr*/u8buf, line->cvts, ((int *)line->varl));
		}
	} else {
		/* string is last resort conversion */
		cvt_ok = sscanf(/*editptr*/u8buf, line->cvts, line->varl);
	}
#ifdef DEBUG_R_LINE
	sout(-6, "cvts_in:u8buf", u8buf);
#endif
	return (cvt_ok);
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
	int     cvt_ret;        /* код возврата из sscanf: OK если = 0 */
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

	char    u8buf[4*STRLEN + 2]; /*промежуточная строка в кодировке UTF-8*/
	int		u8size;			     /* размер в символах в промежуточной строке*/

	attr = line->attr;

	/* ВЫЗОВ ЧЕРЕЗ w_line() ? */
	if(onexit = ((posp == (int *)(-1)) ? 1 : 0))
		attr &= ~INP;  /* НЕТ, вызов r_line */
	cod = 0;

inp_retry:
out_string:
	cvt_ret = tsterror = 0;
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
			cvts_out(line, u8buf);
			u8size = u8wcs(wcsptr, u8buf);
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
		    ((attr & EDT) ? line->test : 0), posp);
	/*после редактора вернуть все в UTF-8*/
#ifdef DEBUG_R_LINE
	sout(-3, "1) e_str; editptr", (char *)editptr);
	sout(-4, "2) u8buf; wcstombs", u8buf);
#endif
	u8size = wcstombs(u8buf, editptr, /*size*/MAXLICO*4); /*like u8wcs, may be better limit*/
#ifdef DEBUG_R_LINE
	sout(-5, "3) e-str; u8buf", u8buf);
#endif
	/*
	 * ЕСТЬ ТОНКОСТИ С ФОРМАТОМ ПОСЛЕ РЕДАКТОРА:
	 *    НАДО РЕАГИРОВАТЬ ТОЛЬКО НА KB_NL
	 */
	if(cod != KB_NL) goto inp_test;

inp_format:
	/*==== ФОРМАТ НА ВВОДЕ */
	if(fmtlock) goto inp_test;  /* М.БЫТЬ БЛОКИРОВАН... */

	if(line->cvtf) {
		cvt_ret = (*line->cvtf)(line, cod, "r", /*editptr*/u8buf);
	} else if(line->cvts) {
		cvt_ret = cvts_in(line, u8buf);
	} else {
		/*просто строка - вернуть содержимое после редактирования*/
		strcpy(line->varl, u8buf);
#ifdef DEBUG_R_LINE
		sout(-9, "4) line->varl", line->varl);
#endif
		cvt_ret = 1;
	}
#ifdef DEBUG_R_LINE
	dout(-8, "5) line->varl", line->varl);
#endif
	if(cvt_ret == 0)
		    bell();
	//TODO: else { COMMIT editing result back to line->varl }
	/*==== ОШИБКА ФОРМАТА ? */
	if(cvt_ret == 0 && onexit == 0 ) goto edit_retry;

inp_test:
	/*==== ТЕСТ ПОСЛЕ РЕДАКТОРА ? */
	if(fmtlock || !(attr & EDT) ) {
		tsterror = !(line->test ? (*line->test)(line, cod) : TRUE);
	}
	/*==== ЕСТЬ ОШИБКА ? */
	if(tsterror)           goto inp_retry;

	switch(cod) {
	case KB_AU:      /* ПОСЛЕ ЭТИХ КОДОВ ПЕРЕХОД К ДР. ПОЛЯМ */
	case KB_AD:
	case KB_AL:
	case KB_AR:
	case KB_NL:
	case KB_KH:
	case KB_KE:
	case KB_PU:
	case KB_PD:
	case KB_TA:
	      break;
	default :       return(cod);
	}

	onexit = 1; attr &= ~INP;       /* АТРИБУТ ДЛЯ ПОКАЗА */
	goto    out_string;
}
