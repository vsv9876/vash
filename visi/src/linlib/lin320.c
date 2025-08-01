/*
**      +----------+    БИБЛИОТЕКА ВВОДА-ВЫВОДА
**     (c) linlib  !    ДЛЯ АЛФАВИТНО-ЦИФРОВЫХ
**      +----------+    ВИДЕОТЕРМИНАЛОВ
**/

/*
 *      $Header: lin320.c,v 1.1 90/12/27 16:29:15 vsv Rel $
 *
 *      $Log:	lin320.c,v $
 * Revision 1.1  90/12/27  16:29:15  vsv
 * ВЕРСИЯ LINLIB_3
 * 
 * Revision 3.2  89/08/29  15:16:53  vsv
 * ВЕРСИЯ LINLIB_3
 * 
 * Revision 3.1  88/06/16  17:52:06  vsv
 * ИСПРАВЛЕНЫ ОШИБКИ УСТАНОВКИ АТРИБУТОВ,
 * БЫЛ НЕВЕРНО ИСПОЛЬЗОВАН ТИП ДАННЫХ:
 * ВМЕСТО short БЫЛ char (bool).
 * 
 * Revision 3.0  88/06/06  08:54:18  vsv
 * ИСПРАВЛЕНА ОШИБКА В ФУНКЦИИ er_scr,
 * КОТОРАЯ ПРИВОДИЛА К ЗАЦИКЛИВАНИЮ.
 * 
 * Revision 3.0  87/12/21  12:18:32  vsv
 * ПРЕДВАРИТЕЛЬНЫЙ ВЫПУСК.
 * 
 */


#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include "line.h"
#include "line0.h"
#include "lineva.h"

extern  char *UP;
extern  char *BC;
SCRN scrn = { 0 };

/* defined in lin210.c */
extern  LPA lpaout[];
extern  LPA lpainp[];
extern  int sgrmode;

#define AW_INIT_STATE (A_SO|A_US|A_MD)
#define NOCOLOR (char *)0

/* old attributes and color sgr */
static    int awstate = AW_INIT_STATE;
static  char *acstate = NOCOLOR;
static    int aixstate = 0;

#define COLOR_ANSI
#ifdef COLOR_ANSI

static void w_sgr(sgr)
char *sgr;
{
	static char s_csi[40] = "";
	char *s = s_csi;
	char *fmt;

	if(sgrmode <= 1) {
		/* dumb + b&w monochrome */
		return;
	}
	if (sgr == NOCOLOR || *sgr == '\0') {
		/* 0: CMD == clean all colors by LINLIB concept */
		sgr = lpaout[0].lpa_sgr;  
		acstate = NOCOLOR;
		return;
	}
	if (acstate == (char *)0 || acstate != sgr) {
		sprintf(s_csi, "\033[%sm", sgr);
		w_raw(s_csi);
		acstate = sgr;
	}
}

void
at_set(aw_new)
/*
 * УСТАНОВИТЬ ВИДЕОАТРИБУТЫ
 */
