#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include "line.h"
#include "assist.h"
#include "astat.h"

#define WSMAX 300

/*char    Cfill[STRBUF] = "";*/
/*u8char_t Cfill_x[2048+4];*/
/*volatile */
char/*u8sobj_t*/
	Cfill_o[U8_STRBUF + 4] = "";	/* real initialisation will done in main.c */
char     *Cfill;			/*fill main view command*/
char    Crepf[STRBUF] = "";	/*reference file*/
char    Csubs[STRBUF] = "";	/*substitution rule*/
char    Coutf[STRBUF] = ""; /*not used anymore*/

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
		ofs = lframe->maxco/10;
		if (ofs > 8) ofs = 8;  /* no expanded look, last key label followed with clear_to_end_of_line */
		if ((ofs * 10) > lframe->maxco) ofs -= 1;
		base = 0;

		for (i = 0, k = keyonp;
				i <= 9 && k->kc_key;
					base += ofs, i++, k++) {
			if ( i ) {
				/* right down corner is "dangerouse", so all labels shown shifted left */
				at_set(TXT);
				w_chr(' ');
			}
			cp_set(-1, base, /*TXT*/LKEY);
			w_chr(k->kc_key);
			sprintf(tmpstr, "%-6.6s", k->kc_cap);
			at_set(HDR);
			w_str(tmpstr);
		}
		/*er_eol(HDR);*//* does not work for monochrome attributes*/
		for (i=base; i < lframe->maxco; i++) {
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
 * skip spaces until next expr; stop on end of string
 */
char *skipsp(p)
char *p;
{
	while(*p != '\0' && *p != '\n' && isspace(*p))
		p++;
	return p;
}

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

char rcopts[50] = "";
static char pmtbuf[30] = "";
const char *pmtsh;
static void pmtsetup()
{
	if (pmtbuf[0] == '\0') {
		pmtbuf[0] = ' ';
		pmtbuf[1] = '\0';
	}
	if (getuid() == 0) {
		strcat(pmtbuf, "# ");
	} else {
		strcat(pmtbuf, "$ ");
	}
	pmtsh = pmtbuf;
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
		s = skipsp(s);
		apndstr(s, 1);
		break;
	case 'f': case 'i': case 'o': case '@': case ':': case 'p':
		switch (*s++) {
		case ':': p = rcopts; break;
		case 'p': p = pmtbuf; break;
		case 'f': p = Cfill; break;
		case 'i': p = Crepf; break;
		case 'o': p = Coutf; break;
		case '@': p = Csubs; break;
		}
		s = skipsp(s);
		if (*s) {
			/* trim line, clear last '\n' */
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

#define MAXFP 8
/*
 * source vash rc file
 */
int
cmdset(file)
const char *file;
{
    FILE *fp[MAXFP];	/* opened included files */
    int fpix;			/* current opened FILE pointer index*/
    char incfile[256];
    char *fname;

    char ws[WSMAX];     /* рабочая строка для чтения из файла */
    register char *p;
    register char *p1;
    
    int predef;         /* 1 if BSD, 0 if other */
    int noskip = 1;
    int isroot = 0;
    /*extern FILE *afopen();*/
    int wslen;

    isroot = getuid() == 0 ? 1 : 0;
    for (fpix=0; fpix<MAXFP; fpix++)
    	fp[fpix] = NULL;

    fpix = 0;
    fname = file;

    pmtbuf[0] = '\0';

    predef = vflag.predef;

	/* инициализация переменных сканирования */
	pc_ix = 0;
	kt2_ix = kt1_ix = -1;
	bufsp = bufss;
	xchr = '\0';

    while(fpix >= 0) {
open_include:
		if (fp[fpix] == NULL) {
			if((fp[fpix]=dafopen(fname, v.vapath, "r")) == NULL) {
				sprintf(ws, "can't read '%s' in vexdir='%s'", fname, v.vapath);
				w_msg(ERR, ws);
				return(0);
			}
			if (predump) {
				printf("#+++ fpix=%d ------------------- <@%s> \n", fpix, fname);
			}
		}
skip:
		while (fgets(ws, WSMAX, fp[fpix]) != NULL) {
			p = ws;

			if(*p == '|') {
				/* preprocessing a block of strings */
				p1 = skipsp(p + 1);
				if (*p1 == '+') {
					noskip = (predef == 1 ? 1 : 0);
				} else
				if (*p1 == '|') {
					noskip = (predef == 0 ? 1 : 0);
				} else
				if (*p1 == '-') {
					noskip = 1;
					p1++;
				}
				/* preprocessing single line of config */
				if (*p1 == '#') {
					if ( ! isroot)
						continue;
				}
				if (*p1 == '$') {
					if (isroot)
						continue;
				}
				/*if (*p1 == '#' || *p1 == '$')*/
				p1++; /*+*/
				p = skipsp(p1);
				if(*p == '|') p++; /* keep leading spaces if required */
			}
			if (noskip == 0) {
				continue;
			}

			if (predump && *p != '\0') {
				printf("%s", p);
			}
			/* 1st symbol determins processing of rest of the string */
			switch (*p) {
			case '#':
			case '\n':
				/* skip any comments and dummy strings */
				continue;
			case '@':
				p = skipsp(p+1);
				/* trim trailing EOL char or space for file name */
				wslen = strlen(p);
				while (wslen > 0) {
					wslen--;
					if (p[wslen] == '\n' || isspace(p[wslen])) {
						p[wslen] = '\0';
					}
				}
				strcpy(incfile, p);

				fname = &incfile[0]; /* new file included */
				fpix++;
				goto open_include;
				continue;
			case '-':
				/* strings like '-i', '-o',... */
				p++;
				setdef(p);
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
			/* TODO здесь мог быть анализ ошибок... */
		}
		if (predump) {
			printf("#--- fpix=%d -----------------------\n", fpix);
		}
		fclose(fp[fpix]); fp[fpix] = NULL;
		fpix--;
    }
	kt2_ix++; kt1_ix++; /* ЗАКРЫТЬ ТАБЛИЦЫ: */
	kt2[ kt2_ix].kt_key = (char *)0;
	kt1[ kt1_ix].kt_key = (char *)0;
	/* Настроить индексы для таблицы клавиш, после чего
	 * правильно отрабатываются синонимы */
	kt1tune(kt1_ix);

	parsopt(rcopts);

	pmtsetup();

    return(1);
}
