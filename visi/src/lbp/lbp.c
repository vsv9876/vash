#include <stdio.h>
#include <ctype.h>
#include "line.h"
#include "linebp.h"

char    sbbs[100]={0};   /* נןהףפעןכב הלס קףסכןך םולן‏י */
char     ss[300]={0};   /* קעוםוממבס ףפעןכב הלס קקןהב ית זבךלב */
char    *ssp  =0;       /* פוכץ‎וו נןלןצומיו ק üפןך ףפעןכו */

LINE_H  lhd = {0};      /* תבחןלןקןכ ןניףבמיס ףפעבמידש */
LINE    *linep=0;       /* םוףפן הלס עבתםו‎ומיס קףוך ףפעבמידש */

char    *hps = 0;       /* כץ‏ב ףפעןכ */
char    *hpsp= 0;       /* פוכץ‎וו נןלןצומיו ק כץ‏ו הלס תבמוףומיס */

linptr_t nmi = 0;      /* פוכץ‎יך ימהוכף יםומ */
char    *nms = 0;      /* פבגלידב יםומ בהעוףןק הלס ûלאת-פבגלידש */

char i_fnam[100];       /* יםס קטןהמןחן זבךלב */
char o_fnam[100];       /* יםס קשטןהמןחן זבךלב */

/*------------*/
/*   main     */
/*------------*/
main(argc, argv)
int		argc;
char    *argv[];
{
	char *malloc(), *calloc();
	register linptr_t i;
	register char *s;
	register int   c;
	linptr_t     *ip;
	FILE *i_fp, *o_fp;
	FILE *fopen();

	/* נןלץ‏יפר יםומב זבךלןק */
	if( argc != 3 ) {
		printf("Usage : bl ifile ofile\n");
		exit(1);
	}
	strcpy(i_fnam, argv[1]);
	strcpy(o_fnam, argv[2]);

	/* ןפכעשפר זבךל מב ‏פומיו */
	if((i_fp = fopen(i_fnam, "r")) == NULL ) {
		printf("%s : Can't read\n", i_fnam);
		exit(1);
	}

	/* ןפכעשפר זבךל מב תבניףר */
	if((o_fp = fopen(o_fnam, "wn")) == NULL ) {
		printf("%s : Can't write\n", o_fnam);
		exit(1);
	}

	/* תבנעןףיפר נבםספר הלס ןניףבמיס ףפעבמידש */
	if((linep = calloc(1000, sizeof(LINE))) == NULL ) {
		printf("No memory for LINE...\n");
		exit(1);
	}
	/* תבנעןףיפר נבםספר הלס כץ‏י ףפעןכ */
	if((hps = malloc(4000)) == NULL ) {
		printf("No memory for HEAP...\n");
		exit(1);
	}
	hpsp = hps;

	/* תבנעןףיפר נבםספר הלס פבגלידש יםומ */
	if((nms = calloc(200, 8)) == NULL ) {
		printf("No memory for NAMTAB...\n");
		exit(1);
	}
	put_nm("**--**");

	/* נןףפעןיפר ןניףבמיו ףפעבמידש */
	linebp(i_fp);

	/* קשעןקמספר נן חעבמידו 16byte עבתםועש ק תבחןלןקכו */
	i = lhd.lh_lines;
	i += (16 - (i % 16));
	lhd.lh_lines = i;

	i = lhd.lh_heaps;
	i += (16 - (i % 16));
	lhd.lh_heaps = i;

	i = lhd.lh_names;
	i += (2 - (i % 2));
	lhd.lh_names = i;

	/* תבניףבפר תבחןלןקןכ ןניףבמיס ףפעבמידש */
	s = (char *)&lhd;
	for(i=0; i<sizeof(LINE_H); i++ ) {
		c = *s++;
		putc( c, o_fp);
	}

	/* תבניףבפר line[]  ק קשטןהמןך זבךל */
	s = (char *)linep;
	for(i=0; i<lhd.lh_lines; i++ ) {
		c = *s++;
		putc( c, o_fp);
	}

	/* תבניףבפר כץ‏ץ ףפעןכ */
	s = hps;
	for(i=0; i<lhd.lh_heaps; i++ ) {
		c = *s++;
		putc( c, o_fp);
	}

	/* תבניףבפר פבגלידץ יםומ */
	s = nms;
	for(i=0; i<lhd.lh_names; i++ ) {
		c = *s++;
		putc( c, o_fp);
	}

	/* תבכעשפר קשטןהמןך זבךל */
	fclose( o_fp );

	exit(0);

}

