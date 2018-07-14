/*
**      +----------+    גיגליןפוכב קקןהב-קשקןהב
**     (c) linlib  !    הלס בלזבקיפמן-דיזעןקשט
**      +----------+    קיהוןפועםימבלןק
**/

/*
 *      $Header: lin420.c,v 3.2 89/08/29 14:44:46 vsv Rel $
 *
 *      $Log:	lin420.c,v $
 * Revision 3.2  89/08/29  14:44:46  vsv
 * קועףיס LINLIB_3
 * 
 * Revision 3.1  88/06/27  15:29:54  vsv
 * עוקיתיס בעטיקב RCS
 * 
 * Revision 3.0  87/12/21  12:21:20  vsv
 * נעוהקבעיפולרמשך קשנץףכ.
 * 
 */

#include <stdio.h>
#include "line.h"

/*---------------------------------------*/
/* ףפבמהבעפמןו ץנעבקלומיו נועוכלא‏בפולום */
/*---------------------------------------*/

cvt_a(line, cod, mod, str)
LINE    *line;
int       cod;
char     *mod;
char     *str;
{
	register        int     *indx;
	register        char    **p;
	register        int     i;

	indx = (int *)line->varl;
	p  = (char **)line->cvts;

	/* הלס מבהוצמןףפי נעי עבגןפו ף "םץףןעןם" */
	/* ףמב‏בלב מבניûום קשקןה - üפן נען‎ו */
	if( *mod == 'w' ) {
		for(i=0; p[i]!=0; i++) ;
		if( *indx >= i )
			*indx = 0;
		strcpy(str, p[*indx]);

	} else if( *mod == 'r' ) {
		switch ( cod ) {
		case 0:         bell();
				break;
		case ' ' :
		case '.' :
		case '>' :
			*indx += 1 ;
			if(p[*indx] == 0)
				*indx = 0;
			break;

		case KB_DE:
		case ',' :
		case '<' :
			if ( *indx == 0 ) {
				while(p[*indx +1] != 0) {
					*indx += 1;
				}
			}
			else *indx -= 1;
			break ;
		default :
			break ;
		}
	}
	return( TRUE ) ;        /* ןûיגןכ גשפר מו הןלצמן גש... */
}
