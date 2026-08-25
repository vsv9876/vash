/*
**      +----------+    библиотека ввода-вывода
**     (c) linlib  !    для алфавитно-цифровых
**      +----------+    видеотерминалов
**/

/*
 *      $Header$
 *
 *      $Log$
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>      /* заголовок стандартной библиотеки вв/выв */
#include <signal.h>
#include "line.h"       /* файл-заголовок LINLIB */
#include "line0.h"
#include "linebp.h"
#include "vhset.h"      /* описание общих данных и процедур */

const char   *vexdir =        /* Каталог вынесенных описаний страниц */
#ifdef DEMOS2
	/* определяется через Makefile, изначально было "/usr/local/lib/visi/vhset/"; */
	VEXDIR;
#else
#ifdef RT11
	"LIN:";
#endif
#endif

static LINE mainp[]; /* defined in mainp.cv */

char    vhname[80] = "";        /* имя файла настройки */
char   *vhfile = vhname;        /* указатель для hw_set */

extern  pag_a();        /* настройка атрибутов */
extern  pag_k();        /* настройка клавиш */
extern  pag_mk();       /* настройка основных клавиш */

extern  LPA lpainp[];
extern  LPA lpaout[];

static  int ex_flg = 0; /* флаг: пора заканчивать */

/* связь физических и логических кодов, а также имен клавиш */
extern  KBL kbl[];
/* флаг: доп. клавиатура включена */
extern  int     kpadon;
extern int		sgrmode;

/*static */struct kblstd {
	KBL kbl[KBLSIZE];
	LPA lpaout[LPASIZE];
	LPA lpainp[LPASIZE];
	/* int sgrmode ; *//* never saved */
} vh_std;

static struct kblstd vh_before;

static void kbl_std()
{
	register int k;
	char *from;
	char *to;
	int psize;

	/* kblstd.kbl = kbl */
	psize = sizeof(KBL) * KBLSIZE;
	from = (char *)&(kbl[0].t_cod);
	to = (char *)&(vh_std.kbl[0].t_cod);
	for (k = 0; k < psize; k++)
		to[k] = from[k];

	/* kblstd.lpaout = lpaout;*/
	psize = sizeof(LPA) * LPASIZE ;
	from = (char *)&(lpaout[0].lpa_p);
	to   = (char *)&(vh_std.lpaout[0].lpa_p);
	for (k = 0; k < psize; k++)
		to[k] = from[k];

	/* kblstd.lpainp = lpainp;*/
	from = (char *)&(lpainp[0].lpa_p);
	to   = (char *)&(vh_std.lpainp[0].lpa_p);
	for (k = 0; k < psize; k++)
		to[k] = from[k];

}

static void kbl_save(pto)
struct kblstd *pto;
{
	register int k;
	char *from;
	char *to;
	int psize;

	/* kblstd.kbl = kbl */
	psize = sizeof(KBL) * KBLSIZE;
	from = (char *)&(kbl[0].t_cod);
	to = (char *)&(pto->kbl[0].t_cod);
	for (k = 0; k < psize; k++)
		to[k] = from[k];

	/* kblstd.lpaout = lpaout;*/
	psize = sizeof(LPA) * LPASIZE ;
	from = (char *)&(lpaout[0].lpa_p);
	to   = (char *)&(pto->lpaout[0].lpa_p);
	for (k = 0; k < psize; k++)
		to[k] = from[k];

	/* kblstd.lpainp = lpainp;*/
	from = (char *)&(lpainp[0].lpa_p);
	to   = (char *)&(pto->lpainp[0].lpa_p);
	for (k = 0; k < psize; k++)
		to[k] = from[k];
}

static void kbl_fetch(pto)
struct kblstd *pto;
{
	register int k;
	char *from;
	char *to;
	int psize;

	/* kblstd.kbl = kbl */
	psize = sizeof(KBL) * KBLSIZE;
	to   = (char *)&(kbl[0].t_cod);
	from = (char *)&(pto->kbl[0].t_cod);
	for (k = 0; k < psize; k++)
		to[k] = from[k];

	/* kblstd.lpaout = lpaout;*/
	psize = sizeof(LPA) * LPASIZE ;
	to   = (char *)&(lpaout[0].lpa_p);
	from = (char *)&(pto->lpaout[0].lpa_p);
	for (k = 0; k < psize; k++)
		to[k] = from[k];

	/* kblstd.lpainp = lpainp;*/
	to   = (char *)&(lpainp[0].lpa_p);
	from = (char *)&(pto->lpainp[0].lpa_p);
	for (k = 0; k < psize; k++)
		to[k] = from[k];
}

