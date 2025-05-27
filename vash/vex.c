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
extern  int     y0_top;

extern int sgrmode;

/*const char   *onoff[] = { "[ ]", "[X]", 0 };*/
const char   *onoff[] = { "[ ]", "[*]", 0 };

/*ARGSUSED*/
int
t_ymax(line, cod)
register LINE *line;
kbcod cod;
{
	int *var;
	char *s = calloc(sizeof(char), U8_STRBUF);

	var = (int *)line->varl;
	if (*var < 2 || *var > (lframe->maxli - 4)) {
		/*w_emsg("Please, select between 2 and 20");*/
		sprintf(s, "Please, select between 2 and %-d ", (int)(lframe->maxli - 4));
		w_emsg(s);
		return(FALSE);
	}
	return(TRUE);
}

const IN_PORTS inport[] = {
	/*NO STRICT*/
	{ "cvt_sp",     cvt_sp  },
	{ "cvt_s",      cvt_s  },
	{ "cvt_hl",     cvt_hl  },
	{ "cvt_lh",     cvt_lh  },
	{ "cvt_a",      cvt_a },
	{ "cvt_fd",     cvt_fd },
	{ "coprts",     coprts },
	{ "buildd",     buildd },
	{ "versn",      &v.versn },
	{ "vexdir",     &vexdir },
	{ "vpath",      &v.vapath },
	{ "vashrc",     &v.rc },
	{ "f_o",        &Cfill_o },
	{ "f",          &Cfill },
	{ "i",          Crepf },
	{ "o",          Coutf },
	{ "r",          Csubs },
	{ "onoff",      onoff },
	{ "t_ymax",     t_ymax },
	{ "pF",         &vflag.panelf },
	{ "xF",         &vflag.xtermf },
	{ "wF",         &vflag.whodirf },
	{ "sF",         &vflag.scrolf },
	{ "HF",         &vflag.histf  },
	{ "SF",         &vflag.histsn },
	{ "cF",         &vflag.clockf },
	{ "mF",         &vflag.cmailf },
	{ "TF",         &vflag.exittrap },
	{ "NF",         &vflag.novice },
	{ "AF",         &vflag.shanyway },
	{ "RF",         &vflag.subatrc },
	{ "aF",         &vflag.subshow },
	{ "PP",         &vflag.predef },
	{ "JF",         &vflag.jobshow },
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

sup(cmd)
const char *cmd;
{
	LINE *supm;

	if ((supm = getvex("sup.lb")) == NULL)
		return(FALSE);
	scrlarea();
	u_page(supm, (LINE *)"sup_help.lb");
	d_page(supm);

	pre_vf();
	er_pag();
	return(TRUE);
}
