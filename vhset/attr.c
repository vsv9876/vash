/*
**      +----------+    БИБЛИОТЕКА ВВОДА-ВЫВОДА
**     (c) linlib  !    ДЛЯ АЛФАВИТНО-ЦИФРОВЫХ
**      +----------+    ВИДЕОТЕРМИНАЛОВ
**/

#include <stdio.h>
#include "line.h"
#include "line0.h"
#include "lineva.h"

extern LPA lpainp[];
extern LPA lpaout[];

extern int cvt_co(); /* color */
extern int cvt_cb(); /* bright mode */

extern char namelh[];

int     wamask[10] = {
	A_SO,   A_US,   A_VS,   A_MD,	A_MR,   A_MB,   A_MK,	A_ZH,      0,      0,
	};

int     lpa_pi = 0;     /* РЕЖИМ ИЗМЕНЕНИЯ АТРИБУТОВ (НА ВВОДЕ/НА ВЫВОДЕ) */
LPA     *lpa_p[2] = {
	lpaout,         lpainp
	};

static char *pimmsg[] = {
	"/set \"w\"/ ->  ",
	"/set \"r\"/ --->",
	"               " /* string for wipe on screen */
};

/* Color support for attr.cv page */
char   *sgrms[] = {
		"#0-dumb       ",
		"#1-mono (b/w) ",
		"#2-color      ",
		"#3-color + bw ",
/*		"#4-color256   ",*/
/*		"#5-color256+bw",*/
		0 };
extern int		sgrmode; /*global LINLIB mode*/

extern  LINE linem[];
LINE *linesgr = (LINE *)0;

/*char *sgra = "";*/
char *sgra = lpaout[0].lpa_sgr; /* SGR code, common (global) pointer, between cvt_sgr, cvt_co, cvt_csel */

char sgrats[20] = ""; /* SGR attrib temporary string */
extern int cvt_sg();

LINE *getlsgr(line, varl)
register LINE *line;
char *varl;
/*вернуть указатель на линию редактирования атрибутов цвета (SGR editor) */
{
	register LINE *lsgr;

	if (linesgr != (LINE *)0)
		return (linesgr);
	for (lsgr=linem; lsgr->size != 0; lsgr++) {
		if (lsgr->varl != varl) /* || lsgr->cvtf != cvt_sg)*/
			continue;
		linesgr = lsgr;
		return(lsgr);
	}
	return( (LINE *)(0) );
}

ref_co(line, cod)
register LINE *line;
kbcod   cod;
/* refresh lines with fg/bg direct togglers */
{
	register LINE *l;

	for (l=linem; l->size != 0; l++) {
		if (l->cvtf != cvt_co)
			continue;
		if (l->colu != line->colu)
			continue;
		w_line(l);
	}
	return( (LINE *)(0) );
}

cvt_atr(line, cod, mod, str)
/*---------------------------------*/
/* формат для изображения лог.типа */
/*---------------------------------*/
LINE   *line;
kbcod   cod;
char   *mod;
char   *str;
{
	int attr;

	if(*mod == 'w') {
		attr = line->attr & VIDEO;
		strcpy(str, line->varl);

		/* couple of lines shown with ofset between them */
		cp_set(line->line + 1, line->colu/* + 2*/, attr|INP);

		/* emulate prompt behavior for input mode - find prompt symbol, show it at 1st position */
		w_chr((char)(lpainp[attr].lpa_p));

		w_str(line->varl);
/*
		if (attr & TXT)
			repage();
*/
	}
	return(TRUE);
}

LINE *getl4(line)
/*-------------------------------------------------------*/
/* вернуть указатель на базовую линию в 4й строке экрана */
/*-------------------------------------------------------*/
register LINE *line;
{
	register LINE *l;

	for (l=linem; l->size != 0; l++) {
/*		if (l->line != 4)*/
		if (l->cvtf != cvt_atr) /* independent of screen coordinates, more flexible */
			continue;
		if (l->colu == line->colu)
			return( l );
	}
	return( (LINE *)(0) );
}

