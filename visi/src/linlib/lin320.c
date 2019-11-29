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
static    int awstate = AW_INIT_STATE;  /* הלס מב‏בלב קףו קכלא‏ומן */
static  char *acstate = (char *)0;

#define HACK_COLOR
#ifdef HACK_COLOR

/* ANSI COLOR out/input, SGR sequence, indexed by LPA (0, TXT, HDR, ... )*/
/* storage for SGR - this is a text for sprintf "\033[%sm", */
/*
extern  char *acout[];
extern  char *acinp[];
*/

w_sgr(sgr)
char *sgr;
{
	static char s_csi[40] = "";
	char *s = s_csi;
	if (sgr == (char *)0 || *sgr == '\0') {
		sgr = lpaout[0].lpa_gsr;
		acstate = (char *)0;
	}
	if (acstate == (char *)0 || acstate != sgr) {
		sprintf(s_csi, "\033[%sm", sgr);
		w_raw(s_csi);
		acstate = sgr;
	}
}

at_set(aw_new)
/*
 * ץףפבמןקיפר קיהוןבפעיגץפש Ó Ã×ÅÔÁÍÉ, ÈÁË v3
 */
register int aw_new;        /* ימהוכף י זלבחי בפעיגץפןק */
{
	extern SCREEN scrn;

	register int aw_old;     /* ףלןקן ףפבעשט בפעיגץפןק */
	register int aw;        /* מןקשו בפעיגץפש */
	int  aix_new;	/* ÉÎÄÅËÓ ÄÌÑ ÐÏÉÓËÁ ÐÏ ÔÁÂÌÉÃÁÍ ÁÔÒÉÂÕÔÏ× */
	char *acolor;	/* color to be set */

	aw_old     = awstate;
	/* VCOLOR (× ÓÏÓÔÁ×Å VIDEOM) ×ÅÒÏÑÔÎÏ, ÎÅ ÎÕÖÅÎ, É ×ÏÏÂÝÅ ÎÅ ÎÕÖÅÎ */
	scrn.sc_at = (aw_new &= VIDEOM);
	aix_new    = aw_new & VIDEO;
	if(aw_new & INP) {
		aw     = lpainp[aix_new].lpa_a;
		acolor = lpainp[aix_new].lpa_gsr;
	} else {
		aw     = lpaout[aix_new].lpa_a;
		acolor = lpaout[aix_new].lpa_gsr;
	}
	/* ÏÐÔÉÍÉÚÁÃÉÑ ÐÏ×ÔÏÒÎÏÊ ×ÙÄÁÞÉ*/
	/* if(aw == aw_old) return; /* ÂÉÔÙ ÁÔÒÉÂÕÔÏ× ÎÅ ÍÅÎÑÌÉÓØ */
	/* תבנןםמיפר ÄÌÑ ÏÐÔÉÍÉÚÁÃÉÉ ÐÏ×ÔÏÒÎÏÊ ×ÙÄÁÞÉ */
	awstate = scrn.sc_at;

	/* ףמב‏בלב קףו ÐÏÇÁÓÉÔØ */
	if(aw_old & A_SO) 					{ w_raw(t_se); }
	if(aw_old & A_US)					{ w_raw(t_ue); }
	if(aw_old & (A_MD|A_MR|A_MB|A_MK))	{ w_raw(t_me); }
	w_sgr(0);

	/* פונוער קכלא‏יפר */
	if(aw_new != 0) {
		w_sgr(lpainp[0].lpa_gsr); /* FGBG, used as prefix of CGR */
		w_sgr(acolor);
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
register int awi;        /* ימהוכף י זלבחי בפעיגץפןק */
/*--------------------------*/
/* ץףפבמןקיפר קיהוןבפעיגץפש */
/*--------------------------*/
{
	extern SCREEN scrn;

	register int awold;     /* ףלןקן ףפבעשט בפעיגץפןק */
	register int aw;        /* מןקשו בפעיגץפש */

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

	if(awi != 0) {
		/* פונוער קכלא‏יפר */
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
/* ץףפבמןקכב נןתידיי כץעףןעב */
/* י בפעיגץפןק פוכףפב        */
/*---------------------------*/
cp_set(li, co, at)
int li, co, at;
{
	char *p;
	int aa;
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
static int s_colu, s_line, s_attr, s_colr;
cp_sav()
{
	s_colu = scrn.sc_co;
	s_line = scrn.sc_li;
	s_attr = scrn.sc_at;
//	s_colr = scrn.sc_ac;
}
cp_fet()
{
//	w_csi(s_colr);
	cp_set(s_line, s_colu, s_attr);
}

er_pag()
/*
 * erase (er) page at all, all screen erased
 */
{
	/*awstate = AW_INIT_STATE;*/
	/*at_set(TXT);    /* ÜËÒÁÎ ÇÁÓÉÔÓÑ ÁÔÒÉÂÕÔÏÍ ÔÅËÓÔÁ (ÔÅÍÎÙÊ ÆÏÎ) */
	/*
	 * ÍÅÎØÛÅ ÏÛÉÂÏË, ÅÓÌÉ ËÏÍÂÉÎÉÒÏ×ÁÔØ ÁÔÒÉÂÕÔÙ ÆÏÎÁ É ÇÁÛÅÎÉÅ ÐÏ ÏÔÄÅÌØÎÏÓÔÉ, ÎÏ
	 * ÓÔÁÒÙÊ ËÏÄ ÓÏÄÅÒÖÉÔ ÍÁÓÓÕ ÐÒÉÍÅÎÅÎÉÊ ÜÔÏÊ ÆÕÎËÃÉÉ - ÐÒÏÝÅ ÓÂÌÏËÉÒÏ×ÁÔØ
	 */
	at_set(FGBG);
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
/* ףפועופר ץ‏בףפןכ üכעבמב */
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
/* תקןמןכ ילי יםיפבדיס תקןמכב */ /* TODO repaire */
/*----------------------------*/
{
	if     ( t_vb[0] )    { w_raw(t_vb); }  /* יםיפבדיס: עוקועף זןמב */
	else if( t_bl[0] )    { w_raw(t_bl); }
	else                  { return;      }
}
