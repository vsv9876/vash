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
#define _XOPEN_SOURCE
#include <alloca.h>
#include <stdio.h>
#include <wchar.h>
#include <ctype.h>
#include "line.h"
#include "line0.h"

/* debug column, same as in "vtest/pmainv.c" */
#define TXT_CO 20
/*trim _OFF in names to debug */
#define DEBUG_VSHIFT_OFF
#define WC_INIT_DEBUG_OFF

extern SCRN scrn;

int     edinsm = 1;     /* флаг: режим вставки 1, замены 0 */
/* extern  int edinff;  */
int     edinff = 1;     /* флаг: показывать состояние редактора строки */
int     edshow = 1;     /* флаг: показывать строку до редактирования */

/*static int edbak = 0;	/* флаг: копировать буфер редактирования обратно при выходе из e_str */

/* variables actual after calling vshift() */
static int Lstop, Rstop; /* left stop, right stop -both inside vsize */
static int Nshift = 0;
static bool Nmore = 0;	/* string out of right vsize border */

static int Nsize = 0;	/* size */

/*
 * edit mode indicator and clock both shared the same place on the screen
 */
static char inf_old[7] = "  :..."; /* index state on editing string */
static char inf_new[7] = "      ";

static int *wc_iv;		/* map string index to visible position */
/*static int *wc_vw;		/* map width at visible position */

/* find internal index (reversive map)
 * whitch bigger or equal than visible index */
static int iv_ge(v)
int v;
{
	int i;
	for (i = 0; wc_iv[i] < v; i++);
	return(i);
}

/* find internal index (revesive map)
 * whitch equal visible index */
static int iv_eq(v)
int v;
{
	int i;
	for (i = 0; wc_iv[i] != v; i++);
	return(i);
}

static edinfo( infflg )
register int infflg;
{
	char *s;
/*	if ( !edinff )  return;*/
#ifndef CP_SAV
	cp_sav();
#endif
/*
	cp_set(lframe->maxli - INF_LI, lframe->maxco - 8, TXT);
	er_eol(TXT);
*/
	cp_set(WSHOW_LI, WSHOW_CO, WSHOW_AT); /*er_eol(WSHOW_AT);*/
	if ( infflg ) {
		s = inf_new;
		if (Nshift == 0) { s[0] = ' ';
		} else {           s[0] = '<';
		}
		if (Nmore) {       s[1] = '>';
		} else {           s[1] = ' ';
		}
		s[2]             = ':'; /*w_str("-");*/
		if ( edinsm ) {
			strcpy(&inf_new[3], "Ins");
		} else {
			strcpy(&inf_new[3], "Ovr");
		}
		if ( infflg < 2) {
			if (strcmp(inf_old, inf_new) != 0) {
				strcpy(inf_old, inf_new);
				w_str(inf_new);
			}
		} else {
			strcpy(inf_old, inf_new);
			w_str(inf_new);
		}
	} else {
		cp_set(WSHOW_LI, WSHOW_CO, TXT);
		er_eol(TXT);
	}
#ifndef CP_SAV
	cp_fet();
#endif

}

/*
 **************************************************************
   i=27                           v
   wcs: 0123456789o123456789o123456789o123456789-      size=40
vshift:                 ^                       ^-- '\0'
                       >.123456789.1<                  vsize=12
 stop-          -left      ^--  --^   -right
        |           |           |           |          i/vsize
 **************************************************************
 */
/*
 *                        ||||||
 * ....+ + + + .........+ + + .............
v= * 0123456789-123456789-123456789-123456789
i= * 0123468-123456789-135789-123456789
 * ....****..........***.............
 * |   | |            ||||
 */

