/*
 * РАБОТА С БУФЕРОМ КОМАНДНЫХ СТРОК.
 * НЕЧТО НАПОДОБИЕ ИСТОРИИ, ЗАПОМИНАЮТСЯ
 * ТОЛЬКО УНИКАЛЬНЫЕ КОМАНДЫ.
 */

#include <sys/types.h>
#include <sys/stat.h>

#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdio.h>
#include <ctype.h>
#include "line.h"       /* ФАЙЛ-ЗАГОЛОВОК LINLIB */
#include "assist.h"

#ifdef TINYSMALL
#define CMDB 512        /* РАЗМЕР БУФЕРА КОМАНД */
#define CMDP 40         /* КОЛИЧЕСТВО КОМАНД В БУФЕРЕ */
#define CMDHL 8         /* КОЛИЧЕСТВО КОМАНД В МЕНЮ НА ЭКРАНЕ */
#else
#define CMDP 120         /* КОЛИЧЕСТВО КОМАНД В БУФЕРЕ */
#define CMDHL 8         /* КОЛИЧЕСТВО КОМАНД В МЕНЮ НА ЭКРАНЕ */
#define CMDB (CMDP * MAXLICO) /* 8192       /* два полных экрана истории хватит... */
#endif /* TINYSMALL */

extern char *homedir;
extern int histsn;

static  char    cmdb[CMDB+1];   /* БУФЕР КОМАНД */
static  char   *cmdp[CMDP+1];   /* УКАЗАТЕЛИ НА КОМАНДЫ */
static  int     cmdplast = 0;   /* ИНДЕКС ПОСЛЕДНЕЙ КОМАНДЫ */
static  int     cmdpi = 0;      /* ИНДЕКС ПОСЛЕДНЕЙ ВЗЯТОЙ/ПОЛОЖ. КОМАНДЫ */
static  int     cmdbot = 0;     /* ИНДЕКС СВОБОДНОГО МЕСТА В БУФЕРЕ */

int cmddel(cmd)
/*
 * УБРАТЬ ИЗ БУФЕРА
 * возвращается номер удаляемой команды.
 */
char *cmd;
{
	register int i;
	int saven;      /* индекс указателя следующей команды */
	int delsize;    /* размер удаляемой команды */

/***
	if (*cmd == '\0')
		return(-1);
 ***/
	for (i = 0; cmdp[i]; i++)
		if (strcmp(cmd, cmdp[i]) == 0) {
			delsize = strlen(cmd) + 1;
			/* СКОПИРУЕМ СОДЕРЖИМОЕ БУФЕРА В НОВОЕ МЕСТО */
			for (saven = i + 1; saven < cmdplast; i++,saven++) {
				cmdp[i] = cmdp[saven] - delsize;
				strcpy(cmdp[i], cmdp[saven]);
			}
			cmdbot -= delsize;
			saven = i; cmdplast--;
			while (i <= CMDP)
				cmdp[i++] = 0;
			return(saven);
		}
	return(-1);
}

void cmdsqz(reqsz)
/*
 * ПРОЧИСТИТЬ СТАРЫЕ КОМАНДЫ И СЖАТЬ */
/**/
int     reqsz;  /* РАЗМЕР, КОТОРЫЙ ТРЕБУЕТСЯ ОСТАВИТЬ СВОБОДНЫМ */
{
	register int i;
	int     realsz; /* освобождаемый размер */
	int     saven;  /* прежний индекс самой старой сохраняемой команды */

	/* если место есть, ничего не делать */
	if ((reqsz < (CMDB - cmdbot)) && cmdplast < CMDP)
		return;

	/* посчитаем занятое командами место */
	i = realsz = 0;
	do {
		realsz += strlen(cmdp[i++]) + 1;
	} while (*cmdp[i] && ((CMDB - cmdbot) + realsz) < reqsz);
	saven = i;

	/* можно не проверять, если размер буфера не меньше двух строк: */
	/* проверить, реально ли освободилось... */
	if (((CMDB - cmdbot) + realsz) < reqsz)
		/* уничтожим все содержимое буфера */
		i = cmdbot = cmdpi = 0;
	else {
		/* скопируем содержимое буфера в новое место */
		for (i = 0; saven < cmdplast; i++,saven++) {
			cmdp[i] = cmdp[saven] - realsz;
			strcpy(cmdp[i], cmdp[saven]);
		}
		cmdbot -= realsz;
	}
	cmdplast = i;
	if (cmdpi > cmdplast)   /* не промазать с текущей историей!!! */
		cmdplast = i;
	while (i <= CMDP)       /* занулить незанятые указатели */
		cmdp[i++] = 0;
}

