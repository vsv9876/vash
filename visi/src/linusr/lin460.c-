/*
**      +----------+    βιβμιοτελα χχοδα-χωχοδα
**     (c) linlib  !    δμρ αμζαχιτξο-γιζςοχωθ
**      +----------+    χιδεοτεςνιξαμοχ
**/

/*
 *      $Header: lin460.c,v 3.2 89/08/29 14:50:26 vsv Rel $
 *
 *      $Log:	lin460.c,v $
 * Revision 3.2  89/08/29  14:50:26  vsv
 * χεςσιρ LINLIB_3
 * 
 * Revision 3.1  88/06/27  15:31:01  vsv
 * ςεχιϊιρ αςθιχα RCS
 * 
 * Revision 3.0  88/06/16  18:04:44  vsv
 * πςεδχαςιτεμψξωκ χωπυσλ
 * 
 */

#include <stdio.h>
#include "line.h"

/*---------------------------*/
/* χωχοδ σοοβύεξιρ οβ οϋιβλε */
/*---------------------------*/
static  int errflg ;    /* ζμαη: σοοβύεξικ οβ οϋιβλε ξε βωμο */

w_emsg ( str )
char    *str ;          /* τελστ σοοβύεξιρ οβ οϋιβλε */
{
	if( *str ) {    /* εσμι αςηυνεξτ χωϊοχα ξεπυσταρ στςολα */
		errflg = 1;
		cp_set(-1, 0, ERR);
		bell();
		printf(" ?!! %s ", str);
	} else {
		if(errflg) {
			errflg = 0;
			cp_set(-1, 0, TXT ) ;
			er_eol();
		}
	}
}

w_msg(attr, str)
int     attr;
char *str;
{
	w_emsg(str);
}