/* копипаста из cvt_va... */
cvt_sgr(line, cod, mod, str)
/*------------------*/
/* формат для цвета */
/*------------------*/
LINE *line;
kbcod cod;
char *mod;
char *str;
{
	char outstr[20];	/* строка для формирования вывода */
	int i;
	int posp;
	register LINE *line4;	/* указатель на базовую линию в 4-й строке */
	register LPA *lpap;
	char *sgr_v;

	i = (int) line->varl;
	if (*mod == 'w') {
		if (sgrmode >= 2) {
			/*strcpy(outstr, ". .");*/
			if (lpa_pi) {
				strcpy(outstr, "  .");
			} else {
				strcpy(outstr, ".  ");
			}
			strcpy(str, outstr);
		} else {
			strcpy(str, "   ");
		}
	} else {
		strcpy(outstr, ".?.");
		strcpy(str, outstr);
		if (sgrmode < 2) return(TRUE);

		if (cod == ' ' || cod == KB_DE) {
			line4 = getl4(line); /* тут будет показан результат */
			/*выбрать, где будет настроен результат */
			switch (lpa_pi) {
			case 0:
				outstr[0] = '#'; /*lpap = lpa_p[0];*/ /*'%';*/
				sgr_v = &lpaout[i].lpa_sgr[0];
				break;
			case 1:
				outstr[2] = '#'; /*lpap = lpa_p[1];*/
				sgr_v = &lpainp[i].lpa_sgr[0];
				break;
			}
			if (sgr_v != (char*) 0) {

				sgra = sgr_v;
				/*w_line(line);*/
				cp_set(line->line, line->colu, line->attr); /* no PMT, if yes: colu + 1*/
				w_str(outstr);
				if (!sgr_csel(line, cod, sgra))
					return(FALSE);
			}
			w_line(line4); /* вызов перенесен в sgr_ed(), но теперь на месте, здесь */
			ref_co(line, cod);
			w_line(linesgr);
			/*if (i == TXT)*/
				repage();
		} else {
			if (linesgr != NULL) {
				sgrats[0] = 0;
				w_line(linesgr);
			}
		}
	}
	return (TRUE);
}

static char fgbg[]     =    "012345679";
static char fgbg_cod[] = "9- 01234567";
int showcs(fgbg, cp, ci)
/* show color strip */
char *fgbg; /* clear show area if NULL */
char cp;	/* color pointer */
int ci;    /* stripe color mode index 0 - for foreground, 2 - for background*/
{
	int i;	char *s;
	char gp[2] = { '0', '7' }; /* black and white background for color sample strip */
	char iFG[4] = { '3', '4', '4', '3' };

	for (i=0; i<2; i++) {
		cp_set(linesgr->line + i /* +1 */, linesgr->colu/* - 4 */, ERR);
		if (fgbg != NULL) {
			w_raw("\033[0m");
			for (s = fgbg; *s != '\0'; s++) {
				w_raw("\033[");
				w_putc(iFG[ci]);
				w_putc(gp[i]);
				w_putc(';');
				w_putc(iFG[ci + 1]);
				w_putc(*s);
				w_putc('m');
				if (*s != cp)	{ w_chr(' '); }
				else			{ w_chr('>'); }
								  w_chr(*s);
				if (*s != cp)	{ w_chr(' '); }
				else 			{ w_chr('<'); }
			}
		}
		er_eol(TXT);
	}
}

/* decode old SGR string, assumed format like 34;44 or 44;34 only */
sgr_decode(sgra, fgp, bgp)
char *sgra;	/* string with SGR attributes to be scaned */
char *fgp;
char *bgp;
{
	int ic, cptrok;
	char *s;
	char *cptr;
	char c;

	cptrok = 0;
	ic = 0;
	*fgp = '\0';
	*bgp = '\0';
	s = sgra;
	while (*s != '\0') {
		c = *s;
		if (cptrok) {
			if (strchr(fgbg, c) != NULL) *cptr = c;
			cptrok = 0;
		} else {
			switch(c) {
			case	'3': cptrok = 1; cptr = fgp; break;
			case	'4': cptrok = 1; cptr = bgp; break;
			case	';': cptrok = 0; break;
			default:	cptrok = 0; break; /* illegal, error detected... TODO complete*/
			}
		}
		s++;
	}
}

/*
 * format SGR sctring with new parameters
 */
sgr_encode(s, fgp, bgp)
char *s; /* output string */
char *fgp;
char *bgp;
{
	char fg, bg;
	fg = *fgp;
	bg = *bgp;

	if (fg != '\0' && bg != '\0') {
		sprintf(s, "3%c;4%c", fg, bg);
	} else if (fg != '\0') {
		s[0] = '3'; s[1] = fg; s[2] = '\0';
	} else if (bg != '\0') {
		s[0] = '4'; s[1] = bg; s[2] = '\0';
	} else if (fg == '\0' && bg == '\0') {
		s[0] = '\0';
	}
}