int cmdput(cmd)
/*
 * ПОЛОЖИТЬ В БУФЕР
 */
char *cmd;
{
	int newsize;
	char *p;

	if (*cmd == 0)          /* НИЧЕГО НЕ ДЕЛАТЬ */
		return(0);
	cmddel(cmd);           /* УБРАТЬ СТАРУЮ КОПИЮ */
	newsize = strlen(cmd) + 1;
	cmdsqz(newsize);

	/* ПОЛОЖИТЬ НОВУЮ КОМАНДУ */
	p = cmdb;
	p += cmdbot;
	cmdp[cmdplast] = p;
	strcpy(cmdp[cmdplast], cmd);

	cmdplast++;
	cmdpi = cmdplast;       /* МОДИФИЦИРОВАТЬ ИНДЕКСЫ */
	cmdbot += newsize;

#ifdef DURA
	/* сначала синхронизация истории в файл? */
	/* при каждом изменении истории команд!!! */
	if (histsn == 1) cmdphist(homedir);
#endif
	return(-1);
}

int cmdprv(cmd)
/*
 * ВЗЯТЬ ПРЕДЫДУЩУЮ КОМАНДУ ИЗ БУФЕРА
 */
char *cmd;
{
	if (cmdpi > 0) cmdpi--;
	else    return(0);
	if (cmdp[cmdpi])
		strcpy(cmd, cmdp[cmdpi]);
	return(1);
}

int cmdnxt(cmd)
/*
 * ВЗЯТЬ СЛЕДУЮЩУЮ КОМАНДУ ИЗ БУФЕРА
 */
char *cmd;
{
	if (cmdp[cmdpi] && cmdplast > cmdpi)
		cmdpi++;
	if (cmdp[cmdpi]) {
		strcpy(cmd, cmdp[cmdpi]);
		return(1);
	}
	else    *cmd = 0;
	return(0);
}

/*
 * HISTORY support
 *
 * cmdphist() & cmdghist()
 * put & get history file
 *
 * NOTE:
 * assistant shell's history is not a full log of invoked commands like shell .history,
 * but a common cache shared by another vash session (in concurrent way if histsn != 0)
 *
 * this two routines called from main() on vash start and exit regardless of histsn value
 */

/* history cache file*/
char *hfile = "/.ashhist";

/*
 * flag: last timestamp of hfile known in this process of vash
 *
 * read history file, in case:
 *  1) first time
 *  2) file modified since last reading
 * write history file (after every command, if histsn!=0)
 * saving internal mark about st_mtime preventing useless read
 */

static time_t hflast = (time_t)0; /* zero for fisrt time comparizon */

/*
 * get history from file into cmdb[] buffer
 *
 * if histsn==0 (syncronize history is disabled),
 * do it once first time when program started
 *
 * returns 1, if get commands from history file
 * returns 0, if no read done
 */