/* calculate shift of visible part of string after i changed */
static int v_shift(size, vsize, i)
int size;	/* full buffer size */
int vsize;	/* visible size */
int i;		/* index of internal string */
{
	int req_shift;
	int v;	/* visible position */
	int iv;	/* wc_iv[i] */
	/*int size_v; /* visible position of end of buffer */

	int ofset = vsize/2; /*/2*/

	if (ofset == 0)
		ofset = 1;
	Nsize = size;
	/* calculate margins */
	Lstop = vsize/3;
	Rstop = vsize/4;
	if (Lstop == 0)
		Lstop = 1;
	/*if (Rstop == 0)
		Rstop = 1;*/
	if (Rstop > 5)
		Rstop = 5;
#ifdef  DEBUG_VSHIFT
	cp_sav();
	cp_set(0, -30, TXT|VEXT);
	fprintf(vttout, " i=%2d v/sz=%2d/%2d L/R=%2d/%2d",
						i, vsize, size, Lstop, Rstop);
	er_eol(TXT);
	cp_fet();
#endif
	Nmore = Nshift = 0;

	if (size <= vsize) {
		return 0;  /* visibility is 100%, this function is not required */
	}
	v = wc_iv[i];
	/*size_v = wc_iv[size - 1];*/
	Nmore = 1;
	/* hints 1st... */
	if (v >= (size - vsize)) {
		Nshift = size - vsize; /* last visible fragment */
		/*goto ret_vshift;*/
	}
	if (/*i > 0 &&*/ v/*i*/ < vsize - Rstop) {
		Nshift = 0;/* 1st visible fragment below Rstop */
	} else {
		/* initial settings */
		Nshift = ((v/*i*/ / ofset) * ofset);
		/* cursor near left border */
		if (v/*i*/ <= (Nshift + Lstop)) {
			Nshift -= ofset;
			if (Nshift < 0)
				Nshift = 0;
		}
		/* cursor near right border */
		if (v      >= (Nshift + vsize - Rstop)) {
			Nshift += ofset;
		}
		/* near end of buffer */
		/*if (v      > size_v - vsize) {*/
		if (Nshift + vsize > size) {
			Nshift = size - vsize; /* glue visible last portion to end */
			Nmore = 0;
		}
	}
ret_vshift:
	/* Nshift must be aligned to any wchar_t */
	iv = iv_ge(Nshift);
#ifdef  DEBUG_VSHIFT
	cp_sav();
	cp_set(1, -30, TXT|VEXT);
	/*fprintf(vttout, "di=%2d Nshift=%2d  vshift=%2d ", i - Nshift, Nshift, wc_iv[i]);*/
	fprintf(vttout, "Nshift=%2d iv_ge()=%2d v=%2d ", Nshift, iv, v);
	er_eol(TXT);
	cp_fet();
#endif
	/* Nshift must be aligned to any wchar_t */
	return iv;
	/*return (Nshift);*/
}

/* absorb wcsobj paramters */
int wcsobj_sup(wcsobj, wcs)
wcsobj_t *wcsobj;
wchar_t *wcs;
{
	int size;

	wcs = wcsobj;
	if (wcsobj->wco_sig == -1) { /* string object setup */
		(*wcs)++;
		(*wcs)++;
		size = wcsobj->wco_size;
		size--;
	}
	return size;
}

/*
 * write on screen visible part of string object
 */
static w_v_obj(wcsobj, vsize, /*i, */vshift)
wcsobj_t *wcsobj;
int vsize;	/* size of visible part of wcsobj->wcs */
/*int i;		/* current position of cursor which have to be visible anyway */
int vshift;	/* start of visible part of wcsobj->wcs */
{
	wchar_t *wcs; /* string to be shown */
	/*register int k;*/
	int size;
	int i;
	int k;
	int iv, w;

	wcs = wcsobj;
	size = vsize; /* preset for ordinary string */
	if (wcsobj->wco_sig == -1) { /* string object setup */
		wcs++;
		wcs++;
		size = wcsobj->wco_size;
		size--;
	}
	i = vshift;
	iv = wc_iv[vshift];
	w = 0;

	for (k = 0; k < vsize; i++, k += w) {
		iv = wc_iv[i];
		w = wcwidth(wcs[i]);
		if (iv + w <= vsize + wc_iv[vshift]) {
			w_wchr(wcs[i]);
		} else {
			w_wchr(L'~');
		}
	}

}

