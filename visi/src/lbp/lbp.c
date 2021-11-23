#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "line.h"
#include "linebp.h"

char    sbbs[100]={0};   /* ПОДСТРОКА ДЛЯ ВСЯКОЙ МЕЛОЧИ */
char     ss[300]={0};   /* ВРЕМЕННАЯ СТРОКА ДЛЯ ВВОДА ИЗ ФАЙЛА */
char    *ssp  =0;       /* ТЕКУЩЕЕ ПОЛОЖЕНИЕ В ЭТОЙ СТРОКЕ */

LINE_H  lhd = {0};      /* ЗАГОЛОВОК ОПИСАНИЯ СТРАНИЦЫ */
LINE    *linep=0;       /* МЕСТО ДЛЯ РАЗМЕЩЕНИЯ ВСЕЙ СТРАНИЦЫ */

char    *hps = 0;       /* КУЧА СТРОК */
char    *hpsp= 0;       /* ТЕКУЩЕЕ ПОЛОЖЕНИЕ В КУЧЕ ДЛЯ ЗАНЕСЕНИЯ */

linptr_t nmi = 0;      /* ТЕКУЩИЙ ИНДЕКС ИМЕН */
char    *nms = 0;      /* ТАБЛИЦА ИМЕН АДРЕСОВ ДЛЯ ШЛЮЗ-ТАБЛИЦЫ */

char i_fnam[100];       /* ИМЯ ВХОДНОГО ФАЙЛА */
char o_fnam[100];       /* ИМЯ ВЫХОДНОГО ФАЙЛА */

/*------------*/
/*   main     */
/*------------*/
main(argc, argv)
int		argc;
char    *argv[];
{
	/*char *malloc(), *calloc();*/
	register linptr_t i;
	register char *s;
	register int   c;
	linptr_t     *ip;
	FILE *i_fp, *o_fp;
	FILE *fopen();

	/* ПОЛУЧИТЬ ИМЕНА ФАЙЛОВ */
	if( argc != 3 ) {
		printf("Usage : bl ifile ofile\n");
		exit(1);
	}
	strcpy(i_fnam, argv[1]);
	strcpy(o_fnam, argv[2]);

	/* ОТКРЫТЬ ФАЙЛ НА ЧТЕНИЕ */
	if((i_fp = fopen(i_fnam, "r")) == NULL ) {
		printf("%s : Can't read\n", i_fnam);
		exit(1);
	}

	/* ОТКРЫТЬ ФАЙЛ НА ЗАПИСЬ */
	if((o_fp = fopen(o_fnam, "wn")) == NULL ) {
		printf("%s : Can't write\n", o_fnam);
		exit(1);
	}

	/* ЗАПРОСИТЬ ПАМЯТЬ ДЛЯ ОПИСАНИЯ СТРАНИЦЫ */
	if((linep = calloc(1000, sizeof(LINE))) == NULL ) {
		printf("No memory for LINE...\n");
		exit(1);
	}
	/* ЗАПРОСИТЬ ПАМЯТЬ ДЛЯ КУЧИ СТРОК */
	if((hps = malloc(4000)) == NULL ) {
		printf("No memory for HEAP...\n");
		exit(1);
	}
	hpsp = hps;

	/* ЗАПРОСИТЬ ПАМЯТЬ ДЛЯ ТАБЛИЦЫ ИМЕН */
	if((nms = calloc(200, 8)) == NULL ) {
		printf("No memory for NAMTAB...\n");
		exit(1);
	}
	put_nm("**--**");

	/* ПОСТРОИТЬ ОПИСАНИЕ СТРАНИЦЫ */
	linebp(i_fp);

	/* ВЫРОВНЯТЬ ПО ГРАНИЦЕ 16byte РАЗМЕРЫ В ЗАГОЛОВКЕ */
	i = lhd.lh_lines;
	i += (16 - (i % 16));
	lhd.lh_lines = i;

	i = lhd.lh_heaps;
	i += (16 - (i % 16));
	lhd.lh_heaps = i;

	i = lhd.lh_names;
	i += (2 - (i % 2));
	lhd.lh_names = i;

	/* ЗАПИСАТЬ ЗАГОЛОВОК ОПИСАНИЯ СТРАНИЦЫ */
	s = (char *)&lhd;
	for(i=0; i<sizeof(LINE_H); i++ ) {
		c = *s++;
		putc( c, o_fp);
	}

	/* ЗАПИСАТЬ line[]  В ВЫХОДНОЙ ФАЙЛ */
	s = (char *)linep;
	for(i=0; i<lhd.lh_lines; i++ ) {
		c = *s++;
		putc( c, o_fp);
	}

	/* ЗАПИСАТЬ КУЧУ СТРОК */
	s = hps;
	for(i=0; i<lhd.lh_heaps; i++ ) {
		c = *s++;
		putc( c, o_fp);
	}

	/* ЗАПИСАТЬ ТАБЛИЦУ ИМЕН */
	s = nms;
	for(i=0; i<lhd.lh_names; i++ ) {
		c = *s++;
		putc( c, o_fp);
	}

	/* ЗАКРЫТЬ ВЫХОДНОЙ ФАЙЛ */
	fclose( o_fp );

	exit(0);

}

