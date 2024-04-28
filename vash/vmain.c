#include <signal.h>
#include <ctype.h>
#include <stdio.h>
#include "line.h"
#include "assist.h"

#ifdef RETRO
extern  char    Cmore[];
extern  char    Cred[];
extern  char    Cfill[];
extern  char    Crepf[];
#endif

#ifdef RT11
 error Программа предназначена исключительно для unix
#endif

LINEMENU clm = { 0 };

int     mark_i = -1;
int     mark_o = -1;

static  int     tstmark(cod)
kbcod   cod;
/* проверить уникальную пометку */
{
	register int i;

	if (clm._itms[clm._itm][1] == '/') {
		w_emsg("it's directory");
		return( 0);
	}
	for(i = 0; i < clm._itmmax; i++) {
		if (clm._itms[i][0] == cod) {
			w_emsg("redirection selected: ");
			/* будет показан и символ типа файла: */
			w_str(clm._itms[i]);
			return(0);
		}
	}
	switch(cod) {
	case '>': mark_o = clm._itm; break;
	case '<': mark_i = clm._itm; break;
	}
	return(1);
}

int t_f1(line, cod)
/*
 * тест для пунктов меню (имен файлов)
 */
register LINE *line;
kbcod cod;
{
	register char *p;
	register char **pp;

	if (cod == ' '
	|| (vashflag.oneitm && cod == KB_NL)
	|| (cod == '<')
	|| (cod == '>')) {
		pp = (char **)line->varl;
		p = *pp;
		if (*p == ' ') {
			switch (cod) {
			case ' ':
				*p = MONEY;
				line->attr = ATT|INP|NED|LFASTR;
				break;
			case '>':
			case '<':
				if (tstmark(cod))
					*p = cod;
				else    return(FALSE);
				if(*p == '>' || *p == '<')
					line->attr = ATT|VEXT|INP|NED|LFASTR;
				break;
			}
		}
		else {
			if (cod != ' ')
				return(FALSE);
			line->attr = TXT|INP|NED|LFASTR;
			switch(*p) {
			case '>': mark_o = -1; break;
			case '<': mark_i = -1; break;
			}
			*p = ' ';
		}
	}
	return (TRUE);
}

int t_file(line, cod)
register LINE *line;
kbcod cod;
{
	if (t_f1(line, cod) == TRUE)
		w_line(line);
	return(1);
}

/*NOXSTR*/
static  char pattfs[42] = "*";  /* строка для шаблона пометки */
/*YESXSTR*/
#if 0
static  int ed_ls_NOT_USED(fill)
/*
 * тотальная пометка
 */
char *fill;
{
	kbcod cod;

	register char *p;
	register LINE *line;
	extern kbcod pmtrstr(); /* ввод строки с промптером */
	char prompts[100];
	char newCfill[U8_STRBUF];
	int maxlen;

	maxlen = (MAXLICO > lframe->maxco ? lframe->maxco : MAXLICO) - 1;
	/* если строка fill пустая, редактировать Cfill, иначе просто скопировать fill */
	if (fill != (char *)0 && fill[0] != '\0') {
		strcpy(Cfill, fill);
		return 1;
	}
	else
		strncpy(newCfill, Cfill, maxlen);

	sprintf(prompts, "-- ");
	cod = pmtrstr(prompts, newCfill, lframe->maxco - strlen(prompts) - 2);
	switch(cod) {
	case KB_CA:
	case KB_EX:
		w_emsg("");
		return 0;
	case KB_NL:
		strncpy(Cfill, newCfill, maxlen);
	}
	return(1);
/*	else w_emsg("");*/
}
#endif

int f_ls(fill)
/*
 * edit fill command
 */
char *fill;
{
	kbcod cod;

	extern kbcod pmtrstr(); /* ввод строки с промптером */
	char prompts[10];
	int maxlen;

	maxlen = (MAXLICO > lframe->maxco ? lframe->maxco : MAXLICO) - 1;

	sprintf(prompts, "-- ");
	cod = pmtrobj(prompts, Cfill_o, lframe->maxco - strlen(prompts) - 2);
	switch(cod) {
	case KB_CA:
	case KB_EX:
		w_emsg("");
		return 0;
	case KB_NL:
		w_msg(TXT, "Please, reload main menu with pressing '='");
		return(0/*1*/);
	}
	return(1);
}

/* счетчик помеченных пунтов меню */
int cntsel()
{
	char *p;
	int cnt;
	int i;

	cnt = 0;
	for (i = 0; i < clm._itmmax; i++) {
	    p = clm._itms[i];
		if(*p == MONEY) {
			cnt++;
		}
	}
	return(cnt);
}

static  int tutsel(cod)
/*
 * тотальная пометка
 */