#ifdef  WC_INIT_DEBUG
static wc_init_show(wc_s, size)
wchar_t *wc_s;
int size;
{
	int i, iv, ix;
	char tmps[10];
	/*cp_set(-5, TXT_CO, TXT);*/
	cp_set(-5, TXT_CO, TXT);
	er_eol(TXT);
	for (iv = i = 0; i < size; i++) {
		iv = wc_iv[i];
		/*if (iv + wc_s[i] >= size)
		 continue;*/
		cp_set(-5, iv + TXT_CO, TXT);
		ix = i % 10;
		sprintf(tmps, "%1d", ix);
		w_str(tmps);
	}
}
#endif

/*
 *  initial map of string in buffer
 */
wc_init(wc_s, size)
wchar_t *wc_s;
int	size;
{
	int v_wc;
	int v = 0;
	int j = 0;
	/*
	 *                         |size
     * ....+ + + + .........+ + + .............
  v= * 0123456789-123456789-123456789-123456789
  i= * 0123468-123456789-135789
     * ....****..........***.............
     * |||||
     */
	while (j < size && wc_s[j] && v < size) {
		v_wc = wcwidth(wc_s[j]);
		/*wc_vw[v] = v_wc;*/
		wc_iv[j] = v; /* map visible position to current one */
		v += v_wc; /* prepare mapping for next position */
		j++;
	};

	while (j < size)
		wc_iv[j] = v++, wc_s[j++] = L' '; /*map outside of visibility*/
	wc_s[size] = L'\0';

	 /* terminate all visible part of the string */
	wc_s[j] = 0;
#ifdef WC_INIT_DEBUG
	cp_sav();
	wc_init_show(wc_s, size);
	cp_fet();
#endif
}

/*
 * ВЫПОЛНИТЬ ИЗМЕНЕНИЯ И ПОКАЗАТЬ
 * note: string buffer may be longer then visible size
 * */
static int fnd_lend(wc_s, i, size)
wchar_t *wc_s;
int i;
int size;
{
	int v, j, k;
	for (v = k = j = 0; j < size; j++) {
		if (isspace(wc_s[j])) {
			if (v == 0)
				k = j;
			v = 1;
		} else {
			v = 0; k = j;
		}
	}
	if (k > 0)
		i = k;
	return i;
}

static int chgstr(wcsobj, vsize, i, cod)
wcsobj_t *wcsobj;     /* editing string object buffer */
int  vsize;		/* visible size of the field/line on the screen */
int i;			/* position of the change if required */
kbcod cod;		/* kbcod() pressed */
{
	register wchar_t *wc_s; /* editing string buffer pointer */
	register int j;
	register int i_end;      /* position of last significant (non-space) symbol */
		 int i_chged;       /* position of symbol to be changed */

    wchar_t c;
		int size = vsize;	/* size (length) of string buffer, without trailing 0 */
		int vshift = 0;	/* start of visible part of s */
		int i_shift = 0; /* cursor position inside of visible part of s */
		int w;			/* width of character to be changed */

	wc_s = wcsobj;
	if (wcsobj->wco_sig == -1) {
		size = wcsobj->wco_size;
		/*wcs = &(wco->wcs);*/
		wc_s++; wc_s++; /* shift pointer to real string buffer */
		size--;
	}

	vshift = v_shift(size, vsize, i);

	if(edinsm)
		i_end = fnd_lend(wc_s, i, size);

	i_chged = i;
	if (cod == KB_KD) {
		for (j = i; j < (size - 1); j++) {
			c = wc_s[j] = wc_s[j + 1];
			if (j == (size - 1))
				c = wc_s[j] = L' ';
		}
	}
	if(cod == KB_DE) {
		if (i == 0)
			return (i);
		i = --i_chged;
		/*wc_init(wcs, size);*/
		w = wcwidth(wc_s[i_chged]);
		if(edinsm) {
			for(j = i_chged; j < i_end; j++)
				wc_s[j] = wc_s[j+1];
			wc_s[j++] = L' ';
		} else {
			wc_s[i_end = i_chged] = L' '/*cod*/;
		}
		/*TODO: new position left to width of deleted character*/
		/******* <--(KB_DE)--- ******/
		cp_abset(scrn.sc_li, scrn.sc_co - w/*1*/, scrn.sc_at);
	}
	else if (!ISCTL(cod)) {
		/* printable, including ' ' */
		if (edinsm) {
			for (j = size; --j > i_chged;)
				wc_s[j] = wc_s[j - 1]; /*Ins*/
			i_end += (i_end < (size - 1) ? 1 : 0);
		/*} else {
			i_end = i_chged;*/ /*Ovr*/
		}
		wc_s[i_chged] = cod;
		i += 1;
		/*wc_init(wcs, size);*/
	}
	wc_init(wc_s, size);
	/*for(j = i_chged; j <= i_end; j++) w_wchr(wcs[j]); */ /* on screen up to end */
	for(j = i_chged;
			j <= i_end &&
			wc_iv[j] + wcwidth(wc_s[j]) <= wc_iv[vshift] + vsize;
				j++)
		w_wchr( wc_s[j] ); /* on screen, up to end of visible part of the string */

	return (i);
}