static  int     s_count;        /* СЧЕТЧИК СТРОК */
/*-----------------------------*/
/* ПОСТРОИТЬ ОПИСАНИЕ СТРАНИЦЫ */
/*-----------------------------*/
linebp(i_fp)
FILE   *i_fp;
{
	register int c;
	register linptr_t i;
	LINE    *line;          /* РАБОЧИЙ УКАЗАТЕЛЬ */

	s_count = 0;
	line=linep;     /* НАЧИНАЕМ СТРОИТЬ ОПИСАНИЕ СТРАНИЦЫ */

	/* ОДНА СТРОКА ФАЙЛА СОДЕРЖИТ ОПИСАНИЕ ОДНОЙ ЛИНИИ */
	while( fgets( ss, 300, i_fp ) != NULL) {

		s_count += 1;

		/* ПРОПУСКАЕМ КОММЕНТАРИИ */
		if(ss[0] == '#')        continue;

		ssp = ss;

	    c = *ssp;
	    line->attr = 0;
	    for( ; !isspace(*ssp); ssp++ ) {
		switch( *ssp ) {
		case 'G' :                  /* ОБЩИЙ СЛУЧАЙ */
		case 'g' :
		    break;
		case 'I' :                  /* ЛИНИЯ ДЛЯ ВВОДА */
		case 'i' :
		    line->attr |= INP;
		    break;
		case 'H' :                  /* ЗАГОЛОВОК */
		case 'h' :
		    line->attr |= LHDR;
		    break;
		case 'C' :                  /* КОММЕНТАРИИ */
		case 'c' :
		    line->attr |= LTXT;
		    break;
		case 'P' :                  /* С ПОДСКАЗКОЙ */
		case 'p' :
		    line->attr |= PMT;
		    break;
		case 'V' :                  /* ПЕРЕМЕННАЯ */
		case 'v' :
		    line->attr |= LVAR;
		    break;
		case 'A' :                  /* ПЕРЕКЛЮЧАТЕЛЬ */
		case 'a' :
		    line->attr |= LALT;
		    break;
		case 'M' :                  /* МЕНЮ */
		case 'm' :
		    line->attr |= LMSE;
		    break;
		case 'K' :                  /* НАЗВАНИЕ КЛАВИШИ */
		case 'k' :
		    line->attr |= (LKEY);
		    break;
		default :
		    printf("%s: %3d: unknown attribute '%c'\n",
		    i_fnam, s_count, *ssp );
		    goto cont;
		    break;
		}
	    }

	    /* ДАЛЬШЕ РАЗБИРАЕМ КАЖДЫЙ ТИП ЛИНИИ */
	    switch( c ) {
	    case 'C':
	    case 'c':
	    case 'H':
	    case 'h':
		    bld_pos(line);  /* ПЕРВЫЕ 4 ПАРАМЕТРА */
		    bld_c(line);        /* ОСТАЛЬНЫЕ ПАРАМЕТРЫ */
		    break;
	    default:
		    bld_pos(line);  /* ПЕРВЫЕ 4 ПАРАМЕТРА */
		    bld_g(line);        /* ОСТАЛЬНЫЕ ПАРАМЕТРЫ */
		    break;
	    }
	    line++;
	    lhd.lh_lines += sizeof(LINE);

cont:   ;
	}
	line->size = 0;
	lhd.lh_lines += sizeof(LINE);

}

/*---------------------------------*/
/* ПОСТРОИТЬ ПОЗИЦИОННЫЕ ПАРАМЕТРЫ */
/*---------------------------------*/

bld_pos(line)
LINE    *line;
{
	char *f_sbbs();
	static  int iii;

	ssp = f_sbbs( ssp );
	sscanf(sbbs, "%d", &iii);
	line->size = (char)iii;

	ssp = f_sbbs( ssp );
	sscanf(sbbs, "%d", &iii);
	line->line = (char)iii;

	ssp = f_sbbs( ssp );
	sscanf(sbbs, "%d", &iii);
	line->colu = (char)iii;

}

/*-------------------------------------*/
/* ПОСТРОИТЬ ПАРАМЕТРЫ ФЛАГОВ ЗАДЕРЖЕК */
/*-------------------------------------*/

