#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "line.h"
#include "linebp.h"
#include "assist.h"
#include "astat.h"

extern  char    coprts[];
extern  char    buildd[];
extern  char   *versn;
extern  char   *vexdir;
/*extern  char   *vlibpath;*/
extern  int     y0_top;

extern int sgrmode;

char   *onoff[] = { "[ ]", "[X]", 0 };
/*char   *onoff[] = { " | ", " x ", 0 };*/

#ifdef RETRO
extern  char    Cfill[];
extern  char    Crepf[];
extern  char    Coutf[];
extern  char    Csubs[];
#endif

/*ARGSUSED*/
int
t_ymax(line, cod)
register LINE *line;
kbcod cod;
{
	int *var;
	char *s = calloc(sizeof(char), MAXLICO);

	var = (int *)line->varl;
	if (*var < 2 || *var > (lframe->maxli - 4)) {
		/*w_emsg("Please, select between 2 and 20");*/
		sprintf(s, "Please, select between 2 and %-d ", (int)(lframe->maxli - 4));
		w_emsg(s);
		return(FALSE);
	}
	return(TRUE);
}

IN_PORTS inport[] = {
	/*NOSTRICT*/
	{ "cvt_sp",     cvt_sp  },
	{ "cvt_s",      cvt_s  },
	{ "cvt_hl",     cvt_hl  },
	{ "cvt_lh",     cvt_lh  },
	{ "cvt_a",      cvt_a },
	{ "coprts",     coprts },
	{ "buildd",     buildd },
	{ "versn",      &versn },
	{ "vexdir",     &vexdir },
	{ "vpath",      &vapath },
	{ "vashrc",     &vashrc },
	{ "f",          &Cfill },
	{ "i",          Crepf },
	{ "o",          Coutf },
	{ "r",          Csubs },
	{ "onoff",      onoff },
	{ "t_ymax",     t_ymax },
	{ "pF",         &panelf },
	{ "xF",         &xtermf },
	{ "wF",         &whodirf },
	{ "sF",         &scrolf },
	{ "hF",         &histf  },
	{ "hS",         &histsn },
	{ "cF",         &clockf },
	{ "mF",         &cmailf },
	{ "yy_max",     &clm._yy_max },
	{ "y0",         &clm._y0     },
	{ "y0_top",     &y0_top },
	{ "maxli",     	&hwframe.maxli },
	{ "maxco",     	&hwframe.maxco },
	{ "lmaxli",     &lfmain.maxli },
	{ "baseli",     &lfmain.baseli },
	{ "u8nopa",     &u8nopass },
	{ "SGR",        &sgrmode },
	{ 0,            0       },
};

/*
 * Получить внешнюю страницу.
 */
LINE *
getvex(pgname)
register char *pgname;   /* имя файла страницы */
{
	register LINE *linep;
	extern LINE *b_page();

	/*NOSTRICT*/
	if((linep=b_page(vexdir, pgname, inport)) == NULL) {
		w_emsg("Not found ");
		w_str(pgname); w_str(" in "); w_str(vexdir);
	}
	return(linep);
}

sup()
{
	LINE *supm;

	if ((supm = getvex("sup.lb")) == NULL)
		return(FALSE);
	scrlarea();
	u_page(supm, "sup_help.lb");
	d_page(supm);

	pre_vf();
	er_pag();
	return(TRUE);
}
