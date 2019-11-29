/*
**      +----------+    βιβμιοτελα χχοδα-χωχοδα
**     (c) linlib  !    δμρ αμζαχιτξο-γιζςοχωθ
**      +----------+    χιδεοτεςνιξαμοχ
**/

/*
 *      $Header: lin330.c,v 1.1 90/08/24 08:08:11 vsv Exp $
 *
 *      $Log:	lin330.c,v $
 * Revision 1.1  90/08/24  08:08:11  vsv
 * Initial revision
 * 
 * Revision 3.1  89/08/29  15:17:03  vsv
 * χεςσιρ LINLIB_3
 * 
 * Revision 3.0  88/06/16  17:50:23  vsv
 * ξαχεδεξ ξελοτοςωκ ημρξεγ ξα λοναξδω ςεδαλτιςοχαξιρ
 * 
 */

#include <stdio.h>
#include <ctype.h>
#include "line.h"
#include "line0.h"

extern SCREEN scrn;

int     edinsm = 1;     /* ΖΜΑΗ: ÒΕΦΙΝ ΧΣΤΑΧΛΙ 1, ΪΑΝΕΞΩ 0 */
/* extern  int edinff;  */
int     edinff = 1;     /* ΖΜΑΗ: ΠΟΛΑΪΩΧΑΤΨ ΣΟΣΤΟΡΞΙΕ ÒΕΔΑΛΤΟÒΑ ΣΤÒΟΛΙ */
int     edshow = 1;     /* ΖΜΑΗ: ΠΟΛΑΪΩΧΑΤΨ ΣΤÒΟΛΥ ΔΟ ÒΕΔΑΛΤΙÒΟΧΑΞΙΡ */

re_str(str_l, size, ctst, ofsp)
/*
 * ςΕΔΑΛΤΟÒ ΣΤÒΟΛΙ ΒΕΪ ΠΟΛΑΪΑ ΣΤΑÒΟΗΟ ΣΟΔΕÒΦΙΝΟΗΟ
 */
register char    * str_l; /* ΣΤÒΟΛΑ ΔΜΡ ÒΕΔΑΛΤΙÒΟΧΑΞΙΡ*/
int     size;           /* ÒΑΪΝΕÒ ΠΟΜΡ ÒΕΔΑΛΤΙÒΟΧΑΞΙΡ */
kbcod   (*ctst)();      /* ΤΕΣΤ ΔΜΡ ΧΧΟΔΑ ΠΕήΑΤΑΕΝΩΘ ΛΟΔΟΧ */
int     *ofsp;          /* ΥΛΑΪΑΤΕΜΨ ΞΑ ΧΕΜΙήΙΞΥ ΣΝΕέΕΞΙΡ ΟΤ ΞΑή. ΠΟΜΡ */
{
	kbcod cod;
	int     _edshow, _edinff, _edinsm;
	_edshow = edshow; edshow = 0;
	_edinff = edinff; edinff = 0;
	_edinsm = edinsm; edinsm = 1;
	cod = e_str(str_l, size, ctst, ofsp);
	edshow = _edshow;
	edinff = _edinff;
	edinsm = _edinsm;
	return cod;
}

static edinfo( infflg )
register int infflg;
/*
 * πΟΛΑΪΑΤΨ ÒΕΦΙΝ ÒΕΔΑΛΤΙÒΟΧΑΞΙΡ, ΣΝ. ΞΙΦΕ
 */
{
/*	if ( !edinff )  return;*/
#ifndef CP_SAV
	cp_sav();
#endif
	cp_set(maxli-1, maxco-8, TXT);
	er_eol();
	cp_set(maxli-1, maxco-7, TXT|INP);
	if ( infflg ) {
		w_str("ed:");
		if ( edinsm ) {
			w_str("Ins");
		} else {
			w_str("Ovr");
		}
	} else {
		cp_set(maxli-1, maxco-8, TXT);
		er_eol(FGBG);
	}
#ifndef CP_SAV
	cp_fet();
#endif

}
/*
static showed()
{
	if ( !edinff )  return;
	if ( infflg ) {
		w_msg(scrn.sc_at, "ed:");
		if ( edinsm )   w_str("Ins") else w_str("Ovr");

	} else {
		w_emsg("");
	}
}
*/

static int
chgstr(s, size, ofs, cod)
/* χωπομξιτψ ιϊνεξεξιρ ι πολαϊατψ */
register char *s;
int  size;
int ofs;
kbcod cod;
{
	register int j;
	register int lend;      /* ποϊιγιρ ποσμεδξεηο ξεπυστοηο σινχομα */
		 int chg;       /* ποϊιγιρ ιϊνεξεξιρ */

	/* ξακτι μοη. λοξεγ στςολι */
	if(edinsm) {
		for(lend=size; lend>0 && s[--lend]==' ';) ;
		if(lend<ofs) lend=ofs;
	}
	chg = ofs;
	if(cod==KB_DE) {
		cod = ' ';
		ofs = --chg;
		if(edinsm) {
			for(j=chg; j<lend; j++) s[j]=s[j+1];
			s[j++]=' ';
		} else {
			s[lend = chg] = cod;
		}
		/******* <--(KB_DE)--- ******/
		cp_set(scrn.sc_li, scrn.sc_co - 1, scrn.sc_at);
	} else {
		if(edinsm) {
			for(j=size; --j>chg; ) s[j]=s[j-1]; /* χσταχιτψ */
			lend += (lend<(size-1) ? 1 : 0);
		} else {
			lend = chg;
		}
		s[chg] = cod;
		ofs += 1;
	}

	for(j=chg; j<=lend; j++) w_chr(s[j]);   /* ξα όλςαξ!!! */
	return (ofs);
}


