#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include "line.h"
#include "assist.h"
#include "astat.h"

#define WSMAX 300

/*char    Cfill[MAXLICO] = "";*/
char    *Cfill;
char    Crepf[MAXLICO] = "";
char    Coutf[MAXLICO] = "";
char    Csubs[MAXLICO] = "";

/*NOXSTR*/
static  char dummy[] = "";
/*YESXSTR*/

struct  keycaps {
	kbcod   kc_key;         /* код клавиши */
	char   *kc_cap;         /* Название действия для панели */
	}
	keys0[11] = {
		{ '1', dummy },
		{ '2', dummy },
		{ '3', dummy },
		{ '4', dummy },
		{ '5', dummy },
		{ '6', dummy },
		{ '7', dummy },
		{ '8', dummy },
		{ '9', dummy },
		{ '0', dummy },
		{ 0,   (char *)0 },
	};

struct  keycaps *keyonp = keys0;        /* "горячие" клавиши */

keyshow(yes)
int yes;
{
	register struct keycaps *k;
	char    tmpstr[20];
	int i, base, ofs;
    if (yes) {
		/*ofs = 8;*/
		ofs = maxco/10;
		if (ofs > 8) ofs = 8;  /* no expanded look, last key label followed with clear_to_end_of_line */
		if ((ofs * 10) > maxco) ofs -= 1;
		base = 0;

		for (i = 0, k = keyonp;
				i <= 9 && k->kc_key;
					base += ofs, i++, k++) {
			if ( i ) {
				/* right down corner is "dangerouse", so all labels shown shifted left */
				at_set(TXT);
				w_chr(' ');
			}
			cp_set(-1, base, TXT);
			w_chr(k->kc_key);
			sprintf(tmpstr, "%-6.6s", k->kc_cap);
			at_set(HDR);
			w_str(tmpstr);
		}
		/*er_eol(HDR);*//* does not work for monochrome attributes*/
		for (i=base; i < maxco; i++) {
			w_chr(' ');
		}
    }
}

/*
 * Основные таблицы.
 * Сделаны статическими, потому что проще реализовать (пока).
 */
PATCMD  pc[PCMAX];      /* образец:действие */
KEYTAB  kt2[KT2MAX];    /* действие:табл(pc):ixbeg:ixend */
KEYTAB  kt1[KT1MAX];    /* клавиша:табл(pc):ixbeg:ixend */
static  char    bufss[BUFSMAX];

/*
 * Переменные для сканирования файла настройки
 * и заполнения таблиц.
 */

	/* первый символ в строке файла:*/
int     xchr;
	/* текущие индексы в таблицах: */
int     kt2_ix;
int     kt1_ix;
int     pc_ix;
	/* текущий указатель на буфер со строками: */
char   *bufsp;

/*
 * Добавить строку в буфер,
 * вернуть указатель на конец проч. части строки.
 * Читать не больше n слов, здесь это означает, что
 * пропускать не более n-1 пробелов.
 */
char *
apndstr(s, n)
register char *s;       /* строка, из которой добавлять */
int     n;              /* макс. кол-во слов, разд. пробелами */
{
	register char *p;

	p = bufsp;
	while (*s) {
		if (*s == '\n') break;
		if (isspace(*s) && (--n <= 0)) break;
		*p++ = *s++;
	}
	*p++ = '\0'; bufsp = p;
	return(s);
}

/*
 * Установить умолчания.
 */