cvt_cb(line, cod, mod, str)
LINE   *line;
kbcod   cod;
char   *mod;
char   *str;
{
	return (TRUE);
}




cvt_co(line, cod, mod, str)
/*---------------------*/
/* формат для атрибута */
/*---------------------*/
LINE   *line;
kbcod   cod;
char   *mod;
char   *str;
{
	char    outstr[20];      /* строка для формирования вывода */
	int     lpax;			/* index for lpa[] */
	char    fbx;			/* row on sceen page: 'fg' or 'bg' 1st char significant only, see attr.cv */
	int		gv;
	int		i;
	register LINE *line4;   /* указатель на базовую линию в 5-й строке */
	register LPA *lpap;

	char *Wsgr;
	char *Rsgr;
	char *p;
	char Wfg, Wbg, Rfg, Rbg;
	Wfg = Wbg = Rfg = Rbg = 0;

	fbx = line->cvts[0];
	lpax = (int)line->varl; /* lpa*[] index*/
	if (!(fbx == 'f' || fbx == 'b')) {
		w_msg(ERR, "internal, cvt_co(): line.cvts has wrong value");
		return(FALSE);
	}
	Wsgr = &lpaout[lpax].lpa_sgr[0];
	sgr_decode(Wsgr, &Wfg, &Wbg);

	Rsgr = &lpainp[lpax].lpa_sgr[0];
	sgr_decode(Rsgr, &Rfg, &Rbg);

	if(*mod == 'r' && sgrmode >= 2) {
		if(cod == ' ' || cod == KB_DE) {
			line4 = getl4(line);

			/* select what to decode for further modification */
			switch (lpa_pi) {
			case 0:
				if (fbx == 'f') p = &Wfg; else p = &Wbg;
				break;
			case 1:
				if (fbx == 'f') p = &Rfg; else p = &Rbg;
				break;
			}

			/* do sircle around selection of SGR code number (fgbg[] array) */
			gv = *p;
			if (cod == ' ') {
				switch(gv) {
				case '0':
				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':	gv = gv + 1; break; /* next code in ASCII table */
				case '7':	gv = '9'; break;
				case '9':	gv = 0;   break;
				case 0:     gv = '0'; break;
				}
			}
			if (cod == KB_DE) {
				switch(gv) {
				case '0':	gv = 0; break;
				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
				case '7':	gv = gv - 1; break; /* previouse code in ASCII table */
				case '9':	gv = '7'; break;
				case 0:     gv = '9'; break;
				}
			}
			*p = (char)gv;

			/* prepare and store new result of SGR */
			switch (lpa_pi) {
			case 0:
				sgr_encode(Wsgr, &Wfg, &Wbg);
				break;
			case 1:
				sgr_encode(Rsgr, &Rfg, &Rbg);
				break;
			}

			strcpy(outstr, " * *");
			if (fbx == 'f') {
				if (Wfg) outstr[1] = Wfg;
				if (Rfg) outstr[3] = Rfg;
			}
			if (fbx == 'b') {
				if (Wbg) outstr[1] = Wbg;
				if (Rbg) outstr[3] = Rbg;
			}

			/* hint in case of TXT attribute - affected all screen view */
			/*if (lpax == 1)*/
				sgrtst(line, KB_NL); /*cod);*//*may be better to refresh all the page*/
			/*else
				w_line(line4);*/
		}
	}
	if(*mod == 'w') {
		if (sgrmode >= 2) {
			strcpy(outstr, "- -");

			if (fbx == 'f') {
				if (Wfg) outstr[0] = Wfg;
				if (Rfg) outstr[2] = Rfg;
			}
			if (fbx == 'b') {
				if (Wbg) outstr[0] = Wbg;
				if (Rbg) outstr[2] = Rbg;
			}
		} else {
			strcpy(outstr, "   ");
		}
	}
	strcpy(str, outstr);
	return(TRUE);
}

