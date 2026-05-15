/*
 * VASH - visual assistant for shell
 * Copyright (c) 1990-2025 Sergey Vovk <vsv>
 *
 * License: GPL and/or MIT,
 * see files COPYING and LICENSE
 */

#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include "line.h"
#include "assist.h"
#include "astat.h"

/* встроенные функции */
extern  int     sup(), v_susp();
extern  int     vshcmd(), menu2();
extern  int     fsh(), fshrpt(), fmenu2();
extern  int     ffile(), fmsg(), fmsgerr();
/*extern  int     rescan();*/
/* перенесены из встроенных фунций */
extern  int		f_ls();	/* редактирование строки команды заполнения меню из .ashstd -@f */
extern  int		f_mark(); /* пометка пунктов меню (имен файлов) + - > < */

extern  char  *nmsubs();

#define DEBUG_KSHOW
#ifdef DEBUG_KSHOW
extern  int     kshow();
#endif

/*extern  int     vashelp();*/

FUNTAB funtab[] = {
	{ "read",       0 },
	{ "sh",         fsh },
	{ "shrpt",      fshrpt },
	{ "menu2",      fmenu2 },
	{ "cmdset",     cmdset },
	{ "sup",        sup },
	{ "sus",        v_susp },
	{ "file",       ffile },
/*	{ "help",       w_help },*/
	{ "help",       vashelp },
	{ "rchelp",		rchelp },
	{ "msg",        fmsg },
	{ "err",        fmsgerr },
	{ "rescan",     rescan },
	{ "f_ls",		f_ls },
	{ "mark",		f_mark },
	{ "itmpos",		itmpos },
/*	{ "itmsel",		itmsel }, not used? */
#ifdef DEBUG_KSHOW
	{ "kshow",      kshow },
#endif
	{ "ls",         0 },
	{ (char *)0,    0 },
};

/*extern  char    *Cfill;*/
extern  char    Crepf[];
extern  char    Coutf[];
extern  char    Csubs[];

/*extern  PATCMD  pc[];*/
extern  KEYTAB  kt2[];
extern  KEYTAB  kt1[];

static u8char_t lastkey[4];     /* код последней нажатой клавиши */

/*ARGSUSED*/
int
vcmd(/*j, */cod/*, mainl*/)
/*
 * Интерпретировать клавишную команду
 *
 * Две фазы интерпретации:
 * 1. Проверяется правомочность самой команды по таблице kt1.
 *    Сравниваются права самого каталога, наличие пометки.
 * 2. Если имя действия начинается со знака '+',
 *    интерпретируется таблица kt2.
 *    Сравниваются права файла, его тип, шаблон имени.
 * Возвращаемые значения:
 *      -1      не найдено соответствие, экран не испорчен.
 *       0      отказ от действия, экран не испорчен.
 *       1      команда выполнена, экран испорчен.
 */
