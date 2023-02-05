/*
**      +----------+    БИБЛИОТЕКА ВВОДА-ВЫВОДА
**     (c) linlib  !    ДЛЯ АЛФАВИТНО-ЦИФРОВЫХ
**      +----------+    ВИДЕОТЕРМИНАЛОВ
**/

/*
 *      $Header$
 *      $Log$
 */

#include <stdlib.h>
#include <locale.h>
#include <stdio.h>
#include "line.h"       /* ФАЙЛ-ЗАГОЛОВОК LINLIB */

#ifdef RT11
/* $$narg = 1 ;            /* НЕ ВЫДАВАТЬ ПОДСКАЗКУ НА ВВОД АРГУМЕНТОВ */
#endif

char *vexdir = "../BLD/usr/lib/vhset:/usr/lib/vhset"; /* anought to debug in current directory */

main()
/*--------*/
/*  MAIN  */
/*--------*/
{
	int ch;
	if (!setlocale(LC_CTYPE, "")) {
		fprintf(stderr, "Can't set the specified locale! "
			"Check LANG, LC_CTYPE, LC_ALL.\n");
		return 1;
	} else {
		mb_cur_max = MB_CUR_MAX;
	}
	printf("VTEST>"); fflush(stdout); fscanf(stdin, "%c", &ch);
    visini();
    hw_set();
    io_set(IO_VIDEO);

    vmain();    /* СМ. ФАЙЛ pvmain.cv */

    er_eop(0);
    io_set(IO_TTYPE);
    printf("\n");
    exit(0);
}