static int kbl_diff(pto)
struct kblstd *pto;
{
	register int k;
	char *from;
	char *to;
	int psize;

	psize = sizeof(KBL) * KBLSIZE;
	from = (char *)&(kbl[0].t_cod);
	to = (char *)&(pto->kbl[0].t_cod);
	for (k = 0; k < psize; k++) {
		if (to[k] != from[k])
			return 1;
	}
	psize = sizeof(LPA) * LPASIZE ;
	from = (char *)&(lpaout[0].lpa_p);
	to   = (char *)&(pto->lpaout[0].lpa_p);
	for (k = 0; k < psize; k++) {
		if (to[k] != from[k])
			return 1;
	}
	from = (char *)&(lpainp[0].lpa_p);
	to   = (char *)&(pto->lpainp[0].lpa_p);
	for (k = 0; k < psize; k++) {
		if (to[k] != from[k])
			return 1;
	}
	return 0;
}

static int warned;

mkcomp() {
	if (kbl_diff(&vh_before) && (warned == 0)) {
		warned = 2;
		w_msg(SEL|INP, "saved settings was changed! confirm...");
		return(FALSE);
	}
	kbl_fetch(&vh_std);
	er_pag();
	w_page(mainp);
	w_msg(SEL, "reset to the factory settings");
	return(FALSE);
}

mkundo() {
	kbl_fetch(&vh_before);
	er_pag();
	w_page(mainp);
	w_msg(SEL, "reset to the last saved");
	return(FALSE);
}

mkquit()
{
	if(kbl_diff(&vh_before) && (warned == 0)) {
		warned = 2;
		w_msg(ERR, "the settings are not saved! press againg if quit");
		return(FALSE);
	} else {
		ex_flg = 1;
	}
	return(TRUE);
}

saveon()
{
	FILE *ofp;
	register int i;
	register KBL *kblp;
	register KBL *kbls;
	register int k;
	char *s;

	if( vhname[0] && (ofp=fopen(vhname, "w")) != NULL ) {
		/*------save settings: */
		s = "%s\n";
		fprintf(ofp, s, "#");
		fprintf(ofp, s, "# vhset(1) settings");
		fprintf(ofp, s, "#");
		fprintf(ofp, "\n");

		/* additional keypad */
		fprintf(ofp, "%c", (kpadon ? '+' : '-'));
		/* color mode */
		fprintf(ofp, "%1d\n", sgrmode);
		fprintf(ofp, "\n");

		/* attributes - read/out, wright/input */
		for(i=0; i<8; i++) {
			if (	(lpaout[i].lpa_p != vh_std.lpaout[i].lpa_p) ||
					(lpaout[i].lpa_a != vh_std.lpaout[i].lpa_a) ||
					(0 != strcmp(lpaout[i].lpa_sgr, vh_std.lpaout[i].lpa_sgr))
				) {
				fprintf(ofp, "w%1d%c\t%03o\t%s\n", i,
				lpaout[i].lpa_p, lpaout[i].lpa_a,
				lpaout[i].lpa_sgr);
			}
		}
		fprintf(ofp, "\n");
		for(i=0; i<8; i++) {
			if (	(lpainp[i].lpa_p != vh_std.lpainp[i].lpa_p) ||
					(lpainp[i].lpa_a != vh_std.lpainp[i].lpa_a) ||
					(0 != strcmp(lpainp[i].lpa_sgr, vh_std.lpainp[i].lpa_sgr))
				) {
				fprintf(ofp, "r%1d%c\t%03o\t%s\n", i,
				lpainp[i].lpa_p, lpainp[i].lpa_a,
				lpainp[i].lpa_sgr);
			}
		}
		fprintf(ofp, "\n");

		/*---- key settings */
		for(kblp=kbl, kbls=vh_std.kbl; kblp->t_cod; kblp++, kbls++) {
			if (
					(kblp->t_cod != kbls->t_cod) ||
					(0 != strncmp(kblp->t_knm, kbls->t_knm, 8-1)) ||
					(kblp->t_key1 != kbls->t_key1) ||
					(kblp->t_key2 != kbls->t_key2)
			) {
				putc(':', ofp);
				putc(cod0(kblp->t_cod), ofp);
				putc(cod1(kblp->t_cod), ofp);
				putc(':', ofp);
	/*			if(kblp->t_knm) fprintf(ofp, "%s", kblp->t_knm);*/
				for (s=kblp->t_knm, i=0; i<8; i++) {
					if(s[i] != '\0')
						putc(s[i], ofp);
					else
						break;
				}
				for (; i<8; i++)
					putc(' ', ofp);
				putc(':', ofp);

				if(kblp->t_key1) {
					putc(cod0(kblp->t_key1), ofp);
					putc(cod1(kblp->t_key1), ofp);
				} else {
					putc(' ', ofp); putc(' ', ofp);
				}
				putc(':', ofp);
				if (kblp->t_key2) {
					putc(cod0(kblp->t_key2), ofp);
					putc(cod1(kblp->t_key2), ofp);
				} else {
					putc(' ', ofp); putc(' ', ofp);
				}
				putc(':', ofp);
				putc('\n', ofp);
			}
		}
		if (fclose(ofp) == 0) {
			kbl_save(&vh_before);
			w_msg(HDR, "OK, new settings are saved");
			return(TRUE);
		} else {
			w_msg(ERR, "settings are not saved, file may be corrupted...");
		}
	} else {
		w_msg(ERR, "cannot open file for save settings");
	}
	/*ex_flg = 1;*/
	return(FALSE/*TRUE*/);
}