sgr_csel(line, cod, out)
/*редактор атрибутов цвета (SGR composer-selector)*/
LINE *line;
kbcod cod;
char *out;
{
	kbcod ed_cod;
	register LINE *line4;	/* указатель на базовую линию в 4-й строке */
	int posp;

	char c;
	char fg = 0;	/* scan SGR indexes - BG, FG, and iterators */
	char bg = 0;
	int i;

	sgr_decode(sgra, &fg, &bg);

	if ((getlsgr(line, sgrats)) == (LINE *)0) {
		return(FALSE);
	}
	/* сhеck before edit, prepare for this a line */
	strcpy(sgrats, sgra);
	w_line(linesgr);

	/* show foreground samples colors */
	showcs(fgbg, fg, 2);
	c = ed_cod = r_cod(0);
	if (ed_cod == KB_CA) return(TRUE);
	if (strchr(fgbg_cod, c) == NULL) {
		w_msg(ATT|INP, "please, use key from list:"); at_set(ATT|VEXT); w_str(fgbg_cod);
		return(FALSE);
	}
	w_msg(TXT, "");
	if (c == '-') fg = '\0'; /* clear color */
	else if (c != ' ') fg = c; /* remain the same color */
	showcs(NULL, 0, 0);

	/* show background sample colors */
	showcs(fgbg, bg, 0);
	c = ed_cod = r_cod(0);
	if (ed_cod == KB_CA) return(TRUE);
	if (strchr(fgbg_cod, c) == NULL)
		return(FALSE);
	if (c == '-') bg = '\0';
	else if (strchr(fgbg, c) != NULL) bg = c;
	showcs(NULL, 0, 0);

	/* prepare new result of SGR */
	if (fg && bg) {
		sprintf(sgrats, "3%c;4%c", fg, bg);
	} else if (fg) {
		sgrats[0] = '3'; sgrats[1] = fg; sgrats[2] = '\0';
	} else if (bg) {
		sgrats[0] = '4'; sgrats[1] = bg; sgrats[2] = '\0';
	} else {
		sgrats[0] = '\0';
	}

	/* store new SGR (no check for changes done) */
	strcpy(out, sgrats);
	return(TRUE);
}

sgr_ed(line, cod)
/*редактор атрибутов цвета (SGR composer)*/
LINE *line;
kbcod cod;
{
	kbcod ed_cod;
	register LINE *line4;	/* указатель на базовую линию в 4-й строке */
	int posp;
/*	if (cod == ' ' || cod == KB_DE) {*/
	if ((/*linesgr=*/getlsgr(line, &sgra)) != (LINE *)0) {
		linesgr->attr |= INP;
		posp = 0;
		w_line(linesgr, &posp);
		ed_cod = r_line(linesgr, &posp);
		linesgr->attr &= (~INP);

		line4 = getl4(line); /* тут будет показан результат с учетом новых параметров SGR */
		w_line(line4);
	}
/*	}*/
	return(TRUE);
}

cvt_va(line, cod, mod, str)
/*---------------------*/
/* формат для атрибута */
/*---------------------*/
LINE   *line;
kbcod   cod;
char   *mod;
char   *str;
{
	char    outstr[6];      /* строка для формирования вывода */
	int     i;
	int     va;             /* видеоатрибуты ( флаги ) */
	register int *ap;       /* указатель на атрибут */
	register LINE *line4;   /* указатель на базовую линию в 5-й строке */
	register LPA *lpap;
	
	char *smask;

	va = *(int *)line->cvts;
	i = (int)line->varl;

	if(*mod == 'w') {
		if (sgrmode & 1) {
			/*strcpy(outstr, ". ."); /* on PMT spec: ". ." + outstr indexes 0,2 */
			if (lpa_pi) {
				strcpy(outstr, "  :");
			} else {
				strcpy(outstr, ":  ");
			}
			if(lpainp[i].lpa_a & va) outstr[2] = 'x';
			if(lpaout[i].lpa_a & va) outstr[0] = 'x';
			strcpy(str, outstr);
		} else {
		    strcpy(str, "   "); /*blank is default*/
		}
	} else {
	    if (sgrmode & 1) {
		if(cod == ' ' || cod == KB_DE) {
			line4 = getl4(line);

			switch(lpa_pi) {
			case 0:
				lpap = lpa_p[0];
				break;
			case 1:
				lpap = lpa_p[1];
				break;
			}
			ap = &(lpap[ i ].lpa_a);

			/* do toggle modification */
			if((*ap) & va) { (*ap) = (*ap) & (~va); }
			else           { (*ap) = (*ap) | ( va); }

			w_line(line4);
			/*if (i == TXT)/* && cod == ' ')*/
				repage();
		}
	    }
	}
	return(TRUE);
}

