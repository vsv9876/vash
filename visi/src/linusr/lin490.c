/*
**      +----------+    βιβμιοτελα χχοδα-χωχοδα
**     (c) linlib  !    δμρ αμζαχιτξο-γιζςοχωθ
**      +----------+    χιδεοτεςνιξαμοχ
**/

/*
 *      $Header: /home/vsv/proj/proj20010321/visi/src/linusr/RCS/lin490.c,v 3.1 1989/08/29 14:50:44 vsv Rel vsv $
 *
 *      $Log: lin490.c,v $
 *      Revision 3.1  1989/08/29 14:50:44  vsv
 *      χεςσιρ LINLIB_3
 *
 * Revision 3.0  88/06/16  18:06:19  vsv
 * πςεδχαςιτεμψξωκ χωπυσλ
 * 
 */

#ifndef RETRO
#include <string.h>
#endif
#include <ediag.h>
#include <stdio.h>
#include "line.h"

lerror(s)
/*-----------------------*/
/* αξαμοη ζυξλγιι perror */
/*-----------------------*/
register char *s;
{
	extern  int errno;              /* σιστενξωκ ξονες οϋιβλι */
	extern  int sys_nerr;           /* ναλσ. ξονες σοοβύεξιρ */
#ifdef DURA_LINUX
	extern  char *sys_errlist[];    /* αξημ. σπισολ σοοβύεξικ */
	extern  char *sys_rerrlist[];   /* ςυσσ. σπισολ σοοβύεξικ */
#endif
	char    outstr[STRLEN];
	char	*erstr;

#ifndef RETRO
	erstr = strerror(errno);
	sprintf(outstr, "CLIB: '%s' : %s", s, erstr);
#else
	if (errno <= sys_nerr ) {
		sprintf ( outstr,
		"CLIB: '%s' : %s",      s,
		ediag ( sys_errlist[ errno ], sys_rerrlist[ errno ] ) );
	} else {
		sprintf ( outstr, "CLIB: %d : %s ",
		ediag ( "Unknown error", "ξειϊχεστξαρ οϋιβλα" ) );
	}
#endif
	/* χωχοδ σοοβύεξιρ οβ Οϋιβλε */
	w_emsg ( outstr );
}
