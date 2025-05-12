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
/*#define CMDHL 8         /* КОЛИЧЕСТВО КОМАНД В МЕНЮ НА ЭКРАНЕ */
#else
#define CMDP 80                /* command power of buffer */
#define CMDB (CMDP * MAXLICO)  /* 8192 */      /* command buffer size */
#endif /* TINYSMALL */

/*extern char *homedir;*/
/*extern int histsn;*/

static  char    cmdbuf[CMDB+1];   /* БУФЕР КОМАНД */
static  char   *cmdptr[CMDP+1];   /* КОМАНДЫ */
static  int     cmdbufree = 0;     /* ИНДЕКС СВОБОДНОГО МЕСТА В БУФЕРЕ */
static  int     cmdplast = 0;   /* ИНДЕКС ПОСЛЕДНЕЙ КОМАНДЫ */

static  char   *cmdpsel[CMDP+1];  /* команды отобранные поиском */

static  int     cmdpi = -3;      /* ИНДЕКС ПОСЛЕДНЕЙ ВЗЯТОЙ/ПОЛОЖ. КОМАНДЫ */
static  int     cmdpisel = -3;      /* ИНДЕКС ПОСЛЕДНЕЙ ВЗЯТОЙ/ПОЛОЖ. КОМАНДЫ в фильтрованом списке */
static  int     cmdsmax;	    /* количество пунктов фильтрованого списка */

void
cmdhreset() {
	/* сбросить все текущие индексы меню истории, применяется при запуске команды */
	/*cmdpi = cmdpisel = -2;*/
	if (cmdplast > 0)
		cmdpi = cmdpisel = cmdplast;
	else
		cmdpi = cmdpisel = -2/*0*/;
}

#if 1
int mystrcpy(to, from)
u8char_t *to;
u8char_t *from;
{
	for(;;) {
		*to++ = *from;
		if (*from == '\0')
			break;
		*from++;
	}
}

#define strcpy mystrcpy
#endif
/*
 * УБРАТЬ ИЗ БУФЕРА
 * возвращается номер удаляемой команды или -1 если удалять было нечего
 */
