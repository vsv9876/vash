/*
**      +----------+    גיגליןפוכב קקןהב-קשקןהב
**     (c) linlib  !    הלס בלזבקיפמן-דיזעןקשט
**      +----------+    קיהוןפועםימבלןק
**/

/*
 *      $Header: lin360.c,v 1.1 90/12/27 16:29:26 vsv Rel $
 *
 *      $Log:	lin360.c,v $
 * Revision 1.1  90/12/27  16:29:26  vsv
 * קועףיס LINLIB_3
 * 
 * Revision 3.2  89/08/29  15:17:36  vsv
 * קועףיס LINLIB_3
 * 
 * Revision 3.1  88/06/27  15:21:45  vsv
 * עוקיתיס בעטיקב RCS
 * 
 * Revision 3.0  87/12/21  12:20:49  vsv
 * נעוהקבעיפולרמשך קשנץףכ.
 * 
 */

#include <stdio.h>
#include "line.h"
#include "line0.h"

/*----------------------------------------------------------------------*/
/*
 * קמיםבמיו ! ק זץמכדיסט fnd_ar(), fnd_al(), ...
 *      יףנןלרתץופףס מוגןלרûןך זןכץף, ןףמןקבממשך מב
 *      עובליתבדיי כןםנילספןעןק ףי הלס ׀ִ׀-11 (DECUS, UNIX,
 *      מו נעןקועומן הלס Whitesmith) :
 *      בעחץםומפש םןצמן נועוהבקבפר ‏ועות עוחיףפעןקשו ןניףבמיס
 *      ק ןהימבכןקןם נןעסהכו (יםומב מו יחעבאפ עןלי).
 *      ק üפןם ףלץ‏בו קשתקבממבס זץמכדיס נןלץ‏יפ תמב‏ומיס ית קשתקבקûוך,
 *      מן מו תבנןעפיפ יט נעי קןתקעבפו.
 *      נעןגלוםש ףקסתבממשו ף נועומןףןם, עוûבאפףס נץפום קשסףמומיס
 *      ןפףץפףפקיס במבלןחי‏משט קןתםןצמןףפוך מב קבûום כןםנילספןעו י
 *      נועוהולכו קשתןקןק üפיט זץמכדיך נןה ףפבמהבעפמשו ףןחלבûומיס.
 *      =========================================================
 *      üפן, כ ףןצבלומיא, בפבקיתםש linlib v2.0 -- פבם גשלי פבכיו
 *      כןםבמהש, כבכ "מבךפי נן ףפעולכו הן ץנןעב" (יפועבדיסםי), מן
 *      üכןמןםיס ןפ ףכןעןףפי נועוהב‏י בעחץםומפןק
 *      ןכבתבלבףר ףןםמיפולרמןך -- üפיםי כןםבמהבםי נןנעןףפץ
 *      מיכפן מו נןלרתןקבלףס.
 */