static  int     s_count;        /* ף‏ופ‏יכ ףפעןכ */
/*-----------------------------*/
/* נןףפעןיפר ןניףבמיו ףפעבמידש */
/*-----------------------------*/
linebp(i_fp)
FILE   *i_fp;
{
	register int c;
	register linptr_t i;
	LINE    *line;          /* עבגן‏יך ץכבתבפולר */

	s_count = 0;
	line=linep;     /* מב‏ימבום ףפעןיפר ןניףבמיו ףפעבמידש */

	/* ןהמב ףפעןכב זבךלב ףןהועציפ ןניףבמיו ןהמןך לימיי */
	while( fgets( ss, 300, i_fp ) != NULL) {

		s_count += 1;

		/* נעןנץףכבום כןםםומפבעיי */
		if(ss[0] == '#')        continue;

		ssp = ss;

	    c = *ssp;
	    line->attr = 0;
	    for( ; !isspace(*ssp); ssp++ ) {
		switch( *ssp ) {
		case 'G' :                  /* ןג‎יך ףלץ‏בך */
		case 'g' :
		    break;
		case 'I' :                  /* לימיס הלס קקןהב */
		case 'i' :
		    line->attr |= INP;
		    break;
		case 'H' :                  /* תבחןלןקןכ */
		case 'h' :
		    line->attr |= LHDR;
		    break;
		case 'C' :                  /* כןםםומפבעיי */
		case 'c' :
		    line->attr |= LTXT;
		    break;
		case 'P' :                  /* ף נןהףכבתכןך */
		case 'p' :
		    line->attr |= PMT;
		    break;
		case 'V' :                  /* נועוםוממבס */
		case 'v' :
		    line->attr |= LVAR;
		    break;
		case 'A' :                  /* נועוכלא‏בפולר */
		case 'a' :
		    line->attr |= LALT;
		    break;
		case 'M' :                  /* םומא */
		case 'm' :
		    line->attr |= LMSE;
		    break;
		case 'K' :                  /* מבתקבמיו כלבקיûי */
		case 'k' :
		    line->attr |= (ATT|MID|PAD);
		    break;
		default :
		    printf("%s: %3d: unknown attribute '%c'\n",
		    i_fnam, s_count, *ssp );
		    goto cont;
		    break;
		}
	    }

	    /* הבלרûו עבתגיעבום כבצהשך פינ לימיי */
	    switch( c ) {
	    case 'C':
	    case 'c':
	    case 'H':
	    case 'h':
		    bld_pos(line);  /* נועקשו 4 נבעבםופעב */
		    bld_c(line);        /* ןףפבלרמשו נבעבםופעש */
		    break;
	    default:
		    bld_pos(line);  /* נועקשו 4 נבעבםופעב */
		    bld_g(line);        /* ןףפבלרמשו נבעבםופעש */
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
/* נןףפעןיפר נןתידיןממשו נבעבםופעש */
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
/* נןףפעןיפר נבעבםופעש זלבחןק תבהועצוכ */
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
/* נןףפעןיפר נבעבםופע-בהעוף */
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
		argp = ssp;     /* ית קמוûמוחן ןניףבמיס */
	else
		argp = argstr;  /* נןהףפבמןקכב כןמףפבמפ : tmp, cvt_a */

	while( isspace(*argp) )
		argp++;
	if( *argp == '-' ) {
		argp++;
		ret_s = 0;
		goto ret_ret;
	}

	/* וףלי ףפעןכב מב‏ימבופףס ף " */
	if( *argp == '"' ) {
		/* תבניףבפר ףפעןכץ ק כץ‏ץ */
		argp++;
		for( sav_hpsp=hpsp; *argp; hpsp++, argp++) {
			if(*argp == '"') {
				if(*(argp+1) != '"') {
					/* תבקועûיפר ףפעןכץ */
					lhd.lh_heaps += 2;
					*hpsp++ = '\0';
					argp++;
					break;
				} else {
					/* "" י וףפר " */
					*hpsp = *argp++;
				}
			} else  {
				*hpsp = *argp;
			}
			lhd.lh_heaps += 1;
		}
		/* קועמץפר ףםו‎ומיו ןפ מב‏בלב כץ‏י + 2048 */
		ret_s = (char *)(sav_hpsp - hps + (linptr_t)2048);

	}
	/* ימב‏ו üפן יםס בהעוףב הלס ûלאת-פבגלידש */
	else {
		argp = f_sbbs( argp );
		/* וףלי יםס מויתקוףפמן */
		if((ret_s=(char *)tst_nm(sbbs)) == 0 ) {
			/* תבעוחיףפעיעןקבפר וחן */
			ret_s = (char *)put_nm(sbbs);
		}
	}
ret_ret:
	if( *argstr == '\0' )
		ssp = argp;

	return( ret_s );
}

/*----------------------------*/
/* נעןקועיפר, יתקוףפמן לי יםס */
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
/* נןםוףפיפר יםס ק פבגלידץ */
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
/* תבנןלמיפר נןהףפעןכץ */
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
/* תבנןלמיפר בהעוףמשו נבעבםופעש */
/*------------------------------*/

bld_c(line)             /* כןםםומפבעיי */
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


bld_g(line)             /* ןג‎יך ףלץ‏בך */
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