cmdghist(hdp)
char *hdp;
{
	FILE *fp;
	struct stat	hfstat;
	time_t      hftime;
	char filename[200];
	char cmdbuf[140]; /* one command from file, without trailing '\n' */
	int c;
	register char *p;
	register int i;

	strcpy(filename, hdp);
	strcat(filename, hfile);

	if (stat(filename, &hfstat) < 0) return(0);
	hftime = hfstat.st_mtime;

	if (histsn == 0) {
		if (hflast != 0) return(1);
	}
/*	if (hflast != 0 && hflast == hftime) return(1);*/
	if (hflast < hftime) {
		hflast = hftime;
		if ((fp = fopen(filename, "r")) == NULL) return(0);

		/* cmdb[CMDB+1];   /* БУФЕР КОМАНД */
		/* *cmdp[CMDP+1];   /* УКАЗАТЕЛИ НА КОМАНДЫ */
		for (i = 0; i <= CMDP; cmdp[i++] = 0) ;

		cmdplast = 0;   /* ИНДЕКС ПОСЛЕДНЕЙ КОМАНДЫ */
		cmdpi = 0;      /* ИНДЕКС ПОСЛЕДНЕЙ ВЗЯТОЙ/ПОЛОЖ. КОМАНДЫ */
		cmdbot = 0;     /* ИНДЕКС СВОБОДНОГО МЕСТА В БУФЕРЕ */

		p = cmdbuf;		/* clear for next line from file */
		i = 0;
		while (i < CMDP && (c = getc(fp)) != EOF) {
			*p = c;
			if (c == '\n') {
				*p = '\0'; p = cmdbuf; i++; /* clear buffer for next line */
				cmdput(p);
			} else
				p++;
		}
		fclose(fp);
	}
	return(1);
}

/*
 * put commands from buffer cmdb[] to file in home directory
 */
cmdphist(hdp)
char *hdp;
{
	char filename[200];
	struct stat	hfstat;
	time_t      hftime;
	FILE *fp;
	int ok;
	register char **pp;

	if (hdp == (char *)0) return(0) ; /* history file is not defined */

	strcpy(filename, hdp);
	strcat(filename, hfile);

	if ((fp = fopen(filename, "w")) == NULL)
		return(0);

	for (pp = cmdp; *pp != (char *)0; pp++)
		/*VARARGS*/
		fprintf(fp, "%s\n", *pp);
	ok = (fflush(fp)==EOF ? 0 : 1);
	fclose(fp);

	if (ok && stat(filename, &hfstat) == 0)
		hflast = hfstat.st_mtime;
	return(ok);
}

static  char *cmdpp;

int t_hist(line, cod)
/*
 * ТЕСТ ДЛЯ СТРОК БУФЕРА ИСТОРИИ КОМАНД
 */
register LINE *line;
kbcod cod;
{
	char *cmd;
	/*extern char Cfill[];*/

	cmd = *(char **)(line->varl);

	switch(cod) {
	case ';':
	case ' ':
		/* ДОБАВИТЬ КОМАНДУ В РАБОЧИЙ БУФЕР */
		strcat(cmdpp, cmd);
		break;
	case '=':
		/* использовать как команду-заполнитель главного меню */
		strncpy(Cfill, cmd, MAXLICO);
		break;
	case KB_NL:
		/* СКОПИРОВАТЬ КОМАНДУ В РАБОЧИЙ БУФЕР */
		strcpy(cmdpp, cmd);
		break;
	case KB_DE:
		/* УБРАТЬ КОМАНДУ ИЗ ПАМЯТИ */
		if (cmdplast > 2 ) {
			cmddel(cmd);
			clritm();
			clm._itmmax = cmdplast;
/***
			if (itm < cmdplast);
			else    itm = cmdplast - 1;
			if (itm > 1)
				itm -= 1;
 ***/
			pre_vf();
			itmshow();
			w_page(clm._vf, 0);
			/* синхронизировать историю при удалении каждой команды */
			if (histsn) {
				cmdphist(homedir);
			} /*else {
				cp_set(-1, -14, ATT|INP);
				w_str("will not saved");
			}*/
		}
		else
			bell();
		break;
	}
	return (TRUE);
}

