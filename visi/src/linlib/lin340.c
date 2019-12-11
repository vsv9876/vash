/*
**      +----------+    גיגליןפוכב קקןהב-קשקןהב
**     (c) linlib  !    הלס בלזבקיפמן-דיזעןקשט
**      +----------+    קיהוןפועםימבלןק
**/

/*
 *      $Header: lin340.c,v 3.5 89/08/29 15:17:13 vsv Rel $
 *
 *      $Log:	lin340.c,v $
 * Revision 3.5  89/08/29  15:17:13  vsv
 * קועףיס LINLIB_3
 * 
 * Revision 3.4  89/08/29  10:36:03  vsv
 * ףימפבכףיף. ףם. rcsdiff
 * 
 * Revision 3.3  88/08/10  10:40:28  vsv
 * פןמכןףפי זןעםבפמשט נעוןגעבתןקבמיך
 * הלס לימיך ף עוהבכפיעןקבמיום
 * 
 * Revision 3.2  88/08/02  10:48:55  vsv
 * הןגבקלומ זלבח עוציםב נועוטןהב כ עוהבכפיעןקבמיא נןלס (allcod).
 * 
 * Revision 3.1  88/06/27  15:21:26  vsv
 * עוקיתיס בעטיקב RCS
 * 
 * Revision 3.0  87/12/21  12:19:56  vsv
 * נעוהקבעיפולרמשך קשנץףכ.
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
/* קשקוףפי נן זןעםבפבם printf */
/*----------------------------*/
static int
docvts(ou, fo, va)
register char *ou;      /* ףפעןכב, כץהב נןםוףפיפר קשקןה */
register char *fo;      /* זןעםבפ printf */
register char *va;      /* ץכבתבפולר מב נועוםוממץא */
{
#ifdef DOUBLE
	if(index(fo, 'f') || index(fo, 'e') || index(fo, 'g'))
		/* זןעםבפש ף נלבקבא‎וך פן‏כןך */
		return( sprintf(ou, fo, *(double *)va) );
	else
		/* הלס ןףפבלרמשט הןףפבפן‏מן י פבכ */
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
/* קשקןה/קקןה לימיי, קועמץפר כןה נןףלוהמוך כלבקיûי */
/*-------------------------------------------------*/
register LINE    *line; /* ץכבתבפולר מב לימיא */
	 int     *posp; /* נןתידיס, ף כןפןעןך מב‏בפר עוהבכפיעןקבפר */
{
	register char *s;
	register int  i;
	kbcod   cod;            /* כןה נןףלוהמוך כלבקיûי */
	int     attr;           /* ףלןקן בפעיגץפןק */
	int     cvterror;       /* זלבח ןûיגכי זןעםבפב */
	int     tsterror;       /* זלבח ןûיגכי פוףפב */
	int     midcnt;         /* ף‏ופ‏יכ הלס קשעבקמיקבמיס נן דומפעץ */
	int     filch;          /* תמבכ הלס תבנןלמומיס */
	int     wksl;           /* הלימב ףפעןכי נועוה קשעבקמיקבמיום */
	int     size;           /* עבתםוע נןלס */
	int     flo;            /* זלבח: זןעםבפ תבגלןכיעןקבמ */
	int     onexit;         /* זלבח: כןמוד עבגןפש */
	char   *eds;            /* ףפעןכב הלס עוהבכפןעב */
	int     base;           /* מב‏בלן נןלותמןך ימזןעםבדיי:
				**   ףםו‎ומיו ןפ נןהףכבתכי */
	char    wks[STRLEN];    /* עבגן‏בס ףפעןכב */

	attr = line->attr;

	/* קשתןק ‏ועות w_line() ? */
	if(onexit = ((posp == (int *)(-1)) ? 1 : 0))
		attr &= ~INP;  /* מופ, ×ÙÚÏ× r_line */
	cod = 0;

inp_retry:
out_string:
	cvterror = tsterror = 0;
	s = eds = wks;
	base = 0; size = line->size;

	i = attr & VIDEO;

#ifdef RETRO_FILCH
	/*==== סקמןו תבנןלמומיו נעי ןפףץפףפקיי בפעיגץפןק */
	if (((line->attr & INP) == 0 ) && (lpaout[i].lpa_a == 0))
			filch = '_';
	else            filch = ' ';
#else
	filch = ' ';
	if ((line->attr & VIDEO) == HDR) {
		filch = lpaout[HDR].lpa_p;
	}
#endif
	/*==== נןהףכבתכב */
	if(attr & PMT) {
		eds++; base++; size--;
		if(attr & INP)  *s++ = lpainp[i].lpa_p;
		else            *s++ = lpaout[i].lpa_p;
	}
	/*==== זןעםבפ מב קשקןה */
	if( !(flo = ((attr & FLO) == FLO))) { /*=== וףלי מו גלןכיעןקבמ */
		if(line->cvtf) {                /* ‏ועות זץמכדיא */
			(*(line->cvtf))(line, cod, "w", s);
		}
		else if(line->cvts) {           /* ק ףפילו printf */
			docvts(s, line->cvts, line->varl);
		}
		else {                          /* נעןףפן ףפעןכב */
			eds = line->varl;
			goto string_simple; }
	} else {
string_simple:
		strncpy(s, line->varl, size); s[size] = 0;
	}

	wksl = strlen(s);

	/*==== קשעבקמיקבמיו */
	if(attr & MID) {
		if((midcnt=((size-wksl)/2)) > 0) {
			for(i= wksl+= midcnt; i>=midcnt; i--)
				s[i] = s[i-midcnt];
			while(i >= 0) s[i--] = filch;
		}
	}
	/*==== תבנןלמומיו */
	if(attr & PAD) {
		for(i=wksl; i<size;)    s[i++] = filch; s[size] = 0;
	}
	/*==== כץעףןע, קיהון (מו תבגשפר נןהףכבתכץ) */
	cp_set(line->line, line->colu, attr);

	/*==== נןכבתבפר */
	if ( (posp != (int *)(-1)) && (attr & (LFASTR|NED)) == (LFASTR|NED) )
		w_chr(*wks);    /* ÔÏÌØËÏ ÄÌÑ r_line */
	else
		w_str(wks);

	if(onexit || posp == (int *)(-1)) return(cod);    /* כןמוד הלס קשתןקב ‏ועות w_line */

	/*==== כץעףןע נועוה קקןהןם נועקןך כלבקיûי */
	cp_set(line->line, line->colu, attr);

	/*==== ‏יפבפר כןה נועקןך כלבקיûי */
	switch(cod = r_cod(0)) {
	case KB_DE: *eds = '\0';
	case  ' ': if(posp != (int *)(-1) && posp) *posp=0;
		   break;
	default:
		/* וףפר פןמכןףפר הלס נןלוך ף עוהבכפיעןקבמיום:
		 * זןעםבפ מב קקןהו מץצומ פןלרכן
		 * נןףלו עוהבכפןעב ףפעןכי י כןהב KB_NL;
		 */
		if((attr & NED) == 0) {
			if(allcod && cod1(cod) == 0) {
				unr_c(cod);     /* "נען‏יפבפר" מבתבה */
				*eds = '\0';    /* ן‏יףפיפר ףפעןכץ */
			} else
	/*                if(cod1(cod) != 0)    *******/
				goto inp_test;
		} else
			goto inp_format;
		break;
	}

edit_retry:
	/*==== מו עוהבכפיעןקבפר ? */
	if(attr & NED) goto inp_format;

	cp_set(line->line, base + line->colu, attr);

	/*==== עוהבכפןע, טעבמיפר כןה */
	cod = e_str(eds, size,
		     /*==== פוףפ הלס עוהבכפןעב ? */
		    ((attr & EDT) ? (linptr_t)(line->test) : 0), posp);
	/*
	 * וףפר פןמכןףפי ף זןעםבפןם נןףלו עוהבכפןעב:
	 *    מבהן עובחיעןקבפר פןלרכן מב KB_NL
	 */
	if(cod != KB_NL) goto inp_test;

inp_format:
	/*==== זןעםבפ מב קקןהו */
	if(flo) goto inp_test;  /* ם.גשפר גלןכיעןקבמ... */

	if(line->cvtf) {
		if(cvterror = !(*line->cvtf)(line, cod, "r", eds))
			bell();
	} else if(line->cvts) {
#ifdef DURA
		/*
		 * מו ףןקףום מבהוצמןו נעוןגעבתןקבמיו מב קקןהו :
		 * ץ כןםנילספןעב ÄÅÃÕÓ עבגןפבופ ןהימ זןעםבפ - %f
		 * (י מו פןלרכן ץ DECUS  -- vsv, 15/02/87)
		 */
		if((index(line->cvts, 'f') != 0)
		    || (index(line->cvts, 'g') != 0)
		    || (index(line->cvts, 'e') != 0))       {
			if(cvterror= (!sscanf(eds,"%f",line->varl)))
				bell();
		}
		/* פן, ‏פן קשûו, םןצמן ץגעבפר, וףלי כןםץ םוûבופ */
		else    {
			if(cvterror= (!sscanf(eds,line->cvts,line->varl)))
				bell();
		}
#endif DURA
		if(cvterror= (!sscanf(eds,line->cvts,line->varl)))
			bell();

	}
	/*==== ןûיגכב זןעםבפב ? */
	if(cvterror && onexit == 0 ) goto edit_retry;

inp_test:
	/*==== פוףפ נןףלו עוהבכפןעב ? */
	if( flo || !(attr & EDT) ) {
		tsterror = !(line->test ? (*line->test)(line, cod) : TRUE);
	}
	/*==== וףפר ןûיגכב ? */
	if(tsterror)           goto inp_retry;

	switch(cod) {
	case KB_AU:      /* נןףלו üפיט כןהןק נועוטןה כ הע. נןלסם */
	case KB_AD:
	case KB_AL:
	case KB_AR:
	case KB_NL:      break;
	default :       return(cod);
	}

	onexit = 1; attr &= ~INP;       /* בפעיגץפ הלס נןכבתב */
	goto    out_string;
}
