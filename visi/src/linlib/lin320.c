/*
**      +----------+    גיגליןפוכב קקןהב-קשקןהב
**     (c) linlib  !    הלס בלזבקיפמן-דיזעןקשט
**      +----------+    קיהוןפועםימבלןק
**/

/*
 *      $Header: lin320.c,v 1.1 90/12/27 16:29:15 vsv Rel $
 *
 *      $Log:	lin320.c,v $
 * Revision 1.1  90/12/27  16:29:15  vsv
 * קועףיס LINLIB_3
 * 
 * Revision 3.2  89/08/29  15:16:53  vsv
 * קועףיס LINLIB_3
 * 
 * Revision 3.1  88/06/16  17:52:06  vsv
 * יףנעבקלומש ןûיגכי ץףפבמןקכי בפעיגץפןק,
 * גשל מוקועמן יףנןלרתןקבמ פינ הבממשט:
 * קםוףפן short גשל char (bool).
 * 
 * Revision 3.0  88/06/06  08:54:18  vsv
 * יףנעבקלומב ןûיגכב ק זץמכדיי er_scr,
 * כןפןעבס נעיקןהילב כ תבדיכליקבמיא.
 * 
 * Revision 3.0  87/12/21  12:18:32  vsv
 * נעוהקבעיפולרמשך קשנץףכ.
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
SCREEN scrn = { 0 };

extern  LPA lpaout[];

extern  LPA lpainp[];

#define AW_INIT_STATE (A_SO|A_US|A_MD)

/* ףפבעןו ףלןקן בפעיגץפןק: */
static  awstate = AW_INIT_STATE;  /* הלס מב‏בלב קףו קכלא‏ומן */

/*#define HACK_COLOR*/
#ifdef HACK_COLOR
at_set(aw_new)
/*
 * ץףפבמןקיפר קיהוןבפעיגץפש Ó Ã×ÅÔÁÍÉ, ÈÁË v2
 */
register int aw_new;        /* ימהוכף י זלבחי בפעיגץפןק */
{
	extern SCREEN scrn;

	register int aw_old;     /* ףלןקן ףפבעשט בפעיגץפןק */
	register int aw;        /* מןקשו בפעיגץפש */

	aw_old = awstate;
	scrn.sc_at = (aw_new &= VIDEOM);
	/* VCOLOR נןכב מו נןההועציקבופףס ... */
	if(aw_new & INP)
		aw = lpainp[aw_new & VIDEO].lpa_a;
	else
		aw = lpaout[aw_new & VIDEO].lpa_a;

/*	if(&& aw == aw_old) return;         /* גיפש קיהון מו יתםומיליףר */
	awstate = aw;                    /* תבנןםמיפר מב ףלוה. עבת */

	/* ףמב‏בלב קףו קשכלא‏יפר */
	if(aw_old & A_SO) {	w_raw(t_se); }
	if(aw_old & A_US) { w_raw(t_ue); }
	if(aw_old & (A_MD|A_MR|A_MB|A_MK)) { w_raw(t_me); }
	/* w_raw("\033[0;37;44m"); /* ÆÏÎ ÐÏ ÕÍÏÌÞÁÎÉÀ */
	w_raw("\033[m"); /* HACK: ÔÏÌØËÏ ÞÔÏ ×ÓÅ ÜÔÏ ÐÒÏÄÅÌÁÎÏ - Á ×ÄÒÕÇ ÎÅ ÓÒÁÂÏÔÁÌÏ */

	/* פונוער קכלא‏יפר */
	if(aw_new != 0) {
		if(aw & A_VS) {                  w_raw("\033[37;44m"); }
		if(aw & A_SO) {	/*w_raw(t_so);*/ w_raw("\033[37m"); }
		if(aw & A_US) {	/*w_raw(t_us);*/ w_raw("\033[36m"); }
		if(aw & A_MD)   w_raw(t_md);
		if(aw & A_MR)   w_raw(t_mr);
		if(aw & A_MB) { /*w_raw(t_mb);*/ w_raw("\033[31m"); }
		if(aw & A_MK) { /*w_raw(t_mk);*/ w_raw("\033[35m"); }
	}
}
#else

