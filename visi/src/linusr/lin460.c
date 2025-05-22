/*
**      +----------+    БИБЛИОТЕКА ВВОДА-ВЫВОДА
**     (c) linlib  !    ДЛЯ АЛФАВИТНО-ЦИФРОВЫХ
**      +----------+    ВИДЕОТЕРМИНАЛОВ
**/

/*
 *      $Header: lin460.c,v 1.1 90/08/24 08:08:17 vsv Exp $
 *
 *      $Log:	lin460.c,v $
 * Revision 1.1  90/08/24  08:08:17  vsv
 * Initial revision
 * 
 * Revision 3.2  89/08/29  14:50:26  vsv
 * ВЕРСИЯ LINLIB_3
 * 
 * Revision 3.1  88/06/27  15:31:01  vsv
 * РЕВИЗИЯ АРХИВА RCS
 * 
 * Revision 3.0  88/06/16  18:04:44  vsv
 * ПРЕДВАРИТЕЛЬНЫЙ ВЫПУСК
 * 
 */

#include <stdio.h>
#include "line.h"

/*---------------------------*/
/* ВЫВОД СООБЩЕНИЯ ОБ ОШИБКЕ */
/*---------------------------*/

static  int msgflg ;    /* На экране есть сообщение */
static  int msgflg;

int     ok_msg()
{
	return(msgflg);
}

void w_lbl(vamode, str)
int 	vamode;
const char	*str;
{
	at_set(vamode);
	w_str(str);
}

void w_msg(vamode, str)
int     vamode;        /* видеоатрибут */
const char    *str ;         /* текст сообщения об ошибке */
{
	int va;	/* attribute of message */
	char *whatmsg = "*";

	if(!io_get(IO_VIDEO)) {
		switch (vamode & VIDEO) {
		case ERR: whatmsg = "[error]";   break;
		case ATT: whatmsg = "[warning]"; break;
		case TXT: whatmsg = "";          break;
		}
		if (*str)
			printf("%s %s: %s", argv0, whatmsg, str);
		fflush(stdout);

	} else {
		va = TXT;
		if (msgflg) {       /* если строка на экране занята */
			msgflg = 0;     /* погасить ее */
			cp_set(-1, 0, TXT);
			er_eol(TXT);
		}
		if (*str) {    /* если аргумент вызова непустая строка */
			msgflg = 1;
			cp_set(-1, 0, vamode);

			switch (vamode & VIDEO) {
			case ERR: whatmsg = "error"; break;
			case ATT: whatmsg = ">>>";   break;
			case TXT: whatmsg = "";      break;
			/*default:  whatmsg = "*";     break;*/
			}
			if (whatmsg[0] == '\0')
				fprintf(vttout, "  ");
			else
				fprintf(vttout, " %s: ", whatmsg);
			at_set(va); w_chr(' ');
			w_str(str);
			at_set(TXT); er_eol(TXT);
		}
	}
}

/* для поддержки совместимости со старыми версиями */
void w_emsg(s)
const char *s;
{
	w_msg(ERR, s);
}
