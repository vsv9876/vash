/*
**      +----------+    БИБЛИОТЕКА ВВОДА-ВЫВОДА
**     (c) linlib  !    ДЛЯ АЛФАВИТНО-ЦИФРОВЫХ
**      +----------+    ВИДЕОТЕРМИНАЛОВ
**/

/*
 *      $Header: line0.h,v 3.4 90/01/11 10:18:16 vsv Rel $
 *
 *      $Log:	line0.h,v $
 * Revision 3.4  90/01/11  10:18:16  vsv
 * ВЕРСИЯ V32
 * 
 * Revision 3.3  89/08/29  16:21:42  vsv
 * ВЕРСИЯ LINLIB_3
 * 
 * Revision 3.2  88/04/27  11:10:28  vsv
 * maxli, maxco В ФАЙЛЕ line.h, ТАК БОЛЕЕ УДОБНО.
 * 
 * Revision 3.1  88/04/27  08:40:08  vsv
 * ПЕРЕДЕЛАНО ДЛЯ ПОДДЕРЖКИ
 * СИСТЕМЫ VISI
 * 
 */
#ifndef line0_h_def
#define line0_h_def

/*
 * ОПИСАНИЕ ВОЗМОЖНОСТЕЙ ТЕРМИНАЛА
 */

/*#define  TTY_FULL       /* ПОЛНЫЕ ВОЗМОЖНОСТИ TERMCAP (ТОЛЬКО ДЛЯ RT-11) */
			/* СМ. ТАКЖЕ TRMCAP.C */
/*
 * temporary buffer size for hf_set() internal usage,
 * must be conformed with BUFSIZ from termcap.c, for tgetent()...
 */
#define TBUFSZ 2048

/* ОПИСАНИЕ ОДНОЙ КЛАВИШИ:
 */
typedef struct {
	kbcod   t_key;  /* ФИЗ. КОД ('ku') */
	char   *t_cap;  /* ESC-КОД (:ku=\EA:) */
} KBF;  /* ФИЗИЧЕСКОЕ - КАК ОПИСАНА В termcap */

typedef struct {
	kbcod   t_key;  /* ФИЗ. КОД ('ku') */
	kbcod   t_cod;  /* ЛОГ. КОД ('AU') */
	char    t_knm[8];  /* НАЗВАНИЕ КЛАВИШИ ("ВВЕРХ") */
} KBL;  /* ЛОГИЧЕСКОЕ - СМЫСЛОВОЙ КОД И НАЗВАНИЕ */
#define KBLSIZE 48

/* ПОДСКАЗКИ И ВИДЕОАТРИБУТЫ */
typedef struct {
	int     lpa_p;  /* ПОДСКАЗКА */
	int     lpa_a;  /* СЛОВО АТРИБУТОВ */
	char    lpa_sgr[20]; /* ANSI Color SGR, instant ASCII format (not coded), 38;5;256;48;5;000 */
} LPA;
#define LPASIZE 8

/* ТЕКУЩЕЕ СОСТОЯНИЕ ЭКРАНА
 */
typedef struct {
	bool    sc_li;  /* LIne - ТЕКУЩ. СТРОКА */
	bool    sc_co;  /* COlumn - ТЕКУЩ. ПОЗИЦИЯ В СТРОКЕ */
	int     sc_at;  /* ATtributes - ТЕКУЩЕЕ СЛОВО АТРИБУТОВ */
/*	char   *sc_ac;     w_sgr() arg - ANSI color */
} SCRN;

/* ВОЗМОЖНОСТИ ВЫВОДА НА ТЕРМИНАЛ ИЗ /etc/termcap;
 * ИМЯ КЛАВИАТУРЫ ДЛЯ НАСТРОЙКИ КЛАВИШ ТОЖЕ ЗДЕСЬ (lh= ... )
 */
extern char *tcapo[];

#define t_cm  tcapo[0]  /* ПРЯМАЯ АДРЕСАЦИЯ КУРСОРА */
#define t_cl  tcapo[1]  /* СТЕР. ЭКРАН */
#define t_cd  tcapo[2]  /* КОНЕЦ ЭКРАНА */
#define t_ce  tcapo[3]  /* КОНЕЦ СТРОКИ */
#define t_ks  tcapo[4]  /* ВКЛ/ВЫКЛ. ДОП. КЛАВ. */
#define t_ke  tcapo[5]
#define t_cs  tcapo[6]  /* ПРОКРУТКА */
#define t_sr  tcapo[7]
#define t_sf  tcapo[8]
#define t_al  tcapo[9]  /* ВСТАВ/УДАЛ. СТРОКИ */
#define t_dl  tcapo[10]
#define t_so  tcapo[11] /* ВЫДЕЛЕНИЕ */
#define t_se  tcapo[12]
#define t_us  tcapo[13] /* ПОДЧЕРКИВАНИЕ */
#define t_ue  tcapo[14]
#define t_md  tcapo[15] /* ЯРКОСТЬ */
#define t_mr  tcapo[16] /* РЕВЕРС */
#define t_mb  tcapo[17] /* МИГАНИЕ */
#define t_mk  tcapo[18] /* РАМКА */
#define t_me  tcapo[19] /* ВСЕ УБРАТЬ */
#define t_bl  tcapo[20] /* ЗВОНОК */
#define t_vb  tcapo[21] /* ВИДЕОЗВОНОК */
#define t_lh  tcapo[22] /* ИМЯ КЛАВИАТУРЫ (РАСШИРЕНИЕ LINLIB) */

#endif /* line0_h_def */
