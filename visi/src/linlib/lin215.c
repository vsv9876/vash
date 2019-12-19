/*
**      +----------+    גיגליןפוכב קקןהב-קשקןהב
**     (c) linlib  !    הלס בלזבקיפמן-דיזעןקשט
**      +----------+    קיהוןפועםימבלןק
**/

/*
 *      $Header: lin215.c,v 1.1 90/12/27 16:28:58 vsv Rel $
 *
 *      $Log:	lin215.c,v $
 * Revision 1.1  90/12/27  16:28:58  vsv
 * קועףיס LINLIB_3
 * 
 * Revision 3.4  89/08/29  15:15:29  vsv
 * קועףיס LINLIB_3
 * 
 * Revision 3.3  88/07/28  09:17:13  vsv
 * הןגבקלומב ןגעבגןפכב קכל/קשכל הןנןלמיפולרמןך כלבקיבפץעש
 * 
 * Revision 3.2  88/06/27  15:19:43  vsv
 * עוקיתיס בעטיקב RCS
 * 
 * Revision 3.1  88/04/29  12:25:29  vsv
 * עבתהולומש פבגלידש י נעןדוהץעש,
 * יםוופףס הקב עבתמשט םןהץלס
 * 
 */

#include <stdio.h>
#include "line.h"
#include "line0.h"


/* קמוûמיו נועוםוממשו הלס tgoto() ית termcap */
extern char *UP;
extern char *BC;

/* עבתםועש üכעבמב */
extern int     maxli;
extern int     maxco;


/*---------------------*/
/* ןניףבמיו כלבקיבפץעש */
/*---------------------*/

extern  KBF   kbf[];


/* ןניףבמיו קןתםןצמןףפוך קשקןהב מב üכעבמ */
extern  char *tcapo[];

/* ףקסתר זיתי‏וףכיט י לןחי‏וףכיט כןהןק, ב פבכצו יםומ כלבקיû */
extern  KBL kbl[KBLSIZE];

int     kpadon = 0;     /* זלבח: נעבקבס הןנ. כלבקיבפץעב קכלא‏ומב */

#define UNREAD_CHAR_RETRO
#ifdef UNREAD_CHAR_RETRO
/*------------------------------------------*/
/* קועמץפר נען‏יפבממשך כןה קן קטןהמןך נןפןכ */
/*------------------------------------------*/
static  kbcod   backcod = 0;
#endif

unr_c(cod)
kbcod   cod;
{
	backcod = cod;
}

int k_pad(on)
int on;
{
	/* ÜÔÏÔ ÍÏÄÕÌØ ÎÕÖÅÎ ÄÌÑ io_set */
	if(on) {
		kpadon = 1; w_raw(t_ks);
	}
	else    {
		kpadon = 0; w_raw(t_ke);
	}
	fflush(vttout);
}

int lastchr; /* last parsed printable code - TODO: there will be stored full utf8 encoded symbol from input */
char  r_chr() /* TODO: UTF8 support; wchar r_chr() */
{
	return (lastchr & 0377); /* 8-bit, no utf8 support yet */
}

kbcod r_cod(cod)
/*------------------------*/
/* קועמץפר לןחי‏וףכיך כןה */
/*------------------------*/
kbcod cod;
{
	register KBL *kblp;
	kbcod   bckc;

#ifdef UNREAD_CHAR_RETRO
	if(backcod) { bckc = backcod; backcod = 0; return(bckc); }
#endif
	if(cod==0) {
		cod=r_key();
	}

	lastchr = cod;
	if (cod == 0 || cod == -1) {
		/*ËÌÁ×ÉÛÁ ÎÅ ÏÐÏÚÎÁÎÁ, ÎÏ ÂÙÌÁ ÎÁÖÁÔÁ*/
		return(-1);
	}

	for(kblp=kbl; kblp->t_cod; kblp++) {
		if(kblp->t_key == cod) {
			cod = (kblp->t_cod);
			break;
		}
	}
	/*TODO : utf8 parsing will be there*/
	/* ןגעבגןפכב עץףףכיט י במחליךףכיט נו‏בפמשט כןהןק */
	/* ËÌÁÓÓÉÆÉÃÉÒÏ×ÁÔØ ËÌÁ×ÉÛÕ, ×ÅÒÎÕÔØ ËÏÄ ÇÒÕÐÐÙ, ÚÁÐÏÍÎÉÔØ ÏÂÙÞÎÙÊ (printable) ÓÉÍ×ÏÌ ÄÌÑ r_chr()
	 * TODO: ÐÅÒÅÎÅÓÔÉ ÓÀÄÁ ÞÁÓÔØ ËÏÄÁ ÉÚ lin310.c: r_key()
	 * */

	if(((cod < 0377)&&(cod > 0277))
		|| ((cod > 037)&&(cod < 0177))) {
			return(cod);
	}
#if 0
	else if(cod == 0177) {
		/* ASCII DEL*/
		return(KBCOD('d','e'));
	}
	else if(cod > 0 && cod < 040) {
		/* Nonprintable ASCII*/
		return( KBCTL(cod + ('A'-'\001'))) ;
	}
#endif

	/* קכלא‏יפר/קשכלא‏יפר הןנןלמיפולרמץא כלבקיבפץעץ */
	if(cod == KB_KP) {
		if(kpadon) { kpadon = 0; w_raw(t_ke); }
		else       { kpadon = 1; w_raw(t_ks); }
		fflush(vttout);
	}
	return(cod);
}

