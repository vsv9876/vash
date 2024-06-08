#include <sys/types.h>
#include <sys/stat.h>

#include <stdlib.h>
#include <string.h>
#include <ediag.h>
#include <time.h>
#include <stdio.h>
#include <ctype.h>
#include "line.h"
#include "assist.h"

/*----------------------*/
/* ВСЯКА ВСЯЧИНА, misc. */
/*----------------------*/


static int doscrl = 1;
scrldo() {
	doscrl = 1;
}

/* vash-specific screen area control */
scrlarea()
{
	int i, n, cltop;
	if (doscrl == 1) {
		doscrl = 0; /* prevent unwanted scrolling */
		n = lframe->maxli;
		/* same as in shstart() */
		if (clm._y0 > y0_top)
			cltop = y0_top - 1;
		else
			cltop = clm._y0 - 1;
		cp_set(cltop, 1, TXT); /* er_eop(TXT);*/

		/*y0_top = 0;    /* установить новую нижнюю границу свитка */
		for (i = 1; i < n; i++) {
			er_eol(TXT); /* hack in hope ... */
			putc('\n', vttout);
		}
	}
}

int
vashelp(page)
LINE *page;
{
	scrlarea();
	w_help (page);
	return(1); /* screen touched */
}

/*------*/
/* ЧАСЫ */
/*------*/
static time_t prevtime = (time_t)0;
static int nxtjflag;

showtime(on)
/* Управление семафором часов и др. асинхронных заданий */
{
	nxtjflag = on;
#ifdef RETRO
	if ( !on && vashflag.clockf) {
		cp_set(1, lframe->maxco - 8, TXT);
		fprintf(vttout, "%8.8s", "");
	}
#endif
}

showclck()
/*
 * Показать системные часы.
 */
{
	time_t curtime;
	struct tm *tp;
	struct tm *localtime();
	char tmps[12] = "";

	if (vashflag.clockf) {

		time(&curtime);
		if (curtime != prevtime) {
			prevtime = curtime;

			tp = localtime(&prevtime);

#ifndef CP_SAV
			cp_sav();
#endif
			/*cp_set(lframe->maxli - 2 //1//, lframe->maxco - 8, TXT|INP);*/
			cp_set(WSHOW_LI, WSHOW_CO, WSHOW_AT);
			sprintf(tmps, "%02d:%02d'%02d",
					tp->tm_hour, tp->tm_min, tp->tm_sec);
			w_str(tmps);
#ifndef CP_SAV
			cp_fet();
#endif
		}
	}
}

void
showitem(on)
/*
 * Показать пункт меню указанный курсором или общее количество помеченных пунктов
 */
{
	char tmps[33] = "";
	char file[256*4];
	int n;
	LINE pline; /* patch line, show over current line */

		if (vashflag.subshow == 0)
			return;
#ifndef CP_SAV
		cp_sav();
#endif
		pline = *clm._line;
		n = pline.colu -1;
		n = n < 0 ? 0 : n;
		if (on) {
			cp_set(pline.line, n, WSHOW_AT);
			w_chr('@');
/*
			pline.attr = TXT|VEXT|NED|LFASTR;
			w_line(&pline);
*/
		} else {
			cp_set(pline.line, n, TXT);
			w_chr(' ');
/*
			pline.attr = TXT;
*/
			/*w_line(clm._line)*/;
		}
		cp_set(WSHOW_LI, WSHOW_ITEM, WSHOW_AT);
		if (on) {
			n = cntsel();
			if(n) {
				sprintf(tmps, " ##:%d ", n);
			}/* else {
				cmdsub(file, "#@", clm._itm, 0, 1);
				sprintf(tmps, " '%-40.40s' ", file);
			}*/
		} else {
			/*sprintf(tmps,"%-30.30s", "");*/
			er_eol(WSHOW_AT);
		}
		w_str(tmps);
#ifndef CP_SAV
		cp_fet();
#endif
}

/* #ifndef SVR4         */
/* #define MAIL_DUMB    */
/* #endif               */

#ifndef CP_SAV
#define CURSOR_SAVE     cp_sav()
#define CURSOR_FETCH    cp_fet()
#else
#define CURSOR_SAVE
#define CURSOR_FETCH
#endif