/*
 * Редактор строки без предварительного показа старого содержимого (e_str() wrapper)
 */
#if 1
kbcod re_str(wcso, vsize, ctst, ofsp)
wcsobj_t   *wcso;   /* wchar_t editing object or simple string */
int     vsize;      /* visible size - размер поля редактирования */
kbcod (*ctst)();    /* тест для ввода печатаемых кодов */
int    *ofsp;       /* указатель на величину смещения от нач. поля */
{
	kbcod cod;
	int     _edshow, _edinff, _edinsm;

	_edshow = edshow; edshow = 1; /* 0 */
	_edinff = edinff; edinff = 0; /* 0 */
	_edinsm = edinsm; edinsm = 1;

	cod = e_str(wcso/*buf*/, vsize, ctst, ofsp);

	edshow = _edshow;
	edinff = _edinff;
	edinsm = _edinsm;

	return cod;
}
#else
/* вариант с конвертацией u8sobj_t -> wcsobj_t и обратно */
kbcod re_str(u8o, vsize, ctst, ofsp)
u8sobj_t   *u8o;      /* UTF-8 encoded -- editing object or simple string */
/*char   *u8s;      /* строка для редактирования*/
int     vsize;       /* visible size - размер поля редактирования */
kbcod (*ctst)();    /* тест для ввода печатаемых кодов */
int    *ofsp;       /* указатель на величину смещения от нач. поля */
{
	char   *u8s;      /* simple string */
	kbcod cod;
	int     _edshow, _edinff, _edinsm;

	wchar_t *wcs;	/* editing buffer, contains object (including string) to be edited */
	wcsobj_t *wcso;
	int edsize;	/* string length to be edited */

	int len, len2; /* length of typed strings trimmed from trailing spaces, before and after editing */
	mbstate_t ps = { 0 };

	if (u8o->u8o_sig == U8O_SIG) {
		edsize = u8o_size(u8o);
		u8s = u8o->u8s; /* u8s++; u8s++; u8s++;*/
	} else {
		edsize = vsize; /* only field size is reasonable */
		u8s = u8o;
	}
	wcs = wcso = alloca(sizeof(wchar_t) * (edsize + 3));
	wcs++; wcs++;
	if (u8o->u8o_sig == U8O_SIG) {
		len = u8owco(wcso, u8o);
	} else {
		wcso->wco_sig  = -1;
		wcso->wco_size = edsize;/*((MAXLICO))*/;
		len = u8swcs(wcs, u8s);
	}

	_edshow = edshow; edshow = 1; /* 0 */
	_edinff = edinff; edinff = 0; /* 0 */
	_edinsm = edinsm; edinsm = 1;

	cod = e_str(wcso/*buf*/, vsize, ctst, ofsp);

	edshow = _edshow;
	edinff = _edinff;
	edinsm = _edinsm;


	if (u8o->u8o_sig == U8O_SIG) {
		len2 = wcou8o(u8o, wcso);
	} else {
		len2 = wcsnu8s(u8s, wcs, edsize + 1);/*wcsrtombs(u8s, &wcs, 4*(edsizeMAXLICO + 1), &ps);*/
	}
	return cod;
}
#endif