/*int     j; */     /* индекс пункта рядом с курсором */
kbcod   cod;    /* код нажатой клавиши */
/*LINE    *mainl;*/ /* указатель на страницу меню */
{
	char  file[256*4];
	char    cods[20];       /* строка с идент. встроен. команды */
	register KEYTAB *ktp;
	register PATCMD *pcp;
	register int i;
	char   *patp;           /* образец для сравнения */
	char   *cmd;

	/* получить строку с кодом клавиши, найти совпадение. */
	/* it was much simple at 8-bit encodings epoch...*/
	if (ISCTL(cod)) {
		lastkey[0] = cod0(cod);
		lastkey[1] = cod1(cod);
	} else if (cod < 0200) {
		lastkey[0] = cod0(cod);
		lastkey[1] = 0;
	} else {
		/*
		 * workaround: any printable non-ascii but single-byte code;
		 * normal solution may be to use wchar_t for patcmp()
		 * and lastkey[] and so on;
		 * anyway rc files are encoded ASCII-only
		 */
		lastkey[0] = 0243 /*0xff & cod*/;
		lastkey[1] = 0;
	}
	lastkey[2] = 0;

	for (ktp=kt1; ; ktp++) {
		if (ktp->kt_key == (char *)0)
			return(-1);     /* ничего не найдено */
		if (patcmp(ktp->kt_key, lastkey))
			break;
	}

	/* получить образец первого условия, найти совпадение */
	patp = tstat1();
	for (pcp=ktp->kt_tab, i=ktp->kt_ib; i <= ktp->kt_ie; i++) {
		if (pcp[i].pc_pat == (char *)0)
			return(-1);     /* конец таблицы */
		if (patcmp(pcp[i].pc_pat, patp))
			break;
		if (i == ktp->kt_ie)
			return(-1);     /* конец поиска, ничего не найдено */
	}
	cmd = pcp[i].pc_cmd;    /* команда получена */
	/* выполнить команду */
	/* если команда нач. со зн. '+' */
	if (*cmd == '+') {
		/* найти программу для второго условия */
		for (i=0, cmd++; cmd[i] && cmd[i] != ' '; i++)
			cods[i] = cmd[i];
		cods[i] = 0;
		for (ktp=kt2; ; ktp++) {
			if (ktp->kt_key == (char *)0)
				return(-1);     /* ничего не найдено */
			if (patcmp(ktp->kt_key, cods))
				break;
		}
		/* получить образец второго условия, найти совпадение */
/*                cmdsub(file, &itms[itm][2]);  */
		cmdsub(file, "#@", clm._itm, 0, 1);
		patp = tstat2(file);
		for (pcp=ktp->kt_tab, i=ktp->kt_ib; i <= ktp->kt_ie; i++) {
			if (pcp[i].pc_pat == (char *)0)
				return(-1);     /* конец таблицы */
			if (patcmp(pcp[i].pc_pat, patp))
				break;
			if (i == ktp->kt_ie)
				/* конец поиска, ничего не найдено */
				return(-1);
		}
		cmd = pcp[i].pc_cmd;    /* новая команда получена */
	}
	if (*cmd == '_' && cmd++) {
		/* встроенная команда */
		return(vincmd(cmd)); /*internal command*/
	}
	return(vexcmd(cmd, (char *)0)); /* external command */
}

int vincmd(cmdarg)
/*register*/ char *cmdarg;     /* встроенная функция */
{
	char  keywd[20];        /* ключевое слово команды */
	char *keywdp;
	int (*keyf)(const char *);
	register FUNTAB *ftabp;
	char *cmd;
	int ret;

	cmd = cmdarg;
	keywdp = keywd;
	while (*cmd && *cmd != ' ')
		*keywdp++ = *cmd++;
	*keywdp = 0;
	keywdp = keywd;
	while (*cmd && *cmd == ' ') cmd++;   /* остаток команды */

	/* установить соответствие */
	for (ftabp = funtab; ftabp->ft_name; ftabp++) {
		if (strcmp(ftabp->ft_name, keywdp) == 0) {
			if ((keyf = ftabp->ft_fun)) {
				/* если функция найдена, ее и вызвать */
				ret = (*keyf)(cmd);
				return(ret);
			}
			else    break;
		}
	}
	w_emsg(keywd); w_str(cmd);
	return(-1);     /* функция не найдена */
}

/*
 * выполнить подстановки #k,#@,#i,#o,#a,..., кроме ##.
 */

extern int sh_esc(char *, char *);