int mailf2 = 0; /* делать повторную проверку почты? */

void
chckmail()
/*
 * Проверить наличие новой почты
 */
{
    extern char *getenv();
    static time_t mchktime = (time_t)0; /* для запом. даты записи ящика*/
    static time_t mboxtime;             /* дата записи ящика из stat */
    struct stat mailstat;
    char *mailbox;
    kbcod cod;

    if (vashflag.cmailf) {
	if (!mailf2) return;

	if ((mailbox = getenv("MAIL")) == (char *)0)
		return;

	if (stat(mailbox, &mailstat) < 0)
		return;

	mboxtime = mailstat.st_mtime;

	if (mailstat.st_size != 0L) {
#ifdef MAIL_DUMB
		if (mchktime == (time_t)0) {
			mchktime = mboxtime;    /* один раз при старте */
		}

#else
		/* модифицировали позже, чем смотрели */
		if (mailstat.st_mtime > mailstat.st_atime) {
#endif
			if (mchktime != mboxtime) {
			    mchktime = mboxtime;
			    CURSOR_SAVE;
			    cp_set(-2, -11, ERR); bell();
			    w_str(" New Mail ");
			    CURSOR_FETCH;
			}
			else {
			    /* есть непрочитанная почта */
			    CURSOR_SAVE;
			    cp_set(-2, -7, ATT);
			    w_str(" mail ");
			    CURSOR_FETCH;
			}
#ifndef MAIL_DUMB
		}
#endif
	}
    }
    mailf2 = 0;     /* взводится в 1 внутри cwdshow() */
}

/*
 * Подключить асинхронные процессы.
 */
next_j()
{
	if (nxtjflag) {
#ifdef CP_SAV
		cp_sav();
#endif
		showclck();
		chckmail();
#ifdef CP_SAV
		cp_fet();
#endif
	}
}

clritm()
/* прочистить место на экране для меню */
{
    register int i;
    register LINE *l;

    for (l = clm._vf,i = 0; l->size && i < clm._yy; l++,i++) {
	cp_set(l->line, l->colu, TXT); er_eol(TXT);
    }
}

scrlnl()
/*
 * to scroll with newlines
 * продвинуть свиток на экране при помощи переводов строки
 */
{
	register int i;

	if (vashflag.scrolf) {
		/*at_set(CMD); */er_eop(CMD);
		io_set(IO_TTYPE);
		for (i = clm._y0; i < lframe->maxli; i++) {
			putc('\n', stdout);
		}
		io_set(IO_VIDEO);
	}
	else    er_pag();
}

fatal()
{
	cp_set(-1, 0, CMD);
	io_set(IO_TTYPE);
	fprintf(stderr, "FATAL ERROR - exit\n");
	exit(1);
}

char itmnav[STRBUF] = "";

itmshow()
/* показать положение "окна" */
{
	int showco; /* позиция нач. строки индикации окна */
	int showscale; /* масштаб показа, 1x, 2x, 3x, ... */
	register int i;
	int lxx, lyy, litmmax;
	int n;

	itmnav[0] = '\0';
	/* показать положение окна */
	if (clm._yy * clm._xx < clm._itmmax) {  /* если не все файлы на экране... */
		/* отцентрировать... */
/*
		showco = (lframe->maxco - (itmmax/yy))/2;
		if (showco < 0) showco = 0;
*/
		showco = 2;
		lxx = clm._xx;
		lyy = clm._yy;
		litmmax = clm._itmmax;
		showscale = ((clm._itmmax/lyy) / (lframe->maxco - 24)) + 1 ; /*TODO WTF is 24 there */

		cp_set(lframe->maxli - 2, showco, TXT);
#ifdef DEBUG_NAV
		sprintf(itmnav, /*"%dK:*/"%d:%d ", /*clm._itmbsz/1024, */clm._itmmax, showscale);
#endif
		n = strlen(itmnav);
		for (i = 0; i < clm._itmmax && n < MAXLICO; i += clm._yy) {
			if (i < clm._itmofs || i >= clm._itmofs+(clm._yy * clm._xx)) {
				if (showscale == 1) itmnav[n] = '-';
				else {
					if (itmnav[n] != ':' && itmnav[n] != '|') {
						if (((i/clm._yy) % showscale) == 0) itmnav[n] = '-';
						else 								itmnav[n] = '.';
					}
				}
			} else {
				if (showscale == 1)
					itmnav[n] = '=';
				else {
					if (((i/clm._yy) % showscale) == 0) itmnav[n] = ':';
					else							itmnav[n] = '|';
				}
			}
			if (showscale == 1 || ((i/clm._yy) % showscale) == 0) {
				n++; itmnav[n] = '\0';
			}
		}
		itmnav[n] = '\0';
		w_str(itmnav);
	}
}


