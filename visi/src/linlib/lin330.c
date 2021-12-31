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
	cp_set(WSHOW_LI, WSHOW_CO, WSHOW_AT); er_eol(WSHOW_AT);
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

/* calculate shift of visible part of string after i changed */
static int v_shift(size, vsize, i)
int size;	/* full buffer size */
int vsize;	/* visible size */
int i;
{
	int req_shift;
	int x;

	int ofset = vsize/2;

	Nsize = size;

	Lstop = vsize/3;
	Rstop = vsize/4;
	if (Lstop == 0)
		Lstop = 1;
	if (Rstop == 0)
		Rstop = 1;
	if (Rstop > 5)
		Rstop = 5;

#define DEBUG_VSHIFT
#ifdef  DEBUG_VSHIFT
	cp_sav();
	cp_set(0, -30, TXT|VEXT);
	fprintf(vttout, " i=%2d v/s=%2d/%2d L/R=%2d/%2d",
						i, vsize, size, Lstop, Rstop);
	er_eol(TXT);
	cp_fet();
#endif
	Nmore = Nshift = 0;
	if (size <= vsize) {
		return 0; /* visible 100%, this function not required */
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

	Nmore = 1;
	/* hints 1st... */
	if (i >= (size - vsize)) {
		Nshift = size - vsize; /* last visible fragment */
	}
	if (i >= 0 && i < vsize - Rstop) {
		Nshift = 0;/* 1st visible fragment below Rstop */
	} else {
		/* initial settings */
		Nshift = (i / ofset) * ofset;
		/* cursor near left border */
		if (i <= (Nshift + Lstop)) {
			Nshift -= ofset;
			if (Nshift < 0)
				Nshift = 0;
		}
		/* cursor near right border */
		if (i >= (Nshift + vsize - Rstop)) {
			Nshift += ofset;
		}
		/* near end of buffer */
		if ((Nshift + vsize) > size) {
			Nshift = size - vsize; /* glue visible last portion to end */
			Nmore = 0;
		}
	}
#ifdef  DEBUG_VSHIFT
	cp_sav();
	cp_set(1, -30, TXT|VEXT);
	fprintf(vttout, "di=%2d Nshift=%2d ", i - Nshift, Nshift);
	er_eol(TXT);
	cp_fet();
#endif
	return Nshift;
}

/*
 * write on screen visible part of string object
 */
static w_v_obj(wcsobj, vsize, i, vshift)
wcsobj_t *wcsobj;
int vsize;	/* size of visible part of wcsobj->wcs */
int i;		/* current position of cursor which have to be visible anyway */
int vshift;	/* start of visible part of wcsobj->wcs */
{
	 wchar_t *wcs;	/* string to be shown */
	 register int j;
	 register int k;
	 int /*i, */size;

	 wcs = wcsobj; size = vsize;
	 if (wcsobj->wco_sig == -1) {
		 wcs++; wcs++;
		 size = wcsobj->wco_size;
	 }

	 for (k = vshift; k < vsize + vshift; k++) {
		 if (wcs[k] == 0)
			 break;
		 w_wchr(wcs[k]);
	 }
	 for (; k < vsize + vshift; k++) {
		 w_wchr(L' ');
	 }

}

/* ВЫПОЛНИТЬ ИЗМЕНЕНИЯ И ПОКАЗАТЬ */
/* note: string buffer may be longer then visible size */
static int chgstr(wcsobj, vsize, i, cod)
wcsobj_t *wcsobj;     /* editing string object buffer */
int  vsize;		/* visible size of the field/line on the screen */
int i;			/* position of the change if required */
kbcod cod;		/* kbcod() pressed */
{
	register wchar_t *wcs; /* editing string buffer pointer */
	register int j;
	register int i_end;      /* position of last significant (non-space) symbol */
		 int i_chged;       /* position of symbol to be changed */

		int size = vsize;	/* size (length) of string buffer, without trailing 0 */
		int vshift = 0;	/* start of visible part of s */
		int i_shift = 0; /* cursor position inside of visible part of s */

	wcs = wcsobj;
	if (wcsobj->wco_sig == -1) {
		size = wcsobj->wco_size;
		/*wcs = &(wco->wcs);*/
		wcs++; wcs++; /* shift pointer to real string buffer */

	}

	vshift = v_shift(size, vsize, i);

	/* find logical end of edited text in the string */
	if(edinsm) {
		for(i_end = size; i_end > 0 && wcs[--i_end] == ' '; ) ;
		if(i_end < i) i_end = i;
	}
	i_chged = i;
	if(cod == KB_DE) {
		/*cod = ' ';*/
		i = --i_chged;
		if(edinsm) {
			for(j = i_chged; j < i_end; j++)
				wcs[j] = wcs[j+1];
			wcs[j++] = ' ';
		} else {
			wcs[i_end = i_chged] = ' '/*cod*/;
		}
		/******* <--(KB_DE)--- ******/
		cp_abset(scrn.sc_li, scrn.sc_co - 1, scrn.sc_at);
	} else {
		if(edinsm) {
			for(j = size; --j > i_chged; )
				wcs[j] = wcs[j-1]; /*Ins*/
			i_end += (i_end < (size-1) ? 1 : 0);
		} else {
			i_end = i_chged; /*Ovr*/
		}
		wcs[i_chged] = cod;
		i += 1;
	}

	/*for(j = i_chged; j <= i_end; j++) w_wchr(wcs[j]); */ /* on screen up to end */
	for(j = i_chged; j <= i_end && j < (vshift + vsize); j++)
		w_wchr( wcs[j] ); /* on screen, up to end of visible part of the string */
	return (i);
}


/*
 * Редактор строки без предварительного показа старого содержимого (e_str() wrapper)
 */
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
		len2 = wcsu8s(u8s, wcs, edsize + 1);/*wcsrtombs(u8s, &wcs, 4*(edsizeMAXLICO + 1), &ps);*/
	}
	return cod;
}


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
	register wchar_t    *wc_s;  /* wide char string to be edited -- СТРОКА ДЛЯ РЕДАКТИРОВАНИЯ*/
	wchar_t c;
	int     size;           /* size of string */
	int     vshift = 0;			/* shift to visible part of the string edited */
	int		vshift_prev = 0;
	int     i_prev;			/* internal cursor position on previouse cycle */
	register int i;			/* internal cursor position */
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

	/*edinfo(1);*/

