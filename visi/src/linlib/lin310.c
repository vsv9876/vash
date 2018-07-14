/*
**      +----------+    גיגליןפוכב קקןהב-קשקןהב
**     (c) linlib  !    הלס בלזבקיפמן-דיזעןקשט
**      +----------+    קיהוןפועםימבלןק
**/

/*
 *      $Header: lin310.c,v 1.1 90/12/27 16:29:12 vsv Rel $
 *
 *      $Log:	lin310.c,v $
 * Revision 1.1  90/12/27  16:29:12  vsv
 * קועףיס LINLIB_3
 * 
 * Revision 3.3  89/08/29  15:16:44  vsv
 * קועףיס LINLIB_3
 * 
 * Revision 3.2  88/07/27  16:34:40  vsv
 * עבגןפב ף כלבקיûוך 'DEL' נעיקוהומב כ ןג‎וםץ קיהץ
 * 
 * Revision 3.1  88/06/27  15:20:48  vsv
 * עוקיתיס בעטיקב RCS
 * 
 * Revision 3.0  87/12/21  12:19:24  vsv
 * נעוהקבעיפולרמשך קשנץףכ.
 * 
 */


#include <stdio.h>
#include "line.h"
#include "line0.h"

extern  int     ttyinp();
extern  KBF     kbf[];


/*------------------------------------------------------*/
/* קועמץפר cod, וףלי מי‏וחן מו ףןקנבלן, ליגן כןה linlib */
/*------------------------------------------------------*/
kbcod  k_pars(cod)
int cod;
{
	register int n;
	register KBF *kbfp;
	register char *esccod;
	int  like;               /* זלבח: וףפר נןטןציו ûבגלןמש */
	char kbuf[10];

	kbuf[0] = cod & 0177;
	kbuf[1] = 0;
	like = 1;               /* מבהן ף ‏וחן-פן מב‏בפר */
	for(n=1; like!=0; ) {

		like = 0;
		for(kbfp = kbf; kbfp->t_key; kbfp++) {
			if(strncmp(kbuf, esccod=kbfp->t_cap, n) == 0) {
				like = 1;
				if(strcmp(kbuf, esccod) == 0)
					return(kbfp->t_key);
				else
					continue;
			}
		}
		if(like == 0) {
			if(n == 1)
				return((kbcod)cod);
			else
				return( 0);     /* מי‏וחן נןטןצוחן */
		}
		else {
			/* ׀ּֿױÞֹװ״ ִֻֿ ִֻֿֿ׳ֿÊ ׀ֿ׃ִֵּֿ׳ֱװֵּ״־ֿ׃װֹ -
			 * Úִֵ׃״ ֵ׃װ״ װׂױִ־ֿ׃װֹ,
			 * ׃׳ׁÚֱ־־Ùֵ ׃ ׀ִֵֵֹֻֿֿׂׂ׳ֻֿÊ ־ֱ ׳׳ִֵֿ
			 * ׳ ֱִׂÊ׳ֵֵׂ ֹ ׳ ׃ֱֽֿÊ ֱֻּ׳ֱֹװױֵׂ,
			 * ֵ׃ֹּ ׳׳ִֿׁװ׃ׁ ׂױ׃׃ֵֹֻ ֲױֻ׳Ù.
			 */
/*                      cod=ttyinp();           */
			kbuf[n++] = escseq(   ttyinp() );
			kbuf[n  ] = 0;
		}
	}
	return(KBCOD('O','O'));   /* מב קףסכיך ףלץ‏בך הלס ןפלבהכי */
}

/*---------------------*/
/* קועמץפר כןה כלבקיûי */
/*---------------------*/
r_key()
{
	register kbcod cod ;
	register pars ;         /* k_pars() = */

	cod = ttyinp();
	if(cod == -1)
		return( -1);    /* בףימטעןממשך עוצים - גות ןציהבמיס */
	else
		cod &= 0377;    /* נןהבקיפר תמבכןקןו עבףûיעומיו int=char */

	if(cod > 0200 && cod < 0300)
		cod &= 0177;    /* ב קהעץח "עץףףכיך" #,%,?, י פ.ה. */

	/* פועםימבלן-תבקיףיםשך כןה */
	if(cod != (pars=k_pars(cod)))
		return(pars);

	/* ןגעבגןפכב עץףףכיט י במחליךףכיט נו‏בפמשט כןהןק */
	else if(((cod < 0377)&&(cod > 0277))
	|| ((cod > 037)&&(cod < 0177)))
		return(cod);

	/* ASCII DEL */
	else if(cod == 0177)
		return(KBCOD('d','e'));

	else if(cod > 0 && cod < 040)   /* Nonprintable ASCII */
		return( KBCTL(cod + ('A'-'\001'))) ;
	else
		return(cod);   /* הלס ןפלבהכי */
}
