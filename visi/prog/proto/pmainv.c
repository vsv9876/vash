/*
**      +----------+    גיגליןפוכב קקןהב-קשקןהב
**     (c) linlib  !    הלס בלזבקיפמן-דיזעןקשט
**      +----------+    קיהוןפועםימבלןק
**/

/*
 *      $Header$
 *      $Log$
 */

#include <stdio.h>
#include "line.h"       /* זבךל-תבחןלןקןכ LINLIB */


static  int ex_flg;     /* זלבח: נןעב תבכבמ‏יקבפר */

mkexit()
{
	cp_set(-1, 0, TXT);
	ex_flg = 1;
	return(TRUE);
}


LINE linem[] = {
{ 80, 0, 0, 0, LHDR, 0,0,0, "נעןפןפינ נעיכלבהמןך נעןחעבםםש  LINLIB V3.4" },
{ 38, 8,24, 0,	INP|PMT|LMSE,	0,	mkexit,	tst_m,	" כןמוד עבגןפש " },
{  4,13, 3, 0, ATT|MID|PAD, 0, cvt_lh, 0, ":? " },
{  8,13, 8, 0, ATT|MID|PAD, 0, cvt_lh, 0, ":HE" },
{ 22,13,18, 0, LTXT, 0,0,0, "קשקןה נןהףכבתכי (HELP)" },
{  8,15, 8, 0, ATT|MID|PAD, 0, cvt_lh, 0, ":RE" },
{ 28,15,18, 0, LTXT, 0,0,0, "נועועיףןקכב üכעבמב (REFRESH)" },
{ 0 }, };



vmain()
/*------------*/
/* VIDEO MAIN */
/*------------*/
{
	kbcod cod;
	LINE *cline;
	cline = 0;

	er_pag();
	w_page(linem);

	for( ;; ) {
		cod = r_page(linem, &cline, 0);
		/* זלבח קתקןהיפףס ‏ועות זץמכדיא ןגףלץציקבמיס םומא */
		if(ex_flg) return;

		switch(cod) {
		case ' ' :
			/* נועועיףןקבפר נןףלו םומא */
			if((cline->attr & LMSE) == LMSE) {
				er_pag();
				w_page(linem);
			}
			break;
		case '?':
		case KB_HE:
			/* קשקןה ףנעבקן‏מןך ימזןעםבדיי */
			w_help(0); w_page(linem);
			break;
		case KB_EX :
			/* תקןמןכ, ית üפןך ףפעבמידש
			 * נןהמיםבפרףס מוכץהב
			 */
			bell();
			break;
		default :
			w_emsg("");
			cp_set(-1, 0, TXT);
			printf("<%c%c>", cod1(cod), cod0(cod));
			fflush(vttout);
			break;
		}
	}
}
