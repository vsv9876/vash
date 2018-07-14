#include <signal.h>
#include <ctype.h>
#include <stdio.h>
#include "line.h"
#include "assist.h"

extern  char    Cmore[];
extern  char    Cred[];
extern  char    Cfill[];
extern  char    Crepf[];

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

	if (itms[itm][1] == '/') {
		w_emsg("it's directory");
		return( 0);
	}
	for(i = 0; i < itmmax; i++) {
		if (itms[i][0] == cod) {
			w_emsg("redirection selected: ");
			/* будет показан и символ типа файла: */
			w_str(itms[i]);
			return(0);
		}
	}
	switch(cod) {
	case '>': mark_o = itm; break;
	case '<': mark_i = itm; break;
	}
	return(1);
}

int t_file(line, cod)
/*
 * тест для пунктов меню (имен файлов)
 */
register LINE *line;
kbcod cod;
{
	register char *p;
	register char **pp;

	if (cod == ' '
	|| (oneitm && cod == KB_NL)
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

/*NOXSTR*/
static  char pattfs[22] = "*";  /* строка для шаблона пометки */
/*YESXSTR*/

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
	sprintf(prompts, "Select [%c]", cod);
	switch(pmtrstr(prompts, pattfs, 20)) {
	case KB_EX:
		w_emsg("");
		return 0;
	}
	if (cod == '-') {
		mark_i = -1;
		mark_o = -1;
	}
	for (i = 0; i < itmmax; i++) {
	    p = itms[i];
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
	for(line=vf; line->size != 0; line++) {
		p = *(char **)line->varl;
		switch(*p) {
		case '>':
		case '<':
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
		fprintf(vttout, "Selected: %3d", total);
		if (unvisible)
			fprintf(vttout, ", out of display: %3d", unvisible);
	}
	else w_emsg("");
}

extern  int     y0_top;         /* defined in main.c */

u_menu(mainl, helpl)
/*
 * работа со страницей меню
 */
register LINE *mainl;
char *helpl;
{
	register int i;
	kbcod cod;
	int   keyreq;   /* флаг: требуется показать панель */
	int   cmdret;

	/* первоначальный показ на экране */
	cp_set(y0, 0, TXT);
	er_eop();
	cwdshow();
	itmshow();
	w_page(vf, 0);
	keyreq = 1;

	itm = i = 0;
	for ( ;; ) {
		/* нет сообщений и требуется нарисовать панель */
		if ( !ok_msg() && keyreq ) {
			keyshow(panelf); keyreq = 0;
		}

		showtime( 1 );  /* восстановить индикацию часов */
		i = itm - itmofs;
		cod = r_line( &vf[i], 0 );

		if ( ok_msg() ) keyreq = 1;
		w_emsg("");

		switch (cod) {
		/* не встроенная команда, надо интерпретировать */
		default:
			w_line( &vf[i] );
			keyreq = 1;
			if ((cmdret = vcmd(i, cod, vf)) > 0) {
				/* экран меню испорчен или
				 * новое главное меню.
				 */
				scrlst();
				cp_sav();
				if (fil_vf(0)) {
					cp_fet();
					scrlnl(); /* новый y0... */
				}
#ifdef  FULLTRUE
				else    cp_sav(); /* необязательно */
#endif
				cwdshow();      /* вывески */
				itmshow();      /* положение окна */
				w_page(vf, 0);  /* пункты меню */
			} else {
				if (cmdret < 0)
					keyreq = 0;
			}
			/* синхронизировать y0 и y0_top */
			if (y0 < y0_top)
				y0_top = y0;
			break;
#ifdef HELPRETRO
		case KB_HE:
			at_set(TXT);
			w_help(helpl);
			/* проваливаемся... */
#endif
		case KB_RE:
			er_pag();
			cwdshow();
			w_emsg("");
			itmshow(); w_page(vf, 0);
			keyreq = 1;
			break;
		case '+':
		case '-':
#ifdef VFRETRO
			if (oneitm)     bell();
			else
#endif
			w_line( &vf[i] );
			tutsel(cod);
			keyreq = 1;
			break;
		case ' ':
		case '<':
		case '>':
			w_line( &vf[i] );
/*                      if (oneitm) return;     */
			cod = KB_AD;
			/* проваливаемся... */
		case KB_AD:
		case KB_AR:
		case KB_AL:
		case KB_AU:
			i = itmadj(cod);
			break;
		}
	}
}
