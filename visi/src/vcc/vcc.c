/*
 *      vcc = video + cc
 *
 *      $Header: vcc.c,v 1.7 90/12/27 16:35:38 vsv Exp $
 *      $Log:	vcc.c,v $
 * Revision 1.7  90/12/27  16:35:38  vsv
 * Добавлен USRATTR и работа с ним.
 * 
 * Revision 1.6  89/08/29  15:59:05  vsv
 * ВЕРСИЯ LINLIB_3
 * 
 * Revision 1.5  89/02/02  15:10:11  vsv
 * ИСПРАВЛЕНА ОШИБКА РАЗБОРА СТРОК СЕКЦИИ ---LINES,
 * ПРИВОДИВШАЯ К ВЫДАЧЕ СООБЩЕНИЯ '=' ДОЛЖЕН БЫТЬ ПОСЛЕ ИМЕНИ
 * В СОВЕРШЕННО ПРАВИЛЬНЫХ ОПИСАНИЯХ
 * 
 * Revision 1.4  89/01/13  17:55:49  vsv
 * ПОСЛЕДНАЯ ВЕРСИЯ, НЕ ПОПАВШАЯ В АРХИВ.
 * СОХРАНЕНА ПЕРЕД ПЕРЕДЕЛКОЙ ПОСЛЕ
 * БОЛЬШОГО ПЕРЕРЫВА.
 * 
 * Revision 1.3  88/06/17  19:23:59  vsv
 * ИСПРАВЛЕНЫ оШИБКИ vlbp
 * 
 * Revision 1.2  88/06/01  09:57:22  vsv
 * КЛЮЧИ МОГУТ БЫТЬ КАК В НИЖНЕМ, ТАК
 * И В ВЕРХНЕМ РЕГИСТРЕ
 * 
 * Revision 1.1  88/05/14  13:03:12  vsv
 * Initial revision
 * 
 */

#include <stdlib.h>
#include <locale.h>
#include <string.h>
#include <ediag.h>
#include <stdio.h>
#include <ctype.h>
#include <wchar.h>
#include "line.h"
#include "linebp.h"
#include "vgen.h"

/*extern  char *malloc();*/

char    Enomem[] = "No memory (%s)\n";
char    Rnomem[] = "Нет памяти (%s)\n";

int     nlin = 0;       /* НОМЕР СЧИТАННОЙ ИЗ ФАЙЛА.lav СТРОКИ */
int  lbptyp = 0;        /* ТЕКУЩИЙ ТИП (ИЗ КАКОЙ СЕКЦИИ "ИМЯ=" */
int  vscan  = 0;        /* ФЛАГ: ОБРАЗ ЭКРАНА СКАНИРУЕТСЯ ПО ВЕРТИКАЛИ */

char    Esxerr[] = "%s: %d:Syntax err:  %s\n";
char    Rsxerr[] = "%s: %d:Синтакс. ош.: %s\n";

/*
#if ! defined(DEBUG)
#define MAKE_VCC
#undef MAKE_VLBP
#endif
*/

#ifdef MAKE_VCC
char    pag_id[] = "---PAGE";   /* НАЧАЛО ОПРЕДЕЛЕНИЯ СТРАНИЦЫ */
#endif

#ifdef FUTURE
char    vis_id[] = ";";
char    com_id[] = ";";
#else
char    vis_id[] = "---";
char    com_id[] = "#";
#endif
char    por_id[] = "PORTS";
char    tab_id[] = "TABLES";
char    lin_id[] = "LINES";
char    scr_id[] = "SCREEN";
char    hel_id[] = "HELP";
char    end_id[] = "END";
wchar_t    markl = L'.';

#define isnamec(c) (isalnum(c) || c == '_')

/*----------------------------*/
/* ОПИСАНИЕ ВНУТРЕННИХ ДАННЫХ */
/*----------------------------*/
typedef struct {
	int     nmt;            /* ТИП */
	char   *nms;            /* СТРОКА, ГДЕ ИМЯ ВСТРЕЧАЕТСЯ */
} NM_DEF;

/* ПРИМЕЧАНИЕ:
 * 1)   ЕСЛИ ИМЯ НЕ ВСТРЕЧАЕТСЯ В ЭТОЙ ТАБЛИЦЕ, ТО
 *      ОНО СООТВЕТСТВУЕТ СТРОКЕ С ТАКИМ ИМЕНЕМ.
 */

/* ТИПЫ СЕКЦИЙ ДЛЯ ТАБЛИЦЫ ОПРЕДЕЛЕНИЙ ИМЕН */

#define DPORT   1
#define DTABLE  2
#define DLINE   3

/* КУЧА СТРОК С ОПРЕДЕЛЕНИЯМИ ИМЕН */
typedef struct {
	char    *top;           /* НАЧАЛО КУЧИ */
	char    *bot;           /* КОНЕЦ, МЕСТО ДЛЯ НОВОЙ ЗАПИСИ */
	linptr_t siz;           /* РАЗМЕР ПОЛЕЗНОГО МЕСТА В БАЙТАХ */
	NM_DEF  *nmd;           /* ТАБЛИЦА: ИЗ КАКОЙ СЕКЦИИ СТРОКА */
	NM_DEF  *nmp;           /* УКАЗАТЕЛЬ НА НОВЫЙ ОПИСАТЕЛЬ ТИПА */
	int     ncnt;           /* СЧЕТЧИК СТРОК */
	int     nmax;           /* МАКСИМАЛЬНОЕ КОЛ-ВО СТРОК В КУЧЕ */
	} HEAP;

HEAP    shp  = {0};     /* КУЧА СТРОК ОПИСАНИЙ lbp */

wchar_t    *scrp = 0;      /* БУФЕР ОБРАЗА ЭКРАНА = malloc(80*24); */

/* УКАЗАТЕЛИ СКАНИРОВАНИЯ ЭКРАНА */
int     cur_li = 0;     /* СТРОКА */
/*int     cur_co = 0;     /* ПОЗИЦИЯ */