int
setdef(s)
register char *s;
{
	register int i;
	register char *p;

	switch(*s) {
	case '0': case '1': case '2': case '3': case '4':
	case '5': case '6': case '7': case '8': case '9':
		/* НАЗВАНИЕ КЛАВИШИ ДЛЯ МЕНЮ */
		i = (int)*s++ - '0'; i--; if (i < 0) i = 9;
		keys0[i].kc_cap = bufsp;
		apndstr(s, 1);
		break;
	case 'f': case 'i': case 'o': case '@':
		switch (*s++) {
		case 'f':
			/* command line on start vash would be preferred */
			/*if (*Cfill != '\0')
				return(1);
			else*/
				p = Cfill;
			break;
		case 'i': p = Crepf; break;
		case 'o': p = Coutf; break;
		case '@': p = Csubs; break;
		}
		if (*s) {
			/* откусить '\n' в конце строки */
			strcpy(p, s); p[strlen(s)-1] = '\0';
		} else  *p = '\0';
		break;
	default:
		return(0);
	}
	return(1);
}

kt1tune(kt1x)
register int kt1x;
{
	register int i;
	for (i = kt1x - 1; i >= 0; i--, kt1x--) {
		if (kt1[kt1x].kt_ib == kt1[i].kt_ib)
			kt1[i].kt_ie = kt1[kt1x].kt_ie;
	}
}

int
cmdset(file)
char *file;
{
    FILE *fp;
    char ws[WSMAX];     /* рабочая строка для чтения из файла */
    register char *p;
    extern FILE *afopen();

    if ((fp=dafopen(file, vapath, "r")) == NULL) {
    	sprintf(ws, "can't read '%s' in vexdir='%s'", file, vexdir);
		w_emsg(ws);
		return(0);
    }
    /* инициализация переменных сканирования */
    pc_ix = 0;
    kt2_ix = kt1_ix = -1;
    bufsp = bufss;
    xchr = '\0';

    while (fgets(ws, WSMAX, fp) != NULL) {
	p = ws;

	switch (*p) {
	/* первый символ определяет поведение сканера */
	case '-':
		/* строка умолчаний */
		p++;
		setdef(p);
	case '#':
	case '\n':
		/* строки коментария и пустые отбрасываются */
		continue;
	case ' ':
	case '\t':
		/* строки таблиц (с отступом) */
		while (isspace(*p)) p++;
		pc[ pc_ix].pc_pat = bufsp;
		switch (xchr) {
		case ':':       /* kt1 */
			p = apndstr(p, 2);
			kt1[ kt1_ix].kt_ie  = pc_ix;
			break;
		case '+':       /* kt2 */
			p = apndstr(p, 3);
			kt2[ kt2_ix].kt_ie  = pc_ix;
			break;
		default:
			/* отбросить такую строку */
			continue;
		}
		while (isspace(*p)) p++;
		pc[ pc_ix].pc_cmd = bufsp;
		p = apndstr(p, 999);
		pc_ix++;
		continue;
	case '+':       /* начало таблицы действия */
		xchr = *p++; kt2_ix++;
		kt2[ kt2_ix].kt_tab = pc;
		kt2[ kt2_ix].kt_ib  = pc_ix;
		kt2[ kt2_ix].kt_ie  = pc_ix;
		kt2[ kt2_ix].kt_key = bufsp;
		p = apndstr(p, 999);    /* всю строку до конца */
		continue;
	case ':':       /* начало таблицы клавиши */
		xchr = *p++; kt1_ix++;
		kt1[ kt1_ix].kt_tab = pc;
		kt1[ kt1_ix].kt_ib  = pc_ix;
		kt1[ kt1_ix].kt_ie  = pc_ix;
		kt1[ kt1_ix].kt_key = bufsp;
		p = apndstr(p, 999);    /* всю строку до конца */
		continue;
	}
	/* здесь м.б. анализ ошибок... */
    }
    kt2_ix++; kt1_ix++; /* ЗАКРЫТЬ ТАБЛИЦЫ: */
    kt2[ kt2_ix].kt_key = (char *)0;
    kt1[ kt1_ix].kt_key = (char *)0;
    /* Настроить индексы для таблицы клавиш, после чего
     * правильно отрабатываются синонимы */
    kt1tune(kt1_ix);
    fclose(fp);
    return(1);
}