/*
 * wrapper for patcmp which by default return true if matches only part of string;
 * pattern prepend with '^' or ended with '$' if not contains '*' in such positions
 */
int wldcmp(wldpat, str)
register char *wldpat;
register char *str;
{
	char bufpat[STRBUF];
	register char *pat;
	pat = bufpat;

	if (*wldpat == '^')
		wldpat++;
	else if (*wldpat == '*')
		/*wldpat++*/;
	else
		*pat++ = '*';
	do {
		*pat++ = *wldpat++;
	} while (*wldpat != 0);
	pat--;
	if (*pat != '$') {
		*pat++ = '\0';
	} else if (*pat == '*')
		;
	*pat = '\0';


	return patcmp(bufpat, str);
}

/*
 * Сравнить шаблон вида "*.c", "???*", "*.[ch]" и т.п.
 * со строкой символов.
 * Сравнение начинается и заканчивается на границах строки.
 */
int patcmp(pat, str)
register char *pat;
register char *str;
{
	register int whileok;

	if (*pat == 0) return (1);
#ifdef RETRO
	if (*str == 0) return (0);
#endif
	for ( ; *pat; str++) {
		/* точно указанный символ */
		if (*pat == *str) {
			pat++; continue;
		}
		switch (*pat++) {
		case '?':
		/* один произвольный символ */
			if (*str) continue;
			break;
		case '[':
		/* символ из списка или диапазона */
			whileok = 0;
			while(*pat && *pat != ']') {
				if (pat[1] == '-') {
				/* диапазон символьного кода */
				    if (pat[2] && pat[2] != ']'
					&& *str >= *pat
					&& *str <= pat[2])
					whileok = 1;
				    pat++;pat++;pat++;
				}
				else if (*pat++ == *str)
					/* любой символ из списка */
					whileok = 1;
			}
			if (whileok && *pat == ']') {
				pat++; continue;
			}
			break;
		case '*':
		/* любое количество любых символов */
			if (*pat == 0) return(1);
			while(*str)
				if (patcmp(pat, str++))
					return(1);
			break;
		} /* конец switch */
		return(0);
	}
	if (*str == 0)
		return(1);
	return(0);
}

/*
 * common part of pmtrobj() and pmtrstr(), below...
 */
static
kbcod
r_pmt(pmtstr, line, savedf)
char *pmtstr;
LINE *line;
int savedf;
{
	kbcod cod;

	for ( ;; ) {
		w_msg(TXT, pmtstr);
		savedf = edinff;
		/*edinff = 0;     /* не показывать состояние редактора */
		cod = r_line(line, 0);
		edinff = savedf;
		switch (cod) {
		default:
			continue;
		case KB_TA:
		case KB_NL:
		case KB_CA:
		case KB_EX:
			return(cod);
			break;
		}
	}
	return(cod);
}

/*
 * Ввод строки с промптером, в последней строке экрана.
 */
kbcod
pmtrobj(pmtstr, obj, size)
char *pmtstr;   /* строка подсказки */
u8sobj_t *obj;      /* строковый объект для ввода */
int   size;     /* размер поля для ввода */
{
	kbcod cod;
	LINE  line;
	int   savedf;
	wcsobj_t *wobj;

	wobj = (wcsobj_t*)obj;
	if(wobj->wco_sig == WCO_SIG)
		line.flag = WCSOBJ;
	else if(obj->u8o_sig == U8O_SIG)
		line.flag = U8SOBJ;
	else
		return (0);

	line.size = size;
	line.colu = strlen(pmtstr) + 1; /*3;*/
	line.line = lframe->maxli - 1;
	line.attr = LVAR|INP|PMT;
	line.cvts = (char *)0;
	line.cvtf = (void *)0;
	line.test = (void *)0;
	line.varl = (char*)wobj;

	cod = r_pmt(pmtstr, &line, savedf);
	return(cod);
}

