/*
**      +----------+    βιβμιοτελα χχοδα-χωχοδα
**     (c) linlib  !    δμρ αμζαχιτξο-γιζςοχωθ
**      +----------+    χιδεοτεςνιξαμοχ
**/

/*
 *      $Header: lin210.c,v 1.1 90/12/27 16:28:54 vsv Rel $
 *
 *      $Log:	lin210.c,v $
 * Revision 1.1  90/12/27  16:28:54  vsv
 * χεςσιρ LINLIB_3
 * 
 * Revision 3.4  89/08/29  15:14:53  vsv
 * χεςσιρ LINLIB_3
 * 
 * Revision 3.3  88/07/28  09:23:43  vsv
 * ςαβοτα σ λμαχιϋεκ 'DEL' πςιχεδεξα λ οβύενυ χιδυ
 * 
 * Revision 3.2  88/06/27  15:19:23  vsv
 * ςεχιϊιρ αςθιχα RCS
 * 
 * Revision 3.1  88/04/29  12:24:13  vsv
 * ςαϊδεμεξω ταβμιγω ι πςογεδυςω,
 * ινεετσρ δχα ςαϊξωθ νοδυμρ
 * 
 */

#include <stdio.h>
#include "line.h"
#include "line0.h"
#include "lineva.h"

/*
 * ξαστςοκλα ξα τιπ τεςνιξαμα
 *
 * termcap ξε σοχσεν υδοβεξ, ταλ λαλ
 * τςεβυετ πυθμοηο λοδα, λοτοςωκ
 * ισπομψϊυετσρ τομψλο οδιξ ςαϊ.
 * χοϊνοφξο, μυώϋε ξαστςοκλυ ιϊ termcap
 * ηοτοχιτψ οτδεμψξο, ϊατεν χ υφε υπαλοχαξξον χιδε
 * σώιτωχατψ ϊα οδξο οβςαύεξιε λ ζακμυ.
 * πςινεώαξιε.  ςαβοταεν
 * σ termcap, τομψλο HELP χωτασλιχαεν πο λμΰώυ :lh= ... :
 *
 * χ ότον ζακμε οβ'χμεξω δαξξωε, τςεβυενωε δμρ
 * οπισαξιρ χοϊνοφξοστεκ τεςνιξαμα.
 * νοφξο οβ'ρχιτψ χσε στςολι ESC-ποσμεδοχατεμψξοστεκ
 * λοξσταξτανι, ι ζυξλγιΰ hw_set λαλ πυστυΰ -
 * ςαϊνες λοδα ισπομξρΰύεκ βιβμιοτελι
 * υνεξψϋιτσρ ϊα σώετ ποτεςι ξεϊαχισινοστι οτ
 * τιπα τεςνιξαμα.
 */


/* χξεϋξιε πεςενεξξωε δμρ tgoto() ιϊ termcap */
char *UP = NULL;        /* πεςενεστιτψ λυςσος χχεςθ ξα 1 ποϊιγιΰ */
char *BC = NULL;        /* ---------""------- χμεχο -----""----- */

/* ςαϊνεςω όλςαξα */
int     maxli = 0;/*24;     /* ςαϊνες πο χεςτιλαμι */
int     maxco = 0;/*80;     /* --"-- πο ηοςιϊοξταμι */


/*---------------------*/
/* οπισαξιε λμαχιατυςω */
/*---------------------*/