e_str(str_l, size, ctst, ofsp)
/*-----------------*/
/* ςεδαλτος στςολι */
/*-----------------*/
register char    *str_l;  /* στςολα δμρ ςεδαλτιςοχαξιρ*/
int     size;           /* ςαϊνες πομρ ςεδαλτιςοχαξιρ */
kbcod   (*ctst)();      /* τεστ δμρ χχοδα πεώαταενωθ λοδοχ */
int     *ofsp;          /* υλαϊατεμψ ξα χεμιώιξυ σνεύεξιρ οτ ξαώ. πομρ */
{
	/* ξαώαμο πομρ ι VIDEO ϊαδαΰτσρ ώεςεϊ cp_set();
	 * ctst χοϊχςαύαετ 0, εσμι λοδ ξε ιϊνεξρετ στςολυ;
	 *                -1, εσμι ξαδο ϊαλοξώιτψ ςεδαλτιςοχαξιε;
	 *              cod,  εσμι λοδ ιϊνεξρετ σοδεςφινοε στςολι.
	 */

	register int i;
	register int j;
	register int  column;      /* ποϊιγιρ ι στςολα ξα όλςαξε */
			 int  linenu;
			 int  attrib; /* ΜΟΗΙήΕΣΛΙΚ ΧΙΔΕΟΑΤÒΙΒΥΤ */
		 kbcod    cod;
		 kbcod    ok;

	column = scrn.sc_co;
	linenu = scrn.sc_li;
	attrib = scrn.sc_at;

	edinfo(1);
	/* ΪΑΠΟΜΞΙΤΨ ΠÒΟΒΕΜΑΝΙ ΛΟΞΕΓ ΣΤÒΟΛΙ */
	j = 0;
	while(j<size && str_l[j]) j++;
	while(j<size)       str_l[j++] = ' ';
	str_l[size] = 0;
	if (edshow) {
		/* ΠΟΛΑΪΑΤΨ ΞΑ άΛÒΑΞΕ ΠΕÒΕΔ ÒΕΔΑΛΤΙÒΟΧΑΞΙΕΝ */
		cp_set(linenu, column, attrib);
		j = 0;
		while(j<size) w_chr(str_l[j++]);
	}
	i = ofsp ? *ofsp : 0;

	/* γιλμ ςεδαλτιςοχαξιρ */
	for( ;; ) {
		if(i >= size) i = size-1;       /* χσεηδα πςοχεςρτψ!!! */
		cp_set(linenu, column+i, attrib);

		cod = r_cod(0);
		/* πςοτεστιςυεν χχοδ */
		ok = ctst ? (*ctst)(cod, size, i) : cod;
		if(ok == -1) {
			goto ret;
		} else if(ok == 0) {
			bell();
		} else {
			switch(cod = ok) {
			case KB_PR: /* δοπ. λοναξδω ςεδαλτοςα στςολι */
				switch(r_cod(0)) {
				case KB_AR: /* λοξεγ ϊξαώαύεκ ιξζ. */
					   for(i=size-1; isspace(str_l[i]);	i--); i++; break;
				case KB_AL: i = 0; break;
				case ' ':
					   for(j=i; j<size; j++) {
						w_chr(str_l[j] = ' ');
					   }; break;
				case KB_DE:
					   for(j=i; j<size-1; j++) {
					       w_chr(str_l[j] = str_l[j+1]);
					   };
					   w_chr(str_l[j] = ' ');
					   str_l[size] = '\0';
					   break;
				case KB_PR: edinsm=(edinsm ? 0 : 1);
					   edinfo(1);
					   break;
				default: bell(); break;
				}; break;

			/* linlib 4 since 2017-05 */
			case KB_KE: /* λοξεγ ϊξαώαύεκ ιξζ. */
			   for(i=size-1; isspace(str_l[i]);	i--); i++; break;
			case KB_KH: i = 0; break;
			case KB_IN: edinsm=(edinsm ? 0 : 1);
				   edinfo(1);
				   break;
			case KB_KD:
				   for(j=i; j<size-1; j++) {
				       w_chr(str_l[j] = str_l[j+1]);
				   };
				   w_chr(str_l[j] = ' ');
				   str_l[size] = '\0';
				   break;
			/*----------------------------*/

			case KB_AL: if(i>0)  i--;
				   else     goto ret;
				   break;
			case KB_TA: /*bell(); break;*/
				/*cp_sav(); cpa(-2, 1, ATT); w_str("ta"); cp_fet();*/
				goto ret; break;
#ifdef OLD_USE_TAB
			case KB_TA: if(i == size-1) goto ret;
				   i += 8-(i%8); break;
#endif
			case KB_AR: if(i == size-1) goto ret;
				   i++;
				   break;
			case KB_NL: goto ret;
			case KB_DE:
				   if(i==0)      goto ret;
				   i = chgstr(str_l, size, i, cod);
				   break;
			default:
				if(cod1(cod))   goto ret;/*ξο νοφξο ι μυώϋε*/
				else            i=chgstr(str_l, size, i, cod);

			}
		}
		/* λοξεγ γιλμα ςεδαλτοςα */
	}
ret:
	str_l[size] = 0;
	if(ofsp) *ofsp = i;

	/* ποδώιστιτψ πςοβεμω χ λοξγε στςολι */
	for (i=size; --i>=0 && (str_l[i]==' ');) ;
	str_l[++i] = 0;
	edinfo(0);
	return(cod);
}
