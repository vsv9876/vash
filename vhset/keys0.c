/*
**      +----------+    библиотека ввода-вывода
**     (c) linlib  !    для алфавитно-цифровых
**      +----------+    видеотерминалов
**/

/*
 *      $Header$
 *
 *      $Log$
 */

#include <stdio.h>      /* заголовок стандартной библиотеки вв/выв */
#include "line.h"       /* файл-заголовок LINLIB */
#include "line0.h"
#include "vhset.h"      /* описание общих данных и процедур */

#include "keys0.i"

pag_mk()
/*------------------*/
/* Basic keys setup  /
/*------------------*/
{
	register int i;
	register int j;
	kbcod cod;
	int li, co;
	LINE *line;

	er_pag();
	w_page(pkeys0);

	w_emsg("step 1: assign new value of every key");
	for(i=0; i<8;    ) {
		/* найти место для маркера */
		for ( j= -1, line=pkeys0; line->size != 0; line++) {
			if( line->attr & INP ) j++;
			if( j == i ) break;
		}

		cp_set(li=line->line, 0, ATT);
		w_str("->");    /* показать маркер */

		cp_set(li, 0, TXT);
		kbl[i].t_key = cod = r_key();

		if(i == 1) {
			if(kbl[0].t_key == cod) {
				r_cod(cod);
				w_emsg(
				"Keypdad are toggled");
				continue;
			}
		}
		if(cod == ' ' && i>0) i--;
		if(cod != ' '       ) i++;

		cp_set(li, 0, TXT);
		w_str("  ");    /* clear marker */

		w_emsg("");
	}
	w_emsg("step 2: keys input finished, let's edit names");

	line = pkeys0;
	while(r_page(pkeys0, &line, 0) != KB_EX) ;
}