char lbpo[LBPOSIZE] = {0};       /* БУФЕР ФОРМИРОВАНИЯ СТРОКИ В ФОРМАТЕ lbp */

/*---- ФАЙЛЫ: ИМЕНА И УКАЗАТЕЛИ НА ПОТОКИ */
char ifn[50];
char ofn[50];
char hvfn[50];

FILE *ifp;
FILE *hvfp;

/*-------  ДИАГНОСТИКА ------*/
usage(s)
char *s;
{
#ifdef MAKE_VLBP

	fprintf(stderr,
	"ВЫЗОВ:\n %s [-h file.hv] [-o file.la] file.lav\n",
	s);
#endif
#ifdef MAKE_VCC
	fprintf(stderr,
	"ВЫЗОВ:\n %s [-o file.c] file.cv\n",
	s);
#endif
	exit(1);
}

int     cnterr = 0 ;    /* СЧЕТЧИК ОШИБОК */


main(argc, argv)
/*----------*/
/*   MAIN   */
/*----------*/
int  argc;
char *argv[];
{
	register int acnt;
	char *s;

	_setediag();

	/*TODO locale constant UTF8 + diag msg*/
	if (!setlocale(LC_CTYPE, "")) {
		fprintf(stderr, "Can't set the specified locale! "
			"Check LANG, LC_CTYPE, LC_ALL.\n");
		return 1;
	} else {
		mb_cur_max = MB_CUR_MAX;
	}

	if (argc <  2) {
		usage(argv[0]);
	}

	for ( acnt=1; acnt < argc ; acnt++ ) {
		if ( (int)argv[acnt][0] == '-' ) {
			switch( (int)argv[acnt][1] ) {
			case 'h' :
			case 'H' :
				strcpy(hvfn, argv[++acnt]);
				break;
			case 'o' :
			case 'O' :
				acnt += 1;
				strcpy( ofn, argv[acnt]);
				break;
			default:
				usage(argv[0]);
				break;
			};
		} else if ( ifn[0] == '\0') {
			strcpy( ifn, argv[acnt]);
		} else {
			usage(argv[0]);
		}
	}

	/* ОТКРЫТЬ ФАЙЛ.lav */
	if ((ifp=fopen(ifn, "r")) == NULL) {
		perror(ifn);
		exit(1);
	}
	/* ЗАПРОСИТЬ ПАМЯТЬ ДЛЯ ОБРАЗА ЭКРАНА */
	if ( (scrp=calloc(4, MAX_BUF_CO * MAX_BUF_LI))==NULL ) {
		fprintf(stderr, ediag(Enomem, Rnomem), "scrp");
		exit(1);
	}
	/* ОТКРЫТЬ ОСНОВНОЙ ВЫВОДНОЙ ФАЙЛ */
	if(*ofn) {
		if ((freopen(ofn, "w", stdout)) == NULL) {
			perror(ofn);
			exit(1);
		}
	}

#ifdef MAKE_VCC
    /*----------- В ОДНОМ ФАЙЛЕ М.БЫТЬ БОЛЬШЕ ОДНОЙ СТРАНИЦЫ */
    while(fgets(lbpo, LBPOSIZE, ifp), feof(ifp) == 0) {

      if (str_eq(lbpo, pag_id) == 0) {
    	  printf("%s", lbpo);             /* ТЕКСТ НА СИ */
      }
      else {
    	  s = lbpo + strlen(pag_id);      /* СНАЧАЛА НАЙТИ ИМЯ СТРАНИЦЫ */
    	  while(isspace(*s)) s++;
    	  s[ strlen(s) - 1 ] = '\0';
    	  printf("LINE %s[] = {\n", s);  /* НАЧАЛО ОПИСАНИЯ СТРАНИЦЫ */
#endif
		/* ИНИЦИАЛИЗИРОВАТЬ КУЧУ СТРОК (РАЗМЕР_БУФЕРА, КОЛИЧЕСТВО_СТРОК) */
		ini_hs( &shp, HS_SIZE, HS_MAXNUM);

		/* ЗАПОЛНИТЬ КУЧУ СТРОК С ОПРЕДЕЛЕНИЯМИ ИМЕН;
		 * ЗАПОЛНИТЬ БУФЕР ЭКРАНА ИЗ СЕКЦИИ SCREEN;
		 */
		cur_li = 0;    /* СКАНИРОВАНИЕ ЭКРАНА НАЧАТЬ СНАЧАЛА */

		pass1();

		if ( cnterr ) {
			fprintf(stderr,
ediag("PASS1: Errors detected: %d\n", "PASS1: ОБНАРУЖЕНО ОШИБОК: %d\n"),
			cnterr);
			exit(1);
		}
		/* СКАНИРОВАТЬ ЭКРАН, ЗАПИСАТЬ ФАЙЛ.la */
		scan_s();

		/* ЕСЛИ ЗАТРЕБОВАН ФАЙЛ.hv, ЗАПИСАТЬ ЕГО,
		 * НО ПОКА НЕПОНЯТНО КАК ЭТО ЛУЧШЕ СДЕЛАТЬ */
/*      do_hv();        */

#ifdef MAKE_VCC
		des_hs( &shp ); /* УНИЧТОЖИТЬ КУЧУ СТРОК */

		printf( "%s\n\n",   "{ 0 }, };"  );

		} /*---------- КОНЕЦ СТРАНИЦЫ */
    } /*--------- ПРОДОЛЖИТЬ ПОИСК ДРУГИХ ОПИСАНИЙ СТРАНИЦ */
#endif

	fclose(ifp);
	exit(0);

}


/*--------- ПРОЦЕДУРЫ НИЖНЕГО УРОВНЯ ----------*/