int aw_new;        /* ИНДЕКС И ФЛАГИ АТРИБУТОВ */
{
	extern SCRN scrn;

	int   aw_old;   /* СЛОВО СТАРЫХ АТРИБУТОВ */
	char *ac_old;
	int aw;        	/* НОВЫЕ АТРИБУТЫ */
	char *ac;	/* color sgr to be set */
	int  ax;	/* индекс для поиска по таблицам атрибутов */

	/* dumb mode, without attributes at all - in some cases its wrong - vt52+ has so/se attribute */
	if (sgrmode == 0) return;

	aw = (aw_new & VIDEOM); /* disable garbage from old code */

	ac =
	ac_old = acstate;
	aw_old = awstate;
	if (sgrmode > 1) {
		ac = NOCOLOR;
	}

	scrn.sc_at = aw;
	ax    = (aw & VIDEO);
	if((aw & INP) || (aw & VEXT)) {
		aw = lpainp[ax].lpa_a;
		ac = lpainp[ax].lpa_sgr;
	} else {
		aw = lpaout[ax].lpa_a;
		ac = lpaout[ax].lpa_sgr;
	}
	/* оптимизация повторной выдачи*/
	if(ac == ac_old && aw == aw_old) {
		/* биты атрибутов и раскраска не менялись */
		return;
	}
	/*aixstate = ax;
	scrn.sc_at = aw;*/

	/* все атрибуты погасить */
	if(sgrmode & 01) {
		if(aw_old & A_SO) 					{ w_raw(t_se); }
		if(aw_old & A_US)		     		{ w_raw(t_ue); }
		if(aw_old & A_ZH)		     		{ w_raw(t_zr); }
		if(aw_old & (A_MD|A_MR|A_MB|/*A_MK|*/A_MH)) 	{ w_raw(t_me); }
	} else {
		w_raw(t_me);
	}
	if (sgrmode > 1) {
		acstate = NOCOLOR;
	}

	/* включить атрибуты, сначала b/w mono */
	if(aw != 0 && (sgrmode & 01)) { /* all odd modes */
		if(aw & A_ZH) w_raw(t_zh);
		if(aw & A_SO) w_raw(t_so);
		if(aw & A_US) w_raw(t_us);
		if(aw & A_MD) w_raw(t_md);
		if(aw & A_MR) w_raw(t_mr);
		if(aw & A_MB) w_raw(t_mb);
		/*if(aw & A_MK) w_raw(t_mk);*/
		if(aw & A_MH) w_raw(t_mh);
		awstate = aw;
	}
	if (sgrmode > 1) {
	    if (ac != lpaout[TXT].lpa_sgr) {
	    	w_sgr(lpaout[TXT].lpa_sgr); /* TXT is a base for other */
	    }
	    w_sgr(ac);
	    acstate = ac;
	}
}
#else

void
at_set(awi)
register int awi;        /* ИНДЕКС И ФЛАГИ АТРИБУТОВ */
/*--------------------------*/
/* УСТАНОВИТЬ ВИДЕОАТРИБУТЫ */
/*--------------------------*/
{
	extern SCRN scrn;

	register int awold;     /* СЛОВО СТАРЫХ АТРИБУТОВ */
	register int aw;        /* НОВЫЕ АТРИБУТЫ */

	awold = awstate;
	scrn.sc_at = (awi &= VIDEOM);
	/* VCOLOR ПОКА НЕ ПОДДЕРЖИВАЕТСЯ ... */
	if(awi & INP) aw = lpainp[awi & VIDEO].lpa_a;
	else          aw = lpaout[awi & VIDEO].lpa_a;

	if(aw == awold) return;         /* БИТЫ ВИДЕО НЕ ИЗМЕНИЛИСЬ */
	awstate = aw;                    /* ЗАПОМНИТЬ НА СЛЕД. РАЗ */

	/* СНАЧАЛА ВСЕ ВЫКЛЮЧИТЬ */
	if(awold & A_SO) w_raw(t_se);
	if(awold & A_US) w_raw(t_ue);
	if(awold & (A_MD|A_MR|A_MB|/*A_MK*/A_MH)) w_raw(t_me);

	if(awi != 0) {
		/* ТЕПЕРЬ ВКЛЮЧИТЬ */
		if(aw & A_ZH) w_raw(t_zh);
		if(aw & A_SO) w_raw(t_so);
		if(aw & A_US) w_raw(t_us);
		if(aw & A_MD) w_raw(t_md);
		if(aw & A_MR) w_raw(t_mr);
		if(aw & A_MB) w_raw(t_mb);
		/*if(aw & A_MK) w_raw(t_mk);*/
		if(aw & A_MH) w_raw(t_mh);
	}
}
#endif

/*---------------------------*/
/* УСТАНОВКА ПОЗИЦИИ КУРСОРА */
/* И АТРИБУТОВ ТЕКСТА        */
/*---------------------------*/
void
cp_cret() /* set cursor at column=0 at the unknown(yet not synced) row */
{
	scrn.sc_co = 0;
	w_raw("\r");
}

void
cp_zero() /* prevent output masking outside screen:
  rare used to sync off_screen and on_screen writing position */
{
	scrn.sc_co = 0; /* not precise, sorry */
}

