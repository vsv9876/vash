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

/*------*/
/* ЧАСЫ */
/*------*/
static time_t prevtime = (time_t)0;
static int nxtjflag;

showtime(on)
/* Управление семафором часов и др. асинхронных заданий */
{
	nxtjflag = on;
	if ( !on && clockf) {
		cp_set(1, maxco-8, TXT);
		fprintf(vttout, "%8.8s", "");
	}
}

showclck()
/*
 * Показать системные часы.
 */
{
	time_t curtime;
	struct tm *tp;
	struct tm *localtime();

	if (clockf) {

		time(&curtime);
		if (curtime != prevtime) {
			prevtime = curtime;

			tp = localtime(&prevtime);

#ifndef CP_SAV
			cp_sav();
#endif
			cp_set(1, maxco-8, ATT);
			fprintf(vttout, "%02d:%02d'%02d",
			tp->tm_hour, tp->tm_min, tp->tm_sec);
#ifndef CP_SAV
			cp_fet();
#endif
		}
	}
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

    if (cmailf) {
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

    for (l = vf,i = 0; l->size && i < yy; l++,i++) {
	cp_set(l->line, l->colu, TXT); er_eol();
    }
}

scrlnl()
/*
 * to scroll with newlines
 * продвинуть свиток на экране при помощи переводов строки
 */
{
	register int i;

	if (scrolf) {
		io_set(IO_TTYPE);
		for (i = y0; i < maxli; i++)
			putc('\n', stdout);
		io_set(IO_VIDEO);
	}
	else    er_pag();
}

fatal()
{
	cp_set(-1, 0, TXT);
	io_set(IO_TTYPE);
	fprintf(stderr, "FATAL ERROR - exit\n");
	exit(1);
}

itmshow()
/* показать положение "окна" */
{
	int showco; /* позиция нач. строки индикации окна */
	int showscale; /* масштаб показа, 1x, 2x, 3x, ... */
	register int i;
	int lxx, lyy, litmmax;

	/* показать положение окна */
	if (yy*xx < itmmax) {  /* если не все файлы на экране... */
		/* отцентрировать... */
/*
		showco = (maxco - (itmmax/yy))/2;
		if (showco < 0) showco = 0;
*/
		showco = 2;
		lxx = xx;
		lyy = yy;
		litmmax = itmmax;
		showscale = ((itmmax/lyy) / (maxco - 16)) + 1 ;

		cp_set(maxli - 2, showco, TXT);
		for (i = 0; i < itmmax; i += yy) {
			if (i >= itmofs && i < itmofs+(yy*xx)) {
				if (showscale == 1)	w_chr('='); else {
					if (((i/yy) % showscale) == 0) w_chr('%');
				}
			} else {
				if (showscale == 1) w_chr('-');
				else {
					if (((i/yy) % showscale) == 0) w_chr('.');
				}

			}
		}
	}
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
 * Ввод строки с промптером, в последней строке экрана.
 */
kbcod
pmtrstr(pmtstr, str, size)
char *pmtstr;   /* строка подсказки */
char *str;      /* строка для ввода */
int   size;     /* размер строки для ввода */
{
	kbcod cod;
	LINE  pattl;
	int   savedf;

	pattl.size = size;
	pattl.colu = strlen(pmtstr) + 3;
	pattl.line = maxli - 1;
	pattl.flag = 0;
	pattl.attr = LVAR|INP;
	/*NOSTRICT*/
	pattl.cvts = (char *)0;
	/*NOSTRICT*/
	pattl.cvtf = (void *)0;  /* тип указатель на функцию, возвр. int */
	/*NOSTRICT*/
	pattl.test = (void *)0;  /* тип указатель на функцию, возвр. int */
	pattl.varl = str;

	for ( ;; ) {
		w_msg(TXT, pmtstr);
		savedf = edinff;
		edinff = 0;     /* не показывать состояние редактора */
		cod = r_line(&pattl, 0);
		edinff = savedf;
		switch (cod) {
		default:
			continue;
		case KB_NL:
		case KB_EX:
			return(cod);
		}
	}
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
	if (saveds == from ||
			saveds[1] == '\0')
		saveds = (char *)0;
	if (sufmode) {
		/* скопировать суффикс */
		if (s = saveds) {
			/* если суффикс был найден */
			if (sufmode == 2) s++;
			strcpy(to, s);
		}
		return;
	}
	else {
		/* скопировать имя до точки суффикса */
		for (s = from; *s && s != saveds; )
			*to++ = *s++;
		*to = '\0';
	}
}

/*
 * Подставить часть строки, определенную в настройке -@
 *
 * пока реализовано два варианта:
 * -@3          третье поле, номер поля задается числом
 * -@20-32      вырезка всех знаков в строке между указанными колонками
 */

static char out_str[200] = "";
char *nmsubs(inps, s)
char *inps;
char *s;        /* формат для подстановки */
{
	int a,b;
	register int i;
	register char *p;
	char tmps[20];

	while (isspace(*s)) s++;
	if (*s == '\0') goto no_subs;

	/* первое число */
	p = tmps;
	while (isdigit(*s)) *p++ = *s++;
	*p = '\0';
	a = atoi(tmps);

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
		while (isspace(*inps) == 0) *p++ = *inps++;
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
	}
	return(out_str);
}