str_eq(s, pat)
/*-------------------------*/
/* СРАВНИТЬ ИМЯ С ОБРАЗЦОМ */
/*-------------------------*/
register char *s;
register char *pat;
{
	/* ВЕРНУТЬ 0, ЕСЛИ НЕ СОВПАДАЕТ;
	 *         1, ЕСЛИ ТОЧНОЕ СОВПАДЕНИЕ;
	 */
	while ( *pat != '\0' ) {
		if ( *pat++ != *s++ )
			return(0);
	}
	return(1);
}

NM_DEF *fnd_nm(s)
char *s;
/*-------------------------*/
/* НАЙТИ ОПРЕДЕЛЕНИЕ ИМЕНИ */
/*-------------------------*/
{
	register NM_DEF *hpp;
	register linptr_t i;

	for (hpp=shp.nmd, i=0; i<shp.ncnt; i++, hpp++) {
		/* ЕСЛИ СОВПАДАЕТ s И "ИМЯ=..", ВЕРНУТЬ УКАЗАТЕЛЬ */
		if(str_eq(hpp->nms, s))
			return(hpp);
	}
	/* НЕ СОВПАДАЕТ: ВЕРНУТЬ NULL */
	return((NM_DEF *)NULL);
}

des_hs( hp )
/*-----------------------*/
/* уничтожить кучу строк */
/*-----------------------*/
register HEAP *hp;
{
	free( hp->nmd );
	free( hp->top );        /* освободить память в обратном порядке */
}

ini_hs( hp, siz, strnum )
/*-----------------------------*/
/* ИНИЦИАЛИЗИРОВАТЬ КУЧУ СТРОК */
/*-----------------------------*/
register HEAP *hp;
linptr_t siz;
int strnum;
{
	/*extern char *malloc();*/
	if((( hp->top = malloc(siz)) != NULL )
	&& (( hp->nmd =
	  (NM_DEF *)malloc(sizeof(NM_DEF)* strnum)) != NULL )) {
		hp->bot = hp->top;
		hp->siz = siz;
		hp->nmp = hp->nmd;
		hp->nmax = strnum;
		return(1);
	}
	return(0);
}

char *
c_ports(name)
/*---------------------------------------*/
/* ПО ИМЕНИ name ВЕРНУТЬ ЗНАЧ. ИЗ por_id */
/*---------------------------------------*/
char *name;
{
	static char nm_fnd[MAX_BUF_CO/*80*/+2]; /* ЗНАЧЕНИЕ ИМЕНИ (СТРОКА ПОСЛЕ "ИМЯ=") */
	register NM_DEF *hpp;   /* ОПРЕДЕЛЕНИЕ ИМЕНИ */
	register char *res;     /* РЕЗУЛЬТАТ */

	strcpy(nm_fnd, name); strcat(nm_fnd, "=");
	if(((hpp=fnd_nm(nm_fnd)) != (NM_DEF *)NULL)
	&& ((hpp->nmt) == DPORT)) {
		for(res=hpp->nms; *res++ != '='; ); /* ПРОПУСТИТЬ "ИМЯ=" */
		strcpy(nm_fnd, res);
	} else {
		nm_fnd[ strlen(nm_fnd) - 1 ] = 0;
	}
	return(nm_fnd);
}