at_set(awi)
register int awi;        /* ימהוכף י זלבחי בפעיגץפןק */
/*--------------------------*/
/* ץףפבמןקיפר קיהוןבפעיגץפש */
/*--------------------------*/
{
	extern SCREEN scrn;

	register int awold;     /* ףלןקן ףפבעשט בפעיגץפןק */
	register int aw;        /* מןקשו בפעיגץפש */

	if(awi == 0) return;    /* מי‏וחן מו הולבפר */
	awold = awstate;
	scrn.sc_at = (awi &= VIDEOM);
	/* VCOLOR נןכב מו נןההועציקבופףס ... */
	if(awi & INP) aw = lpainp[awi & VIDEO].lpa_a;
	else          aw = lpaout[awi & VIDEO].lpa_a;

	if(aw == awold) return;         /* גיפש קיהון מו יתםומיליףר */
	awstate = aw;                    /* תבנןםמיפר מב ףלוה. עבת */

	/* ףמב‏בלב קףו קשכלא‏יפר */

	if(awold & A_SO) {
		w_raw(t_se);
	}
	if(awold & A_US) w_raw(t_ue);
	if(awold & (A_MD|A_MR|A_MB|A_MK)) w_raw(t_me);

	/* פונוער קכלא‏יפר */
	if(aw & A_SO) w_raw(t_so);
	if(aw & A_US) w_raw(t_us);
	if(aw & A_MD) w_raw(t_md);
	if(aw & A_MR) w_raw(t_mr);
	if(aw & A_MB) w_raw(t_mb);
	if(aw & A_MK) w_raw(t_mk);
}
#endif

/*---------------------------*/
/* ץףפבמןקכב נןתידיי כץעףןעב */
/* י בפעיגץפןק פוכףפב        */
/*---------------------------*/
cp_set(li, co, at)
int li, co, at;
{
	register char *p;
	register int aa;
	extern char *tgoto();

	if(li < 0) li = (maxli+li);
	if(co < 0) co = (maxco+co);

	scrn.sc_li = li; scrn.sc_co = co;       /* נןםמיפר כןןעהימבפש */
	p = tgoto(t_cm, co, li);
	w_raw( p);

	aa = (at & VIDEOM);
	at_set(aa);
}
/*-----------------------------------------------------*/
/* ףןטעבמיפר/קןףףפבמןקיפר נןלןצומיו י בפעיגץפש כץעףןעב */
/*-----------------------------------------------------*/
static int s_colu, s_line, s_attr;
cp_sav()
{
	s_colu = scrn.sc_co;
	s_line = scrn.sc_li;
	s_attr = scrn.sc_at;
}
cp_fet()
{
	cp_set(s_line, s_colu, s_attr);
}

/*-------------------------------------------------*/
/* ףפועופר üכעבמ, הן כןמדב üכעבמב, הן כןמדב ףפעןכי */
/*-------------------------------------------------*/
er_pag()
{
	/*awstate = AW_INIT_STATE;*/
	/*at_set(TXT);    /* ÜËÒÁÎ ÇÁÓÉÔÓÑ ÁÔÒÉÂÕÔÏÍ ÔÅËÓÔÁ (ÔÅÍÎÙÊ ÆÏÎ) */
	/*
	 * ÍÅÎØÛÅ ÏÛÉÂÏË, ÅÓÌÉ ËÏÍÂÉÎÉÒÏ×ÁÔØ ÁÔÒÉÂÕÔÙ ÆÏÎÁ É ÇÁÛÅÎÉÅ ÐÏ ÏÔÄÅÌØÎÏÓÔÉ
	 */
	w_raw(t_cl);

}

er_eop()
{
	w_raw(t_cd);
}

er_eol()
{
	w_raw(t_ce);
}

/*------------------------*/
/* ףפועופר ץ‏בףפןכ üכעבמב */
/*------------------------*/
er_scr(from, to)
register int from, to;
{
	while ( from <= to ) {
		cp_set(from++, 0, TXT);
		er_eol();
	}
}

/*----------------------------*/
/* תקןמןכ ילי יםיפבדיס תקןמכב */
/*----------------------------*/
bell()
{
	if     ( t_vb[0] )    { w_raw(t_vb); }  /* יםיפבדיס: עוקועף זןמב */
	else if( t_bl[0] )    { w_raw(t_bl); }
	else                  { return;      }
}