#if 0
cvt_sg(line, cod, mod, str) /*TODO*/ /*основа - копипаста из cvt_sp*/
/*---------------------*/
/* формат для атрибута */
/*---------------------*/
LINE   *line;
kbcod   cod;
char   *mod;
char   *str;
{
	register char **spp;
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
		}
		else    {
			/*str[0] = '\0';*/  /* TODO: make hidden line */
			strncpy(str, *spp, size);
			if (strlen(*spp) >= size) str[size] = '\0';
		}
	}
	return(TRUE);
}
#endif

cvt_pmt(line, cod, mod, str)
/*----------------------*/
/* формат для подсказки */
/*----------------------*/
LINE   *line;
kbcod   cod;
char   *mod;
char   *str;
{
	int     i;
	kbcod tmpcod;

	i = (int)line->varl;

	if(*mod == 'w') {
		sprintf(str, "%c %c",
		lpaout[i].lpa_p, lpainp[i].lpa_p);
	} else {
		if (cod == ' ' || cod == KB_DE) {
			w_msg(ATT, "Please, type a prompt symbol");
			if (lpa_pi)
				w_str(" on input: ");
			else
				w_str(" on output: ");

			tmpcod = r_key();
			if (tmpcod == 0 || cod1(tmpcod) != 0)
				w_msg(ERR, "Control key pressed");
			else    {
				if(lpa_pi) {    /* подсказка для вывода */
					lpainp[i].lpa_p = cod0(tmpcod);
				} else {        /* для ввода */
					lpaout[i].lpa_p = cod0(tmpcod);
				}
				w_msg(TXT, "");
			}
			w_line(getl4(line));
		}
	}
	return(TRUE);
}

extern  int     cvt_hl();
extern  int     cvt_s();
extern  int     sgrtst();
extern  int     cvt_pi();
extern  int     cvt_pim();

/*
 * LINE linem[] **************************************************************
 */
#include "attr.i"

cvt_pi(line, cod, mod, str)
LINE   *line;
kbcod   cod;
char   *mod;
char   *str;
{
	char outs[30];
	char *cvts;
	size_t size = line->size;
	cvts = line->cvts;

	outs[0] = '\0';
	if (mod[0] == 'w') {
		/*strcpy(outs, pimode[lpa_pi]);*/
		strcpy(outs, "       ");
		if(lpa_pi == 0) {
			outs[0] = cvts[0];
			outs[2] = cvts[1];
		}
		else {
			outs[0] = cvts[1];
			outs[2] = cvts[0];
		}
	}
	strcpy(str, outs);
	return(TRUE);
}

cvt_pim(line, cod, mod, str)
LINE   *line;
kbcod   cod;
char   *mod;
char   *str;
{
	char outs[30];
	int xactive;

	size_t size = line->size;
	xactive = line->cvts[0] - '0'; /*iconv the special*/

	outs[0] = '\0';
	if (mod[0] == 'w') {
		if (lpa_pi == xactive) {
			strcpy(outs, pimmsg[lpa_pi]);
		} else {
			strcpy(outs, pimmsg[2]);
		}
	}
	strcpy(str, outs);
	return(TRUE);
}

show_pi()
/* alternate mode: Input/Output; called from uspage() below */
{
	LINE *l;

	for(l=linem; l->size > 0; l++) {
		if (l->cvtf == cvt_pi || l->cvtf == cvt_pim ||
			l->cvtf == cvt_va || l->cvtf == cvt_sgr) {
			w_line(l);			/*break;  multiply lines there */
		}
	}
}

repage()
{
	sgrtst(linem, KB_NL);
}

sgrtst(line, cod)
LINE *line;
kbcod cod;
{
	LINE *l;
	
	/*if (sgrmode == 0)*/
	w_raw("\033[m"); /*hint for attributes on dumb mode*/
	switch(cod) {
	case ('0'):
	case ('1'):
	case ('2'):
	case ('3'):
	case ('4'):
	case ('5'):
	case(' '):
	case(KB_DE):

	case(KB_NL):
		cp_set(0,0,CMD); er_eop(CMD);
		/*er_pag();*/
		/* find 1st line with HDR type wide of screen */
		for (l=linem; l->size != 0; l++) {
			if (l->colu <= 16 && l->attr & (VIDEO & HDR)) {
				cp_set(l->line, 0/*l->colu*/, TXT);
				er_eop(TXT);
				break;
			}
		}
		w_page(linem);
		break;
	}
	return(TRUE);
}