char *
c_lines(lbps)
/*---------------------------------------------------------*/
/* СТРОКУ В ФОРМ. lbp ПРЕОБРАЗОВАТЬ К ФОРМАТУ СТРУКТ. LINE */
/*---------------------------------------------------------*/
register char *lbps;
{
	static  char nm_ret[100];    /* СТРОКА РЕЗУЛЬТАТА */
			/* ЭЛЕМЕНТЫ СТРОКИ В ФОРМАТЕ lbp: */
	char    fldtyp[20];     /* ТИП ПОЛЯ, НАПР "ipa" */
	char    susflg[20];     /* ФЛАГИ ЗАДЕРЖКИ */
	char    lpar[20];       /* ОЧЕРЕДНОЙ ПАРАМЕТР ЛИНИИ */
	int     multp;    /* ФЛАГ: НАДО ВСТАВИТЬ '|' ПЕРЕД СЛЕД. КОНСТ. */
	int     parn;   /* КОЛИЧЕСТВО ПАРАМЕТРОВ ДЛЯ ПРОВЕРКИ СИНТАКСИСА */
	char   *lbpssav;
	register char *s;
	register char *os;

	lbpssav = lbps;
	/* СНЯТЬ АТРИБУТЫ */
	for(s=fldtyp; !isspace(*lbps); lbps++)
		*s++ = (isupper(*lbps)) ? tolower(*lbps) : *lbps;
	*s = '\0';
	while(isspace(*lbps)) lbps++;

	/* СНЯТЬ ФЛАГИ */
	for(s=susflg; !isspace(*lbps); lbps++)
		*s++ = (isupper(*lbps)) ? tolower(*lbps) : *lbps;
	*s = '\0';
	while(isspace(*lbps)) lbps++;

	/* НАЧАТЬ ЗАПИСЫВАТЬ РЕЗУЛЬТАТ */
	os = nm_ret;

	/* НАРИСОВАТЬ ФЛАГИ В ФОРМАТЕ СИ */
	for(multp=0,s=susflg; *s; ) {
		if(multp)       *os++ = '|';
		else            multp = 1;
		switch(*s) {
		case  'U':
		case  'u': strcpy(os, "SUSU"); os += 4; break;
		case  'D':
		case  'd': strcpy(os, "SUSD"); os += 4; break;
		case  'L':
		case  'l': strcpy(os, "SUSL"); os += 4; break;
		case  'R':
		case  'r': strcpy(os, "SUSR"); os += 4; break;
		case  'N':
		case  'n': strcpy(os, "SUSNL"); os += 5; break;
		case  'O':
		case  'o': strcpy(os, "U8SOBJ"); os += 6; break;

		case  '*':
		case  '-': *os++ = '0'; break;
		default:
			fprintf(stderr, "'%c': %s\n", *s,
			ediag("bad flag","ПЛОХОЙ ФЛАГ"));
		}
		s++;
	}
	*os++ = ','; *os++ = '\t';

	/* НАРИСОВАТЬ АТРИБУТЫ В ФОРМАТЕ СИ */
	for(multp=0,s=fldtyp; *s; ) {
		if(multp)       *os++ = '|';
		else            multp = 1;
		switch(*s) {
		case  'I':
		case  'i': strcpy(os, "INP"); os += 3; break;
		case  'P':
		case  'p': strcpy(os, "PMT"); os += 3; break;
		case  'C':
		case  'c': strcpy(os, "LTXT"); os += 4; break;
		case  'H':
		case  'h': strcpy(os, "LHDR"); os += 4; break;
		case  'V':
		case  'v': strcpy(os, "LVAR"); os += 4; break;
		case  'A':
		case  'a': strcpy(os, "LALT"); os += 4; break;
		case  'E':
		case  'e': strcpy(os, "ERR"); os += 4; break;
		case  'X':
		case  'x': strcpy(os, "VEXT"); os += 4; break;
		case  'M':
		case  'm': strcpy(os, "LMSE"); os += 4; break;
		case  '1':
		case  '2':
		case  '3':
		case  '4':
		case  '5':
		case  '6':
		case  '7':
		case  '8':
		case  '9':
		case  '0': strcpy(os, "USRATTR");
			   os[7] = *s;
			   os += 8; break;

		case  '*':
		case  '-':
		case  'G':
		case  'g': *os++ = '0'; break;
		default:
			fprintf(stderr, "'%c': %s\n", *s,
			ediag("bad attribute","ПЛОХОЙ АТРИБУТ"));
		}
		s++;
	}

	/* ОСТАЛЬНЫЕ ПАРАМЕТРЫ */
	parn = 0;
	while( *lbps ) {

		parn++;
		/* РАЗДЕЛИТЕЛЬ ПЕРЕД ПАРАМЕТРОМ */
		*os++ = ','; *os++ = '\t';

		switch( *lbps ) {
		case '\0':
			goto ret;
		case  '*':
		case  '-':
			lbps++;
			/* ПАРАМЕТР ПУСТОЙ */
			*os++ = '0';
			break;
		case  '"':
			/* НАЙТИ СТРОКУ И СКОПИРОВАТЬ ЕЕ */
			do {
				*os++ = *lbps;
			} while( ! ((*lbps != '\\') && (*++lbps == '"') ));

			*os++ = *lbps++;
			break;
		default:

			/* НАКОПИТЬ ИМЯ */
			for(s=lpar; *lbps && !isspace(*lbps); )
				*s++ = *lbps++;
			*s = '\0';

			/* НАЙТИ ПОДСТАНОВКУ */
			sprintf(os, "%s", c_ports(lpar));
			os += strlen(os);
		}
		/* ПРОПУСТИТЬ ДО СЛЕД. ПАРАМЕТРА */
		while(*lbps && isspace(*lbps))  lbps++;
	}
ret:
	*os = '\0';

	/* НАДО БЫ ПРОВЕРИТЬ КОЛИЧЕСТВО НАЙДЕННЫХ ПАРАМЕТРОВ... */
	/** parn = 1 (c,h); ДЛЯ ОСТАЛЬНЫХ parn = 5 **/
	if(*fldtyp=='c' || *fldtyp=='h') {
		if(parn == 1 )
			return(nm_ret);
		else
			goto err_exit;
	} else if(parn == 4) {
		return(nm_ret);
	} else { ; }
err_exit:
	fprintf(stderr, "%s: '%s'\n",
	ediag("Bad parameters count","ПЛОХОЕ КОЛИЧЕСТВО ПАРАМЕТРОВ"), lbpssav);

}


subst(siz)
/*---------------------*/
/* СДЕЛАТЬ ПОДСТАНОВКИ */
/*---------------------*/
int siz;        /* РАЗМЕР ПОЛЯ */
{
	static  char subs[300];         /* СТРОКА С ПОДСТАНОВКАМИ */
	int     prompted = 0;           /* ПОЛЕ ИМЕЕТ ПОДСКАЗКУ */
	register char *from;
	register char *to;
	int     comma;

	prompted = 0;
	to = subs;
	from = lbpo;
#ifdef MAKE_VLBP
	while(!isspace(*from)) {
		if(*from == 'p' || *from++ == 'P') prompted = 1;
		from++;
	}
#endif
#ifdef MAKE_VCC
	comma = 0;
	while(*from) {
		if(*from == ',') comma++;
		if(comma == 4 && *from=='P'
		&& from[1]=='M' && from[2]=='T')  /* attr == PMT ? */
			prompted = 1;
		from++;
	}
#endif
	from = lbpo;
	while(*from) {
		if(*from == '$' && from[1] == '#') {      /* РАЗМЕР ПОЛЯ */
			from++, from++;
			sprintf(to, "%d", siz-prompted); /* ПОЛЕЗНЫЙ РАЗМЕР*/
			while(*to) to++;        /* КОНЕЦ ПОСЛЕ ПОДСТ.*/
		}
		else {
			*to++ = *from++;
		}
	}
	*to = '\0';

	/* ЕСЛИ НЕ УКАЗАН КЛЮЧ -o, РЕЗУЛЬТАТ НА ТЕРМИНАЛЕ */
	printf("%s\n", subs);

}

#ifdef MAKE_VCC