/*
	str_l = alloca(sizeof(wchar_t) * (size+1));
	u8wcs(str_l, ext_l);
*/
	size = vsize;
	wc_s = wcsobj;
	if (wcsobj->wco_sig == WCO_SIG) {
		size = wcsobj->wco_size;
		/*wcs = &(wco->wcs);*/
		wc_s++; wc_s++; /* find pointer to actual string in object buffer */
	}
	/* fill buffer with spaces (' ') incuding part which is not shown -- заполнить пробелами конец строки -- в буфере, не для экрана */
	j = 0;
	while(j < size && wc_s[j]) j++;
	while(j < size)       wc_s[j++] = L' ';
	//wc_s[j] = 0;
	wc_s[size] = L'\0'; /* terminate all, not only visible part of the string */

	i = 0; /* always start from zero position on regular lines via r_line() */
	if (ofsp != NULL) {
		i = *ofsp;
	}
	/*i_prev = i;*/

	vshift = v_shift(size, vsize, i);
	edinfo(2);
	if (edshow) {
		/* 1st show if required on start editing  */
		cp_abset(linenu, column, attrib);
		/* j = 0; while(j<size) w_wchr(wc_s[j++]); */
		w_v_obj(wcsobj, vsize, i, vshift);
	}

	/*
	 * editing cycle on every key pressed
	 */
	i_prev = i;
	for( ;; ) {
		if(i >= size) i = size-1;       /* check everytime - bounce from end of buffer 1 position back */

		/*cp_abset(linenu, column+i, attrib);*/
		/* cursor on screen != cursor on the editing object */
		vshift_prev = vshift;
		vshift = v_shift(size, vsize, i);
		if (vshift != vshift_prev/* || i != i_prev*/) {
			cp_abset(linenu, column, attrib);
			w_v_obj(wcsobj, vsize, i, vshift);
			edinfo(1);
		}
		cp_abset(linenu, column + (i - vshift), attrib);

		i_prev = i;
		cod = r_cod(0);

		/* test&modify input with optional function, if requested */
		ok = ctst ? (*ctst)(cod, size, i) : cod;
		if(ok == -1) {	/* prevent output outside real screen borders */
			goto ret;
		} else if(ok == 0) {
			bell();
		} else {
			switch(cod/* = ok*/) {
			case KB_PR: /* prefixed option -- ДОП. КОМАНДЫ РЕДАКТОРА СТРОКИ */
				switch(r_cod(0)) {
				case KB_AR: /* find the end of typed text */
					for(i=size-1; isspace(wc_s[i]);	i--)
						;
					i++;
					break;
				case KB_AL:
					i = 0;
					break;
				case ' ':
					for(j=i; j<size; j++) {
						c = wc_s[j] = ' ';
						if (j < (vshift + vsize))
							w_wchr(c);
					};
					break;
				case KB_DE:
					for(j = i; j < (size - 1); j++) {
					   c = wc_s[j] = wc_s[j+1];
					   if (j == (size - 1))
						   c = wc_s[j] = L' ';
					   if (j < (vshift + vsize))
						   w_wchr(c);
					};
					wc_s[size] = L'\0';
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
			case KB_KE: /* find the end of typed text */
				for(i = size-1; isspace(wc_s[i]);	i--)
					;
				i++;
				break;
			case KB_KH:
				i = 0;
				break;
			case KB_PD:
				i = size-1;
				break;
			case KB_KI:
				edinsm=(edinsm ? 0 : 1); /* toggle Ins/Ovr*/
				edinfo(1);
				break;
			case KB_KD:
				for(j = i; j < (size - 1); j++) {
				   c = wc_s[j] = wc_s[j+1];
				   if (j == (size - 1))
					   c = wc_s[j] = L' ';
				   if (j < (vshift + vsize))
					   w_wchr(c);
				};
				wc_s[size] = L'\0';
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
				   i = chgstr(/*wc_s*/wcsobj, vsize, i, cod);
				   break;
			default:
				if(ISCTL(cod))
					goto ret;
				else /* printable codes, including ' ' */
					i = chgstr(/*wc_s*/wcsobj, vsize, i, cod);
			}
		}
		/* КОНЕЦ ЦИКЛА РЕДАКТОРА */
	}
ret:
	wc_s[size] = L'\0';
	if(ofsp != NULL) *ofsp = i;

	/* ПОДЧИСТИТЬ ПРОБЕЛЫ В КОНЦЕ СТРОКИ */
	for (i=size; --i>=0 && (wc_s[i]==' ');) ;
	wc_s[++i] = L'\0';
	edinfo(0);
/*	wctomb(ext_l, str_l);*/
	return(cod);
}