static  char    helpf[] = "vhseta.lb";

pag_a()
/*---------------------*/
/* настройка атрибутов */
/*---------------------*/
{
	uspage(linem, helpf);
	return(TRUE);
}

#ifdef DURA
kbcod
n_page(line_e, page, posp)
/*-----------------*/
/* ЧИТАТЬ СТРАНИЦУ */
/*-----------------*/
LINE    *line_e;                /* СТРАНИЦА ДЛЯ РЕДАКТИРОВАНИЯ  */
LINE   **page;                  /* ТЕКУЩАЯ ЛИНИЯ (СТАТУС)       */
int    *posp;                   /* ПОЗИЦИЯ КУРСОРА ПРИ РЕДАКТИРОВАНИИ */
{
	int     cod;            /* КОД, ВОЗВРАЩАЕМЫЙ r_line()   */

	register LINE *lni;             /* УКАЗАТЕЛЬ НА ТЕКУЩУЮ ЛИНИЮ */
	register LINE *line ;           /* УКАЗАТЕЛЬ НА ВСЮ СТРАНИЦУ */

	line = line_e;
	if(*page != (LINE *)NULL)
		lni = *page;
	else
		lni = line_e;

	/* ПРОПУСТИТЬ ТО, ЧТО НЕЛЬЗЯ РЕДАКТИРОВАТЬ */
	while( INP & ~(lni->attr)) {
		lni++;
		/* ЗАЦИКЛИТЬСЯ ЧЕРЕЗ НАЧАЛО */
		if(lni->size == 0)
			lni = line_e;
	}

	cod = r_line(lni, posp);

	/* НАЙТИ УКАЗАТЕЛЬ ДЛЯ СЛЕДУЮЩЕГО ЧТЕНИЯ */
	switch( cod ) {
	case KB_AR :
		if ( (lni->flag & SUSR) == FALSE )
			lni = fnd_ar(lni, line) ;
		break ;
	case KB_AL :
		if ( (lni->flag & SUSL) == FALSE )
			lni = fnd_al(lni, line) ;
		break ;
	case KB_AU :
		if ( (lni->flag & SUSU) == FALSE )
			lni = fnd_au(lni, line) ;
		break ;
	case KB_AD :
		if ( (lni->flag & SUSD) == FALSE )
			lni = fnd_ad(lni, line) ;
		break ;
	case KB_NL :
		if ( (lni->flag & SUSNL) == FALSE )
			lni = fnd_nxt(lni, line) ;
		break ;
	case KB_RE :
		/*
		er_pag();
		w_page(line, 0);
*/
		repage();
		break ;
	default :
		break;
	}

	*page = lni;    /* keep pointer to current line!... */
	return(cod);
}
#endif

uspage(page, phline)
/*-----------------------------*/
/* special for vhset
/*-----------------------------*/
LINE *page;
LINE *phline;           /* pointer to instant page with help screen */
{
    kbcod   cod;
    LINE *cline;

    cline = page;
    /*er_pag();*/
    /*w_page(page);*/
    repage();

    while ( -1 ) {
/*		cod = n_page( page, &cline, 0);*/
		cod = r_page( page, &cline, 0);
		switch ( cod ) {
		case '0':
		case '1':
		case '2':
		case '3':
			sgrmode = cod - '0'; repage();
			break;
		case 'o':
		case 'w':
			lpa_pi = 0; show_pi();
			break;
		case 'i':
		case 'r':
			lpa_pi = 1; show_pi();
			break;
		case KB_KI:
			lpa_pi = lpa_pi ? 0 : 1; show_pi();
			break;
		case ' ':
			/* refresh after menu *//* there no menu lines on this page */
			if((cline->attr & LMSE) == LMSE) {
				/*er_pag();*/
				w_page(page);
			}
			break;
		case KB_EX :
			/*NOBREAK*/
			return;
		case '?':
		case KB_HE:
			w_help(phline);
			/*w_page(page);*/
			repage();
			break;
		default:   w_emsg("");     /* clear message string */
		}
    }
}