kbcod h_menu()
/*
 * РАБОТА С МЕНЮ БУФЕРА ИСТОРИИ КОМАНД.
 */
{
	register unsigned i;
	kbcod cod;

	/* первоначальный показ на экране */
	cp_set(clm._y0, 0, TXT);
	w_str("!");	er_eop(TXT);
	w_cmd(cmdpp);   /* ОСТАВИТЬ КОМАНДУ НА ЭКРАНЕ */
	itmshow();
	w_page(clm._vf, 0);

	for ( ;; ) {

		i = clm._itm - clm._itmofs;
		cod = r_line( &clm._vf[i], 0 );

		w_emsg("");

		switch (cod) {

		default:
			break;
		case KB_HE:      /* справка */
			cp_set(-1, 0, TXT);
			fprintf(vttout, "Cmd# %2d/%2d, use %d (%d%%)",
			clm._itm, cmdplast, cmdbot, (int)((cmdbot * 100)/CMDB));
			break;
		case '=':
		case ';':
		case KB_CA:
		case KB_EX:      /* выход */
		case ' ':       /* добавить */
		case KB_NL:      /* заменить */
			return(cod);

		case KB_RE:      /* перерисовка */
			er_pag();
			cwdshow();
			w_emsg("");
			itmshow(); w_page(clm._vf, 0);
			break;
#ifdef RETRO
		case KB_DE:
			if (itm - itmofs < 0)
				cod = KB_AU;
			else    break;
			/* проваливаемся... */
#else
		case KB_DE:
			cod = KB_AU;
			/* проваливаемся... */
#endif
		case KB_AL:
		case KB_AU:
		case KB_AD:
		case KB_AR:
			i = itmadj(cod);
			break;
		}
	}
	return(cod);
}

static  LINE tmplate =
/*NOSTRICT*/
{ /*16*/MAXLICO, 0, 0, 0,
       TXT|INP|PMT|NED|LFASTR,
	       0,
		       cvt_sp,
		       t_hist,
			       (char **)0 };

/*
 * submenu for working with history cache
 * returns 0, in case no view was done
 * returns 2, in case select of new content for Cfill
 */
cmdvew(cmd)
char  *cmd;
{
	extern int  y0_top;     /* определено в vshcmd */
	extern char *pmtsh;    /* --"-- */
	LINEMENU savelm;
	kbcod hv_cod;

	int ret = 0;

	if (cmdplast <= 1) {
		bell(); return (ret);
	}
	savelm = clm;
	cp_set(clm._y0 - 1, maxco - 1, TXT);   /* СОХРАНИТЬ СВИТОК, СМ. НИЖЕ */
	/* сначала синхронизация истории из файла? */
	if (histsn) {
		cmdghist(homedir);
		w_str("=");
	}
	er_eop(TXT);

	/* инициализация меню команд */
	clm._itms   = cmdp;          /* УКАЗАТЕЛИ НА СТРОКИ КОМАНД */
	clm._itmmax = cmdplast;      /* ПОСЛЕДНЯЯ КОМАНДА В ИСТОРИИ */
	clm._vf     = (LINE *)0;		/* hint to avoid new overlapping malloc? /* TODO WTF */
	clm._itmlen = maxco - ((strlen(pmtsh)) * 2); /*по феншую, отступ на промптер слева и справа*/
	clm._ltmpl  = &tmplate;

	clm._itm    = cmdpi;         /* ТЕКУЩАЯ КОМАНДА */
	if (cmdpi >= cmdplast) {
		clm._itm -= 1;
	}
	clm._yy_max = 10;
	clm._itmofs = 0;
	while((clm._itm - clm._itmofs) >= clm._yy_max)
		clm._itmofs += clm._yy_max;
	itmini();
	pre_vf();

	/* СОХРАНИТЬ СВИТОК */
	if (y0_top > clm._y0) {
		y0_top = clm._y0;
		scrlnl();
	}
	cmdpp = cmd;    /* ДЛЯ КОПИРОВАНИЯ НОВОЙ КОМАНДЫ */

	hv_cod = h_menu();
	switch(hv_cod) {
	default:
		ret = 1;
		break;
	case '=':
		ret = 2;
		break;
	};

	free((char *)clm._vf); clm._vf = (LINE *)0;
	cp_set(y0_top, 0, TXT); er_eop(TXT);

	cmdpi = clm._itm;    /* НОВОЕ ЗНАЧ. ИНДЕКСА ИСТОРИИ */
	clm._vf = (LINE *)0;
	clm = savelm;
	return(ret);
}
