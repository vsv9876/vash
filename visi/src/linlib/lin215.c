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

/*------------------------------------------*/
/* קועמץפר נען‏יפבממשך כןה קן קטןהמןך נןפןכ */
/*------------------------------------------*/
static  kbcod   backcod = 0;

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

static int last_c;
char  r_chr() /* TODO: UTF8 support; wchar r_chr() */
{
	return (last_c & 0377);
}

kbcod r_cod(oldcod)
/*------------------------*/
/* קועמץפר לןחי‏וףכיך כןה */
/*------------------------*/
kbcod oldcod;
{
	register KBL *kblp;
	kbcod   bckc;
	kbcod   newcod;

	if(backcod) { bckc = backcod; backcod = 0; return(bckc); }

	if(oldcod==0) {
		oldcod=r_key();
	}
	newcod = 0;
	for(kblp=kbl; kblp->t_cod; kblp++) {
		if(kblp->t_key == oldcod) {
			newcod = (kblp->t_cod);
			break;
		}
	}
	/* ËÌÁÓÓÉÆÉÃÉÒÏ×ÁÔØ ËÌÁ×ÉÛÕ, ×ÅÒÎÕÔØ ËÏÄ ÇÒÕÐÐÙ;
	 * TODO: ÐÅÒÅÎÅÓÔÉ ÓÀÄÁ ÞÁÓÔØ ËÏÄÁ ÉÚ lin310.c: r_key()
	 * */
	if (newcod == 0 || newcod == -1) {
		/*ËÌÁ×ÉÛÁ ÎÅ ÏÐÏÚÎÁÎÁ, ÎÏ ÂÙÌÁ ÎÁÖÁÔÁ*/
		return(-1);
/*	} else if (newcod > 0 ...) {*/
	}
	/* קכלא‏יפר/קשכלא‏יפר הןנןלמיפולרמץא כלבקיבפץעץ */
	if(newcod == KB_KP) {
		if(kpadon) { kpadon = 0; w_raw(t_ke); }
		else       { kpadon = 1; w_raw(t_ks); }
		fflush(vttout);
	}
	last_c = newcod;
	return(newcod);
}

