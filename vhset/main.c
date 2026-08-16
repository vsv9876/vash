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

char    namelh[80] = "";        /* имя файла настройки */
char   *filelh = namelh;        /* указатель для hw_set */

extern  pag_a();        /* настройка атрибутов */
extern  pag_k();        /* настройка клавиш */
extern  pag_mk();       /* настройка основных клавиш */

extern  LPA lpainp[];
extern  LPA lpaout[];

static  int ex_flg = 0; /* флаг: пора заканчивать */

mkquit()
{
/*
	er_pag();
	cp_set(0, 0, ATT);
	printf("%s", "New setup is lost");
	cp_set(2, 0, CMD);
*/
	ex_flg = 1;
	return(TRUE);
}

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
} kblstd;

static int kbl_std()
{
	register int k;
	char *from;
	char *to;
	int psize;

	/* kblstd.kbl = kbl */
	psize = sizeof(KBL) * KBLSIZE;
	from = (char *)&(kbl[0].t_cod);
	to = (char *)&(kblstd.kbl[0].t_cod);
	for (k = 0; k < psize; k++)
		to[k] = from[k];

	/* kblstd.lpaout = lpaout;*/
	psize = sizeof(LPA) * LPASIZE ;
	from = (char *)&(lpaout[0].lpa_p);
	to   = (char *)&(kblstd.lpaout[0].lpa_p);
	for (k = 0; k < psize; k++)
		to[k] = from[k];

	/* kblstd.lpainp = lpainp;*/
	from = (char *)&(lpainp[0].lpa_p);
	to   = (char *)&(kblstd.lpainp[0].lpa_p);
	for (k = 0; k < psize; k++)
		to[k] = from[k];

}

saveon()
{
	FILE *ofp;
	register int i;
	register KBL *kblp;
	register KBL *kbls;
	register int k;
	char *s;

	if( namelh[0] && (ofp=fopen(namelh, "w")) != NULL ) {
		/*------save settings: */

		/* additional keypad */
		fprintf(ofp, "%c", (kpadon ? '+' : '-'));
		/* color mode */
		fprintf(ofp, "%1d\n", sgrmode);

		/* attributes */
		for(i=0; i<8; i++) {
			if (	(lpaout[i].lpa_p != kblstd.lpaout[i].lpa_p) ||
					(lpainp[i].lpa_p != kblstd.lpainp[i].lpa_p) ||
					(lpaout[i].lpa_a != kblstd.lpaout[i].lpa_a) ||
					(lpainp[i].lpa_a != kblstd.lpainp[i].lpa_a) ||
					(0 != strcmp(lpaout[i].lpa_sgr, kblstd.lpaout[i].lpa_sgr)) ||
					(0 != strcmp(lpainp[i].lpa_sgr, kblstd.lpainp[i].lpa_sgr))
				) {
				fprintf(ofp, "%1d%c%03o%c%03o\t%s\t%s\n", i,
				lpaout[i].lpa_p, lpaout[i].lpa_a,
				lpainp[i].lpa_p, lpainp[i].lpa_a,
				lpaout[i].lpa_sgr,
				lpainp[i].lpa_sgr);
			}
		}

		/*---- key settings */
		for(kblp=kbl, kbls=kblstd.kbl; kblp->t_cod; kblp++, kbls++) {
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
			w_msg(HDR, "OK, new settings are saved");

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

	cline = mainm;  /* current line is main menu pagxnse */

	er_pag();
	sigwinch(0);
	w_page(mainm);
	signal(SIGWINCH, sigwinch);

	if( namelh[0] == 0 )
		w_emsg("setup file directory unknown, see manual");

	while( -1 ) {
		cod = r_page(mainm, &cline, 0);

		/* exit flag was raised */
		if(ex_flg) {
			cp_set(0,0,TXT); er_pag();
			return 0;
		}

		switch(cod) {
		case '0' :
			/* basic keys setup, now hidden from menu */
			pag_mk();
			/*NO BREAK*/
		case '?' :
		case ' ' :
		case KB_NL:
			er_pag();
			w_page(mainm);  /* refresh screen after submenu */
			break;
		
		case KB_HE:
			w_help((LINE *)helpf);
			er_pag();
			w_page(mainm);
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

    /* specific for this utility - save statically compiled constants before do_kbl() */
    kbl_std();

    hw_set();

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