int cmdsub(ptmp_sh, p, i, need_sh_esc, subatrc)
/*возвращает то же, что и nmsubs - необходимость обработки экранированных символов /bin/sh*/
register char *ptmp_sh;    /* куда копировать */
register const char *p;       /* откуда копировать */
register int  i;        /* копия itm */
int need_sh_esc;			/*требуется экранирование для /bin/sh*/
int subatrc;		/* substitution of #@ required */
{
    extern char *getenv();
    const char *s;
    char *nm_ptr;
    int sh_req = 0;	/* флаг - сделаны подстановки, требуется вызов /bin/sh*/
    char *ptmp;
    char  ptmp_buf[U8_STRBUF];

    ptmp = ptmp_buf/*alloca(U8_STRBUF)*/;

    *ptmp_sh = '\0';
    while (*p) {
	if (*p == MONEY) {
	    p++;
	    switch(*p) {

	    default:    /* пропустить без изменений */
		*ptmp_sh++ = MONEY;
		*ptmp_sh++ = *p++;
		continue;
	    case 0:     /* КОНЕЦ */
		break;
	    case 'k':   /* код последней клавиши */
		strcpy(ptmp, lastkey);
		break;
	    case 'A':   /* vexdir contains library path, eg /usr/lib/vash */
		strcpy(ptmp, vexdir);
		break;
	    case '@':
	    	/* имя файла возле курсора, или его часть если есть правило */
	    	if (subatrc == 0) {
	    		*ptmp_sh++ = MONEY;
	    		*ptmp_sh++ = *p++;
	    		continue;
	    	} else {
	    	    nm_ptr = nmsubs(&clm._itms[i][2], Csubs);
	    	    strcpy(ptmp, nm_ptr);
	    	}
		break;
	    case '$':
	    	/* имя файла возле курсора - аналогично #@, но безусловно */
    	    nm_ptr = nmsubs(&clm._itms[i][2], Csubs);
    	    strcpy(ptmp, nm_ptr);
	    	break;
	    case 'i':   /* имя реперного файла/текущего каталога */
		strcpy(ptmp, Crepf);
		break;
	    case 'o':   /* имя целевого (второго) файла/каталога */
		strcpy(ptmp, Coutf);
		break;
	    case 'e':   /* имя редактора текстов (EDITOR=) */
		if ((s=getenv("EDITOR")) == NULL)
#ifdef DEMOS2_EDITOR
		    s = "re";   /* стандарт ДЕМОС */
#else
		    s = "vi";   /* стандарт UNIX */
#endif
		strcpy(ptmp, s);
		break;
	    case 'm':   /* имя программы просмотра, обычно more */
		if ((s=getenv("PAGER")) == NULL)
		    s = "more";
		strcpy(ptmp, s);
		break;
	    case '*':   /* имя файла до последнего суффикса */
		fnsplit(ptmp, &clm._itms[i][2], 0);
		break;
	    case 's':   /* последний суффикс имени файла, с точкой */
		fnsplit(ptmp, &clm._itms[i][2], 1);
		break;
	    case 'S':   /* последний суффикс имени файла, без точки */
		fnsplit(ptmp, &clm._itms[i][2], 2);
		break;
#ifdef DEVELOPE_NEVER_USED
	    case 'n':   /* новое имя для создания/переименования */
	    case 'a':   /* строка пользователя */
	    case 'b':   /* строка пользователя */
#endif
	    }
	    if (need_sh_esc) {
	    	sh_req += sh_esc(ptmp_sh, ptmp);
	    } else {
	    	strcpy(ptmp_sh, ptmp);
	    }
	    p++;
	    while (*ptmp_sh) ptmp_sh++;
	    continue;
	}
	else
	    *ptmp_sh++ = *p++;
    }
    *ptmp_sh = '\0';       /* конец подстановки */
    return(sh_req == 0 ? 0 : 1);
}

/*ARGSUSED*/
int vexcmd(cmd, cmdlbl)
char *cmd;      /* "сырая" команда, требуются подстановки */
char *cmdlbl;   /* вывеска взамен команды */
{
	/*char    tmpcmd[140];*/
	char tmpcmd[U8_STRBUF];

	/*tmpcmd = alloca(U8_STRBUF);*/

	if (cmd) {
	    /* выполнить подстановки */
	    cmdsub(tmpcmd, cmd, clm._itm, 1, vflag.subatrc); /* TODO: при команде cd здесь не нужно задваивать символы '\\' */
	    cmd = tmpcmd;
	}
	return( vshcmd(cmd, NULL/*(char *)0*/) );
}

int
fsh(cmd)        /* команда sh */
/*register */char *cmd;
{
	int ret;
	if (*cmd == '\0')
		/* vshcmd будет использовать старую команду */
		cmd = NULL/*(char *)0*/;
	ret = vexcmd(cmd, NULL/*(char *)0*/);
	return(ret);
}

int
fshrpt(cmd)       /* repeat command */
/*register */char *cmd;
{
	int ret;
	/**cmd = '@';*/
	ret = vshcmd("@", NULL);
	return(ret);
}

static int x_rckey;
static void
rckeyprnt(rckey)
char *rckey;
{
	register int i, x;
	int ypos, xpos;
	/*sprintf(tmps, " %s", kt1[i].kt_key);*/

	xpos = (x_rckey / 10) * 12;
	ypos = (x_rckey % 10) + clm._y0;
	cp_set(ypos, xpos, TXT);
	w_lbl(LKEY, rckey);
	er_eol(TXT);
	x_rckey++;
}

