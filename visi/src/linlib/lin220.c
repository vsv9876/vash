/*
**      +----------+    גיגליןפוכב קקןהב-קשקןהב
**     (c) linlib  !    הלס בלזבקיפמן-דיזעןקשט
**      +----------+    קיהוןפועםימבלןק
**/

/*
 *      $Header: lin220.c,v 1.2 94/05/27 22:04:20 vsv Exp $
 *
 *      $Log:	lin220.c,v $
 * Revision 1.2  94/05/27  22:04:20  vsv
 * ÓÉÎÈÒÏÎÉÚÁÃÉÑ ÁÒÈÉ×Á RCS
 * 
 * Revision 1.1  90/12/27  16:29:01  vsv
 * קועףיס LINLIB_3
 * 
 * Revision 3.2  89/08/29  15:16:00  vsv
 * קועףיס LINLIB_3
 * 
 * Revision 3.1  88/06/27  15:19:57  vsv
 * עוקיתיס בעטיקב RCS
 * 
 */

#include <stdlib.h>
#include <string.h>
#include <ediag.h>
#include <stdio.h>
#include "line.h"
#include "line0.h"

/*
 * מבףפעןךכב מב פינ פועםימבלב
 *
 * נעןדוהץעש הלס קשנןלמומיס מבףפעןךכי מב פינ פועםימבלב
 */

#include <ctype.h>

/*extern char *malloc();*/

#ifdef DEMOS2
extern  char *getenv();
#endif

#ifdef RT11
static  char ttynm[30] = "VT0:TRMCAP.VT0";

/*-------------------------------*/
/*  נןלץ‏יפר יםס זבךלב מבףפעןךכי */
/*-------------------------------*/
char *
gttynm()
{
	FILE    *fopen();
	FILE    *i_fp;
	register char *s;

	for( s = &ttynm[2]; *s < '9'; *s += 1 ) {
		if((i_fp=fopen(ttynm, "r")) == NULL )
			continue;
		else {
			fclose(i_fp);
			ttynm[13] = *s;
			break;
		}
	}
	ttynm[0] = 'T';
	ttynm[1] = 'T';
	ttynm[2] = 'Y';
	return( ttynm );
}
#endif

/*  תבנעןףש ית גיגליןפוכי termcap */
extern  int  tgetent(), tgetnum(), tgetflag();
extern  char *tgetstr();

extern  char *UP;       /* קמוûמיו נועוםוממשו הלס tgoto() */
extern  char *BC;

/* גץזוע ןניףבמיך linlib, י ץכבתבפולר
 * הלס ן‏ועוהמןחן תבמוףומיס ק גץזוע.
 */
static char *codesp = 0;

/* עבתםועש üכעבמב */
extern  int     maxli;
extern  int     maxco;


/*---------------------*/
/* ןניףבמיו כלבקיבפץעש */
/*---------------------*/
extern  KBF   kbf[];
extern  char *tcapo[];


static int cant()
/*-------------*/
/* היבחמןףפיכב */
/*-------------*/
{
#ifdef  DEMOS2
	fprintf(stderr, "%s '%s'\n",
	ediag("Bad environ", "מו ץףפבמןקלומב נועוםוממבס"), "TERM");

#endif
#ifdef  RT11
	fprintf(stderr,
"=== מופ זבךלב מבףפעןךכי :\n\r\
	TTY:TRMCAP.VT* (.ASSIGN NL: VT*:)\n\r");
#endif
	exit(1);

}

/*----------------------------------------*/
/* יתגבקיפרףס ןפ ליהיעץא‎יט דיזע תבהועצכי */
/* ‏יפבפר ית s2, ניףבפר ק s1              */
/*----------------------------------------*/
static int stripd(s1, s2)
register char *s1, *s2;
{
	while(isdigit(*s2))
		s2++;
	strcpy(s1, s2);
}