kbcod cod;
{
	register char *p;
	register LINE *line;
	register int i;
	extern kbcod pmtrstr(); /* ввод строки с промптером */
	char prompts[100];
	int total, unvisible;

	total = 0;
	/* ввести шаблон пометки */
	sprintf(prompts, " mark #%c ", cod);
	switch(pmtrstr(prompts, pattfs, 40)) {
	case KB_CA:
	case KB_EX:
		w_emsg("");
		return 0;
	}
	if (cod == '-') {
		mark_i = -1;
		mark_o = -1;
	}
	for (i = 0; i < clm._itmmax; i++) {
	    p = clm._itms[i];
		       /* Проверять по шаблону... */
	    if (patcmp(pattfs, &p[2]))
		switch(cod) {
		case '+':
			/* помечать только при явном указании */
			if (p[2] == '.' && pattfs[0] != '.')
				break;
#ifdef RETRO
			/* не помечать каталоги */
			if (p[1] == '/')
				break;
#endif
			*p = MONEY;
			break;
		case '-':
			*p = ' ';
			break;
		}
		switch (*p) {
		default:
		case ' ':
			break;
		case MONEY:
			total++;
		}
	}
	unvisible = total;
	for(line=clm._vf; line->size != 0; line++) {
		p = *(char **)line->varl;
		switch(*p) {
		case '>':
		case '<':
			line->attr = ATT|VEXT|INP|NED|LFASTR;
			break;
		case ' ':
			line->attr = TXT|INP|NED|LFASTR;
			break;
		case MONEY:
			line->attr = ATT|INP|NED|LFASTR;
			unvisible--;
			break;
		}
		w_line(line);
	}
	if (total) {
		w_msg(TXT, " ");
		fprintf(vttout, " # marked: %d ", total);
		if (unvisible)
			fprintf(vttout, ", not shown: %d ", unvisible);
	}
	else {
		w_msg(TXT, " # no mark ");
	}
	return 0;
}

/* команды пометки вынести во внешние файлы .ashstd */
int f_mark(cmd)
char *cmd;
{
	kbcod cod;
	cod = cmd[0];

	switch (cod) {
	case ' ':
	case 'x':
		cod = ' ';
		/*NO BREAK*/
	case '>':
	case '<':
		t_file(&clm._vf[clm._itm - clm._itmofs], cod);
		return(-1);
		break;
	case '+':
	case '-':
		return(tutsel(cod));
		break;
	default:
		w_emsg("invalid arg for _mark");
	}
	return (0);
}

static char pattpos[20] = "";
static matched, ispatt;
/*void*/int itmsel(i)
register int i; /* search start position */
{
	register char *p;
	for (/*i = 0*/; i < clm._itmmax; i++) {
		p = clm._itms[i];
		/* Проверять по совпадению или образцу */
		matched = 0;
		if (ispatt == 0 && strncmp(pattpos, &p[2], strlen(pattpos)) == 0)
			matched++;
		if (ispatt != 0 && /*wldcmp*/patcmp(pattpos, &p[2]) != 0)
			matched++;
		if (matched) {
			clm._itm = i;
			break;
		}
	}
	return(i);
}

/* position cursor on first letter given in dialog */
int itmpos(cmd)
char *cmd;
{
	extern kbcod pmtrstr(); /* ввод строки с промптером */
	kbcod cod;
	int i, ilast;

	cod = pmtrstr(" position on: ", pattpos, 16);
	if (strchr(pattpos, '*') || strchr(pattpos, '?'))
		ispatt++;

	switch(cod) {
	case KB_CA:
	case KB_EX:
		w_emsg("");
		return 0;
		break;
	case KB_TA:
		ilast = clm._itm;
		i = itmsel(ilast + 1);
		if (i >= clm._itmmax) {
			w_emsg("no more items: ");
			w_str(pattpos);
			return -1;
		}
		if (ilast != clm._itm) {
			itmadj(0);
		}
		break;
	case KB_NL:
		ilast = clm._itm;
		i = itmsel(0);
		if (i >= clm._itmmax) {
			w_emsg("no such item: ");
			w_str(pattpos);
			return -1;
		}
		if (ilast != clm._itm) {
			itmadj(0);
		}
		break;
	}
	return 0;
}

extern  int     y0_top;         /* defined in main.c */

u_menu(mainl, helpl)
/*
 * Main Menu Loop
 */
register LINE *mainl;
char *helpl;
{
	int i;
	kbcod cod;
	int   keyreq;   /* flag: keyshow() required */
	int   cmdret;
	int	  refresh;  /* flag: itmshow() required */

	/* 1st show before main loop */
	cp_set(clm._y0, 0, TXT);
	er_eop(TXT);
	cmdret = 1;
	refresh = keyreq = 1;

	clm._itm = 0;

	for ( ;; ) {

		if (refresh) {
			cwdshow();
			itmshow();
			w_page(clm._vf, 0);
			refresh = 0;
			keyreq = 1;
		}
		if (!ok_msg() && keyreq) {
			keyshow(vashflag.panelf);
			keyreq = 0;
		}
		showtime( 1 );  /* restore clock */

		i = clm._itm - clm._itmofs;

		/* entry code of main menu */
		cod = r_line( &clm._vf[i], 0 );

		if ( ok_msg() ) {
			w_emsg("");
			keyreq = 1;
		}

		switch (cod) {
		case KB_RE:
			er_pag();
			refresh = 1;
			break;
		case KB_KH: case KB_KE: case KB_PU: case KB_PD:
		case KB_AD: case KB_AR: case KB_AL: case KB_AU:
			i = itmadj(cod);
			break;
		default:
			/*w_line( &clm._vf[i] );*/
			cmdret = vcmd(/*i,*/ cod/*, clm._vf*/);
			if (cmdret == 0)
				keyreq = 1;
			if (cmdret > 0) {
				refresh = 1;
				scrlst();
				cp_sav();
				if (fil_vf(0)) {
					cp_fet();
					scrlnl(); /* set new y0... */
				}
				er_eop(TXT);
			}
			if (cod == ' ') {
				/* hint after space: advance next line */
				/* hit space on current line!!! */
				cod = KB_AD;
				i = itmadj(cod);
				cod = ' ';
			}

			/* syncronize y0 and y0_top */
			if (clm._y0 < y0_top)
				y0_top = clm._y0;

			break;
		}

	}
}