KBF   kbf[] = {

/*      λοδ χοϊχςαύαετσρ πο ϊαπςοσυ r_key();
 *      χτοςωε όμενεξτω ϊαπομξρΰτσρ hw_set(),
 *      ιμι χςυώξυΰ δμρ οπισαξιρ εδιξστχεξξοηο
 *      δισπμερ. χ ποσμεδξεν σμυώαε νοφξο
 *      ιϊνεξιτψ λομιώεστχο λμαχιϋ, ι/ιμι ϊαξεστι
 *      λοξσταξτξωε οπισαξιρ χ ταβμιγυ kbl (σν. ξιφε).
 */
{ KBCOD('k','u'), 0 },
{ KBCOD('k','d'), 0 },
{ KBCOD('k','l'), 0 },
{ KBCOD('k','r'), 0 },
{ KBCOD('k','1'), 0 },
{ KBCOD('k','2'), 0 },
{ KBCOD('k','3'), 0 },
{ KBCOD('k','4'), 0 },
{ KBCOD('I','C'), 0 },
{ KBCOD('D','C'), 0 },
{ KBCOD('I','L'), 0 },
{ KBCOD('D','L'), 0 },
{ KBCOD('k','h'), 0 },
/*
{ KBCOD('i','c'), 0 },
{ KBCOD('i','m'), 0 },
{ KBCOD('k','A'), 0 },
{ KBCOD('k','A'), 0 },
*/
{ KBCOD('k','D'), 0 },
{ KBCOD('k','I'), 0 },
{ KBCOD('@','7'), 0 },
{ KBCOD('k','P'), 0 },
{ KBCOD('k','N'), 0 },

{ KBCOD('f','.'), 0 },
{ KBCOD('f','-'), 0 },
{ KBCOD('f',','), 0 },
{ KBCOD('f','0'), 0 },
{ KBCOD('f','1'), 0 },
{ KBCOD('f','2'), 0 },
{ KBCOD('f','3'), 0 },
{ KBCOD('f','4'), 0 },
{ KBCOD('f','5'), 0 },
{ KBCOD('f','6'), 0 },
{ KBCOD('f','7'), 0 },
{ KBCOD('f','8'), 0 },
{ KBCOD('f','9'), 0 },
{ 0, 0 },
};

/* οπισαξιε χοϊνοφξοστεκ χωχοδα ξα όλςαξ */
/*
 * ζυξλγιρ hw_set ϊανεξρετ στςολι, λοτοςωε ρχμρΰτσρ
 * λμΰώανι χοϊνοφξοστεκ τεςνιξαμα, σν. termcap(5)
 * ξα στςολι ϊξαώεξικ ότιθ λμΰώεκ.
 * δμρ οπισαξιρ εδιξστχεξξοηο τεςνιξαμα ϊανεξιτε λμΰώι ξα ϊξαώεξιρ
 * (τ.ε. ξα σοοτχετστχυΰύιε ESC-ποσμεδοχατεμψξοστι)
 * ι πονεστιτε χ ότον ζακμε οπισαξιε πυστοκ ζυξλγιι "hw_set() {}"
 */
char *tcapo[] = {
	"cm", "cl", "cd", "ce", "ks", "ke",
	"cs", "sr", "sf", "al", "dl",
	"so", "se", "us", "ue",
	"md", "mr", "mb", "mk", "me",
	"bl", "vb",
	"lh",   /* ινρ λμαχιατυςω */
	0 };