kbcod
pmtrstr(pmtstr, str, size)
char *pmtstr;   /* строка подсказки */
char *str;      /* строка для ввода */
int   size;     /* размер строки для ввода */
{
	kbcod cod;
	LINE  pmtline;
	int   savedf;

	if(str[0] == U8O_SIG)
		pmtline.flag = U8SOBJ;
	else
		pmtline.flag = 0;

	pmtline.size = size;
	pmtline.colu = strlen(pmtstr) + 1; /*3;*/
	pmtline.line = lframe->maxli - 1;
	pmtline.attr = LVAR|INP/*|PMT*/;
	/*NOSTRICT*/
	pmtline.cvts = (char *)0;
	/*NOSTRICT*/
	pmtline.cvtf = (void *)0;  /* тип указатель на функцию, возвр. int */
	/*NOSTRICT*/
	pmtline.test = (void *)0;  /* тип указатель на функцию, возвр. int */
	pmtline.varl = str;

	cod = r_pmt(pmtstr, &pmtline, savedf);
	return(cod);

}

/*
 * Разбить имя файла на компоненты.
 *
 * Имя файла from копируется в выходную строку out.
 * Режим задается переменной sufmode:
 * sufmode == 0         копируется имя до последнего суффикса;
 * sufmode == 1         копируется суффикс с точкой;
 * sufmode == 2         копируется суффикс без точки.
 */
fnsplit(to, from, sufmode)
	 char *to;      /* куда копировать */
register char *from;    /* откуда копировать */
int sufmode;            /* режим суффикса */
{
	register char *s;
	register char *saveds;  /* указатель на последнюю точку в строке */

	*to = '\0';
	saveds = (char *)0;
	for (s = from; *s; s++)
		if (*s == '.' )
			saveds = s;
	if (saveds != (char *)0 &&
			(saveds == from ||
					saveds[1] == '\0'))
		saveds = (char *)0;
	if (sufmode) {
		/* скопировать суффикс */
		if (s = saveds) {
			/* если суффикс был найден */
			if (sufmode == 2) s++;
			strcpy(to, s);
		}
		return 0;
	}
	else {
		/* скопировать имя до точки суффикса */
		for (s = from; *s && s != saveds; )
			*to++ = *s++;
		*to = '\0';
	}
}

const static wchar_t wc_esc_sh[] = L"|\\' \t\r`~!@#\";$%()[]{}^&*?<>";
int sh_wcesc(outs, inps, mode_quote)
register wchar_t *outs;
register wchar_t *inps;
int mode_quote; /* 0 - простое экранирование каждого символа, 1 - кавычки вокруг строки */
{
	int in_quote = 0; /* режим квотирования, если ноль то strcpy */
	wchar_t c;
	wchar_t *inps_keep;

	inps_keep = inps;

	while (*inps != '\0') {
		c = *inps++;
		if (wcschr(wc_esc_sh, c)) {
			in_quote = 1;
		} else {
			in_quote = 0;
		}
		if (mode_quote == 0) {
			if (in_quote) {
				*outs++ = '\\';
			}
			*outs++ = c;
		}
	}

	if (mode_quote) {
		inps = inps_keep;
		if (in_quote == 0) {
			strcpy(outs, inps);
			return 0;
		} else {
			*outs++ = '\'';
			while (*inps != '\0') {
				if (*inps == '\'' /*|| *inps == '$'*/) {
					*outs++ = '\'';
					*outs++ = '\\';
					*outs++ = *inps++;
					*outs++ = '\'';
				} else {
					*outs++ = *inps++;
				}
			}
			*outs++ = '\'';
		}
	}
	*outs++ = '\0'; /* string termination after copy */
	return 1;
}
/*
 * TODO: закончить эксперимент с экранированием имен файлов c метасимволами (пока в этом файле...)
 * три варианта экранирования метасимволов sh:
 * - в одиночных кавычках
 * - в двойных кавычках
 * - без кавычек
 *
 * имена (файлов) подставляются в команду sh,
 * а также участвуют в подстановках для пробы (#@, #*, и т.п.)
 *
 * соответственно, нужна прямая и обратная функция окавычивания строки,
 * но можно обойтись только прямой функцией
 *
 * фозвращает 0, если имя было без метасимволов, 1 если пришлось экранировать
 */