char
bld_flg()
{
	char    *f_sbbs();
	int     flg;
	register c;
	register char *s;

	ssp = f_sbbs( ssp );
	flg = 0;
	for( s=sbbs; *s; s++ ) {
		c = *s;
		switch( c ) {
		case '-' :
		case '*' :
			flg = 0;
			break;
		case 'U' :
		case 'u' :
			flg |= SUSU;
			break;
		case 'D' :
		case 'd' :
			flg |= SUSD;
			break;
		case 'L' :
		case 'l' :
			flg |= SUSL;
			break;
		case 'R' :
		case 'r' :
			flg |= SUSR;
			break;
		case 'N' :
		case 'n' :
			flg |= SUSNL;
			break;
		case 'T' :
		case 't' :
			flg |= SUST;
			break;
		default :
		    printf("%s: %3d: unknown suspend type: '%c'\n",
		    i_fnam, s_count, c );
		    break;
		}
	}
	return(flg);
}

/*--------------------------*/
/* ПОСТРОИТЬ ПАРАМЕТР-АДРЕС */
/*--------------------------*/
char *
bld_par(argstr)
char    *argstr;
{
	register i;
	static char *argp;
	char    *p;
	char    *sav_hpsp;
	char    *ret_s;
	char    *f_sbbs();

	if( *argstr == '\0' )
		argp = ssp;     /* ИЗ ВНЕШНЕГО ОПИСАНИЯ */
	else
		argp = argstr;  /* ПОДСТАНОВКА КОНСТАНТ : tmp, cvt_a */

	while( isspace(*argp) )
		argp++;
	if( *argp == '-' ) {
		argp++;
		ret_s = 0;
		goto ret_ret;
	}

	/* ЕСЛИ СТРОКА НАЧИНАЕТСЯ С " */
	if( *argp == '"' ) {
		/* ЗАПИСАТЬ СТРОКУ В КУЧУ */
		argp++;
		for( sav_hpsp=hpsp; *argp; hpsp++, argp++) {
			if(*argp == '"') {
				if(*(argp+1) != '"') {
					/* ЗАВЕРШИТЬ СТРОКУ */
					lhd.lh_heaps += 2;
					*hpsp++ = '\0';
					argp++;
					break;
				} else {
					/* "" И ЕСТЬ " */
					*hpsp = *argp++;
				}
			} else  {
				*hpsp = *argp;
			}
			lhd.lh_heaps += 1;
		}
		/* ВЕРНУТЬ СМЕЩЕНИЕ ОТ НАЧАЛА КУЧИ + 2048 */
		ret_s = (char *)(sav_hpsp - hps + (linptr_t)2048);

	}
	/* ИНАЧЕ ЭТО ИМЯ АДРЕСА ДЛЯ ШЛЮЗ-ТАБЛИЦЫ */
	else {
		argp = f_sbbs( argp );
		/* ЕСЛИ ИМЯ НЕИЗВЕСТНО */
		if((ret_s=(char *)tst_nm(sbbs)) == 0 ) {
			/* ЗАРЕГИСТРИРОВАТЬ ЕГО */
			ret_s = (char *)put_nm(sbbs);
		}
	}
ret_ret:
	if( *argstr == '\0' )
		ssp = argp;

	return( ret_s );
}

/*----------------------------*/
/* ПРОВЕРИТЬ, ИЗВЕСТНО ЛИ ИМЯ */
/*----------------------------*/
tst_nm( s )
char  *s;
{
	register int i;
	register char *p;

	for(p=nms, i=0; i<nmi; p += 8, i++) {
			if(strncmp(p, s, 6) == 0)
				return( i );
		}
	return( 0 );
}

/*-------------------------*/
/* ПОМЕСТИТЬ ИМЯ В ТАБЛИЦУ */
/*-------------------------*/
int
put_nm( s )
char  *s;
{
	register char *p;

	p = nms + (nmi * 8);
	strncpy(p, s, 6);
	lhd.lh_names += 8;
	nmi += 1;
	return( nmi-1 );
}

/*---------------------*/
/* ЗАПОЛНИТЬ ПОДСТРОКУ */
/*---------------------*/
char *
f_sbbs( ispp )
register char *ispp;
{

	register int i;

	while( isspace(*ispp) ) {
		ispp++;
	}
	i = 0;
	while( (isspace(*ispp) == 0) && (i < 6) ) {
		sbbs[i++] = *ispp++;
	}
	while( i<8 )
		sbbs[i++] = 0;

	return( ispp );
}

/*------------------------------*/
/* ЗАПОЛНИТЬ АДРЕСНЫЕ ПАРАМЕТРЫ */
/*------------------------------*/

bld_c(line)             /* КОММЕНТАРИИ */
LINE  *line;
{
	char   bld_flg();
	char  *bld_par();

	line->flag = 0;
	line->cvts = 0;
	line->cvtf = 0;
	line->test = 0;
	line->varl = bld_par("");
}


bld_g(line)             /* ОБЩИЙ СЛУЧАЙ */
LINE  *line;
{
	char   bld_flg();
	char  *bld_par();

	line->flag = bld_flg();
	line->cvts = bld_par("");
	line->cvtf = bld_par("");
	line->test = bld_par("");
	line->varl = bld_par("");
}