/* σχρϊψ ζιϊιώεσλιθ ι μοηιώεσλιθ λοδοχ, α ταλφε ινεξ λμαχιϋ */
KBL kbl[KBLSIZE] = {
		/*==== main set */
      { KBCTL(    'K'),   KB_KP,   "Ctrl-K " },    /* KEYPAD ON */
      { KBCOD('k','u'),   KB_AU,   "UP     " },		/* arrow keys */
      { KBCOD('k','d'),   KB_AD,   "DOWN   " },
      { KBCOD('k','l'),   KB_AL,   "LEFT   " },
      { KBCOD('k','r'),   KB_AR,   "RIGHT  " },
      { KBCTL(    'J'),   KB_NL,   "Enter  " },
      { KBCOD('d','e'),   KB_DE,   "<-del  " }, /* delete left from cursor */
      { KBCTL(    'C'),   KB_EX,   "Ctrl-C " },	/* exit */
      { KBCOD('k','1'),   KB_HE,   "F1     " }, /* help screen */
      { KBCTL(    'V'),   KB_PR,   "Ctrl-V " }, /* prefix for complex keyboard commands */
      { KBCTL(    'L'),   KB_RE,   "Ctrl-L " }, /* refresh/redraw page content on screen */
      { KBCTL(    'I'),   KB_TA,   "TAB    " },
	  { KBCTL(    'A'), KB_KH,    "Home  " },
	  { KBCTL(    'E'), KB_KE,    "End   " },
	  { KBCTL(    'B'), KB_PU,    "PgUp  " },
	  { KBCTL(    'F'), KB_PD,    "PgDown" },
	  /* extended for linlib 4 since 2017-05 */
	  /* delUnderCursor, insert, home, end, PageUp, PageDown */
      { KBCOD('k','D'),   KB_KD,   "del->  " },
	  { KBCOD('k','I'), KB_IN,    "Insert" },
	  { KBCOD('k','h'), KB_KH,    "Home  " },
	  { KBCOD('@','7'), KB_KE,    "End   " },
	  { KBCOD('k','P'), KB_PU,    "PgUp  " },
	  { KBCOD('k','N'), KB_PD,    "PgDown" },
	  /**/
      { KBCOD('k','4'),   KB_PR,   "PF4    " },
		/*==== extended set */
      { KBCOD('f','0'),   KBUSR('0'),   0         },
      { KBCOD('f','1'),   KBUSR('1'),   0         },
      { KBCOD('f','2'),   KBUSR('2'),   0         },
      { KBCOD('f','3'),   KBUSR('3'),   0         },
      { KBCOD('f','4'),   KBUSR('4'),   0         },
      { KBCOD('f','5'),   KBUSR('5'),   0         },
      { KBCOD('f','6'),   KBUSR('6'),   0         },
      { KBCOD('f','7'),   KBUSR('7'),   0         },
      { KBCOD('f','8'),   KBUSR('8'),   0         },
      { KBCOD('f','9'),   KBUSR('9'),   0         },
      { KBCOD('f','.'),   KBUSR('.'),   0         },
      { KBCOD('f','-'),   KBUSR('-'),   0         },
      { KBCOD('f',','),   KBUSR(','),   0         },
      { KBCTL(    'M'),   KB_NL,   0         },    /*==== aliases */
      { KBCTL(    'H'),   KB_DE,   0         },
      { KBCTL(    'X'),   KB_EX,   0         },
/*    { KBCOD('k','h'),   KB_EX,   0         }, */
      { KBCTL(    'R'),   KB_RE,   0         },
      { 0 },
	};


/*
 * TODO: line0.h
 *
 * ANSI color logical tables for SGR
 * note: there is no defined constant to index FGBG - it is simpy 0 (zero)
 *
 * Note: active code is in w_sgr() from lin320.c and vhset/attr.c
 *
 * now a member of LPA strcture:
 * char *acout [LPASIZE];
 * char *acinp [LPASIZE];
 *
 */
LPA lpaout[LPASIZE] = {
	{       ' ',    0,          "0"     },
	{       ' ',    0,          "37;44" },              /* TXT  */
	{       ' ',    A_SO,       "36;40" },              /* HDR  */
	{       ' ',    A_MD,       "33"    },              /* VAR  */
	{       ' ',    A_MD,       "33"    },              /* ALT  */
	{       '-',    A_MD,       "33"    },              /* MSE  */
	{       ' ',    A_US|A_MB,  "33;41" },              /* ERR  */
	{       '!',    A_SO,       "32;40" },              /* ATT  */
};

LPA lpainp[LPASIZE] = {
	{       ' ',    0,          "1;37;44", },             /* FGBG */
	{       ' ',    A_MD,       "31;49" },              /* TXT  */
	{       ' ',    A_SO,       "36;44" },              /* HDR  */
	{       '#',    A_SO,       "37;40" },              /* VAR  */
	{       '?',    A_SO,       "37;40" },              /* ALT  */
	{       '*',    A_SO,       "37;40" },              /* MSE  */
	{       ' ',    A_US|A_MD,  "36;41" },              /* ERR  */
	{       ' ',    A_US,       "37;45" },              /* ATT  */
};

int		sgrmode = 0; /* initial monochrome */