/* find the end of typed text: position of last space after */
kbcod e_str(wcsobj, vsize, ctst, ofsp)
/*-----------------*/
/* РЕДАКТОР СТРОКИ */
/*-----------------*/
register wcsobj_t    *wcsobj;  /* container for wide char string to be edited */
int     vsize;           /* size of field visible -- РАЗМЕР ПОЛЯ РЕДАКТИРОВАНИЯ */
kbcod   (*ctst)();      /* test&check for input a printable codes -- ТЕСТ ДЛЯ ВВОДА ПЕЧАТАЕМЫХ КОДОВ */
int     *ofsp;          /* index pointer for editing position (cursor position) -- УКАЗАТЕЛЬ НА ВЕЛИЧИНУ СМЕЩЕНИЯ ОТ НАЧ. ПОЛЯ */
{
	/* НАЧАЛО ПОЛЯ И VIDEO ЗАДАЮТСЯ ЧЕРЕЗ cp_set();
	 * ctst ВОЗВРАЩАЕТ 0, ЕСЛИ КОД НЕ ИЗМЕНЯЕТ СТРОКУ;
	 *                -1, ЕСЛИ НАДО ЗАКОНЧИТЬ РЕДАКТИРОВАНИЕ;
	 *              cod,  ЕСЛИ КОД ИЗМЕНЯЕТ СОДЕРЖИМОЕ СТРОКИ.
	 */
	wchar_t tmps[8];
	register
	wchar_t    *wc_s;  /* wide char string to be edited -- СТРОКА ДЛЯ РЕДАКТИРОВАНИЯ*/
	wchar_t c;
	kbcod   cod;
	kbcod   ok;
	/*int     i_prev;			/* internal cursor position on previous cycle */
	register int i;			/* internal cursor position */
	register int j;
	         int k;
	register int  column;      /* ПОЗИЦИЯ И СТРОКА НА ЭКРАНЕ */
			 int  linenu;
			 int  attrib; /* логический видеоатрибут */
	int		v;				/* current visual position */
	int		iv;
	int		v_wc;			/* width of character */
	int     size;           /* size of string buffer */
	int     vshift = 0;			/* shift to visible part of the string edited */
	int		vshift_prev = 0;	/* shift on previouse cycle of editing */

	column = scrn.sc_co;
	linenu = scrn.sc_li;
	attrib = scrn.sc_at;

	size = vsize;
	wc_s = wcsobj;
	if (wcsobj->wco_sig == WCO_SIG) {
		size = wcsobj->wco_size;
		size--;
		/*wcs = &(wco->wcs);*/
		wc_s++; wc_s++; /* find pointer to actual string in object buffer */
	}
	/* build map: char_width(i)*/
	wc_iv = alloca((size + 2) * sizeof(int));
	/*wc_vw = alloca((size + 2) * sizeof(int));*/
	wc_init(wc_s, size);

	i = 0; /* always start from zero position on regular lines via r_line() */
	if (ofsp != NULL) {
		i = *ofsp;
	}

	v = j = 0; /* tmp usage of v */
	/*i_prev = i;*/
	/*v = w_v_obj(wcsobj, vsize, i, vshift);*/

	vshift = v_shift(size, vsize, i);
	v = wc_iv[i];

	if (edshow) {
		/* 1st show if required on start editing  */
		cp_abset(linenu, column, attrib);
		/* j = 0; while(j<size) w_wchr(wc_s[j++]); */
		w_v_obj(wcsobj, vsize, /*i, */vshift);
		edinfo(2);
	}

	/*
	 * editing cycle on every key pressed
	 */
	/*i_prev = i;*/

	/*vshift = -1; /*DEBUG*/

	for( ;; ) {
		/* check everytime - bounce from boundaries of buffer */
		if (i >= size)
			i = size - 1;
		if ((iv = wc_iv[i]) == 0
				|| iv >= size) {
			while (wc_iv[i] /*+ wcwidth(wc_s[i])*/ >= size )
				i -= wcwidth(wc_s[i]);
			if(i < 0)
				i = 0;
		}
#ifdef DEBUG_VSHIFT
		cp_sav();
		sprintf(tmps, "'%lc' ", wc_s[i]);
		cp_set(-6, 8, TXT); w_str(tmps);
		cp_fet();
#endif
		v = wc_iv[i];
		/*cp_abset(linenu, column+i, attrib);*/
		/* cursor on screen != cursor on the editing object */
		vshift_prev = vshift;
		/* calculate room for visible part of string buffer */
		vshift = v_shift(size, vsize, i);
		if (vshift != vshift_prev/* || i != i_prev*/) {
			cp_abset(linenu, column, attrib);
			w_v_obj(wcsobj, vsize, /*i, */vshift);
			edinfo(1);
		}

		cp_abset(linenu, column + v/*i*/ - wc_iv[vshift], attrib);

		/*i_prev = i;*/
		cod = r_cod(0);

		/* test&modify input with optional function, if requested */
		ok = ctst ? (*ctst)(cod, size, i) : cod;
		if(ok == -1) {	/* prevent output outside real screen borders */
			goto ret;
		} else if(ok == 0) {
			bell(); /* TODO: another indication */
		} else {
			switch(cod/* = ok*/) {
			case KB_PR: /* prefixed option -- ДОП. КОМАНДЫ РЕДАКТОРА СТРОКИ */
				switch(r_cod(0)) {
				case KB_AR:
					/* find the end of typed text */
					i = fnd_lend(wc_s, i, size);
					break;
				case KB_AL:
					i = 0;
					break;
				case L' ':
				case KB_SP:
					for(j=i; j<size; j++) {
						c = wc_s[j] = L' ';
						/*if (j < (vshift + vsize))
							w_wchr(c);*/
					}
					wc_init(wc_s, size);
					for (j = i; j < size; j++) {
						c = wc_s[j];
						if (j < (vshift + vsize))
							w_wchr(c);
					}
					break;
				case KB_DE:
					i = chgstr(/*wc_s*/wcsobj, vsize, i, cod);
					break;
				case KB_PR:
					edinsm=(edinsm ? 0 : 1); /* toggle Ins/Ovr*/
					edinfo(1);
					break;
				default:
					bell();
					break;
				};
				break;

			/* linlib 4 since 2017-05 */
			case KB_KE:
				i = fnd_lend(wc_s, i, size);
				break;
			case KB_PU:
			case KB_KH:
				i = 0;
				break;
			case KB_PD:
				i = wc_iv[size - 1];
				break;
			case KB_KI:
				edinsm=(edinsm ? 0 : 1); /* toggle Ins/Ovr*/
				edinfo(1);
				break;
			case KB_KD:
				i = chgstr(/*wc_s*/wcsobj, vsize, i, cod);
				break;
			/*----------------------------*/

			case KB_TA: /*TODO smth*//*bell(); break;*/
				/*cp_sav(); cpa(-2, 1, ATT); w_str("ta"); cp_fet();*/
				goto ret;
				break;
#ifdef OLD_USE_TAB
			case KB_TA: if(i == size-1) goto ret;
				   i += 8-(i%8); break;
#endif
			case KB_AL:
				if (i > 0) {
					i--;
				} else
					goto ret;
				break;
			case KB_AR:
				if (v/*i*/ == size - 1)
					goto ret;
				i++;
				break;
			case KB_NL: goto ret;
			case KB_DE:
				i = chgstr(/*wc_s*/wcsobj, vsize, i, cod);
					   break;
			default:
				if (ISCTL(cod))
					goto ret;
				else {/* printable codes, including ' ' */
					/*if (wc_iv[i] + wcwidth(cod) > size) {
						bell();
						 w_wchr(wc_s[i] = L' '); /* cod will not enter ever
					} else*/
					i = chgstr(/*wc_s*/wcsobj, vsize, i, cod);
				}
			}
		}
		/* КОНЕЦ ЦИКЛА РЕДАКТОРА */
	}

ret:
	iv = iv_ge(size);
	wc_s[iv/*size*/] = L'\0';

	j = iv - 1;
	if (wc_iv[j] + wcwidth(wc_s[j]) > size) {
		bell();
		w_wchr(wc_s[j] = L' '); /* cod will not enter ever */
		wc_iv[j] = 1;			/* actualize the map */
	}

	if(ofsp != NULL)
		*ofsp = i;

	/* ПОДЧИСТИТЬ ПРОБЕЛЫ В КОНЦЕ СТРОКИ */
	for (i = size; --i >= 0 && (wc_s[i] == L' ');)
		;
	wc_s[++i] = L'\0';
	edinfo(0);
	/*free(wc_iv);*/
	return(cod);
}
