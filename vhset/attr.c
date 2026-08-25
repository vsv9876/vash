/*
**      +----------+    БИБЛИОТЕКА ВВОДА-ВЫВОДА
**     (c) linlib  !    ДЛЯ АЛФАВИТНО-ЦИФРОВЫХ
**      +----------+    ВИДЕОТЕРМИНАЛОВ
**/

#include <stdio.h>
#include "line.h"
#include "line0.h"
#include "lineva.h"

extern int sgr_csel(LINE *, kbcod, char *);

static LINE *attrpg;	/* global reference for partial refresh routines */

extern LPA lpainp[];
extern LPA lpaout[];

extern int cvt_co(); /* color */
extern int cvt_cb(); /* bright mode */

int     wamask[10] = {
	A_SO,   A_US,   A_VS,   A_MD,	A_MR,   A_MB,   A_MH/*A_MK*/,	A_ZH,      0,      0,
	};

int     lpa_pi = 0;     /* РЕЖИМ ИЗМЕНЕНИЯ АТРИБУТОВ (НА ВВОДЕ/НА ВЫВОДЕ) */
LPA     *lpa_p[2] = {
	lpaout,         lpainp
	};

#if 0
static const char *pimmsg[] = {
	"/set \"w\"/ ->  ",
	"/set \"r\"/ --->",
	"                " /* string for wipe on screen */
};
#else
static const char *pimmsg[] = {
	".*...",
	"...*.",
	"     " /* string for wipe on screen */
};
#endif

/* Color support description TODO move to linlib */
const char   *sgrms[] = {
		"#0 dumb (prompter)",
		"#1 monochrome (b&w)  ",
		"#2 color (16+16)     ",
		"#3 color + b&w       ",
/*		"#4-color256   ",*/
/*		"#5-color256+bw",*/
		0 };
extern int		sgrmode; /*global LINLIB mode*/

LINE linem[];

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
	LINE *lsgr;

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
	return( 0 );
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

		/* couple of lines will shown with ofset between them */
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
	case (' '):
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


void repage()
{
	sgrtst(linem, KB_NL);
}
/*
 * refresh (drow from scratch) any lines with
 * attribute pointed by vai;
 * keep in mind value of lpa_pi in runtime and that TXT is the base for each other
 */
void reline(lbase)
LINE *lbase;
{
	LINE *l;
	short mask;
	short lattr;

	mask = lbase->attr & (VIDEOM);
	if (mask == TXT && sgrmode > 1 && lpa_pi == 0) {
		repage(); /* hint to total redraw because TXT is the base for other colors */
		return;
	}
	for (l = linem; l->size != 0; l++) {
		lattr = (l->attr & (VIDEOM));
		if ((lpa_pi == 0) && ((lattr & (INP | VEXT)) != 0))
			continue;
		if ((lpa_pi == 1) && ((lattr & (INP | VEXT)) == 0))
			continue;
		if ((lattr & (VIDEO)) == (mask & (VIDEO)))
			w_line(l);
	}
	w_line(lbase);
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
	LINE *line4;	/* указатель на базовую линию в 4-й строке */
	register LPA *lpap;
	char *sgr_v;

	i = (int) line->varl;   /* a cast from pointer to int: that's it (since ash1.0) */
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
/*		strcpy(outstr, ".?.");*/
		strcpy(outstr, ". .");
		strcpy(str, outstr);
		if (sgrmode < 2) return(TRUE);

		if (cod == ' ' || cod == KB_DE) {
			line4 = getl4(line); /* тут будет показан результат */
			/*выбрать, где будет настроен результат */
			switch (lpa_pi) {
			case 0:
				outstr[0] = '?'/*'#'*/; /*lpap = lpa_p[0];*/ /*'%';*/
				sgr_v = &lpaout[i].lpa_sgr[0];
				break;
			case 1:
				outstr[2] = '?'/*'#'*/; /*lpap = lpa_p[1];*/
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
			/*w_line(line4); /* вызов был перенесен в sgr_ed(), но теперь на месте, здесь */
			ref_co(line, cod);
			w_line(linesgr);
			/*if (i == TXT)*/
/*				repage();*/
			/* w_line(line4); /* TODO remove after ok on reline() */
			reline(line4);

		} else {
			if (linesgr != NULL) {
				sgrats[0] = 0;
				w_line(linesgr);
			}
		}
	}
	return (TRUE);
}

static char fgbg_list[] =    "012345679";
static char fgbg_cod[]  = "9- 01234567+";

