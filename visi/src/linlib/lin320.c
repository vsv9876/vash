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

extern  LPA lpaout[];

extern  LPA lpainp[];

#define AW_INIT_STATE (A_SO|A_US|A_MD)

/* СТАРОЕ СЛОВО АТРИБУТОВ: */
static    int awstate = AW_INIT_STATE;  /* ДЛЯ НАЧАЛА ВСЕ ВКЛЮЧЕНО */
static  char *acstate = (char *)0;
extern  int sgrmode;

#define HACK_COLOR
#ifdef HACK_COLOR

/* ANSI COLOR out/input, SGR sequence, indexed by LPA (0, TXT, HDR, ... )*/
/* storage for SGR - this is a text for sprintf "\033[%sm", */
/*
 * now incorporated into LPA sctructure:
extern  char *acout[];
extern  char *acinp[];
*/

w_sgr(gsr)
char *gsr;
{
	static char s_csi[40] = "";
	char *s = s_csi;
	char *fmt;

	if(sgrmode <= 1) {
		/*awstate = AW_INIT_STATE;*/
		/* dumb, monochrome */
		return;
	}
	if (gsr == (char *)0 || *gsr == '\0') {
		gsr = lpaout[0].lpa_sgr;
		acstate = (char *)0;
	}
	if (acstate == (char *)0 || acstate != gsr) {
		sprintf(s_csi, "\033[%sm", gsr);
		w_raw(s_csi);
		acstate = gsr;
	}
}

at_set(aw_new)
/*
 * УСТАНОВИТЬ ВИДЕОАТРИБУТЫ, выбрать для установки  с цветами, хак v3
 */
register int aw_new;        /* ИНДЕКС И ФЛАГИ АТРИБУТОВ */
{
	extern SCRN scrn;

	register int aw_old;     /* СЛОВО СТАРЫХ АТРИБУТОВ */
	char *ac_old;
	register int aw;        /* НОВЫЕ АТРИБУТЫ */
	char *ac;	/* color to be set */
	int  aix_new;	/* индекс для поиска по таблицам атрибутов */

	/* dumb mode, without attributes at all - in some cases its wrong - vt52 has so/se attribute */
	if (sgrmode == 0) return;

	/* VCOLOR (в составе VIDEOM) вероятно, не нужен, и вообще не нужен */
	aw_new &= VIDEOM; /* cleanup arg called, which may contain unsupported constants*/
	aw_old = awstate;
	ac_old = acstate;

	scrn.sc_at = aw_new;
	aix_new    = aw_new & VIDEO;
	if(aw_new & INP) {
		aw = lpainp[aix_new].lpa_a;
		ac = lpainp[aix_new].lpa_sgr;
	} else {
		aw = lpaout[aix_new].lpa_a;
		ac = lpaout[aix_new].lpa_sgr;
	}
	/* оптимизация повторной выдачи*/
	if(aw == aw_old && ac == ac_old) {
		 /* биты атрибутов не менялись; и расцветка тоже */
		return;
	}
	/* СНАЧАЛА ВСЕ погасить */
	if(aw_old & A_SO) 					{ w_raw(t_se); }
	if(aw_old & A_US)					{ w_raw(t_ue); }
	if(aw_old & (A_MD|A_MR|A_MB|A_MK))	{ w_raw(t_me); }

	/* ЗАПОМНИТЬ для оптимизации повторной выдачи */
	awstate = aw; /*scrn.sc_at;*/
	/*acstate = ac;*/

	/* ТЕПЕРЬ ВКЛЮЧИТЬ */
	/*if (ac != ac_old) {*/
		/*w_sgr(0);*/
	/*w_sgr(lpainp[0].lpa_sgr); /* FGBG, used as preamble of GCR */
	    w_sgr(lpaout[TXT].lpa_sgr); /* TXT as default background */
		w_sgr(ac);
	/*}*/
	if(aw != 0) {
		if(aw & A_SO) w_raw(t_so);
		if(aw & A_US) w_raw(t_us);
		if(aw & A_MD) w_raw(t_md);
		if(aw & A_MR) w_raw(t_mr);
		if(aw & A_MB) w_raw(t_mb);
		if(aw & A_MK) w_raw(t_mk);
	}
}
#else
char *acout [LPASIZE] = {
	"",				/* FGBG */
	"",				/* TXT  */
	"",				/* HDR  */
	"",				/* VAR  */
	"",				/* ALT  */
	"",				/* MSE  */
	"",				/* ERR  */
	"",				/* ATT  */
};
char *acinp [LPASIZE] = {
	"",				/* FGBG */
	"",				/* TXT  */
	"",				/* HDR  */
	"",				/* VAR  */
	"",				/* ALT  */
	"",				/* MSE  */
	"",				/* ERR  */
	"",				/* ATT  */
};

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
	if(awold & (A_MD|A_MR|A_MB|A_MK)) w_raw(t_me);

	if(awi != 0) {
		/* ТЕПЕРЬ ВКЛЮЧИТЬ */
		if(aw & A_SO) w_raw(t_so);
		if(aw & A_US) w_raw(t_us);
		if(aw & A_MD) w_raw(t_md);
		if(aw & A_MR) w_raw(t_mr);
		if(aw & A_MB) w_raw(t_mb);
		if(aw & A_MK) w_raw(t_mk);
	}
}
#endif

/*---------------------------*/
/* УСТАНОВКА ПОЗИЦИИ КУРСОРА */
/* И АТРИБУТОВ ТЕКСТА        */
/*---------------------------*/
cp_set(li, co, at)
int li, co, at;
{
	char *p;
	int aa;
	extern char *tgoto();

	if(li < 0) li = (maxli+li);
	if(co < 0) co = (maxco+co);

	scrn.sc_li = li; scrn.sc_co = co;       /* ПОМНИТЬ КООРДИНАТЫ */
	p = tgoto(t_cm, co, li);
	w_raw( p);

	aa = (at & VIDEOM);
	at_set(aa);
}
/*-----------------------------------------------------*/
/* СОХРАНИТЬ/ВОССТАНОВИТЬ ПОЛОЖЕНИЕ И АТРИБУТЫ КУРСОРА */
/*-----------------------------------------------------*/
static int s_colu, s_line, s_attr, s_colr;
cp_sav()
{
	s_colu = scrn.sc_co;
	s_line = scrn.sc_li;
	s_attr = scrn.sc_at;
/*	s_colr = scrn.sc_ac;*/
}
cp_fet()
{
/*	w_csi(s_colr);*/
	cp_set(s_line, s_colu, s_attr);
}

er_pag()
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
	w_raw(t_cl);

}

er_eop(aw)
/*
 * erase to end of page
 */
int aw; /* (visual) attributes word */
{
	at_set(aw);
	w_raw(t_cd);
}

er_eol(aw)
int aw; /* (visual) attributes word */
{
	at_set(aw);
	w_raw(t_ce);
}

er_scr(from, to, aw)
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
		cp_set(from++, 0, aw);
		er_eol(aw);
	}
}

bell()
/*----------------------------*/
/* ЗВОНОК ИЛИ ИМИТАЦИЯ ЗВОНКА */ /* TODO repaire */
/*----------------------------*/
{
	if     ( t_vb[0] )    { w_raw(t_vb); }  /* ИМИТАЦИЯ: РЕВЕРС ФОНА */
	else if( t_bl[0] )    { w_raw(t_bl); }
	else                  { return;      }
}
