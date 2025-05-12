/*
 * VISI(LINLIB)
 * Copyright (c) 1986-1990 Sergey Vovk and the team of RIAR, Dimitrovgrad, USSR
 * Copyright (c) 2017-2025 Sergey Vovk
 *
 * VISI -- a visual interactive simple interface for non-GUI terminals
 * LINLIB -- library for video terminals
 *
 * This is free software, 
 * please keep applied LICENSE file and copyright notice above
 *
 */

#ifndef line0_h_def
#define line0_h_def

/*
 * terminal support based on termcap capability
 */

/*#define  TTY_FULL       /* ПОЛНЫЕ ВОЗМОЖНОСТИ TERMCAP (ТОЛЬКО ДЛЯ RT-11) */
			/* СМ. ТАКЖЕ TRMCAP.C */
/*
 * temporary buffer size for hf_set() internal usage,
 * must be conformed with BUFSIZ from termcap.c, for tgetent()...
 */
#define TBUFSZ 2048

/*
 * single key description
 */
typedef struct {
	kbcod   t_key;  /* termcap capability key ('ku') */
	char   *t_cap;  /* ESC code (:ku=\EA:) */
} KBF;  /* phisical - as described in termcap */

typedef struct {
	kbcod   t_cod;  /* logical  'AU' to be assigned to phisical */
	char    t_knm[8];  /* key description (label), "up arrow" */
	kbcod   t_key1;  /* phisical primary 'ku' */
	kbcod   t_key2;  /* phisical secondary */
} KBL;  /* translation to logical */
#define KBLSIZE 48

typedef struct {
	const kbcod	  t_cod;  /* logical 'AU' */
	const char *  t_descr;/* human description, eg. 'cursor up'*/
} KBDESCR;

/*
 * prompter and video attributes
 */
typedef struct {
	int     lpa_p;  /* prompter character */
	int     lpa_a;  /* word of attributes */
	char    lpa_sgr[20]; /* ANSI Color SGR,
					 instant ASCII format, eg. '38;5;256;48;5;000' */
} LPA;
#define LPASIZE 8

/*
 * current state of screen, point on screen before output of printable char(s)
 */
typedef struct {
	bool    sc_li;  /* line */
	bool    sc_co;  /* column */
	int     sc_at;  /* attributes */
/*	char   *sc_ac;     w_sgr() arg - ANSI color */
} SCRN;

extern int		sgrmode; /* runtime attributes setup mode */

/*
 * list of termcap capabilities (retrieved from /etc/termcap
 * most common based on restricted of DEC vt100
 */
extern const char *tcapo[];

#define t_cm  tcapo[0]  /* direct cursor addressing */
#define t_cl  tcapo[1]  /* erase (clear) screen */
#define t_cd  tcapo[2]  /* clear to end of screen */
#define t_ce  tcapo[3]  /* clear to end of string (row) */
#define t_ks  tcapo[4]  /* keypad on, off */
#define t_ke  tcapo[5]
#define t_cs  tcapo[6]  /* scroll region set */
#define t_sr  tcapo[7]  /* scroll rewind */
#define t_sf  tcapo[8]  /* scroll forward */
#define t_al  tcapo[9]  /* insert (append) row */
#define t_dl  tcapo[10] /* destroy (exclude) row */
#define t_so  tcapo[11] /* stand out */
#define t_se  tcapo[12] /* stand out end */
#define t_us  tcapo[13] /* underline */
#define t_ue  tcapo[14] /* underline end */
#define t_md  tcapo[15] /* bright */
#define t_mr  tcapo[16] /* revers */
#define t_mb  tcapo[17] /* blink */
/*#define t_mk  tcapo[18]*/
#define t_mh  tcapo[18] /* dim */
#define t_me  tcapo[19] /* all remove */

#define t_bl  tcapo[20] /* bell */
#define t_vb  tcapo[21] /* visual bell */
#define t_zh  tcapo[22] /* start italic */
#define t_zr  tcapo[23] /* start italic */
#define t_lh  tcapo[24] /* obsoleted, LINLIB extension */

#endif /* line0_h_def */

