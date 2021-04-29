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

extern  char *phelp0;

extern  IN_PORTS in_help[];

extern char   *vexdir;  /* Каталог вынесенных описаний страниц */

w_help(page)
/*--------------------------------*/
/* ПОКАЗАТЬ СПРАВОЧНУЮ ИНФОРМАЦИЮ */
/*--------------------------------*/
char    *page;
{
    char *pages[3];
    LINE *phelp = (LINE *)NULL;
    register int i;
    char tmps[48];

    pages[1] = page;
    pages[2] = phelp0;

    for (i = 1; i> 0; ) {
		if (i > 2)  i = 1;
		er_pag();
		/*w_msg(INP|MSE, "Help");*/
		/*fprintf(vttout, " #%d ", i); /* there was stdout, why? */
		/*fprintf(vttout, "help page #%d '%s'", i, pages[i]);*/
		w_msg(INP|HDR, " help");
		sprintf(tmps, " page #%d: '%s'", i, pages[i]);
		w_str(tmps);
		if(pages[i]) {
			if(phelp=b_page(vexdir, pages[i], in_help)) {
				w_page(phelp);
				d_page(phelp);
				cp_set(-1, 0, TXT);
				switch(r_cod(0)) {

				case KB_HE:
					i++;
				case KB_RE:
					continue;
				default:
					i = 0;
				}
			} else {
				at_set(ERR);
				fprintf(vttout, " not found on %s", vexdir);
				fflush(vttout);
				if ( ++i > 2 )   return(FALSE);
			}
		}
    }
    er_pag();
    w_msg(TXT, "");
    return(TRUE);
}