int sh_esc(outs, inps)
register char *outs;
register char *inps;
{
	int in_quote = 0; /* режим квотирования, если ноль то strcpy */
	char c;
	char *inps_keep;
	int mode_quote = 0; /* 0 - простое экранирование каждого символа, 1 - кавычки вокруг строки */

	inps_keep = inps;

	while (*inps != '\0') {
		c = *inps++;
		switch (c) {
		case '|':
		case '\\': case '\'':
		case ' ': case '\t': case '\r':
		case '`': case '~':
		case '!': case '@': case '#': case '"': case ';':
		case '$': case '%': case '(': case ')':
		case '[': case ']': case '{': case '}':
		case '^': case '&': case '*': case '?':
		case '<': case '>':
				in_quote = 1;
				break;
		default:
				in_quote = 0;
				break;
		}
		if (mode_quote == 0) {
			if (in_quote) {
				*outs++ = '\\';
			}
			*outs++ = c;
		}
	}

	if (mode_quote) {
		inps = inps_keep;
		if (in_quote == 0) {
			strcpy(outs, inps);
			return 0;
		} else {
			*outs++ = '\'';
			while (*inps != '\0') {
				if (*inps == '\'' /*|| *inps == '$'*/) {
					*outs++ = '\'';
					*outs++ = '\\';
					*outs++ = *inps++;
					*outs++ = '\'';
				} else {
					*outs++ = *inps++;
				}
			}
			*outs++ = '\'';
		}
	}
	*outs++ = '\0'; /* string termination after copy */
	return 1;
}

/*декодировать строку имени файла, убрать экранирование /bin/sh*/
sh_unesc(to, from)
char *to;
char *from;
{
	while(*from != '\0')
	{
		switch ((int)*from) {
		case '\\':
			from++;
			break;
		default:
			break;
		}
		*to++ = *from++;
	}
	*to = '\0';
}

/*
 * Подставить часть строки, определенную в настройке -@
 *
 * пока реализовано два(три) варианта:
 * -@3          в примере третье поле (разделенное пробелами), номер поля задается числом
 * -@7$			седьмое поле и до конца строки (пробелы после 7 поля игнорируются)
 * -@20-32      вырезка всех знаков в строке между указанными колонками
 */
static char out_str[800] = "";
char *nmsubs(inps, s)
char *inps;
char *s;        /* формат для подстановки */
{
	int a,b;
	register int i;
	register char *p;
	char tmps[20];
	int toend = 0;

	while (isspace(*s)) s++;
	if (*s == '\0') goto no_subs;

	/* первое число */
	p = tmps;
	while (isdigit(*s)) *p++ = *s++;
	*p = '\0';
	a = atoi(tmps);
	/* если до конца строки */
	if (*s == '$') {
		toend++;
		s++;
	}

	while (isspace(*s)) s++;
	if (*s == '\0') {
		/* подставить номер слова из inps */
		while(a>1) {
			while (   isspace(*inps) ) inps++;
			while ( ! isspace(*inps) ) inps++;
			a -= 1;
		}
		while (   isspace(*inps) ) inps++;
		p = out_str;
		if(toend) {
			while (*inps != '\0') *p++ = *inps++;
		} else {
			while (isspace(*inps) == 0) *p++ = *inps++;
		}
		*p = '\0';
	}
	else if (*s++ == '-') {
		while (isspace(*s)) s++;
		/* второе число */
		p = tmps;
		while (isdigit(*s)) *p++ = *s++;
		*p = '\0';
		b = atoi(tmps);

		/* подставить диапазон (вырезка) */
		p = out_str;
		for (i=a; i < b && isspace(inps[i]); i++)
			/* пропускаем лидирующие пробелы */
			;
		for (   ; i < b; i++)
			*p++ = inps[i];
		*p = '\0';
	}
	else {

no_subs:
		strcpy(out_str, inps);
		/*sh_esc(out_str, inps);*/
	}
	return(out_str);
}