mk_vcc(siz, li, co, s, typ)
/*----------------------------------------*/
/* ПРЕОБРАЗОВАТЬ К ФОРМАТУ СТРУКТУРЫ LINE */
/*----------------------------------------*/
int     siz;
int     li;
int     co;
register wchar_t *s;    /* СОДЕРЖИМОЕ ПОЛЯ */
int     typ;            /* 'h', 'c', 'k', 'K', '?' */
{
	/* РЕЗУЛЬТАТ ПОПАДАЕТ В СТРОКУ lbpo;
	 * ФУНКЦИИ mk_lbp И mk_vcc ОЧЕНЬ ПОХОЖИ,
	 * ОДНА СДЕЛАНА ИЗ ДРУГОЙ копипастой;
	 * ПОДСТАНОВКИ И ВЫВОД В ФАЙЛ ДЕЛАЮТСЯ
	 * ФУНКЦИЕЙ subst;
	 */
	static  char mb_s[8]; /* символ в кодировке UTF-8, включая терминатор строки '\0' */
			char *mb_ptr;
	static  char nm_fnd[MAX_BUF_CO + 2/*82=80+2*/];        /* ИМЯ, КОТОРОЕ НУЖНО НАЙТИ */
	register NM_DEF *hpp;           /* ОПРЕДЕЛЕНИЕ ИМЕНИ */
	register char *os;
	register char *o2s;
	char *typp;

	if(typ == '?') {        /* ПОЛЕ С ИМЕНЕМ, НАДО СДЕЛАТЬ ПОДСТАНОВКУ */
		/*strcpy(nm_fnd, s);*/
		sprintf(nm_fnd, "%ls", s);
		strcat(nm_fnd, "=");
		if(((hpp=fnd_nm(nm_fnd)) != (NM_DEF *)NULL)
		&& ((hpp->nmt) != DPORT)) {
			os = (hpp->nms);
			while(*os++ != '=');    /* НАЙТИ ЗНАЧЕНИЕ ИМЕНИ */
			  strcpy(nm_fnd, os);
			  /*sprintf(nm_fnd, "%s", os);*/
			  strcat(nm_fnd, "=");
			  if(hpp->nmt == DTABLE) {   /* РАСШИР. ТАБЛ. */
			  sprintf(lbpo,
			  "{ %2d,%2d,%2d, SUST|%s, 0,0,0,0, %s },",
			  siz, li, co, vscan?("SUSL"):("SUSU"),
			  c_ports(os));
			} else
			if(hpp->nmt == DLINE) {
				sprintf(lbpo,
				"{ %2d,%2d,%2d, %s },",
				siz, li, co, c_lines(os));
		       }
		} else {
			/* УМОЛЧАНИЕ: СТРОКА ДЛЯ ВВОДА */
			sprintf(lbpo,
			"{ %2d,%2d,%2d, 0, LVAR|INP|PMT, 0,0,0, %ls },",
			siz, li, co, s);
		}
	} else
	if(typ == 'h' || typ == 'c' || typ == 'k') {
		switch (typ) {
		case 'h': typp = "LHDR"; break;
		case 'c': typp = "LTXT"; break;
		case 'k': typp = "LKEY"; break;
		}
		sprintf(lbpo, "{ %2d,%2d,%2d, 0, %s, 0,0,0, ",
		siz, li, co, typp );

		for(os=lbpo; *os; os++) ;       /* НАЙТИ КОНЕЦ */
		*os++ = '"';
		while(*s) {
			if(*s == '"' || *s == '\\') *os++ = '\\';
			/*sprintf(os++, "%lc", *s++);*/
			sprintf(mb_s, "%lc", *s++);
			for(mb_ptr = mb_s; *mb_ptr != '\0'; mb_ptr++) {
				*os++ = *mb_ptr;
			}
		}
		strcpy(os, "\" },");
	} else
	if(typ == 'K') {
		sprintf(lbpo,
   "{ %2d,%2d,%2d, 0, LKEY, 0, cvt_lh, 0, \"%ls\" },",
		siz, li, co, s);
	}
	subst(siz);        /* СДЕЛАТЬ ПОДСТАНОВКИ */
}
#endif

#ifdef MAKE_VLBP

mk_lbp(siz, li, co, s, typ)
/*-------------------------------*/
/* ЗАПИСАТЬ СТРОКУ В ФОРМАТЕ lbp */
/*-------------------------------*/
int     siz;
int     li;
int     co;
register wchar_t *s;
int     typ;
{
	static  char mb_s[8]; /* символ в кодировке UTF-8, включая терминатор строки '\0' */
			char *mb_ptr;
	static  char nm_fnd[MAX_BUF_CO + 2/*82=80+2*/];        /* ИМЯ, КОТОРОЕ НУЖНО НАЙТИ */
	register NM_DEF *hpp;           /* ОПРЕДЕЛЕНИЕ ИМЕНИ */
	register char *os;
	register char *o2s;

	if(typ == '?') {        /* ПОЛЕ С ИМЕНЕМ, НАДО СДЕЛАТЬ ПОДСТАНОВКУ */
		/*strcpy(nm_fnd, s);*/
		sprintf(nm_fnd, "%ls", s);
		strcat(nm_fnd, "=");
		if(((hpp=fnd_nm(nm_fnd)) != (NM_DEF *)NULL)
		&& ((hpp->nmt) != DPORT)) {
			os = (hpp->nms);
			while(*os++ != '=');    /* НАЙТИ ЗНАЧЕНИЕ ИМЕНИ */
			if(hpp->nmt == DTABLE) {
			sprintf(lbpo,
			"g\t%2d %2d %2d\tt%c - - - %s",
			siz, li, co, vscan?('l'):('u'), os);
			} else
			if(hpp->nmt == DLINE) {
				o2s = lbpo;
				while(!isspace(*os))   /* АТРИБУТЫ */
					*o2s++ = *os++;
				while( isspace(*os)) os++; /* ДЛЯ КРАСОТЫ*/
				sprintf(o2s,
				"\t%2d %2d %2d\t%s",
				siz, li, co, os);
		       }
		} else {
			sprintf(lbpo,
			"ip\t%2d %2d %2d\t- - - - %ls",
			siz, li, co, s);
		}
	} else
	if(typ == 'h' || typ == 'c') {
		sprintf(lbpo, "%c\t%2d %2d %2d\t", typ, siz, li, co);
		for(os=lbpo; *os; os++) ;       /* НАЙТИ КОНЕЦ */
		*os++ = '"';
		while(*s) {
			if(*s == '"') *os++ = '"';  /* ТАКОВ ФОРМАТ lbp...*/
			/**os++ = *s++;*/
			sprintf(mb_s, "%lc", *s++);
			for(mb_ptr = mb_s; *mb_ptr != '\0'; mb_ptr++) {
				*os++ = *mb_ptr;
			}
		}
		*os++ = '"'; *os = '\0';
	} else
	if(typ == 'k') {
		sprintf(lbpo, "%c\t%2d %2d %2d\t- - - - \"%ls\"",
			typ, siz, li, co, s);
	} else
	if(typ == 'K') {
		sprintf(lbpo, "%c\t%2d %2d %2d\t- - cvt_lh - \"%ls\"",
			typ, siz, li, co, s);
	}
	subst(siz);        /* СДЕЛАТЬ ПОДСТАНОВКИ */
}
#endif


