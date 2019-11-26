/*
**      +----------+    βιβμιοτελα χχοδα-χωχοδα
**     (c) linlib  !    δμρ αμζαχιτξο-γιζςοχωθ
**      +----------+    χιδεοτεςνιξαμοχ
**/

/*
 *      πομψϊοχατεμψσλικ υςοχεξψ βιβμιοτελι LINLIB
 *
 *      cvt_sp()   χχοδ στςολι πο υλαϊατεμΰ;
 *
 */

/*
 *      $Header: lin472.c,v 1.1 89/08/29 14:50:42 vsv Rel $
 *      $Log:	lin472.c,v $
 * Revision 1.1  89/08/29  14:50:42  vsv
 * χεςσιρ LINLIB_3
 * 
 */

#include <stdio.h>
#include <string.h>
#include "line.h"


int     cvt_sp(line, cod, mod, str)
/*-----------------------------------*/
/* χχοδ στςολι πο δχοκξονυ υλαϊατεμΰ */
/*-----------------------------------*/
LINE *line;
kbcod cod;
char *mod;
char *str;
{
	register char **spp;
#ifndef OLD_CVT_SP
	register int max_co;
	int size;
	register char *si;
	register char *so;
	register int i;

	spp = (char **)line->varl;
	size = line->size;
	if (line->attr & PMT) size -= 1;
	max_co = maxco-2;
	if (str) {
		if(*mod == 'r') {
			strcpy(*spp, str);
			/*for (so=*spp, si=str, i=size; *si!='\0' && i > 0; i--)
				*so++ = *si++;
			*so = '\0';*/
		}
		else    {
			strncpy(str, *spp, size);
			if (strlen(*spp) >= size) str[size] = '\0';
			/*for (so=str, si=*spp, i=size; *si!='\0' && i > 0; i--)
				*so++ = *si++;
			*so = '\0';*/
		}
	}
#else

	spp = (char **)line->varl;
	if(*mod == 'r') {
		strcpy(*spp, str);
	}
	else    {
		strcpy(str, *spp);
	}
#endif
	return(TRUE);
}

int     cvt_s(line, cod, mod, str)
/*--------------------------*/
/*χχοδ στςολι πο υλαϊατεμΰ */
/*--------------------------*/
/*TODO ΠÒΟΧΕÒΙΤΨ ΞΑ ΤΑΧΤΟΜΟΗΙΐ ΠΟΧΕΔΕΞΙΡ ΠÒΙ ΟΤΥΤΣΤΧΙΙ ΠΟΜΡ line.varl, ΤΟΗΔΑ άΤΑ ΖΥΞΛΓΙΡ ΠΟΠÒΟΣΤΥ ΞΕ ΞΥΦΞΑ...*/
LINE *line;
kbcod cod;
char *mod;
char *str;
{
	char *sp;

	int max_co;
	int size;
	register char *si;
	register char *so;
	register int i;

	sp = (char *)line->varl;
	size = line->size;
	if (line->attr & PMT) size -= 1;
	max_co = maxco-2;
	if (str) {
		if(*mod == 'r') {
			strcpy(sp, str);
			/*for (so=sp, si=str, i=size; *si!='\0' && i > 0; i--)
				*so++ = *si++;
			*so = '\0';*/
		}
		else    {
			strncpy(str, sp, size);
			if (strlen(sp) >= size) str[size] = '\0';
			/*for (so=str, si=sp, i=size; *si!='\0' && i > 0; i--)
				*so++ = *si++;
			*so = '\0';*/
		}
	}
	return(TRUE);
}