int showcs(fgbg, cp, br, ci)
/* show color strip */
char *fgbg; /* clear show area if NULL */
char cp;	/* color pointer */
int br; 	/* bright color */
int ci;    /* stripe color mode index 0 - for foreground, 2 - for background*/
{
	int i;	char *s;
	/* black and white background codes for sample strip */
	char gp[4] = { '7', '0', '7', '0' };
	char *nFG[4] = { "3", "4", "3", "4" };
	char *bFG[4] = { "3", "10", "4", "9" };
	char **iFG;

	for (i=0; i<2; i++) {
		cp_set(linesgr->line + i /* +1 */, linesgr->colu/* - 4 */, ERR);
		if (fgbg != NULL) {
			if(br) iFG = bFG;
			else   iFG = nFG;
			w_raw("\033[0m");
			for (s = fgbg; *s != '\0'; s++) {
				w_raw("\033[");
				w_raw(iFG[ci]);
				w_putc(gp[i]);
				w_putc(';');
				w_raw(iFG[ci + 1]);
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

/* 1st color */
void showc1(color_list, cp, br)
char *color_list; /* clear show area if NULL */
char cp;	/* color */
int br; 	/* bright color */
/*int rev;    /* stripe mode: 0 == normal, 1 == reverse video */
{
	char tmps[30];
	char *fmt;
	char *s;
	/*int mode; /* stripe mode as function of rev&i below */

	int i;

	for (i=0; i<2; i++) {
		cp_set(linesgr->line + i, linesgr->colu, ERR);
		if (color_list != NULL) {
			w_raw("\033[0m"); /* reset before draw the row of the samples */
			/*mode = (i == 0 ? 1 : 0);*/
			if(br == 0) {
				switch(i /*mode*/) {
				case 0: fmt = "\033[3%c;40m"; break;
				case 1: fmt = "\033[4%c;37m"; break;
				}
			} else {
				switch(i /*mode*/) {
				case 0: fmt = "\033[9%c;40m"; break;
				case 1: fmt = "\033[10%c;97m"; break;
				}
			}
			for (s = color_list; *s != '\0'; s++) {
				sprintf(tmps, fmt, *s);
				w_raw(tmps);
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

/* 1st color sample with 1st color */
void showc2(color_list, cp, br, /*rev, */bg2s, br_bg2)
char *color_list; /* clear show area if NULL */
char cp;	/* color pointer */
int br; 	/* bright color */
/*int rev;    /* stripe mode: 0 == normal, 1 == reverse video first row */
char bg2s;
int br_bg2;
{
	char tmps[30];
	char *fmt;
	char *s;
	/* int mode; /* stripe mode as function of rev&i below */
	char bg2;;

	int i;
	char FG[5];
	char BG[5];
	char *p;

	if (bg2s == '\0') bg2 = '9';
	else				bg2 = bg2s;
	if (br_bg2) {
		sprintf(FG, "9%c", bg2);
		sprintf(BG, "10%c", bg2);
	} else {
		sprintf(FG, "3%c", bg2);
		sprintf(BG, "4%c", bg2);
	}
	for (i=0; i<2; i++) {
		cp_set(linesgr->line + i, linesgr->colu, ERR);
		if (color_list != NULL) {
			w_raw("\033[0m"); /* reset before draw the row of the samples */
			/*mode = (i == 0 ? 1 : 0);*/
			if(br == 0) {
				switch(i /*mode*/) {
				case 0: fmt = "\033[4%c;%sm"; p = FG; break;
				case 1: fmt = "\033[3%c;%sm"; p = BG; break;
				}
			} else {
				switch(i /*mode*/) {
				case 0: fmt = "\033[10%c;%sm"; p = FG; break;
				case 1: fmt = "\033[9%c;%sm";  p = BG; break;
				}
			}
			for (s = color_list; *s != '\0'; s++) {
				sprintf(tmps, fmt, *s, p);
				w_raw(tmps);
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

/* decode old SGR string, assumed format like 34;44 or 44;34 or 94;104 or 104;94 */
sgr_decode(sgra, fgp, bgp, brifgp, bribgp)
char *sgra;	/* string with SGR attributes to be scaned */
char *fgp;
char *bgp;
int *brifgp;
int *bribgp;
{
	char *s = sgra;
	char *cptr;
	char c;
	int cptrok = 0;

	*fgp = '\0';
	*bgp = '\0';
	*brifgp = 0;
	*bribgp = 0;

	while (*s != '\0') {
		c = *s;
		if (cptrok) {
			if (strchr(fgbg_list, c) != NULL)
				*cptr = c;
			cptrok = 0;
		} else {
			switch(c) {
			case	'3': cptrok = 1; cptr = fgp; *brifgp = 0; break;
			case	'9': cptrok = 1; cptr = fgp; *brifgp = 1; break;
			case	'4': cptrok = 1; cptr = bgp; *bribgp = 0; break;
			case	'1': s++; /* next char is 0 anyway */
						 cptrok = 1; cptr = bgp; *bribgp = 1; break;
			case	';': cptrok = 0; break;
			default:	cptrok = 0; break; /* illegal, error detected... TODO complete*/
			}
		}
		s++;
	}
}

/*
 * format SGR string with new parameters
 */
sgr_encode(s, fgp, bgp, brifg, bribg)
char *s; /* output string */
char *fgp;
char *bgp;
int *brifg;
int *bribg;
{
	char fg, bg;
	fg = *fgp;
	bg = *bgp;
	const char *bfg;
	const char *bbg;

	 bfg = "3";
	 bbg = "4";
	 if (fg != 0) {
		if (*brifg)
			bfg = "9";
	}
	if (bg != 0) {
		if (*bribg)
			bbg = "10";
	}
	if (fg != '\0' && bg != '\0') {
		sprintf(s, "%s%c;%s%c", bfg, fg, bbg, bg);
	}
	else if (fg != '\0') {
		sprintf(s, "%s%c", bfg, fg);
	}
	else if (bg != '\0') {
		sprintf(s, "%s%c", bbg, bg);
	}
	else if (fg == '\0' && bg == '\0') {
		s[0] = '\0';
	}
}

/* color brightness formatting -- copy&paste from cvt_co() */
cvt_cb(line, cod, mod, str)
LINE   *line;
kbcod   cod;
char   *mod;
char   *str;
{
	char    outstr[20];      /* строка для формирования вывода */
	int     lpax;			/* index for lpa[] */
	/* row on screen page: 'fg' or 'bg' 1st char significant only, see attr.cv */
	char    fbx;

	LINE *line4;   /* pointer to base LINE in 4th row */
	int vai;				/*video attribute index on sample row*/
	char *Rsgr;
	char *Wsgr;
	/* on input need both: lpaout+lpainp*/
	char Rfg, Rbg, Wfg, Wbg;
	int  Rbrifg, Rbribg, Wbrifg, Wbribg;

	/*Rbrifg = Rbribg = Wbrifg = Wbribg = 0;*/

	fbx = ((char *)line->cvts)[0];	/* only 1st symbol of field is significant */
	lpax = (int)line->varl; /* lpa*[] index*/
	if (!(fbx == 'f' || fbx == 'b')) {
		w_msg(ERR, "internal, cvt_cb(): line.cvts has wrong value");
		return(FALSE);
	}

	Rsgr = &lpaout[lpax].lpa_sgr[0];
	sgr_decode(Rsgr, &Rfg, &Rbg, &Rbrifg, &Rbribg);
	Wsgr = &lpainp[lpax].lpa_sgr[0];
	sgr_decode(Wsgr, &Wfg, &Wbg, &Wbrifg, &Wbribg);

	if(*mod == 'w') {
		if (sgrmode > 1) {
			if (lpa_pi) {
				strcpy(outstr, "  .");
			} else {
				strcpy(outstr, ".  ");
			}
			if (fbx == 'f') {
				if (Rbrifg) outstr[0] = '+';
				if (Wbrifg) outstr[2] = '+';
			}
			if (fbx == 'b') {
				if (Rbribg) outstr[0] = '+';
				if (Wbribg) outstr[2] = '+';
			}
		} else {
			strcpy(outstr, "   ");
		}
	}
	if (*mod == 'r' && sgrmode > 1) {
		if (cod == ' ' || cod == KB_DE) {
			line4 = getl4(line);
			/* do toggle modification */
			if (lpa_pi) {
				if (fbx == 'f') {
					Wbrifg = Wbrifg ? 0 : 1;
				}
				if (fbx == 'b') {
					Wbribg = Wbribg ? 0 : 1;
				}
				sgr_encode(Wsgr, &Wfg, &Wbg, &Wbrifg, &Wbribg);
			} else {
				if (fbx == 'f') {
					Rbrifg = Rbrifg ? 0 : 1;
				}
				if (fbx == 'b') {
					Rbribg = Rbribg ? 0 : 1;
				}
				sgr_encode(Rsgr, &Rfg, &Rbg, &Rbrifg, &Rbribg);
			}
			/* actualize new view of page */
#if 0
			w_line(line4);
			if (lpax == TXT && sgrmode > 1 && lpa_pi == 0)/* && cod == ' ')*/
				repage();
			else
#endif
				reline(line4/*lpax*/);
		}
	}

	strcpy(str, outstr);
	return (TRUE);
}

/* color control formatting */
cvt_co(line, cod, mod, str)
LINE   *line;
kbcod   cod;
char   *mod;
char   *str;
{
	char    outstr[20];      /* строка для формирования вывода */
	int     lpax;			/* index for lpa[] */
	char    fbx;			/* row on screen page: 'fg' or 'bg' 1st char significant only, see attr.cv */
	const char *s;
	int		gv;
	int		i;

	LINE *line4;   /* указатель на базовую линию в 4-й строке */
	register LPA *lpap;

	char *Wsgr;
	char *Rsgr;
	char *p;
	char Wfg, Wbg, Rfg, Rbg;
	int  Wbrifg, Wbribg, Rbrifg, Rbribg;
	Wfg = Wbg = Rfg = Rbg = 0;

	s = line->cvts;
	fbx = s[0];
	lpax = (int)line->varl; /* lpa*[] index*/
	if (!(fbx == 'f' || fbx == 'b')) {
		w_msg(ERR, "internal, cvt_co(): line.cvts has wrong value");
		return(FALSE);
	}
	Wsgr = &lpaout[lpax].lpa_sgr[0];
	sgr_decode(Wsgr, &Wfg, &Wbg, &Wbrifg, &Wbribg);

	Rsgr = &lpainp[lpax].lpa_sgr[0];
	sgr_decode(Rsgr, &Rfg, &Rbg, &Rbrifg, &Rbribg);

	if(*mod == 'r' && sgrmode > 1) {
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

			/* do circle around selection of SGR code number (fgbg[] array) */
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
				sgr_encode(Wsgr, &Wfg, &Wbg, &Wbrifg, &Wbribg);
				break;
			case 1:
				sgr_encode(Rsgr, &Rfg, &Rbg, &Rbrifg, &Rbribg);
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
#if 0
			if (lpax == 1/*TXT*/)
				sgrtst(line, KB_NL); /*cod);*//*may be better to refresh all the page*/
			else
				w_line(line4);
/* тут был endif */
			w_line(line4);
			if (lpax == TXT && sgrmode > 1 && lpa_pi == 0)/* && cod == ' ')*/
				repage();
			else
#endif
				reline(line4/*lpax*/);
		}
	}
	if(*mod == 'w') {
		if (sgrmode > 1) {
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

static void err_csel()
{
	w_msg(SEL|INP, "please, use key from list:");
	at_set(SEL|VEXT);
	w_str(fgbg_cod);
}

sgr_csel(line, cod, out)
/*редактор атрибутов цвета (SGR composer-selector)*/
LINE *line;
kbcod cod;
char *out;
{
	kbcod ed_cod;
	LINE *line4;	/* указатель на базовую линию в 4-й строке */
	int posp;

	char c;
	char fg = 0;	/* scan SGR indexes - BG, FG, and iterators */
	char bg = 0;
	int br_fg, br_bg;	/*bright indicators */
	int i;

	sgr_decode(sgra, &fg, &bg, &br_fg, &br_bg);

	if ((getlsgr(line, sgrats)) == (LINE *)0) {
		return(FALSE);
	}
	/* сhеck before edit, prepare for this a line */
	strcpy(sgrats, sgra);
	w_line(linesgr);

	w_msg(TXT, "key for FG: ");
	w_lh_msg(":0..9 select  :- clear  :+ bright        :SP next(BG)  :CA cancel");
	while(1) {
		/* show foreground samples colors */
		/*showcs(fgbg_list, fg, br_fg, 2);*/
		showc1(fgbg_list, fg, br_fg);
		c = ed_cod = r_cod(0);
		if (ed_cod == KB_CA) return(TRUE);
		if (ed_cod == KB_EX) return(TRUE);
		if (strchr(fgbg_cod, c) == NULL) {
			/*err_csel();*/
			/*return(FALSE);*/
			continue;
		}
		if (c == '+') {
			br_fg = (br_fg ? 0 : 1);
			continue;
		}
		if (c == ' ')
			break;
		if (c == '-') {
			fg = '\0'; /* clear color */
			continue;
		}
		fg = c;
	}
	w_emsg("");
	showcs(NULL, 0, 0, 0);

	w_msg(TXT,
			" BG: ");
	w_lh_msg(":0..9 select  :- clear  :+ bright     :SP complete  :CA cancel");
	while(1) {
		/* show background sample colors */
		showc2(fgbg_list, bg, br_bg, fg, br_fg);
	/*	showc1(fgbg_list, bg, br_bg, 1);*/
		c = ed_cod = r_cod(0);
		if (ed_cod == KB_CA) return(TRUE);
		if (ed_cod == KB_EX) return(TRUE);
		if (strchr(fgbg_cod, c) == NULL) {
			/*err_csel();*/
			/*return(FALSE);*/
			continue;
		}
		if (c == '+') {
			br_bg = ( br_bg ? 0 : 1);
			continue;
		}
		if (c == '-') {
			bg = '\0';
			continue;
		}
		if (c == ' ')
			break;
		bg = c;
	}
	showcs(NULL, 0, 0, 0);

	w_emsg("");

	/* prepare new result of SGR */
#if 0
	if (fg && bg) {
		sprintf(sgrats, "3%c;4%c", fg, bg);
	} else if (fg) {
		sgrats[0] = '3'; sgrats[1] = fg; sgrats[2] = '\0';
	} else if (bg) {
		sgrats[0] = '4'; sgrats[1] = bg; sgrats[2] = '\0';
	} else {
		sgrats[0] = '\0';
	}
#else
	sgr_encode(sgrats, &fg, &bg, &br_fg, &br_bg);
#endif

	/* store new SGR (no check for changes done) */
	strcpy(out, sgrats);
	return(TRUE);
}

#if 0
sgr_ed_notused(line, cod)
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
		w_line(linesgr);
		ed_cod = r_line(linesgr, &posp);
		linesgr->attr &= (~INP);

		line4 = getl4(line); /* тут будет показан результат с учетом новых параметров SGR */
		w_line(line4);
	}
/*	}*/
	return(TRUE);
}
#endif

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
	LINE *line4;   /* указатель на базовую линию в 5-й строке */
	register LPA *lpap;
	
	char *smask;

	va = *(int *)line->cvts;
	i = (int)line->varl;

	if(*mod == 'w') {
		if (sgrmode & 1) {
			/*strcpy(outstr, ". ."); /* on PMT spec: ". ." + outstr indexes 0,2 */
			if (lpa_pi) {
				strcpy(outstr, "  .");
			} else {
				strcpy(outstr, ".  ");
			}
			if(lpainp[i].lpa_a & va) outstr[2] = '+';
			if(lpaout[i].lpa_a & va) outstr[0] = '+';
			strcpy(str, outstr);
		} else {
		    strcpy(str, "   "); /*blank is default*/
		}
	} else {
	    if (sgrmode & 01) {
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
#if 0
				w_line(line4);
				if (i == TXT && sgrmode > 1 && lpa_pi == 0)/* && cod == ' ')*/
					repage();
				else
#endif
					reline(line4/*i*/);
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
			w_msg(SEL, "Please, type a prompt symbol");
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
/*
 * defined for attr.i
 */
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
	const char *cvts;

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
	char *p;

	size_t size = line->size;
	p = (char *)line->cvts;
	/*xactive = line->cvts[0] - '0'; /*iconv the special*/
	xactive = p[0] - '0'; /*iconv the special*/

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
		if (l->cvtf == cvt_pi
				|| l->cvtf == cvt_pim
				|| l->cvtf == cvt_va
				|| l->cvtf == cvt_sgr
				|| l->cvtf == cvt_cb) {
			w_line(l);			/*break;  multiply lines there */
		}
	}
}

static  char    helpf[] = "vhseta.lb";


static LINE *cline = (LINE *)-1;
uspage(page, phline)
/*-----------------------------*/
/* special for vhset
/*-----------------------------*/
LINE *page;
LINE *phline;           /* pointer to instant page with help screen */
{
    kbcod   cod;

    attrpg = page;
    if (cline == (LINE *)-1)
    	cline = attrpg;
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
			return 0;
		case '?':
		case KB_HE:
			w_help(phline);
			/*w_page(page);*/
			repage();
			break;
		default:   /* w_emsg("");     /* clear message string */
		}
    }
}

pag_a()
/*---------------------*/
/* настройка атрибутов */
/*---------------------*/
{
	uspage(linem, helpf);
	return(TRUE);
}
