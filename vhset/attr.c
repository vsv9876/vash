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

int     lpa_pi = 0;     /* РЕЖИМ ИЗМЕНЕНИЯ АТРИБУТОВ (НА ВВОДЕ/НА ВЫВОДЕ) */

int     wamask[10] = {
	A_SO,   A_US,   A_VS,   A_MD,	A_MR,   A_MB,   A_MK,	0,      0,      0,
	};

LPA     *lpa_p[2] = {
	lpaout,         lpainp
	};

/* Color support for attr.cv page */
char   *sgrms[] = {
		"[0] monochrome ",
		"[1] color ",
		0 };
extern int		sgrmode;

extern  LINE linem[];
LINE *linesgr = (LINE *)0;

/*char *sgra = "";*/
char *sgra = lpaout[0].lpa_sgr; /* SGR code, common (global) pointer, between cvt_sgr, cvt_co, cvt_csel */

char sgrats[20] = ""; /* SGR attrib tempopary string */
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

LINE *getl4(line)
/*-------------------------------------------------------*/
/* вернуть указатель на базовую линию в 4й строке экрана */
/*-------------------------------------------------------*/
register LINE *line;
{
	register LINE *l;

	for (l=linem; l->size != 0; l++) {
		if (l->line != 4)
			continue;
		if (l->colu == line->colu)
			return( l );
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

		cp_set(line->line + 1, line->colu, attr|INP);
		w_str(line->varl);
	}
	return(TRUE);
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
//	int     va;             /* видеоатрибуты ( флаги ) */
	int posp;
	register LINE *line4;	/* указатель на базовую линию в 4-й строке */
	register LPA *lpap;
	char *sgr_v;

//	va = *(int *)line->cvts;
	i = (int) line->varl;
	if (*mod == 'w') {
		strcpy(outstr, " . .");
		strcpy(str, outstr);
	} else {
		/*hack. TODO cleanup */
		strcpy(outstr, " . .");
		strcpy(str, outstr);

		if (cod == ' ' || cod == KB_DE) {
			line4 = getl4(line); /* тут будет показан результат */
			/*выбрать, где будет настроен результат */
			switch (cod) {
			case ' ':
				outstr[1] = '%'; /*lpap = lpa_p[0];*/
				sgr_v = &lpaout[i].lpa_sgr[0];
				break;
			case KB_DE:
				outstr[3] = '%'; /*lpap = lpa_p[1];*/
				sgr_v = &lpainp[i].lpa_sgr[0];
				break;
			}
			if (sgr_v != (char*) 0) {

				sgra = sgr_v;
				/*w_line(line);*/
				cp_set(line->line, line->colu, line->attr);
				w_str(outstr);
				if (!sgr_csel(line, cod, sgra))
					return(FALSE);
			}
			w_line(line4); /* вызов перенесен в sgr_ed(), но теперь на месте, здесь */
			w_line(linesgr);
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
char *fgbg; /* clear show area if NULL */
char cp;	/* color pointer */
int ci;    /* stripe color mode index 0 - for oreground, 2 - for background*/
{
	int i;	char *s;
	char gp[2] = { '0', '7' }; /* black and white background for color sample strip */
	char iFG[4] = { '3', '4', '4', '3' };

	for (i=0; i<2; i++) {
		cp_set(i+1+linesgr->line, linesgr->colu - 4 , CMD);
		if (fgbg == NULL) {
			er_eol(TXT);
		} else {
			for (s = fgbg; *s != '\0'; s++) {
				w_str("\033[0m");
				if (*s != cp) { w_str(" "); } else { w_str(" >"); }
				w_raw("\033["); w_chr(iFG[ci]);
				w_chr(gp[i]); w_chr(';'); w_chr(iFG[ci + 1]);
				w_chr(*s); w_chr('m');
				if (*s != cp) { w_str(" "); } else { w_str("*"); }
				w_chr(*s);
				if (*s != cp) { w_str(" "); } else { w_str("*"); }
			}
		}
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

	if(*mod == 'r') {
		if(cod == ' ' || cod == KB_DE) {
			line4 = getl4(line);

			/* select what to decode for further modification */
			switch (cod) {
			case ' ':
				if (fbx == 'f') p = &Wfg; else p = &Wbg;
				break;
			case KB_DE:
				if (fbx == 'f') p = &Rfg; else p = &Rbg;
				break;
			}

			/* do sircle around selection of SGR code number (fgbg[] array) */
			gv = *p;
			switch(gv) {
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':	gv = gv + 1; break; /* next code in ASCII table */
			case '7':	gv = '9'; break;
			case '9':	gv =   0; break;
			case 0:     gv = '0'; break;
			}
			*p = (char)gv;

			/* prepare and store new result of SGR */
			switch (cod) {
			case ' ':
				sgr_encode(Wsgr, &Wfg, &Wbg);
				break;
			case KB_DE:
				sgr_encode(Rsgr, &Rfg, &Rbg);
				break;
			}

			strcpy(outstr, " .!.");
			if (fbx == 'f') {
				outstr[1] = Wfg ? Wfg : '-';
				outstr[3] = Rfg ? Rfg : '-';
			}
			if (fbx == 'b') {
				outstr[1] = Wbg ? Rbg : '-';
				outstr[3] = Rbg ? Rbg : '-';
			}

			/* hint in case of TXT attribute - affected all screen view */
			if (lpax == 1)
				sgrtst(line, cod);
			else
				w_line(line4);
		}
	}
	if(*mod == 'w') {
		strcpy(outstr, " .|.");

		if (fbx == 'f') {
			outstr[1] = Wfg ? Wfg : '-';
			outstr[3] = Rfg ? Rfg : '-';
		}
		if (fbx == 'b') {
			outstr[1] = Wbg ? Wbg : '-';
			outstr[3] = Rbg ? Rbg : '-';
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

	char c;/* char *cptr; char *s;*/
	char fg = 0;	/* scan SGR indexes - BG, FG, and iterators */
	char bg = 0;
	int i;
/*	char gp[2] = { '0', '7' }; /* black and white background for color sample strip */

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
	if (strchr(fgbg_cod, c) == NULL) {
		w_msg(ATT|INP, "please, use key from list:"); at_set(ATT); w_str(fgbg_cod);
		return(FALSE);
	}
	w_msg(TXT, "");
	if (c == '-') fg = '\0'; /* clear color */
	else if (c != ' ') fg = c; /* remain the same color */
	showcs(NULL, 0, 0);

	/* show background sample colors */
	showcs(fgbg, bg, 0);
	c = ed_cod = r_cod(0);
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
//	if (cod == ' ' || cod == KB_DE) {
	if ((/*linesgr=*/getlsgr(line, &sgra)) != (LINE *)0) {
		linesgr->attr |= INP;
		posp = 0;
		w_line(linesgr, &posp);
		ed_cod = r_line(linesgr, &posp);
		linesgr->attr &= (~INP);

		line4 = getl4(line); /* тут будет показан результат с учетом новых параметров SGR */
		w_line(line4);
	}
//	}
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

	va = *(int *)line->cvts;
	i = (int)line->varl;

	if(*mod == 'w') {
		strcpy(outstr, " . .");
		if(lpainp[i].lpa_a & va) outstr[3] = 'x';
		if(lpaout[i].lpa_a & va) outstr[1] = 'x';
		strcpy(str, outstr);
	} else {
		if(cod == ' ' || cod == KB_DE) {
			line4 = getl4(line);

			switch(cod) {
			case ' ':
				lpap = lpa_p[0];
				break;
			case KB_DE:
				lpap = lpa_p[1];
				break;
			}
			ap = &(lpap[ i ].lpa_a);

			/* do toggle modification */
			if((*ap) & va) { (*ap) = (*ap) & (~va); }
			else           { (*ap) = (*ap) | ( va); }

			w_line(line4);
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
			/*str[0] = '\0';*/  /* TODO: make line hidden */
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
		sprintf(str, " %c %c",
		lpaout[i].lpa_p, lpainp[i].lpa_p);
	} else {
		if (cod == ' ' || cod == KB_DE) {
			switch(cod) {
			case ' ':
				  lpa_pi = 0; break;
			case KB_DE:
				  lpa_pi = 1; break;
			}
			w_msg(ATT, "Please, type a prompter char ");
			if (lpa_pi)
				w_str("on input: ");
			else
				w_str("on output: ");

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
		}
	}
	return(TRUE);
}

extern  int     cvt_hl();
extern  int     cvt_s();
extern  int     sgrtst();

#include "attr.i"

sgrtst(line, cod)
LINE *line;
kbcod cod;
{
    switch(cod) {
    case(' '):
    case(KB_DE):
	at_set(0);
	er_pag();
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
	u_page(linem, helpf);
	return(TRUE);
}
