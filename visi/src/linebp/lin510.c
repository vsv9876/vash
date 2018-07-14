/*
**      +----------+    גיגליןפוכב קקןהב-קשקןהב
**     (c) linlib  !    הלס בלזבקיפמן-דיזעןקשט
**      +----------+    קיהוןפועםימבלןק
**/

/*
 *      $Header: lin510.c,v 1.1 90/12/27 16:25:31 vsv Rel $
 *
 *      $Log:	lin510.c,v $
 * Revision 1.1  90/12/27  16:25:31  vsv
 * קועףיס V3_3
 * 
 * Revision 3.5  90/01/10  16:06:39  vsv
 * קועףיס V3_3
 * 
 * Revision 3.4  89/08/29  14:50:05  vsv
 * קועףיס LINLIB_3
 * 
 * Revision 3.3  88/12/28  12:04:35  vsv
 * ן‏ומר נועוהולבממבס קועףיס.
 * עבגןפבופ, כבכ גשלן תבהץםבמן.
 * 
 * Revision 3.2  88/12/25  13:48:30  vsv
 * מוםמןחן קשלו‏ומב גןלותמר ןפמןףיפולרמן
 * malloc() & free() יצו ף מיםי fopen() & fclose(),
 * מן םןצמן ףהולבפר י לץ‏ûו, מבהן ףהולבפר מןקץא קועףיא
 * lbp & b_page().
 * 
 * Revision 3.1  88/06/27  15:30:39  vsv
 * עוקיתיס בעטיקב RCS
 * 
 * Revision 3.0  87/12/21  12:22:20  vsv
 * נעוהקבעיפולרמשך קשנץףכ.
 * 
 */

/*
 * עבגןפב ף קמוûמיםי ןניףבמיסםי ףפעבמיד
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "line.h"
#include "linebp.h"

LINE_H  lhd = {0};    /* םוףפן הלס ף‏יפשקבמיס תבחןלןקכב */
static  char **atabp = NULL;   /* פבגלידב בהעוףןק הלס מבףפעןךכי line[] */
static  LINE *lineb = NULL;     /* ץכבתבפולר מב ףפעבמידץ, עות-פבפ עבגןפש */
	char exname[8] = { 0 };       /* יםס ית קמוûמוחן ןניףבמיס */
static  linptr_t maxid = 0;     /* םבכף. מןםוע יםומי הלס נןהףפבמןקכי */

static int errflg = 0;     /* זלבח ןûיגכי מבףפעןךכי בהעוףב */
static err_l(line)
LINE *line;
{
	if( errflg ) {
		line->flag = 0;
		line->attr = LHDR;
		line->cvts = 0;
		line->cvtf = 0;
		line->test = 0;
		line->varl = "<!>";
	}
}

d_page(llll)
/*---------------------*/
/* ץמי‏פןציפר ףפעבמידץ */
/*---------------------*/
LINE *llll;
{
	free( llll );
}