static int cmddel(cmd0)
/*wchar_t*/u8char_t *cmd0;
{
	static /*u8char_t*/char cmd[U8_STRBUF];

	/*register*/ int i;
	int savenext;      /* индекс указателя следующей команды */
	int delsize;    /* размер удаляемой команды */

	/*wcsu8s*/strcpy(cmd, cmd0);

/***
	if (*cmd == '\0')
		return(-1);
 ***/
	for (i = 0; cmdptr[i] != NULL && i < CMDP; i++) {
		if (strcmp(&cmd[0], cmdptr[i]) == 0) {
			delsize = strlen(cmd) + 1;
			/* copy buffer to new place */
			for (savenext = i + 1; savenext < cmdplast; i++,savenext++) {
				cmdptr[i] = cmdptr[savenext] - delsize;
				strcpy(cmdptr[i], cmdptr[savenext]);
			}
			cmdbufree -= delsize;
			savenext = i; cmdplast--;

			/* настроить индекс на следующую команду после удаленной */
/*
			if (clm._vf != (LINE *)0) {
				clm._itmmax = cmdplast;
				if (clm._itms == cmdptr) {
					cmdpi = i;
				}
				if (clm._itms == cmdpsel) {
					cmdpisel = i;
				}
			}
*/
			while (i <= CMDP)
				cmdptr[i++] = 0;
			return(savenext);
		}
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
	if ((reqsz < (CMDB - cmdbufree)) && cmdplast < CMDP)
		return;

	/* посчитаем занятое командами место */
	i = realsz = 0;
	do {
		realsz += strlen(cmdptr[i++]) + 1;
	} while (*cmdptr[i] && ((CMDB - cmdbufree) + realsz) < reqsz);
	saven = i;

	/* можно не проверять, если размер буфера не меньше двух строк: */
	/* проверить, реально ли освободилось... */
	if (((CMDB - cmdbufree) + realsz) < reqsz)
		/* уничтожим все содержимое буфера */
		i = cmdbufree = cmdpi = 0;
	else {
		/* скопируем содержимое буфера в новое место */
		for (i = 0; saven < cmdplast; i++,saven++) {
			cmdptr[i] = cmdptr[saven] - realsz;
			strcpy(cmdptr[i], cmdptr[saven]);
		}
		cmdbufree -= realsz;
	}
	cmdplast = i;
	if (cmdpi > cmdplast)   /* не промазать с текущей историей!!! */
		cmdplast = i;
	while (i <= CMDP)       /* занулить незанятые указатели */
		cmdptr[i++] = 0;
}

/*
 * ПОЛОЖИТЬ В БУФЕР
 */
int cmdput(newcmd)
u8char_t *newcmd;
{
	int reqsize;
	char *p;
#if 0
	u8char_t cmd[U8_STRBUF];
	wcsu8s(cmd, newcmd);
#else
	u8char_t *cmd;
	cmd = newcmd;
#endif

	if (*cmd == 0)          /* НИЧЕГО НЕ ДЕЛАТЬ */
		return(0);
	cmddel(cmd);           /* УБРАТЬ СТАРУЮ КОПИЮ */
	reqsize = strlen(cmd) + 1;
	cmdsqz(reqsize);

	/* ПОЛОЖИТЬ НОВУЮ КОМАНДУ */
	p = cmdbuf;
	p += cmdbufree;
	cmdptr[cmdplast] = p;
	strcpy(cmdptr[cmdplast], cmd);

	cmdplast++;
	cmdpi = cmdplast;       /* МОДИФИЦИРОВАТЬ ИНДЕКСЫ */
	cmdbufree += reqsize;

#if 0
	/* сначала синхронизация истории в файл? */
	/* при каждом изменении истории команд!!! */
	if (v.flag.histsn == 1) cmdphist();
#endif
	return(-1);
}

/*
 * ВЗЯТЬ ПРЕДЫДУЩУЮ КОМАНДУ ИЗ БУФЕРА
 */
int cmdprv(cmd)
wchar_t *cmd;
{
	wchar_t tmp[STRBUF];
	if (cmdpi >= cmdplast)
		cmdpi = cmdplast/* - 1*/;
	if (cmdpi > 0)
		cmdpi--;
	else
		return (0);
	if (cmdptr[cmdpi]) {
		/* не предлагать дважды последнюю команду */
		u8swcs(tmp, cmdptr[cmdpi]);
		if (wcscmp(tmp, cmd) == 0) {
			cmdpi--;
		}
		/*strcpy*/u8swcs(cmd, cmdptr[cmdpi]);

	}
	return (1);
}

int cmdnxt(cmd)
/*
 * ВЗЯТЬ СЛЕДУЮЩУЮ КОМАНДУ ИЗ БУФЕРА
 */
wchar_t *cmd;
{
	if (cmdptr[cmdpi] && cmdplast > cmdpi)
		cmdpi++;
	if (cmdptr[cmdpi]) {
		/*strcpy*/u8swcs(cmd, cmdptr[cmdpi]);
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
 * but a common cache shared by another vash session (in concurrent way if v.flag.histsn != 0)
 *
 * this two routines called from main() on vash start and exit regardless of v.flag.histsn value
 */

/* history cache file*/
const char *hfile = "/.ashhist";

/*
 * flag: last timestamp of hfile known in this process of vash
 *
 * read history file, in case:
 *  1) first time
 *  2) file modified since last reading
 * write history file (after every command, if v.flag.histsn!=0)
 * saving internal mark about st_mtime preventing useless read
 */

static time_t hflast = (time_t)0; /* zero for fisrt time comparizon */

/*
 * get history from file into cmdb[] buffer
 *
 * if v.flag.histsn==0 (syncronize history is disabled),
 * do it once when program started
 *
 * returns 1, if get commands from history file
 * returns 0, if no read done
 */
cmdghist()
{

	FILE *fp;
	struct stat	hfstat;
	time_t      hftime;
	char filename[200];
	char cmdbuftmp[U8_STRBUF/*140*/]; /* one command from file, without trailing '\n' */
	int c;
	register char *p;
	register int i;

	strcpy(filename, v.home);
	strcat(filename, hfile);

	if (stat(filename, &hfstat) < 0)
		return(0);
	hftime = hfstat.st_mtime;

	if (v.flag.histsn == 0) {
		if (hflast != 0) return(1);
	}
/*	if (hflast != 0 && hflast == hftime) return(1);*/
	if (hflast < hftime) {
		if ((fp = fopen(filename, "r")) == NULL) return(0);
		/*hflast = hftime;*/

		/* cmdbuf[CMDB+1];   /* БУФЕР КОМАНД */
		/* *cmdptr[CMDP+1];   /* УКАЗАТЕЛИ НА КОМАНДЫ */
		for (i = 0; i < CMDP; cmdptr[i++] = 0) ;

		cmdplast = 0;   /* ИНДЕКС ПОСЛЕДНЕЙ КОМАНДЫ */
		cmdpi = 0;      /* ИНДЕКС ПОСЛЕДНЕЙ ВЗЯТОЙ/ПОЛОЖ. КОМАНДЫ */
		cmdbufree = 0;     /* ИНДЕКС СВОБОДНОГО МЕСТА В БУФЕРЕ */

		p = cmdbuftmp;		/* clear for next line from file */
		i = 0;
		while (/*i < CMDP && // old history file may be longer */
				(c = fgetc(fp)) != EOF) {
			if (c == '\n') {
				*p = '\0'; p = cmdbuftmp; i++; /* clear buffer for next line */
				cmdput(p);
			} else {
				*p = (char)c;
				p++;
			}
		}
		fclose(fp);
	}
	return(1);
}

/*
 * put commands from buffer cmdb[] to file in home directory
 */
cmdphist()
{
	char filename[STRBUF]; /*[200]*/
	struct stat	hfstat;
	time_t      hftime;
	FILE *fp;
	int ok;
	register char **pp;

	if (v.home == NULL/*(char *)0*/) return(0) ; /* history file is not defined */

	strcpy(filename, v.home);
	strcat(filename, hfile);

	if ((fp = fopen(filename, "w")) == NULL)
		return(0);

	for (pp = cmdptr; *pp != (char *)0; pp++) {
		/*VARARGS*/
		fprintf(fp, "%s\n", *pp);
	}
	ok = (fflush(fp)==EOF ? 0 : 1);
	fclose(fp);

	if (/*ok &&*/ stat(filename, &hfstat) == 0)
		hflast = hfstat.st_mtime;
	return(ok);
}

static  wchar_t *cmdpp;

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
		/*strcat(cmdpp, cmd)s*/
		while(*cmdpp != L'\0') cmdpp++;
		u8swcs(cmdpp, cmd);
		break;
	case '=':
		/* использовать как команду-заполнитель главного меню */
		strncpy(Cfill, cmd, STRBUF);
		break;
	case KB_NL:
		/* СКОПИРОВАТЬ КОМАНДУ В РАБОЧИЙ БУФЕР */
		/*strcpy*/u8swcs(cmdpp, cmd);
		break;
	case KB_DE:
		/* УБРАТЬ КОМАНДУ ИЗ ПАМЯТИ */
		if (cmdplast > 2 ) {
			cmddel(cmd);
			/*clritm();*/
			/*clm._itmmax = cmdplast;*/
/***
			if (itm < cmdplast);
			else    itm = cmdplast - 1;
			if (itm > 1)
				itm -= 1;
 ***/
/*
			pre_vf();
			itmshow();
			w_page(clm._vf);
*/
			/* синхронизировать историю при удалении каждой команды */
			if (v.flag.histsn) {
				cmdphist();
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

/*
 * РАБОТА С МЕНЮ БУФЕРА ИСТОРИИ КОМАНД.
 */
kbcod h_menu()
{
	register unsigned i;
	kbcod cod;

	/* первоначальный показ на экране */

#if 0
	/* ОСТАВИТЬ КОМАНДУ НА ЭКРАНЕ */
	for (i = clm._y0 -1; i <= lframe->maxli -1 ; i++) {
		cp_set(clm._y0, 0, TXT); er_eol(TXT);
	}
	/*w_cmd(cmdpp);*/
#endif
	itmshow();
	w_page(clm._vf);

	for ( ;; ) {

		i = clm._itm - clm._itmofs;
		cod = r_line( &clm._vf[i], 0 );

		w_emsg("");

		switch (cod) {

		default:
			break;
		case '1':      /* справка */
#if 1
			cp_set(-1, 0, TXT);
			fprintf(vttout, "Cmd# %2d/%2d, use %d (%d%%)",
			clm._itm, cmdplast, cmdbufree, (int)((cmdbufree * 100)/CMDB));
			break;
#endif
		case KB_HE:		/* TODO help msg */
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
			itmshow(); w_page(clm._vf);
			break;
#ifdef RETRO
		case KB_DE:
			if (itm - itmofs < 0)
				cod = KB_AU;
			else    break;
			/* проваливаемся... */
#else
		case KB_DE:
			/*cod = KB_AU;*/
			return(cod);
			break;
			/* НЕ проваливаемся... */
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
int
cmdvew(cmd0)
wchar_t  *cmd0;
{
	extern int  y0_top;     /* определено в vshcmd */
	/*extern char *pmtsh;     --"-- */
	LINEMENU savelm;
	kbcod cod;
	
	int ret;

	int cmp_sz;
	int i;
	u8char_t u8cmp[U8_STRBUF];
	u8char_t **cmds;
	u8char_t **pcmd;
	u8char_t  *p;

	savelm = clm;

	/* start setup new instance of clm */
	/*clm._itms   = cmdpsel*//*cmdptr*/;          /* УКАЗАТЕЛИ НА СТРОКИ КОМАНД */

	/* force 1st malloc() for extra clm, save clm instance of main menu */
	clm._vf = (LINE *)0;

#if 0
	clm._itmmax = cmdplast;      /* количество КОМАНД В ИСТОРИИ */
	clm._itm    = cmdpi;         /* ТЕКУЩАЯ КОМАНДА */
	if (cmdpi >= cmdplast) {
		clm._itm -= 1;
	}
#endif
	ret = 0;

	/* сначала синхронизация истории из файла? */
	if (v.flag.histsn) {
		cmdghist();
	}

	if (cmdplast < 1) {
		bell(); return (ret);
	}
	/*cp_set(clm._y0 - 1, lframe->maxco - 1, TXT);*/   /* СОХРАНИТЬ СВИТОК, СМ. НИЖЕ */

restart:
	/* определить размер списка меню команд, если заказана селекция по образцу ^R */
	wcsnu8s(u8cmp, cmd0, STRBUF);	/*сравнивать при поиске в utf-8*/
	cmds = (u8char_t **)cmdpsel;
	cmdsmax = 0;
	cmp_sz = strlen(u8cmp);
	if (*u8cmp != '\0') {
		/* заодно посчитать размер полного списка */
		for (i = 0, pcmd = (u8char_t **)cmdptr;
				*pcmd != '\0'; /*comparizon between zero char and u8char_t */
				pcmd++) {
			i++;
			for (p = *pcmd; *p != '\0'; p++) {
				/*поиск образца в любом месте команды, не только от начала*/
				if (strncmp(u8cmp, p, cmp_sz) == 0) {
					*cmds++ = *pcmd;
					cmdsmax += 1;
					/*break;*/
				}
			}
		}
	}
	/*cmdplast = i;*/
	*cmds = '\0';	/* терминировать селектированный список меню команд */
	/* построить список по шаблону поиска, если шаблона нет, то показывать только полный список */
	if (cmdpi < 0) {
		cmdpi = cmdplast - 1; /*anyway conditionless */
	}
	if (cmdsmax > 0) {
		clm._itms = cmdpsel;
		/*clm._itm*/
		if (cmdpisel < 0) {
			cmdpisel = cmdsmax - 1; /* установить индекс в конец списка */
		}
	} else {
		clm._itms = cmdptr;		/* переключиться на полный список истории */
		/*clm._itm*/
		if (cmdpi < 0) {
			cmdpi = cmdplast - 1;
		}
	}

rebuild_help_menu:

	/*
	 * инициализация меню команд
	 */
	/* настроить полный или селектированный список команд истории */
	if (clm._itms == cmdpsel) {
		clm._itmmax = cmdsmax;	/* размер селектированного списка */
		clm._itm = cmdpisel; /* установить текущий индекс (было в конец списка) */
		if (cmdpisel > cmdsmax) {
			cmdpisel = clm._itm = cmdsmax -1;
		}
	} if (clm._itms == cmdptr) {
		clm._itmmax = cmdplast; /* количество КОМАНД В ИСТОРИИ */
		clm._itm = cmdpi;       /* восстановить текущий индекс */;
		if (cmdpi >= cmdplast) {
			cmdpi = clm._itm = cmdplast -1;
		}
	}

	/* hint to avoid new overlapping malloc? /* TODO WTF */
/*	clm._vf     = (LINE *)0;		*/

	/*по феншую, отступ на промптер слева и справа*/
	clm._itmlen = lframe->maxco - ((strlen(pmtsh)) * 2);
	clm._ltmpl  = &tmplate;

	clm._yy_max = 10;		/*TODO вынести в начало функции как константу */
	clm._itmofs = 0;
	while((clm._itm - clm._itmofs) >= clm._yy_max)
		clm._itmofs += clm._yy_max;


#if 0
	/* force pre_vf() to do new malloc() */
	if (clm._vf != (LINE *)0) {
		free(clm._vf);
		clm._vf = (LINE *)0;
	}
#endif
	/* Подготовить и показать страницу меню истории */
	itmini();
	pre_vf();	/* malloc() called inside */

	/* раздвинуть свиток до высоты меню команд истории */
	if (y0_top > clm._y0) {
		cp_set(y0_top/*clm._y0*/ - 1, lframe->maxco - 1, TXT);
		/*er_eop(TXT);*/
		scrlnl();
		y0_top = clm._y0;
		w_cmd(cmd0);
		w_wcstr(cmd0);
	}

	/* очистить область свитка, каждый раз при смене содержимого */
	for (i = y0_top/*clm._y0*/; i <= lframe->maxli -2 ; i++) {
		cp_set(i, 0,TXT);
		er_eol(TXT);
	}
	cmdpp = cmd0;    /* ДЛЯ КОПИРОВАНИЯ НОВОЙ КОМАНДЫ -- TODO is this string obsolete? */

	w_cmd(cmd0);
	if (clm._itms == cmdpsel) {
		at_set(ATT|INP/*CMD|VEXT*//*ATT*/);
	} else {
		at_set(CMD);
	}
	w_wcstr(cmd0);

	cod = h_menu();
	switch(cod) {
	case KB_DE:
		if (clm._itm >= clm._itmmax) {
			clm._itm = clm._itmmax;
		}
		clm._itm -= 1;
		if (clm._itm < 0) {
			clm._itm = 0;
		}
		if (clm._itms == cmdpsel) {
			/*cmdsmax -= 1;*/
			cmdpisel = clm._itm;
		} else {
			cmdpi = clm._itm;
		}
		goto restart; /*rebuild_help_menu;*/
		break;
	case KB_HE:
		if (cmdsmax > 0) {
			if (clm._itms == cmdpsel) {
				cmdpisel = clm._itm;
				clm._itms = cmdptr;
			} else {
				clm._itms = cmdpsel;
				cmdpi = clm._itm;
				/*goto restart;*/
			}
			goto rebuild_help_menu;
		}
		break;
	default:
		ret = 1;
		break;
	case '=':
		ret = 2;
		break;
	};

	free((char *)clm._vf); clm._vf = (LINE *)0;
	cp_set(y0_top, 0, TXT); er_eop(TXT);

	if (clm._itms == cmdptr) {
		cmdpi = clm._itm; /* НОВОЕ ЗНАЧ. ИНДЕКСА ИСТОРИИ */
	} else {
		cmdpisel = clm._itm;
	}
	/*clm._vf = (LINE *)0;*/
	clm = savelm;
	return(ret);
}