mk_lin(li, co)
/*----------------------------------------*/
/* НАЙТИ ИНФОРМАЦИЮ ДЛЯ ФОРМИРОВАНИЯ ПОЛЯ */
/*----------------------------------------*/
int li;
int co;
{
	/* ПОСЛЕ ТОГО, КАК СТРОКА lbp ЗАПИСАНА,
	 * В БУФЕРЕ ОБРАЗА ЭКРАНА НЕ ОСТАВЛЯЕТСЯ
	 * НИКАКОГО СЛЕДА ОТ ПОЛЯ - ВСЕ РАСПИСЫВАЕТСЯ ПРОБЕЛАМИ.
	 */
	register wchar_t *s;
	register wchar_t *os;    /* УКАЗАТЕЛЬ ДЛЯ КОПИРОВАНИЯ */
	         wchar_t *fbeg;  /* СИМВОЛ В ПОЗИЦИИ НАЧАЛА ПОЛЯ */
	static   wchar_t  stro[MAX_BUF_CO/*80*/];  /* ЗДЕСЬ НАКОПИТЬ СТРОКУ СОДЕРЖИМОГО ПОЛЯ */
			 linptr_t spcnt; /* СЧЕТЧИК ПРОБЕЛОВ В КОНЦЕ КОММЕНТАРИЯ */
			 int  fldsiz;    /* РАЗМЕР ПОЛЯ */
			 int  li_lbp, co_lbp;    /* КООРДИНАТЫ ПОЛЯ ДЛЯ lbp */
			 int  typ;               /* ТИП ПОЛЯ (c,h,k,?) */

	spcnt = fldsiz = 0;
	os = &stro[0];
	fbeg = s = &scrp[(MAX_BUF_CO/*80*/ * li) + co]; /*индекс в координатах экрана*/
	li_lbp = li; co_lbp = co;       /* СОХРАНИТЬ КООРДИНАТЫ */

	if(*fbeg == markl) {  /*--------------* ПОЛЕ С ИМЕНЕМ */
		s++; fldsiz = 1; typ = '?';
		while (co++, isnamec(*s)) {      /* НАКОПИТЬ ИМЯ ПОЛЯ */
			fldsiz++; *os++ = *s++;
		}
		if (fldsiz == 1) {       /* МОЖЕТ, ЭТО БЫЛО НЕ ИМЯ ПОЛЯ ?: */
			if(fbeg[1] == L'"') {         /* ЗАГОЛОВОК ? */
				fldsiz = 2; s = &fbeg[2]; typ = 'h';
				while(co++<MAX_BUF_CO/*80*/) {
					fldsiz++;
					if (*s == '"' && s[1] == markl) {
						s++; break;
					}
					else {
						*os++ = *s++;
					}
				}
			} else
			if(fbeg[1] == L'\'') {  /*имя кнопки*/
				if (fbeg[1] == L'\'') {
					typ = 'k'; /* без расшифровки*/
				}
				s++;
				*os = fbeg[1]; /* в качестве индикатора ошибки - на экране будет только символ кавычки */
				fldsiz = 2; /*меньше скипнуть нельзя*/
				while(co < MAX_BUF_CO/*80*/ && *s != L' ') {
					co++; *os++ = *s++; fldsiz++;
				}
			} else
			if(fbeg[1] == ':') {  /* имя кнопки с расшифровкой через kbl */
				typ = 'K'; spcnt = 3;
				while(spcnt--) {        /* ОШ. НЕ ЛОВЯТСЯ */
					co++; *os++ = *s++; }
				fldsiz = 4;     /* ЛУЧШЕ БЫ 7..8 */
			} else {
				fprintf(stderr,
ediag("Screen scan err:line %d, col %d\n", "Ош. сканирования: стр %d, поз. %d\n"),
				li, co); cnterr++;
			}

		}
		while(*s++ == markl && (co++)<MAX_BUF_CO/*80*/) {
			fldsiz++;       /* НАКОПИТЬ ЕГО РАЗМЕР */
		}
		*os = L'\0';
		goto fill_end;
	} else
	if(*fbeg == '=' && co_lbp == 0) { /*-----* ЗАГОЛОВОК НА ВСЮ ШИРИНУ */
		co++; s++;
		while(co++ < MAX_BUF_CO/*80*/ ) {
			if(*s==' ') {
				if((spcnt++) >=4)    break;
			} else {
				spcnt = 0;
			}
			*os++ = *s++;
		}
		*os = L'\0'; os--;
		while(*os == ' ') *os-- = L'\0';
		fldsiz = MAX_BUF_CO/*80*/;
		typ = 'h'; goto fill_end;
	}
	else {          /*---------------* ТЕКСТ КОММЕНТАРИЯ */
/***
		*os++ = *s++; co++;
***/
		while((co++)<MAX_BUF_CO/*80*/ ) {
			*os++ = *s++; fldsiz++;
			if(*s==' ') {
				if(s[1]   == markl)    break;
				if((spcnt++) >=3)    break;
			} else {
				spcnt = 0;
			}
		}
		*os = L'\0';
		while(*--os == ' ') { *os = L'\0'; fldsiz--; }
		typ = 'c';
	}
fill_end:

#ifdef MAKE_VLBP
	/* ЗАПИСАТЬ СТРОКУ В ФОРМАТЕ lbp */
	mk_lbp
#endif
#ifdef MAKE_VCC
	/* ЗАПИСАТЬ СТРОКУ ИНИЦИАЛИЗАЦИИ СТРУКТУРЫ LINE */
	mk_vcc
#endif
	(fldsiz, li_lbp, co_lbp, stro, typ);
	/* УБРАТЬ ИЗ БУФЕРА ОБРАЗА ЭКРАНА ВСЕ СЛЕДЫ */
	for(s=fbeg; fldsiz > 0; fldsiz--) *s++ = ' ';
}

