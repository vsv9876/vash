/*
**      +----------+    БИБЛИОТЕКА ВВОДА-ВЫВОДА
**     (c) linlib  !    ДЛЯ АЛФАВИТНО-ЦИФРОВЫХ
**      +----------+    ВИДЕОТЕРМИНАЛОВ
**/

/*
 *      $Header: /home/vsv/proj/proj20010321/visi/src/linusr/RCS/lin490.c,v 3.1 1989/08/29 14:50:44 vsv Rel vsv $
 *
 *      $Log: lin490.c,v $
 *      Revision 3.1  1989/08/29 14:50:44  vsv
 *      ВЕРСИЯ LINLIB_3
 *
 * Revision 3.0  88/06/16  18:06:19  vsv
 * ПРЕДВАРИТЕЛЬНЫЙ ВЫПУСК
 * 
 */

#ifndef RETRO
#include <string.h>
#endif
#include <ediag.h>
#include <stdio.h>
#include "line.h"

lerror(s)
/*-----------------------*/
/* АНАЛОГ ФУНКЦИИ perror */
/*-----------------------*/
register char *s;
{
	extern  int errno;              /* СИСТЕМНЫЙ НОМЕР ОШИБКИ */
	/*extern  int sys_nerr;*/           /* МАКС. НОМЕР СООБЩЕНИЯ */
#ifdef DURA_LINUX
	extern  char *sys_errlist[];    /* АНГЛ. СПИСОК СООБЩЕНИЙ */
	extern  char *sys_rerrlist[];   /* РУСС. СПИСОК СООБЩЕНИЙ */
#endif
	char    outstr[STRBUF];
	char	*erstr;

#ifndef RETRO
	erstr = strerror(errno);
	sprintf(outstr, "CLIB: '%s' : %s", s, erstr);
#else
	if (errno <= sys_nerr ) {
		sprintf ( outstr,
		"CLIB: '%s' : %s",      s,
		ediag ( sys_errlist[ errno ], sys_rerrlist[ errno ] ) );
	} else {
		sprintf ( outstr, "CLIB: %d : %s ",
		ediag ( "Unknown error", "НЕИЗВЕСТНАЯ ОШИБКА" ) );
	}
#endif
	/* ВЫВОД СООБЩЕНИЯ ОБ оШИБКЕ */
	w_emsg ( outstr );
}
