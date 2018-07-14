/*
**      +----------+    βιβμιοτελα χχοδα-χωχοδα
**     (c) linlib  !    δμρ αμζαχιτξο-γιζςοχωθ
**      +----------+    χιδεοτεςνιξαμοχ
**/

/*
 *      πομψϊοχατεμψσλικ υςοχεξψ βιβμιοτελι LINLIB
 *
 *      u_page(page, phline)    ςαβοτα σο στςαξιγεκ;
 */

/*
 *      $Header: lin471.c,v 1.1 89/08/29 14:50:41 vsv Rel $
 *      $Log:	lin471.c,v $
 * Revision 1.1  89/08/29  14:50:41  vsv
 * χεςσιρ LINLIB_3
 * 
 */

#include <stdio.h>
#include "line.h"

u_page(page, phline)
/*-----------------------------*/
/* ζυξλγιρ ςαβοτω σο στςαξιγεκ */
/*-----------------------------*/
LINE *page;             /* οσξοχξαρ στςαξιγα */
LINE *phline;           /* ινρ ζακμα στςαξιγω HELP */
{
    kbcod   cod;        /* λοδ ποσμεδξεκ λμαχιϋι */
    LINE *cline;        /* υλαϊατεμψ ξα τελυύυΰ μιξιΰ χ στςαξιγε */

    cline = page;       /* υσταξοχιτψ χ ξαώαμο στςαξιγω */
    er_pag();
    w_page(page);

    while ( -1 ) {
	cod = r_page( page, &cline, 0);
	switch ( cod ) {
	case ' ':
		/* πεςεςισοχατψ ποσμε νεξΰ */
		if((cline->attr & LMSE) == LMSE) {
			er_pag();
			w_page(page);
		}
		break;
	case KB_EX :
		/*NOBREAK*/
		return;
	case '?':
	case KB_HE:
		/* πολαϊατψ σπςαχοώξυΰ ιξζοςναγιΰ */
		w_help(phline);
		w_page(page);
		break;
	default:   w_emsg("");     /* ποηασιτψ σοοβύεξιε οβ οϋιβλε */
	}
    }
}