void
cp_abset(li, co, at) /* absolute in hw screen coordinates */
int li, co, at;
{
	char *p;
	int aa;
	extern char *tgoto();

	if(li < 0) li = (hwframe.maxli + li);
	if(co < 0) co = (hwframe.maxco + co);

	/* remember abs pos for w_chr(), w_wchr() */
	scrn.sc_li = li; scrn.sc_co = co; scrn.sc_at = (at & VIDEOM);

	if (scrn.sc_li <= hwframe.maxli && scrn.sc_co <= hwframe.maxco) {
		p = tgoto(t_cm, co, li);
		w_raw( p);

		aa = (at & VIDEOM);
		at_set(aa);
	}
}

void 
cp_set(li, co, at) /* relative */
int li, co, at;
{
	char *p;
	int aa;
	extern char *tgoto();

	/* convert relative coordinates of logical frame to positive ones */
	if(li < 0) li = (lframe->maxli + li);
	if(co < 0) co = (lframe->maxco + co);
	if (li < 0 || co < 0) return;
	/* prevent new positioning outside of logical borders */
	if (li > lframe->maxli || co > lframe->maxco) return;

	/* get absolute coordinates on scren */
	li = lframe->baseli + li;
	co = lframe->baseco + co;
	if(li < 0) li = (hwframe.maxli + li);
	if(co < 0) co = (hwframe.maxco + co);

	/* remember abs pos for w_chr(), w_wchr() */
	scrn.sc_li = li; scrn.sc_co = co;
	/* prevent output outside real screen borders */
	if (scrn.sc_li <= hwframe.maxli && scrn.sc_co <= hwframe.maxco) {
		p = tgoto(t_cm, co, li);
		w_raw( p);

		aa = (at & VIDEOM);
		at_set(aa);
	}
}
/*-----------------------------------------------------*/
/* СОХРАНИТЬ/ВОССТАНОВИТЬ ПОЛОЖЕНИЕ И АТРИБУТЫ КУРСОРА */
/*-----------------------------------------------------*/
static int s_colu, s_line, s_attr/*, s_colr*/;
void cp_sav()
{
	s_colu = scrn.sc_co;
	s_line = scrn.sc_li;
	s_attr = scrn.sc_at;
/*	s_colr = scrn.sc_ac;*/
}
void cp_fet()
{
/*	w_csi(s_colr);*/
	cp_abset(s_line, s_colu, s_attr);
}

void er_pag()
/*
 * erase (er) page at all, all screen erased
 */
{
	/*awstate = AW_INIT_STATE;*/
	at_set(TXT);    /* экран гасится атрибутом текста для видеорежима */
	/*
	 * меньше ошибок, если комбинировать атрибуты фона и гашение по отдельности, но
	 * старый код содержит массу применений этой функции - проще сблокировать
	 */
	if (hwframe.baseli == lframe->baseli) {
		w_raw(t_cl);
	} else {
		cp_set(0, 0, TXT); /* logical position, TODO check if out of borders */
		er_eop(TXT);
	}

}

void er_eop(aw)
/*
 * erase to end of page
 */
int aw; /* (visual) attributes word */
{
	at_set(aw);
	w_raw(t_cd);
}

void er_eol(aw)
int aw; /* (visual) attributes word */
{
	at_set(aw);
	w_raw(t_ce);
}

void er_scr(from, to, aw)
/*
 * clear (erase) part of screen between lines 'from' and 'to' with attribute word 'aw'
 */
/*------------------------*/
/* СТЕРЕТЬ УЧАСТОК ЭКРАНА */
/*------------------------*/
int from;
int to;
int aw;
{
	while ( from <= to ) {
		cp_abset(from++, 0, aw);
		er_eol(aw);
	}
}

void
bell()
/*----------------------------*/
/* ЗВОНОК ИЛИ ИМИТАЦИЯ ЗВОНКА */ /* TODO repaire the logic */
/*----------------------------*/
{
	if     ( t_vb[0] )    { w_raw(t_vb); }  /* ИМИТАЦИЯ: РЕВЕРС ФОНА */
	else if( t_bl[0] )    { w_raw(t_bl); }
	else                  { return;      }
}
