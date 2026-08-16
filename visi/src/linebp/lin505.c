/*
**      +----------+    БИБЛИОТЕКА ВВОДА-ВЫВОДА
**     (c) linlib  !    ДЛЯ АЛФАВИТНО-ЦИФРОВЫХ
**      +----------+    ВИДЕОТЕРМИНАЛОВ
**/

/*
 *      $Header: lin505.c,v 1.1 90/12/27 16:25:29 vsv Rel $
 *
 *      $Log:	lin505.c,v $
 * Revision 1.1  90/12/27  16:25:29  vsv
 * ВЕРСИЯ V3_3
 * 
 */

#include <stdio.h>
#include "line.h"
#include "linebp.h"

extern  const char *phelp0;

extern  IN_PORTS in_help[];

extern const char   *vexdir;  /* Каталог вынесенных описаний страниц */

static hlppmt(i, phelp)
int i;
LINE *phelp;
{
	char tmps[80];
	sprintf(tmps,
			/*"%s (%d/2) ", phelp, i);*/
			"page #%d ", i);
	w_msg(HDR, tmps);
	er_eol(TXT);
}

void 
w_help(page)
/*--------------------------------*/
/* ПОКАЗАТЬ СПРАВОЧНУЮ ИНФОРМАЦИЮ */
/*--------------------------------*/
const LINE    *page;
{
	char tmps[200];
    const char *pages[3];
    LINE *phelp = (LINE *)NULL;
    register int i;

    pages[1] = page != (LINE *)0 ? (void *)page : phelp0;
    pages[2] = phelp0;

    for (i = 1; i> 0; ) {
		if (i > 2)  i = 1;
		er_pag();
		if(pages[i]) {
			if(phelp = b_page(vexdir, pages[i], in_help)) {
				w_page(phelp);
				d_page(phelp);
				hlppmt(i, pages[i]);
				w_lh_msg("   :HE get next help");
			} else {
				hlppmt(i, pages[i]);
				at_set(ERR);
				sprintf(tmps, "not in \"%s\"", vexdir);
				w_str(tmps);
			}
			fflush(vttout);
		}

		cp_set(-1, 0, TXT);

		switch(r_cod(0)) {
		case '?':
		case KB_HE:
					i++; /*NO BREAK*/
		case KB_RE:
					continue;
		default:
					i = 0;
		}
    }
    er_pag();
    w_msg(TXT, "");
}