char *
gettcp(key)
/*-----------------------------------*/
/* נןלץ‏יפר ESC-כןה הלס üלוםומפב key */
/*-----------------------------------*/
char *key;
{
	/* תבניûום מבךהוממץא ESC-נןףלוהןקבפולרמןףפר
	 * ק גץזוע, קועמום ץכבתבפולר מב מב‏בלן
	 * ףפעןכי ק גץזועו.
	 */
	register char *p, *pp;
	char *ptcap;
	char tbuf[64];          /* גץזוע ן‏ועוהמןחן ןניףבמיס */
	char *x;                /* ץכבתבפולר הלס ןגעב‎ומיס כ tgetstr() */

	x = tbuf;
	pp = codesp;    /* קףנןםמיפר, כץהב כלבףפר ן‏ועוהמןו ןניףבמיו */
	p = tgetstr(key, &x);          /* ן‏ועוהמןו ןניףבמיו */
	if (p && *p!=0) { ; }
	else {  p = ""; }

	ptcap = pp;
#ifdef USE_W_RAW
	/* ץהבליפר תבהועצכי, ףןטעבמיפר ןניףבמיו ק גץזועו: */
	stripd(pp, p);
#else
	strcpy(pp, p);
#endif
	pp += (linptr_t)(strlen(pp)+1);

	codesp = pp;    /* תבנןםמיפר, כץהב כלבףפר ן‏ועוהמןו ןניףבמיו */
	return(ptcap);
}

hw_set()
/*--------------------------------------*/
/* קשנןלמיפר מבףפעןךכץ מב פינ פועםימבלב */
/*--------------------------------------*/
{
	register char *p;
	register char **pp;
	register KBF *kbfp;
	kbcod    cod;
	char  *buf;             /* ץכבתבפולר מב קעוםוממשך גץזוע termcap */
	char     keys[4];       /* ףפעןכב הלס נןיףכב ק termcap ("k1") */
	int     ok;

	/* נבםספר הלס מבףפעןךכי linlib */
	codesp = malloc(TBUFSZ);  /* היבחמןףפיכב מיצו... */

	/* נןלץ‏יפר נבםספר הלס קעוםוממןחן גץזועב */
	if( (buf = malloc(TBUFSZ)) == 0 ) {
		fprintf(stderr, "\nHWSET:%s\n", ediag("NO Memory\n", "םבלן נבםספי"));
		exit(1);
	}
	/* נןלץ‏יפר ןניףבמיו פועםימבלב */
#ifdef  DEMOS2
	if( p = getenv("TERM") ) {
#endif
#ifdef  RT11
	if( p = gttynm() ) {
#endif
		ok = tgetent(buf,p);
		if(ok != 1) {
			fprintf(stdout, "%s: '%s'\n",
	   ediag("Unknown terminal type", "פועםימבל מויתקוףפומ"), p);
			exit(1);
		}
	}
	else    {
		cant();
	}

	/*---------------------------------*/
	/* מבףפעןיפר ץכבתבפולי הלס tgoto() */
	/*---------------------------------*/
/***    p = tgetstr("bc", &x);
	if (p && *p==0) {       BC = 0; }
	else    {
		stripd(bc_str, p); BC = bc_str; }
	p = tgetstr("up", &x);
	if (p && *p==0) {       UP = 0; }
	else    {
		stripd(up_str, p); UP = up_str; }

****/   /* םןצמן נען‎ו: */
	BC = gettcp("bc");   if(*BC=='\0') BC = 0;
	UP = gettcp("up");   if(*UP=='\0') UP = 0;

	/*-------------------------*/
	/* נןלץ‏יפר עבתםועש üכעבמב */
	/*-------------------------*/
#ifdef RT11
	maxli = tgetnum("li");
	maxco = tgetnum("co");
#else
	/*maxli = maxco = 0;*/
	if (0 == gtty_sz()) {
		maxli = tty_li;
		maxco = tty_co;
	}
	if (p = getenv("LINES"))    maxli = atoi(p);
	if (maxli == 0)             maxli = tgetnum("li");
	if (maxli > MAXLICO)		maxli = MAXLICO;
	if (p = getenv("COLUMNS"))  maxco = atoi(p);
	if (maxco == 0)             maxco = tgetnum("co");
	if (maxco > MAXLICO)		maxco = MAXLICO;
#endif

	/*--------------------------------*/
	/* מבףפעןיפר ץכבתבפולי הלס linlib */
	/*--------------------------------*/
	for (pp=tcapo; *pp; pp++) {
		*pp = gettcp(*pp);
	}
	for (kbfp= &kbf[0]; cod=(kbfp->t_key); kbfp++) {
		keys[0] = cod0(cod);
		keys[1] = cod1(cod);
		keys[2] = '\0';
#ifdef DEBUG
		printf("keys = '%s'\n", keys);
#endif
		kbfp->t_cap = gettcp(keys);
	}
	do_kbl();       /* מבףפעןיפר פבגל. לןח. כןהןק */
	free(buf);
}
