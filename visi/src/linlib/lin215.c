/*
**      +----------+    БИБЛИОТЕКА ВВОДА-ВЫВОДА
**     (c) linlib  !    ДЛЯ АЛФАВИТНО-ЦИФРОВЫХ
**      +----------+    ВИДЕОТЕРМИНАЛОВ
**/

/*
 *      $Header: lin215.c,v 1.1 90/12/27 16:28:58 vsv Rel $
 *
 *      $Log:	lin215.c,v $
 * Revision 1.1  90/12/27  16:28:58  vsv
 * ВЕРСИЯ LINLIB_3
 * 
 * Revision 3.4  89/08/29  15:15:29  vsv
 * ВЕРСИЯ LINLIB_3
 * 
 * Revision 3.3  88/07/28  09:17:13  vsv
 * ДОБАВЛЕНА ОБРАБОТКА ВКЛ/ВЫКЛ ДОПОЛНИТЕЛЬНОЙ КЛАВИАТУРЫ
 * 
 * Revision 3.2  88/06/27  15:19:43  vsv
 * РЕВИЗИЯ АРХИВА RCS
 * 
 * Revision 3.1  88/04/29  12:25:29  vsv
 * РАЗДЕЛЕНЫ ТАБЛИЦЫ И ПРОЦЕДУРЫ,
 * ИМЕЕТСЯ ДВА РАЗНЫХ МОДУЛЯ
 * 
 */

#include <stdio.h>
#include <setjmp.h>
#include "line.h"
#include "line0.h"

/* СВЯЗЬ ФИЗИЧЕСКИХ И ЛОГИЧЕСКИХ КОДОВ, А ТАКЖЕ ИМЕН КЛАВИШ */
extern  KBL kbl[KBLSIZE];

int     kpadon = 0;     /* ФЛАГ: ПРАВАЯ ДОП. КЛАВИАТУРА ВКЛЮЧЕНА */

#define UNREAD_CHAR_RETRO
#ifdef UNREAD_CHAR_RETRO
/*------------------------------------------*/
/* ВЕРНУТЬ ПРОЧИТАННЫЙ КОД ВО ВХОДНОЙ ПОТОК */
/*------------------------------------------*/
static  kbcod   backcod = 0;

sigjmp_buf jenv;

void unr_c(cod)
kbcod   cod;
{
	backcod = cod;
}

void jkb_re() {
	siglongjmp(jenv, 1);
}

#endif

int k_pad(on)
int on;
{
	/* этот модуль нужен для io_set */
	if(on) {
		kpadon = 1; w_raw(t_ks);
	}
	else    {
		kpadon = 0; w_raw(t_ke);
	}
	fflush(vttout);
}

int lastchr; /* last parsed printable code - TODO: there will be stored full utf8 encoded symbol from input */
char  r_chr() /* TODO: UTF8 support; wchar r_chr() */
{
	return (lastchr & 0377); /* 8-bit, no utf8 support yet */
}

kbcod r_cod(cod)
/*------------------------*/
/* ВЕРНУТЬ ЛОГИЧЕСКИЙ КОД */
/*------------------------*/
kbcod cod;
{
	register KBL *kblp;
	kbcod   bckc;

#ifdef UNREAD_CHAR_RETRO
	int jmp;

	jmp = sigsetjmp(jenv, 1);
	if (jmp != 0) {
		fflush(vttout);
		/*unr_c(KB_RE);*/
		backcod = KB_RE;
	}

	if(backcod) { bckc = backcod; backcod = 0; return(bckc); }
#endif
	if(cod==0) {
		cod=r_key(); /* сначала прочитать код с клавиатуры и распознать по базе termcap/terminfo*/
	}

	lastchr = cod;
	if (cod == 0 || cod == -1) {
		/*клавиша не опознана, но была нажата*/
		return(0); /*(-1);*/
	}

	/*сопоставить с базой назначения логических кодов (не путать с termcap/terminfo) */
	for(kblp=kbl; kblp->t_cod; kblp++) {
		if(kblp->t_key1 == cod || kblp->t_key2 == cod) {
			cod = (kblp->t_cod);
			break;
		}
	}

	/* ВКЛЮЧИТЬ/ВЫКЛЮЧИТЬ ДОПОЛНИТЕЛЬНУЮ КЛАВИАТУРУ */
	if(cod == KB_KP) {
		if(kpadon) { kpadon = 0; w_raw(t_ke); }
		else       { kpadon = 1; w_raw(t_ks); }
		fflush(vttout);
	}
	return(cod);
}

