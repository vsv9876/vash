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
#include "line.h"       /* файл-заголовок LINLIB */
#include "line0.h"
#include "linebp.h"
#include "vhset.h"      /* описание общих данных и процедур */

char   *vexdir =        /* Каталог вынесенных описаний страниц */
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
	er_pag();
	cp_set(0, 0, ATT);
	printf("%s", "New setup is lost");
	cp_set(2, 0, CMD);
	ex_flg = 1;
	return(TRUE);
}

/* связь физических и логических кодов, а также имен клавиш */
extern  KBL kbl[];
/* флаг: доп. клавиатура включена */
extern  int     kpadon;
extern int		sgrmode;


mkexit()
{
	FILE *ofp;
	register int i;
	register KBL *kblp;

	if( namelh[0] && (ofp=fopen(namelh, "w")) != NULL ) {
		/*------сохраняем настройку: */

		/*---- атрибуты */
		for(i=0; i<8; i++) {
			fprintf(ofp, "%1d%c%03o%c%03o\t%s\t%s\n", i,
			lpaout[i].lpa_p, lpaout[i].lpa_a,
			lpainp[i].lpa_p, lpainp[i].lpa_a,
			lpaout[i].lpa_sgr,
			lpainp[i].lpa_sgr);
		}

		/*---- доп. клавиатура. TODO: color mode index: 0==BW, 1==1st_color_set -- not yet implemented */
		fprintf(ofp, "%c", (kpadon ? '+' : '-'));
		fprintf(ofp, "%1d\n", sgrmode);

		/*---- клавиши */
		for(kblp=kbl; kblp->t_cod; kblp++) {
			putc(':', ofp);
			putc(cod0(kblp->t_key), ofp);
			putc(cod1(kblp->t_key), ofp);
			putc(cod0(kblp->t_cod), ofp);
			putc(cod1(kblp->t_cod), ofp);
			if(kblp->t_knm) fprintf(ofp, "%s", kblp->t_knm);
			putc('\n', ofp);
		}
		cp_set(-1, 0, CMD);
	} else {
		er_pag();
		cp_set(2, 0, ERR); fflush(vttout);
		printf("%s", "cannot save file, setup data lost"); fflush(stdout);
	}
	ex_flg = 1;
	return(TRUE);
}

#include "mainp.i"

static  char    helpf[] = "vhsetm.lb";

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

	if( namelh[0] == 0 )
		w_emsg("setup file directory unknown, see manual");

	w_page(mainm);

	while( -1 ) {
		cod = r_page(mainm, &cline, 0);

		/* exit flag was raised */
		if(ex_flg) return 0;

		switch(cod) {
		    case '0' :
			pag_mk(); /* basic keys setup, now hidden from menu */
		    case '?' :
		    case ' ' :
			er_pag();
			w_page(mainm);  /* refresh screen after submenu */
			break;
		
		    case KB_HE:
			w_help(helpf);
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
    visini();
    hw_set();
    io_set(IO_VIDEO);

    vmain();

    er_eop(0);
    io_set(IO_TTYPE);
    printf("\n");
    exit(0);
}
