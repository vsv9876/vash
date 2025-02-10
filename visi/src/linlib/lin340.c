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
#include <stdlib.h>

#ifndef NO_ALLOCA_H
#include <alloca.h>
#endif

#define _XOPEN_SOURCE
#include <stdio.h>

#include <wchar.h>
#include <string.h>
#include "line.h"
#include "line0.h"

extern int wcwidth();


extern LPA lpainp[];
extern LPA lpaout[];

extern int allcod;

#define FLOAT_CVT /*not supported, use line->cvtf instead, please*//*TODO WTF comment about */

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
	const char *fo;      /* ФОРМАТ printf */
	const char *va;      /* УКАЗАТЕЛЬ НА ПЕРЕМЕННУЮ */
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


void w_line(line)
register LINE *line;
{
/*      next_j();     */

	r_line(line, (int *)(-1)); /* preset to call in write mode (only output, no input) */
}

/*
 * ВЫВОД/ВВОД ЛИНИИ, ВЕРНУТЬ КОД ПОСЛЕДНЕЙ КЛАВИШИ
 */
kbcod r_line(line, posp)
register LINE    *line;
	 int     *posp; /* position of editing */
{

	extern void unr_c();
	kbcod   cod;
	short   attr;
	int     cvt_ret;        /* exit code from sscanf: OK if = 0 */
	int     tsterror;       /* testing error flag */
	int     midcnt;         /* counter for middle alinment */
	int     filch;          /* fill character */
	int     slen;           /* string length before alignment */
	int     size;           /* screen size of shown part of line */
	int     fmtlock;        /* flag: formatting locked */
	int     onexit;         /* fkag: on exit from this function */
	int     base;           /* begin of content, offset from the prompter */

	wcsobj_t *wcoptr = NULL;		/* tmp editing object wchar_t converted from u8sobj_t */
	wcsobj_t *objptr = NULL;		/* editing object wchar_t */
	u8sobj_t *u8optr = NULL;		/* editing object UTF-8 encoded */

	int  i;

	int		clrwcs = 0;			/* flag: clear before editing (wcsptr[0] = 0) */
	wchar_t *wcsptr = NULL;			/* editing start after prompter */
	wchar_t  wcsbuf[STRBUF + 1];  /* internal buffer in internal encoding WCS-4 */

	char    u8buf[(4*STRBUF) + 4];  /* intermediate string UTF-8 encoded */
	int		u8size;                 /* intermediate string size*/

 	int		vlen;		/* visible size in column occupied on screen */
 	int		v;
 	int		v_cw;

 	attr = line->attr;
	fmtlock = (attr & FLO) == FLO ? 1 : 0;

	/* calling via w_line() ? */
	if(onexit = ((posp == (int *)(-1)) ? 1 : 0))
		attr &= ~INP;  /* НЕТ, вызов r_line */
	cod = 0;

	/* init object pointers if required */
	if (line->flag & U8SOBJ)
		u8optr = (u8sobj_t*)(line->varl);
	else if (line->flag & WCSOBJ)
		objptr = (wcsobj_t*)(line->varl);
	else {
		;/* TODO: optimized alloca sizes for buffers there */
	}
inp_retry:
out_string:
	cvt_ret = tsterror = 0;
	wcsptr = wcsbuf;
	/*editptr = wcsbuf;*/
	base = 0;
	size = line->size;
	slen = -1;
	v = 0;

	i = attr & VIDEO;

#ifdef RETRO_FILCH
	/*==== ЯВНОЕ ЗАПОЛНЕНИЕ ПРИ ОТСУТСТВИИ АТРИБУТОВ */
	if (((line->attr & INP) == 0 ) && (lpaout[i].lpa_a == 0))
			filch = '_';
	else            filch = ' ';
#else
	filch = ' ';
	if ((line->attr & VIDEO) == HDR && sgrmode == 0) {
		if (line->attr & VEXT) {
			filch = lpainp[/*HDR*/ i].lpa_p;
		} else {
			filch = lpaout[/*HDR*/ i].lpa_p;
		}
	}
#endif
	/*==== prompter */
	if(attr & PMT) {
		/*editptr++;*/ base++; size--;
		if(attr & INP)  *wcsptr++ = lpainp[i].lpa_p;
		else            *wcsptr++ = lpaout[i].lpa_p;
	}
	/*==== output format on write */
	if (line->flag & U8SOBJ) {
		/* note: only visible first part need to be shown */
 		u8size = u8snwcs(wcsptr, u8optr->u8s, /*size*/u8o_size(u8optr));
	} else if( !fmtlock) { /* if no format lock raised */
		if(line->cvtf) { /* function raised */
			(*(line->cvtf))(line, cod, "w", u8buf);
			u8size = u8swcs(wcsptr, u8buf);
		}
		else if(line->cvts) { /* fprintf style */
			cvts_out(line, u8buf);
			u8size = u8swcs(wcsptr, u8buf);
		}
		else { /* simple string as is */
			goto string_simple;
		}
	} else {
string_simple:
		/*strncpy(s, line->varl, size); s[size] = 0;*/
		u8size = u8snwcs(wcsptr, line->varl, size);
	}
	for (v = 0, vlen = 0; v < size; v++) {
		v_cw = wcwidth(wcsptr[v]);
		if (vlen + v_cw <= size) {
			vlen += v_cw;
		} else {
			while(vlen++ < size)
				wcsptr[v++] = 0 ;
			break;
		}
	}
	/*wcsptr[size] = 0;*/
	wcsptr[v] = 0;
	/* slen = strlen(s);*/
	slen = wcslen(wcsptr);
	vlen = vsize(wcsptr);

	/*==== align on middle */
	if(attr & MID) {
		if((midcnt = ((size - /*slen*/vlen)/2)) > 0) {
			slen += midcnt;
			vlen += midcnt;
			for(i = slen; i >= midcnt; i--)
				wcsptr[i] = wcsptr[i - midcnt];
			while(i >= 0)
				wcsptr[i--] = filch;
		}
	}
	/*==== padding fill */
	if(attr & PAD) {
		for(i = slen; vlen < size;) {
			wcsptr[i++] = filch;
			vlen++;
		}
		wcsptr[i] = 0;
	}

#ifdef RETRO_R_LINE
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
#else
	/*==== cursor + video + prompter */

	/*==== display (write or read) lines which FAST READ of NOT EDIT */
	if ( (posp != (int *)(-1)) && (attr & (LFASTR|NED)) == (LFASTR|NED) ) {
		;/* w_wchr(*wcsbuf);    /* только для r_line */
	}
	else {
		/* display common part */
		cp_set(line->line, line->colu, attr);
		if (objptr) {
			if (attr & PMT)
				w_wchr(wcsbuf[0]);
			w_wcstrv(objptr->wcs, size); /* only first size will be shown */
		} else {
			w_wcstr(wcsbuf); /* may contain a prompter char */
		}
	}
	if(onexit || posp == (int *)(-1))
		return(cod);    /* end for calling via w_line */

	/*==== cursor before 1st input key */
	/* hint for selector lines */
	if ((attr & LTYPE) == ALT &&
			(wcsptr[0]=='[' || wcsptr[0]=='(' || wcsptr[1]==' ')) {
		cp_set(line->line, 1 + line->colu, attr);
	} else {
		cp_set(line->line, line->colu, attr);
	}
#endif

	/*==== 1st code reading */
	switch(cod = r_cod(0)) {
	case KB_DE:
		    clrwcs = 1;
#if 0
			*wcsptr/**editptr*/ = '\0';
#endif
			/*NO BREAK*/
	case  ' ':
			if(posp != (int *)(-1) && posp)
				*posp=0;
			break;
	default:
		/* note:
		 * formatting on input required
		 * after calling editor and after KB_NL only;
		 */
		if((attr & NED) == 0) {
			if(allcod && ISCTL(cod) == 0) { /*&& cod1(cod) == 0) {*/
				unr_c(cod);     /* unread a code back */
				clrwcs = 1;
				*wcsptr/**editptr*/ = L'\0';    /* clean string */
			} else
				goto inp_test;
		} else {
			if (cod == KB_NL) {
				/* special case: line is menu item (and nаvigation suspend flag is raised) */
				if (/* 0 != (line->flag & SUSNL) &&*/ (MSE & (line->attr & VIDEO))) {
						if(posp != (int *)(-1) && posp)
							*posp=0;
				}
				goto inp_format;
			}
			break;
		}
	}

edit_retry:
	/*==== no edit flag raised? */
	if(attr & NED)
		goto inp_format;

	cp_set(line->line, base + line->colu, attr);

	/*
	 * caling editor there
	 */
	if (line->flag & U8SOBJ) {
		/* init an editing object from u8sobj_t */
		wcoptr = alloca(sizeof(wcsobj_t)
				+ (u8o_size(u8optr) * sizeof(wchar_t) /* WCO_SIZE_MAX*/));
		u8owco(wcoptr, u8optr);
		if (clrwcs)
			wcoptr->wcs[0] = 0;
		cod = e_str(wcoptr, size, 0, posp);
	} else if (objptr && objptr->wco_sig == WCO_SIG) {
		if (clrwcs)
			objptr->wcs[0] = 0;
		cod = e_str(objptr, size, 0, posp);
	} else {
		if (clrwcs)
			wcsptr[0] = 0;
		cod = e_str((wcsobj_t *)wcsptr/*editptr*/, size,
				((attr & EDT) ? line->test : 0), posp);
		/* finally back to UTF-8 encoding */
#ifdef DEBUG_R_LINE
		sout(-3, "1) e_str; editptr", (char *)editptr);
		sout(-4, "2) u8buf; wcstombs", u8buf);
#endif
		u8size = wcstombs(u8buf, wcsptr/*editptr*/, /*size*/MAXLICO * 4); /*like u8wcs, may be better limit*/
#ifdef DEBUG_R_LINE
		sout(-5, "3) e-str; u8buf", u8buf);
#endif
	}
	clrwcs = 0;

	if (cod != KB_NL)
		goto inp_test;

inp_format:
	/*==== format on input */
	if (fmtlock)
		goto inp_test;

	if (u8optr && (line->flag & U8SOBJ)) {
		/*cvt_ret = wcou8o(u8optr, wcsptr);*/
		cvt_ret = wcsnu8s(u8optr->u8s, wcoptr->wcs, /*size*/
					u8o_size(u8optr)) ? 1 : 0;
    }
	else if (objptr && objptr->wco_sig == WCO_SIG) {
		cvt_ret = 1;
	} else if (line->cvtf/* && !(line->flag & U8SOBJ)*/) {
		cvt_ret = (*line->cvtf)(line, cod, "r", /*editptr*/u8buf);
	} else if(line->cvts) {
		cvt_ret = cvts_in(line, u8buf);
	} else {
		/* simple string: back content after editing */
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
	/*TODO: else { COMMIT editing result back to line->varl }*/
	/*==== formatting error? */
	if(cvt_ret == 0 && onexit == 0 )
		goto edit_retry;

inp_test:
	if(fmtlock || !(attr & EDT) ) {
		tsterror = !(line->test ? (*line->test)(line, cod) : TRUE);
	}
	if(tsterror)
		goto inp_retry;

	switch (cod) {
	case KB_AU: /* navigation codes, including KB_NL */
	case KB_AD:
	case KB_AL:
	case KB_AR:
	case KB_TA:
	case KB_NL:
	case KB_KH:
	case KB_KE:
	case KB_PU:
	case KB_PD:
		break;
	default:
		return (cod);
	}

	onexit = 1; attr &= ~INP;       /* attribute on writeout to display */
	goto    out_string;
}