int
rckeys(file)
char *file;
{
	extern int  y0_top;     /* определено в main.c //vshcmd */
	kbcod cod;
	LINEMENU savelm;
	char *tmpkey;
	char tmps[300];
	register int i;
	int ypos, xpos;

	savelm = clm;
#if 0
	cp_set(clm._y0 + 1, 0, TXT);   /* СОХРАНИТЬ СВИТОК, СМ. НИЖЕ */
	er_eop(TXT);
#endif
	cp_set(clm._y0 - 1, 0, TXT);
	clm._y0 = 12;
	/* keep scroll area */
	if (y0_top > clm._y0) {
		y0_top = clm._y0;
		scrlnl();
	}
	cp_set(clm._y0 - 1, 0, HDR);
	w_str(file);
#if 0
	cp_set(y0_top, 0, TXT);
	w_str(file);
	/*er_eop(TXT);*/
	cp_set(-1, 0, TXT);
	w_msg(TXT, "*>");
	r_cod(0);
#endif
	x_rckey = 0;
	for (i = 0; kt1[i].kt_key; i++) {
		tmpkey = kt1[i].kt_key;
		sprintf(tmps, " %s", tmpkey);
		if (patcmp("[0-9]", tmpkey))
			continue;
		if (patcmp("U[0-9]", tmpkey))
			continue;
		if (patcmp("?", tmpkey))
			rckeyprnt(tmps);
	}
	for (i = 0; kt1[i].kt_key; i++) {
		tmpkey = kt1[i].kt_key;
		sprintf(tmps, " %s", tmpkey);
		if (patcmp("[0-9]", tmpkey))
			continue;
		if (patcmp("U[0-9]", tmpkey))
			continue;
		if (patcmp("??", tmpkey))
			continue;
		if (patcmp("?", tmpkey))
			continue;
		rckeyprnt(tmps);
	}

	w_msg(TXT, ""); /* assumed cp_set(-1, 0, TXT) */
	w_lh_msg("press :SP to continue:");
	cod = r_cod(0);
	w_msg(TXT, "");
	clm = savelm;

	return 1;
}

int
rchelp(file)        /* показать легенду профиля */
const /*register*/ char *file;
{
	char    cmdlbl[100];    /* вывеска взамен команды */
	char    tmpcmd[40];
	char	*tmpflnm;

	switch(rckeys(file)) {
	case 1:
		/* результат команды игнорируется */
		/* NOBREAK */
	case 0:
		/* восстановить гл. меню */
		return( 1 );
	}
	return( 0 );
}

int
fmenu2(file)        /* меню из файлов .ashmenu */
register char *file;
{
	extern int unlink(); /*workaround define*/

	char    cmdlbl[100];    /* вывеска взамен команды */
	char    tmpcmd[40];
	char	*tmpflnm;

	switch(menu2(tmpcmd, cmdlbl, file)) {
	case 1:
		/* результат команды игнорируется */
		vshcmd(tmpcmd, cmdlbl);
		unlink(tmpflnm);
		/* NO BREAK */
	case 0:
		/* восстановить гл. меню */
		return( 1 );
	}
	return( 0 );
}

int
ffile(cmd)
/*register*/ const char *cmd;
{
	char  file[140];

	cmdsub(file, cmd, clm._itm, 0, 1);
	w_msg(TXT, tstat2(file));
	return( 0 );
}

int
fmsg(s)
/*register*/ const char *s;
{
	w_msg(TXT, s);
	return(0);
}

int
fmsgerr(s)
/*register*/ const char *s;
{
	w_msg(ERR, s);
	return(0);
}

int
kshow(s)
/*
 * Показать программу настройки (для отладки)
 */
/*register*/ const char *s;
{
	char tmps[300];
	register int i, j;

	for (i=0; kt1[i].kt_key; i++) {
		sprintf(tmps, "kt1: %d:%d:'%s'",
		kt1[i].kt_ib,
		kt1[i].kt_ie,
		kt1[i].kt_key);
		cp_set((i%10)+2, 0, TXT); w_str(tmps); er_eol(TXT);
		if ( (i+1) % 10 ) ;
		else    { bell(); r_cod(0); er_pag(); }
	}
	for (i=0; kt2[i].kt_key; i++) {
		sprintf(tmps, "kt2: %d:%d:'%s'",
		kt2[i].kt_ib,
		kt2[i].kt_ie,
		kt2[i].kt_key);
		cp_set((i%10)+2, 0, TXT); w_str(tmps);
		if ( (i+1) % 10 ) ;
		else    { bell(); r_cod(0); er_pag(); }
	}
	return(1);
}