/*-------------------------*/
/* מבךפי נן ףפעולכו קנעבקן */
/*-------------------------*/
LINE *
fnd_ar (lni, line)
	register LINE *lni;
	register LINE *line ;
{
	register LINE *lnj;

	for(lnj=lni; lnj->size != 0; lnj++) {
		if( (INP & ~(lnj->attr)) )
			continue;
		if(((int)lnj->line == (int)lni->line)
		&& ((int)lnj->colu > (int)lni->colu))
			return(lnj);
	}
	/* תבדיכליפר ק üפןך צו ףפעןכו */
	for(lnj=line; lnj->size != 0; lnj++) {
		if( (INP & ~(lnj->attr)) )
			continue;
		if(((int)lnj->line == (int)lni->line))
			return(lnj);
	}
	return(lni);
}
/*------------------------*/
/* מבךפי נן ףפעולכו קלוקן */
/*------------------------*/
LINE *
fnd_al (lni, line)
	register LINE *lni;
	register LINE *line ;
{
	register LINE *lnj;

	for(lnj=lni; lnj>=line; lnj--) {
		if( (INP & ~(lnj->attr)) )
			continue;
		if((lnj->line == lni->line)
		&& (lnj->colu < lni->colu))
			return(lnj);
	}
	/* תבדיכליפר ק üפןך צו ףפעןכו : י‎ום ף כןמדב */
	for(lnj=lni; lnj->size != 0; lnj++ )       ;
	for(   ; lnj>=line; lnj--) {
		if( (INP & ~(lnj->attr)) )
			continue;
		if((lnj->line == lni->line))
			return(lnj);
	}
	return(lni);
}
/*------------------------*/
/* מבךפי נן ףפעולכו קקועט */
/*------------------------*/
LINE *
fnd_au (lni, line)
	register LINE *lni;
	register LINE *line ;
{
	register LINE *lnj;
	int     nxt_line ;      /* מןםוע גליצבךûוך ףפעןכי */

	if((nxt_line = lni->line) > 0) {
		for(lnj=lni; lnj>=line; lnj--) {
			if((lnj->line == lni->line))    /*ףפע. ףפבעבס*/
				continue;
			else if( (INP & ~(lnj->attr)) )
				continue;               /*מו הלס קקןהב*/
			else if((lnj->line <  lni->line)
			      &&(lnj->colu >  lni->colu))
				continue;               /*ףפע. קשûו, מן כןל*/
			else if((lnj->line <  lni->line)
			      &&(lnj->colu <= lni->colu))
				return(lnj);    /*ףפע. קשûו, כןל. מו נעבקוו*/
		}
	}
	/* נןנשפכב מבךפי ק ןגעבפמןם מבנעבקלומיי */
	for(lnj=lni; lnj->size != 0; lnj++) ; /*מבטןהים כןמוד*/
	for(   ; lnj>=line; lnj--) {          /*י‎ום כ מב‏בלץ*/
		if( (INP & ~(lnj->attr)) )    /*מו הלס קקןהב*/
			continue;
		else if(lnj->colu <= lni->colu) /*מו נעבקוו*/
			return(lnj);
	}
	return(lni);      /* ןףפבקלומן מב קףסכיך ףלץ‏בך */
}
/*-----------------------*/
/* מבךפי נן ףפעולכו קמית */
/*-----------------------*/
LINE *
fnd_ad (lni, line)
	register LINE *lni;
	register LINE *line ;
{
	register LINE *lnj;
	int     nxt_line ;

	/* תהוףר מבהן גש פ‎בפולרמוו(צקבמודכיך ם.ם.) :
	 *       24 ףפעןכי מו הלס קףוט פועםימבלןק נעבקהב...
	 */
/*#define MAXLI_ 24*/
#define MAXLI_ maxli
	if((nxt_line = (int)lni->line ) < MAXLI_ ) { /*קנועוה, וףלי וףפר כץהב*/
		for(lnj=lni; lnj->size!=0; lnj++) {     /* כןמוד ? */
			if((lnj->line <= lni->line))    /*ףפע. מו מיצו */
				continue;
			else if( (INP & ~(lnj->attr)) ) /*מו הלס קקןהב*/
				continue;
			else {                          /*ףפע. מיצו */
			      if((lnj->colu + lnj->size) >= lni->colu)
				  return(lnj);  /*נןלו נןה כץעףןעןם*/
			      else
				  continue;     /*קףו נןלו ףלוקב*/
			}
		}
	}
	/* נןנשפכב מבךפי ןפ מב‏בלב ףפעבמידש */
	for(lnj=line; lnj->size!=0; lnj++) {
		if( (INP & ~(lnj->attr)) )
			continue;
		else if(lnj->colu >= lni->colu)
			return(lnj);
	}
	return(lni);      /* ןףפבקלומן מב קףסכיך ףלץ‏בך */
}
/*---------------------------------------------------*/
/* ףבםשך נעןףפןך נןיףכ: כ ן‏ועוהמןך לימיי נן נןעסהכץ */
/*---------------------------------------------------*/
LINE *
fnd_nxt(lni, line)
	register LINE *lni;
	register LINE *line ;
{
/*      register LINE *lnj;     */

	lni++;
	if(lni->size == 0)
		lni = line;          /* תבדיכליפר מב מב‏בלן */
	return(lni);
}