scan_s()
/*--------------------------*/
/* СКАНИРОВАТЬ ОБРАЗ ЭКРАНА */
/*--------------------------*/
{
	register int li;        /* СТРОКА */
	register int co;        /* ПОЗИЦИЯ */
	wchar_t *lbeg;          /* НАЧ. СТРОКИ В БУФЕРЕ ЭКРАНА */
	int     vertli;         /* НАЧ. ВЕРТИКАЛЬНОГО ПРОСМОТРА (СТРОКА)*/

	/* СНАЧАЛА ЦИКЛ ПО СТРОКАМ */
	for(li=0; vscan=0,li<cur_li; li++) {
		lbeg = &scrp[ MAX_BUF_CO * li ];
		if(*lbeg == L'+') {      /* ПРОСМОТР ПО ВЕРТИКАЛИ */
			vscan=1; vertli = li;
			/* ЦИКЛ ПО ПОЗИЦИЯМ СТРОКИ */
			for(co=1; co<MAX_BUF_CO/*80*/; co++) {
				/* ЦИКЛ ПО СТРОКАМ */
				for(li=vertli;
						lbeg = &scrp[MAX_BUF_CO/*80*/*li], *lbeg == L'+';
							li++)
				{
					if(lbeg[co] != L' ')
						mk_lin(li, co);
				}
			}
			li--;
		} else {        /* НОРМ. ПРОСМОТР ПО СТРОКАМ */
			/* ЦИКЛ ПО ПОЗИЦИЯМ СТРОКИ */
			for(co=0; co<MAX_BUF_CO/*80*/; co++) {
				if(scrp[MAX_BUF_CO/*80*/*li+co] != ' ')
					mk_lin(li, co);
			}
		}
	}
}

put_hs(s, hp)
/*-----------------*/
/* ПОЛОЖИТЬ В КУЧУ */
/*-----------------*/
register char *s;
register HEAP *hp;
{
	linptr_t siz;


/*      printf("-(%d)->\t%s\n", lbptyp, s);     *ОТЛАДКА*/

	/*-------------------------- ПРОВЕРИТЬ, МОЖЕТ МЕСТА НЕТ... */
	if((hp->ncnt == hp->nmax) ||
	(hp->bot - hp->top + hp->siz)<(siz=strlen(s) + 1)) {
		/* КРАНТЫ */
		fprintf(stderr,
ediag("%s: %d:\t:No working plase, sorry...\n", "%s: %d:\t:Нет места в раб. таблице..\n"), ifn, nlin);
		exit(1);
	} else
	      { /*------------- НАДО БЫ ДЕЛАТЬ ПРОВЕРКУ НА ДУБЛИКАТ... */
		/*--- ЗАКИНУТЬ СТРОКУ В КУЧУ */
		strcpy(hp->bot, s);
		hp->nmp->nms = hp->bot; /* ЗАПОМНИТЬ НАЧ. СТРОКИ */
		hp->nmp->nmt = lbptyp;  /* ЗАП. ТИП СЕКЦИИ */
		hp->nmp++; hp->ncnt++;  /* СЛЕД. ИНФ. О ТИПЕ СЮДА */
		hp->bot += siz;         /* А СЮДА СТРОКУ "ИМЯ=..." */
	}
}

fil_sc()
/*-------------------------*/
/* ЗАПОЛНИТЬ СТРОКУ ЭКРАНА */
/*-------------------------*/
{
	/* НУЖНО ВСЕГО ЛИШЬ ЗАМЕНИТЬ ПРОБЕЛАМИ
	 * ТАБУЛЯЦИИ И ПУСТЫЕ КОНЦЫ СТРОК.
	 * и сконвертировать в строку wchar_t
	 */
	int i;
	wchar_t *s;
	wchar_t *scr;
	wchar_t *savescr;
	static wchar_t wclbpo[LBPOSIZE];
	int    len;
	int   c;

	/*s = lbpo; /* lbpo все еще в utf-8 */
	/*fprintf(stderr, "lbpo:='%s'\n", lbpo); fflush(stderr);*/
	len = u8snwcs(wclbpo, lbpo, LBPOSIZE);
	s = wclbpo;

	savescr = scr = &scrp[ MAX_BUF_CO * cur_li ];
	for(i=0; i<MAX_BUF_CO; i++) *scr++ = L' ';
	scr = savescr;
	for(i=0;  i<MAX_BUF_CO ; i++) {
		c = (int)*s++;
		switch(c) {
		case L'\t' :     i +=(7 - (i%8));   break;
		case L'\0' :
		case L'\n' :     goto end_fil; break;
		default:
			scr[i] = (wchar_t)c;  break;
		}
	}
end_fil:
	cur_li += 1;
}

