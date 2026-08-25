/*
**      +----------+    БИБЛИОТЕКА ВВОДА-ВЫВОДА
**     (c) linlib  !    ДЛЯ АЛФАВИТНО-ЦИФРОВЫХ
**      +----------+    ВИДЕОТЕРМИНАЛОВ
**/

/*
 *      $Header$
 *
 *      $Log$
 */

/* ВСЕ ТАБЛИЦЫ НАХОДЯТСЯ В ИСПОЛНЯЮЩЕЙ БИБЛИОТЕКЕ LINLIB */
#ifndef line0_h_def
#include "line0.h"
#endif
extern  LPA lpainp[];
extern  LPA lpaout[];
extern  KBL kbl[];
/*extern  int cvt_sp();*/

/* vhset private formatter of attributes */
extern  int cvt_atr(LINE *line, kbcod cod, char *mod, char *str);