aj_tbl(scnd, line_e)
/*-----------------------------------*/
/* מבףפעןיפר üלוםומפ פבגלידש נן גבתו */
/*-----------------------------------*/
register LINE *scnd;
LINE *line_e;
{
	register LINE *base;
	int     isleft;
	LINE saved;

	saved = *scnd;
	isleft = (saved.flag & SUSL) ? 1 : 0;
	base = scnd;
	while(base != line_e) {
		base--;
		if(isleft == 1) {
			if (base->colu <  saved.colu) {
				if (base->line == saved.line) {
					goto on_base;
				}
			}
		} else
		if(isleft == 0) {
			if (base->line <  saved.line) {
				if (base->colu == saved.colu) {
					goto on_base;
				}
			}
		} else {
			continue;
		}
	}
	/* ףאהב קשטןהב מופ, וףלי ףפעבמידב נעבקילרמן ףןףפבקלומב */
	w_chr('\007');
	return;

on_base:
	*scnd = *base;
	scnd->line = saved.line;
	scnd->colu = saved.colu;
	scnd->varl += (linptr_t)(saved.varl);
}

/*---------------------*/
/* מבעיףןקבפר ףפעבמידץ */
/*---------------------*/
w_page ( line_e, cod )
LINE    *line_e;
register kbcod cod;
{
	register LINE *line;

	/* ףמב‏בלב קףו, ‏פן מו הלס קקןהב */
	for(line=line_e; line!=NULL && line->size != 0; line++) {
		/* קפןעי‏משך üלוםומפ פבגלידש מבהן ףמב‏בלב מבףפעןיפר... */
		if(line!=NULL && (line->flag & SUST)) {
			aj_tbl(line, line_e);
		}
		if( line!=NULL && (INP & ~(line->attr)) )
			w_line(line, cod);
	}
	/* לימיי הלס קקןהב ק נןףלוהמאא ן‏ועוהר */
	for(line=line_e; line->size != 0; line++) {
		if( (INP & ~(line->attr)) == 0 )
			w_line(line, cod);
	}
}

kbcod
r_page(line_e, page, posp)
/*-----------------*/
/* ‏יפבפר ףפעבמידץ */
/*-----------------*/
LINE    *line_e;                /* ףפעבמידב הלס עוהבכפיעןקבמיס  */
LINE   **page;                  /* פוכץ‎בס לימיס (ףפבפץף)       */
int    *posp;                   /* נןתידיס כץעףןעב נעי עוהבכפיעןקבמיי */
{
	int     cod;            /* כןה, קןתקעב‎בוםשך r_line()   */

	register LINE *lni;             /* ץכבתבפולר מב פוכץ‎ץא לימיא */
	register LINE *line ;           /* ץכבתבפולר מב קףא ףפעבמידץ */

	line = line_e;
	if(*page != (LINE *)NULL)
		lni = *page;
	else
		lni = line_e;

	/* נעןנץףפיפר פן, ‏פן מולרתס עוהבכפיעןקבפר */
	while( INP & ~(lni->attr)) {
		lni++;
		/* תבדיכליפרףס ‏ועות מב‏בלן */
		if(lni->size == 0)
			lni = line_e;
	}

	cod = r_line(lni, posp);

	/* מבךפי ץכבתבפולר הלס ףלוהץא‎וחן ‏פומיס */
	switch( cod ) {
	case KB_AR :
		if ( (lni->flag & SUSR) == FALSE )
			lni = fnd_ar(lni, line) ;
		break ;
	case KB_AL :
		if ( (lni->flag & SUSL) == FALSE )
			lni = fnd_al(lni, line) ;
		break ;
	case KB_AU :
		if ( (lni->flag & SUSU) == FALSE )
			lni = fnd_au(lni, line) ;
		break ;
	case KB_AD :
		if ( (lni->flag & SUSD) == FALSE )
			lni = fnd_ad(lni, line) ;
		break ;
	case KB_NL :
		if ( (lni->flag & SUSNL) == FALSE )
			lni = fnd_nxt(lni, line) ;
		break ;
	case KB_RE :     /* ןףקוציפר יתןגעבצומיו */
		er_pag();
		w_page(line, 0);
		break ;
	default :
		break;
	}

	*page = lni;    /* ףןטעבמיפר ץכבתבפולר מב פוכ. לימיא!... */
	return(cod);
}
