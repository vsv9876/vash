/*
 * test [linlib]
 *
 * 19.05.87     ПОЗИЦИОНИРОВАНИЕ КУРСОРА
 * 20.05.87     ПРОВЕРКА ТЕРМИНАЛЬНЫХ РЕЖИМОВ ВВОДА-ВЫВОДА
 *
 */

#include <stdio.h>
#include <ctype.h>

char buf[1024];
char *getenv(), *tgetstr();
char *rdchar();
char *tgoto();
char *CM;
char cmbuff[30];
char *x;
char *UP;
char *tgout;
char *BC;
char bcbuff[10];

main(argc, argv) int argc; char **argv; {
	char *p;
	int rc;
	int row, col;

	if (argc < 2)
		p = getenv("TERM");
	else
		p = argv[1];
	rc = tgetent(buf,p);

	x = bcbuff;
	BC = tgetstr("bc", &x);
	if (BC && *BC==0)
		BC = 0;
	rc = tgetent(buf,p);
	x = cmbuff;
	UP = tgetstr("up", &x);
	if (UP && *UP==0)
		UP = 0;
	CM = tgetstr("cm", &x);

	io_set();
	diag();
	io_off();

	exit(0);
}

/*
** cursor position set
*/

cp_set(nr, nc)
{
	register char *p;

	p = tgoto(CM, nr, nc);
	while( isdigit(*p) ) p++;
	w_raw( p);
}

/*
**      diagonal with '*'
**
*/

diag()
{
	register i, j, k;

	for(j=0; j<80-24; j++) {   /* column */
		for(k=j,i=0; i<24; i++,j++) {   /* row */
			cp_set(j, i);
			putchar('*');
		}
		j=k;
	}
}