void
ins_hs()
/* РАСПОТРОШИТЬ СТРОКУ ИЗ СЕКЦИЙ С ОПРЕДЕЛЕНИЯМИ ИМЕН:
 *
 * НАЙТИ ОБРАЗЕЦ ВИДА "ИМЯ = ВЫРАЖЕНИЕ ;"
 * ПОМЕСТИТЬ ЕГО В ОТДЕЛЬНУЮ СТРОКУ В КУЧУ;
 * ПЕРВЫЕ НЕЗНАЧАЩИЕ ПРОБЕЛЫ УДАЛИТЬ;
 * ЗАВЕРШАЮЩУЮ ТОЧКУ С ЗАПЯТОЙ УДАЛИТЬ ТОЖЕ;
 * ПРИМЕЧАНИЕ:
 *      СТРОКИ ОПИСАНИЯ В ИСХОДНОМ ФАЙЛЕ НЕ ПЕРЕНОСЯТСЯ.
 */
{
	register char *from;
	register char *to;
	int     consts;         /* ФЛАГ: СТРОКОВАЯ КОНСТАНТА */

	static  char    tmps[LBPOSIZE*2];

	from = lbpo;

BEG_OF_SCAN:
	/* В СТРОКЕ СЕКЦИИ ---LINES М.БЫТЬ НЕ ОДНО ОПИСАНИЕ */
	if (*from == '\0')
		return;

	consts = 0;
	to = tmps;
	while(isspace(*from))
		from++;
	/* НАЙТИ "ИМЯ=" */
	while(isnamec(*from))   *to++ = *from++;
	while(isspace(*from))   from++;

	if ((*from != '\0') && (*from != '=')) {
		fprintf(stderr, ediag(Esxerr,Rsxerr), ifn, nlin,
		ediag(" the '=' must be after name.", " '=' должен быть после имени.") );
		return;
	} else {
		*to++ = *from++;
	}
	while(isspace(*from)) from++;
	while(*from) {
		if ( *from == '\n' ) *from = ';'; /* ФОКУС ДЛЯ ЗАВЕРШЕНИЯ */
		if ( *from == ';' && !consts ) {
			if ( to != tmps ) {
				*to = ' ';
				/* ПОДЧИСТИТЬ ПРОБЕЛЫ В КОНЦЕ СТРОКИ:  */
				while(isspace(*to) && tmps<=to)
					*to-- = '\0';
				/* ПОЛОЖИТЬ В КУЧУ, ЕСЛИ НЕ ПУСТО */
				if(tmps[0])
					put_hs(tmps, &shp);
				from++;
				goto BEG_OF_SCAN;
			} else  {
				from++;
			}
		} else
		if ( *from == '"' && from[1] == '"' ) {
			*to++ = *from++;
		} else
		if ( *from == '"' && from[1] != '"' ) {
			if (consts) consts = 0;
			else        { *to++ = *from++; consts = 1; }
		} else    ;
		*to++ = *from++;
	}
}

pass1()
/* ПЕРВЫЙ ПРОХОД :
 * ПРОЧИТАТЬ В КУЧУ СТРОКИ ИЗ СЕКЦИЙ PORTS, TABLES, LINES;
 * ОТФИЛЬТРОВАТЬ, ПОЛОЖИТЬ В КУЧУ СТРОК;
 * ОСТАНОВИТЬСЯ НА СЕКЦИИ SCREEN;
 */
{
	register char *s;       /* УКАЗАТЕЛЬ НА КЛЮЧ. СЛОВО */
	int skpflg;     /* ФЛАГ: 1=ПРОПУСКАТЬ СТРОКУ, 0=УЧИТЫВАТЬ */

	s = &lbpo[ strlen(vis_id) ];
	skpflg = 1;

	for ( nlin=1; fgets(lbpo, LBPOSIZE, ifp), feof(ifp) == 0; nlin++) {

		/* ЕСЛИ com_id -- УЗНАТЬ, КАКАЯ КОНКРЕТНО СЕКЦИЯ */
		/* ПРОЧИТАТЬ СТРОКУ С КЛЮЧОМ scr_id,
		 * НА ЭТОМ ОСТАНОВИТЬСЯ.
		 */
		if ( str_eq(lbpo, vis_id) ) {

			/*--- end_id -- ОСТАНОВИТЬСЯ */
			if      (str_eq( s, end_id )) {
				return;
			}
			/*--- ЭТИ СЕКЦИИ ЕЩЕ НЕ ПОДДЕРЖИВАЮТСЯ */
			else if (str_eq( s, hel_id )) {
				skpflg = 1; lbptyp = 0;
			}
			/*--- ЭТИ СЕКЦИИ НАДО УЧИТЫВАТЬ */
			else if (str_eq( s, scr_id )) {
				skpflg = 0; lbptyp = 0; /* DSCREEN */
			}
			else if (str_eq( s, por_id )) {
				skpflg = 0; lbptyp = DPORT;
			}
			else if (str_eq( s, tab_id )) {
				skpflg = 0; lbptyp = DTABLE;
			}
			else if (str_eq( s, lin_id )) {
				skpflg = 0; lbptyp = DLINE;
			}
			else if (str_eq( s, com_id )) {
				;       /* НИЧЕГО НЕ МЕНЯЕТСЯ */
			}
			else {
				if (skpflg == 0 && lbptyp) {
					fprintf(stderr,
					"%s: %d: error in key:%20.20s\n",
					ifn, nlin, lbpo);
					cnterr += 1;
					/* ПРОПУСКАТЬ ПОСЛЕ ОШИБКИ */
					skpflg = 1;
				}
				else if (skpflg == 0 && lbptyp == 0/*SCREEN*/) {
					fil_sc(); /* СТРОКА ЭКРАНА */
				}
			}
		} else {
			/* ТЕПЕРЬ НАДО УЧИТЫВАТЬ СТРОКИ,
			 * ЕСЛИ ВЗВЕДЕН ФЛАГ
			 */
			if ( skpflg == 0 ) {
			    if (lbptyp)
				ins_hs();       /* ВСТАВИТЬ В КУЧУ */
			    else
				fil_sc();       /* ЗАПОЛНИТЬ СТРОКУ ЭКРАНА */
			}
		}
	}
	/* НОРМАЛЬНО ЕСТЬ ВЫХОД ПО КЛЮЧУ scr_id */
	fprintf(stderr,
	"%s: %d: section not found '%s%s'\n",
	ifn, nlin, vis_id, end_id );
	cnterr++;
}

