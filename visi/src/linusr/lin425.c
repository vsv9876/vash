/*
**      +----------+    βιβμιοτελα χχοδα-χωχοδα
**     (c) linlib  !    δμρ αμζαχιτξο-γιζςοχωθ
**      +----------+    χιδεοτεςνιξαμοχ
**/

/*
 *      $Header: lin425.c,v 3.2 89/08/29 14:45:19 vsv Rel $
 *
 *      $Log:	lin425.c,v $
 * Revision 3.2  89/08/29  14:45:19  vsv
 * χεςσιρ LINLIB_3
 * 
 * Revision 3.1  89/01/13  19:12:04  vsv
 * ξαϊχαξιε λμαχιϋι τεπεςψ νοφετ σοδεςφατψ πςοβεμ.
 * λςονε τοηο, πςινεξεξω ναλςοσω tocod ιϊ line.h
 * 
 * Revision 3.0  88/06/16  18:02:25  vsv
 * πςεδχαςιτεμψξωκ χωπυσλ
 * 
 * Revision 3.0  87/12/21  12:21:20  vsv
 * πςεδχαςιτεμψξωκ χωπυσλ.
 * 
 */

#include <stdio.h>
#include <ctype.h>
#include "line.h"
#include "line0.h"

cvt_lh(line, cod, mod, str)
/*------------------------*/
/* χωχοδ ξαϊχαξιρ λμαχιϋι */
/*------------------------*/
LINE    *line;
int       cod;
char     *mod;
char     *str;
{
	/* ξακτι ι ποδσταχιτψ ξαϊχαξιε λμαχιϋι
	 * πο εε σνωσμοχονυ (μοηιώεσλονυ) λοδυ
	 * ξαπςινες ":NL" ϊανεξρετσρ ξα "RETURN"
	 */
	register char   *s;
	extern   KBL    kbl[];  /* ταβμιγα σχρϊι λοδοχ ι ξαϊχαξικ λμαχιϋ */
	register KBL   *kblp;
		kbcod   codh;   /* λοδ, δμρ λοτοςοηο ξακτι ξαϊχαξιε */

	if( *mod == 'w' ) {
		s = line->varl;
		/* δμρ ότοηο εστψ ναλςοσω χ "line.h" */
		codh = tocod0(s[1]);
		codh |= tocod1(s[2]);

		for(kblp=kbl; kblp->t_key!=0; kblp++) {
			if(kblp->t_cod == codh) {
				strcpy(str, kblp->t_knm);
				goto ret;
			}
		}
		strcpy(str, &line->varl[1]);
ret:
/***** ρχξαρ ώεϋυρ...
		for(s=str+strlen(str), s--; s > str && *s; s--) {
			if(isspace(*s)) *s = '\0';
		}
 *****/
		;
	}
	return( TRUE ) ;        /* οϋιβολ βωτψ ξε νοφετ... */
}
