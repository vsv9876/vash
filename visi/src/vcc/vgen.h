/*
 *      vgen = Visi GENeration
 *
 *      $Header: vgen.h,v 1.2 90/12/27 17:01:18 vsv Exp $
 *      $Log:	vgen.h,v $
 * Revision 1.2  90/12/27  17:01:18  vsv
 * *** empty log message ***
 * 
 * Revision 1.1  89/08/29  16:01:12  vsv
 * Initial revision
 * 
 */

#define HS_SIZE 016000    /* размер буфера для кучи строк, в байтах */
#define HS_MAXNUM 200		/*количество строк в куче*/

/*
 * ОПРЕДЕЛИТЬ ТОЛЬКО ОДИН ФЛАГ ГЕНЕРАЦИИ:
 * ПРИМЕЧАНИЕ:
 *       В ОС ДЕМОС ЭТО ДЕЛАЕТСЯ ЧЕРЕЗ Makefile
 */
/* #define MAKE_VLBP            /* ПРЕПРОЦЕССОР ДЛЯ lbp */
/* #define MAKE_VCC             /* ПРЕПРОЦЕССОР ДЛЯ СИ-КОМПИЛЯТОРА */
/* #define VINTF           /* ФИЛЬТР ДЛЯ ИНТЕРПРЕТАТОРА vint */

#define MAX_BUF_LI 60
#define MAX_BUF_CO 80 /*132*/
#define MAX_TMP_LEN 200

/* КОНСТАНТЫ ПРЕПРОЦЕССОРА */
#define LBPOSIZE MAX_BUF_CO*4 /*200             /* РАЗМЕР ВРЕМЕННОЙ СТРОКИ */