LINE *
b_page(libnam, fnam, ports)
/*-----------------------------------------*/
/* נןףפעןיפר ףפעבמידץ נן קמוûמוםץ ןניףבמיא */
/*-----------------------------------------*/
char  *libnam;          /* יםס גיגליןפוכי ף ןניףבמיסםי */
char  *fnam;            /* יםס זןעםש (ןניףבמיס) */
IN_PORTS *ports;        /* פבגלידב יםומ ץכבתבפולוך */
{
	register LINE *line;
	int      j;
	linptr_t i;
	register char *s;
	int      c;
	linptr_t count;
	FILE   *fp;
	IN_PORTS *in_pp;
	char    b_pgnm[200];
		/* יםס תבחעץתן‏מןחן זבךלב ןניףבמיס ףפעבמידש */

	/* מבךפי זןעםץ ק גיגליןפוכו */
	if( libnam == NULL )
		strcpy(b_pgnm, "");
	else
/*
		strcpy(b_pgnm, libnam);
	strcat(b_pgnm, fnam);
*/
		sprintf(b_pgnm, "%s/%s", libnam, fnam);

	/* ןפכעשפר זבךל ןניףבמיס מב ‏פומיו */
	if((fp=fopen(b_pgnm, "rb")) == NULL) {
		return(NULL);
	}

	/* נען‏יפבפר תבחןלןקןכ */
	s = (char *) &lhd;
	for(i=0; i<sizeof(LINE_H); i++) {
		if ((c=fgetc(fp)) == EOF)  break;
		else                      *s++ = c;
	}
	fclose(fp);     /* גץזוע ןףקןגןצהבופףס */

	/*  פונוער קףו נן נןעסהכץ (מבילץ‏ûבס ףפעבפוחיס malloc & free) */

	/* תבנעןףיפר נבםספר הלס line[] י heaps */
	count = lhd.lh_lines + lhd.lh_heaps;
	lineb = (LINE *)malloc(count);
	if ( lineb == NULL ) {
		fclose(fp);
		return(NULL);
	}

	/* ףמןקב ןפכעשפר זבךל ןניףבמיס מב ‏פומיו */
	if((fp=fopen(b_pgnm, "rb")) == NULL) {
		free(lineb);
		return(NULL);
	}
	/* נעןנץףפיפר תבחןלןקןכ */
	for(i=0; i<sizeof(LINE_H); i++) {
		if ((c=fgetc(fp)) == EOF)  break;
	}

	/* נען‏יפבפר line י heaps */
	s = lineb;
	for(i=0; i<count; i++) {
		if ((c=fgetc(fp)) == EOF)  break;
		else                      *s++ = c;
	}

	/* תבנעןףיפר נבםספר מב פבגלידץ בהעוףןק */
/*--TODO	atabp = (linptr_t *)malloc( lhd.lh_names); */
	atabp = (char **)malloc( lhd.lh_names);
	if ( atabp == NULL ) {
		fclose(fp);
		free(lineb);
		return(NULL);
	}

	/* נןףפעןיפר פבגלידץ בהעוףןק */
		/* םבחי‏וףכןו 8 - הלימב יםומי ף מץלסםי קן קמוû. ןניףבמיי */
	maxid = (linptr_t)((lhd.lh_names)/8);
	for(i=0; i < maxid; i++) {

		/* ‏יפבפר ן‏ועוהמןו יםס */
		s = exname;
		for(j=0; j<8; j++) {
			if ((c=fgetc(fp)) == EOF)  break;
			else                      *s++ = c;
		}
		/* מבךפי הלס מוחן בהעוף נן ûלאת-פבגלידו */
		atabp[i] = (char *)(~0);
		for(in_pp=ports; in_pp->in_name; in_pp++) {
			if(strcmp(in_pp->in_name, exname) == 0) {
				atabp[i] = in_pp->in_addr;
				break;
			}
		}
	}
	/* מבףפעןיפר ץכבתבפולי ק נעוהולבט lineb[] */

	for(line=lineb; line->size; line++) {
		errflg = 0;
		adj_lin(line);
		err_l(line);
	}

	/* קועמץפר ףיףפוםו נבםספר ןפ פבגלידש בהעוףןק */
	free( atabp );

	/* --- תבכעשפר זבךל --- */
	fclose(fp);

	/* --- קועמץפר ץכבתבפולר מב ףפעבמידץ --- */
	return(lineb);
}

/*---------------------------------------*/
/* מבףפעןךכב ץכבתבפולוך ק נעוהולבט לימיי */
/*---------------------------------------*/
adj_lin(line)
LINE *line;
{
	char    *adj_ptr();

	line->cvts = adj_ptr( (char *)line->cvts );
	line->cvtf = adj_ptr( (char *)line->cvtf );
	line->test = adj_ptr( (char *)line->test );
	line->varl = adj_ptr( (char *)line->varl );
}

char *
adj_ptr( s )
/*---------------------*/
/* מבףפעןיפר ץכבתבפולר */
/*---------------------*/
char  *s;
{
	register linptr_t u;
	char   *rets;

	u = (linptr_t)s;

	if(u == 0)
		return( NULL );
	if(u<maxid) {
		if( atabp[u] == (char *)(~0) ) {
			errflg = 1;
		} else {
			/* בהעוף נןהףפבקלסופףס ית פבגלידש */
			rets = atabp[u];
		}
	} else {
		/* בהעוף ףפעןכי ית קמוûמוחן ןניףבמיס */
		u -= 2048;             /* ץגעבפר כןםנומףבדיא */
		u += (lhd.lh_lines);   /* ףםו‎. ןפ line */
		u += (linptr_t)(lineb); /* גבתב */
		rets = (char *)u;
	}
	return( rets );
}
