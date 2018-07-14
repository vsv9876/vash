/*
**      +----------+    βιβμιοτελα χχοδα-χωχοδα
**     (c) linlib  !    δμρ αμζαχιτξο-γιζςοχωθ
**      +----------+    χιδεοτεςνιξαμοχ
**/

/*
 *      $Header: line0.h,v 3.4 90/01/11 10:18:16 vsv Rel $
 *
 *      $Log:	line0.h,v $
 * Revision 3.4  90/01/11  10:18:16  vsv
 * χεςσιρ V32
 * 
 * Revision 3.3  89/08/29  16:21:42  vsv
 * χεςσιρ LINLIB_3
 * 
 * Revision 3.2  88/04/27  11:10:28  vsv
 * maxli, maxco χ ζακμε line.h, ταλ βομεε υδοβξο.
 * 
 * Revision 3.1  88/04/27  08:40:08  vsv
 * πεςεδεμαξο δμρ ποδδεςφλι
 * σιστενω VISI
 * 
 */
#ifndef line0_h_def
#define line0_h_def

/*
 * οπισαξιε χοϊνοφξοστεκ τεςνιξαμα
 */

/*#define  TTY_FULL       /* πομξωε χοϊνοφξοστι TERMCAP (τομψλο δμρ RT-11) */
			/* σν. ταλφε TRMCAP.C */
/*
 * temporary buffer size for hf_set() internal usage,
 * must be conformed with BUFSIZ from termcap.c, for tgetent()...
 */
#define TBUFSZ 2048

/* οπισαξιε οδξοκ λμαχιϋι:
 */
typedef struct {
	kbcod   t_key;  /* ζιϊ. λοδ ('ku') */
	char   *t_cap;  /* ESC-λοδ (:ku=\EA:) */
} KBF;  /* ζιϊιώεσλοε - λαλ οπισαξα χ termcap */

typedef struct {
	kbcod   t_key;  /* ζιϊ. λοδ ('ku') */
	kbcod   t_cod;  /* μοη. λοδ ('AU') */
	char   *t_knm;  /* ξαϊχαξιε λμαχιϋι ("χχεςθ") */
} KBL;  /* μοηιώεσλοε - σνωσμοχοκ λοδ ι ξαϊχαξιε */
#define KBLSIZE 48

/* ποδσλαϊλι ι χιδεοατςιβυτω */
typedef struct {
	int     lpa_p;  /* ποδσλαϊλα */
	int     lpa_a;  /* σμοχο ατςιβυτοχ */
} LPA;
#define LPASIZE 8

/* τελυύεε σοστορξιε όλςαξα
 */
typedef struct {
	bool    sc_li;  /* LIne - τελυύ. στςολα */
	bool    sc_co;  /* COlumn - τελυύ. ποϊιγιρ χ στςολε */
	short   sc_at;  /* ATtributes - τελυύεε σμοχο ατςιβυτοχ */
} SCREEN;

/* χοϊνοφξοστι χωχοδα ξα τεςνιξαμ ιϊ /etc/termcap;
 * ινρ λμαχιατυςω δμρ ξαστςοκλι λμαχιϋ τοφε ϊδεσψ (lh= ... )
 */
extern char *tcapo[];

#define t_cm  tcapo[0]  /* πςρναρ αδςεσαγιρ λυςσοςα */
#define t_cl  tcapo[1]  /* στες. όλςαξ */
#define t_cd  tcapo[2]  /* λοξεγ όλςαξα */
#define t_ce  tcapo[3]  /* λοξεγ στςολι */
#define t_ks  tcapo[4]  /* χλμ/χωλμ. δοπ. λμαχ. */
#define t_ke  tcapo[5]
#define t_cs  tcapo[6]  /* πςολςυτλα */
#define t_sr  tcapo[7]
#define t_sf  tcapo[8]
#define t_al  tcapo[9]  /* χσταχ/υδαμ. στςολι */
#define t_dl  tcapo[10]
#define t_so  tcapo[11] /* χωδεμεξιε */
#define t_se  tcapo[12]
#define t_us  tcapo[13] /* ποδώεςλιχαξιε */
#define t_ue  tcapo[14]
#define t_md  tcapo[15] /* ρςλοστψ */
#define t_mr  tcapo[16] /* ςεχεςσ */
#define t_mb  tcapo[17] /* νιηαξιε */
#define t_mk  tcapo[18] /* ςανλα */
#define t_me  tcapo[19] /* χσε υβςατψ */
#define t_bl  tcapo[20] /* ϊχοξολ */
#define t_vb  tcapo[21] /* χιδεοϊχοξολ */
#define t_lh  tcapo[22] /* ινρ λμαχιατυςω (ςασϋιςεξιε LINLIB) */

#endif /* line0_h_def */