#include "mainp.i"

static  char    helpf[] = "vhsetm.lb";

LFRAME lfmain = { 0 };

/*ARGSUSED*/
void sigwinch(signo)
int signo;
{
	if (0 != gtty_sz()) {
		return;
	}

	/* this is restriction for lfmain to use classic 24 lines */
	lfmain.maxli  =  24;
	/*lfmain.maxli  =  hwframe.maxli; TODO complete hacking a limit 24 lines*/
	lfmain.baseli = hwframe.maxli - lfmain.maxli;
	if (lfmain.baseli < 0) {
		lfmain.maxli = hwframe.maxli;
		lfmain.baseli = 0;
	}
	lfmain.maxco  = hwframe.maxco;

	lframe = &lfmain;

	if (signo)
		jkb_re();
}



/*------------*/
/* VIDEO MAIN */
/*------------*/
vmain()
{
	register char *s;
	kbcod cod ;
	LINE *cline;    /* pointer to page's current line */

	cline = mainp;  /* current line is main menu pagxnse */

	er_pag();
	sigwinch(0);
	w_page(mainp);
	signal(SIGWINCH, sigwinch);

	if( vhname[0] == 0 )
		w_emsg("setup file directory unknown, see manual");

	while( -1 ) {
		cod = r_page(mainp, &cline, 0);

		if (warned) {
			warned -= 1;
		}
		/* exit flag was raised */
		if(ex_flg) {
			cp_set(0,0,TXT); er_pag();
			return 0;
		}

		switch(cod) {
#if 0
		case KB_EX:
			cline = mainp; /* back to 1st INP line */
			break;
#endif
		case '0' :
			/* basic keys setup, now hidden from 'mainp' page */
			pag_mk();
			/*NO BREAK*/
		case ' ' :
		case KB_NL:
			er_pag();
			w_page(mainp);  /* refresh screen after submenu */
			break;
		
		case '?' :
		case KB_HE:
			w_help((LINE *)helpf);
			er_pag();
			w_page(mainp);
			break;
		}
	}
}

#ifdef RT11
/* $$narg = 1 ;            /* не выдавать подсказку на ввод аргументов */
#endif

main()
{
	int i;
#define VHSET_DEBUG
#ifdef VHSET_DEBUG
	int ch;
	char *s;
	if ((s = getenv("VHSET_DEBUG")) != NULL) {
		fprintf(stdout, "--> ready to debug, Please, press <Enter> to continue ");
		fscanf(stdin, "%c", &ch);
	}
#endif
    if ((s = getenv("VHSET_LIB")) != (char *)0) vexdir = s;

    visini();

    /* specific for this utility -
     * save statically compiled constants before do_kbl()
     * because hw_set() will overwrite them */
    /*kbl_std();*/
    kbl_save(&vh_std);

    hw_set();

    /* save tuned settings before editing them */
    kbl_save(&vh_before);

	lfmain.maxli  =  24;
	/* lfmain.baseli = -24; */
	lfmain.baseli = hwframe.maxli - lfmain.maxli;
	/*lfmain.baseco = 0;*/
	/* correct below maxsize */
	if (lfmain.baseli < 0) {
		lfmain.maxli = hwframe.maxli;
		lfmain.baseli = 0;
	}
	lfmain.maxco  = hwframe.maxco;
	lframe = &lfmain;

	/* keep content of a terminal emulator's screen which area is bigger then standard 24x80 */
	for (i = 1; i < lframe->maxli; i++) {
		putc('\n', stdout);
	}

    io_set(IO_VIDEO);

    vmain();

    er_eop(0);
    io_set(IO_TTYPE);
    printf("\n");
    exit(0);
}
