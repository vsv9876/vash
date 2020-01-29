#include <string.h>
#include <stdio.h>
#include "line.h"
#include "assist.h"
#include "slist.h"

extern SLIST_HEAD *sgglist;

extern  char *getenv();

extern  int     y0_top; /* defined in main.c */

/*NOXSTR*/
extern char    *pmtsh;/* = ".$"; /*"sh>" ;*/
/*YESXSTR*/
int pmtshsz;            /* размер подсказки */

static  char cmd0[MAXLICO + 2] = "";    /* ТЕКУЩАЯ КОМАНДА ОБОЛОЧКИ */
static  int  cmdsize = 0;       /* МАКС. РАЗМЕР СТРОКИ */
static  int  pos = 0;           /* позиция в строке */
static  int old_pos = 0;		/* позиция курсора до попытки окончить ввод (completion),
								если изменилась, выполнена вставка в буфер команды */
extern int hlp_cmpl();
/*
 * ФУНКЦИЯ w_cmd ВЫЗЫВАЕТСЯ В ЭТОМ МОДУЛЕ
 * И В МОДУЛЕ РАБОТЫ С ИСТОРИЕЙ
 */
w_cmd(cmd)
register char *cmd;
{
	register int i;

	cp_set(-1 /*y0-1*/, 0, CMD); er_eol(CMD);
	at_set(CMD|INP);
	/*if (getuid() == 0) pmtsh = ".#";*/
	w_str(pmtsh);
	cp_set(-1 /*y0-1*/, pmtshsz, CMD);
	for(i = 0; cmd[i] && i < cmdsize; i++)
		w_chr(cmd[i]);
}

scrlst()        /* курсор к началу свитка */
{
	cp_set(y0_top, 0, CMD); er_eop(CMD);
	cp_set(y0_top - 1, 0, CMD);
	fflush(vttout);
}


static char tmpstr[MAXLICO*2];

vshcmd(cmd, cmdlbl)
/*
 * выполнить команду /bin/sh
 */
char *cmd;      /* команда для выполнения */
char *cmdlbl;   /* вывеска для показа вместо команды */
{
	kbcod cod;
	int syscod;             /* код возврата system */
	int cmdrun;             /* ФЛАГ: КОМАНДА ЗАПУСКАЛАСЬ */
	int justrun;            /* флаг: запускать, не редактировать */
	int okwait;             /* флаг: после wait() ожидать подтверждения пробелом, если OK */
	int slsize;				/* количество возможных окончаний */

	cmdrun = 0;
	pmtshsz = strlen(pmtsh) /* + 1*/;
	cmdsize = maxco - 1 - pmtshsz;
	justrun = 0;
	okwait = 1;  /* флаг: после wait() не ожидать подтверждения пробелом, если OK */
	cod = 0;

	if (cmd == (char *)0) {
	    /* используется последняя команда */
	    ;
	}
	else {
		if (*cmd == ':' || *cmd == ';') {
			okwait = (*cmd == ';')? 0 : 1;
			justrun = 1;
			cod = KB_NL;
			/* копируем команду полностью */
			cmd++;
			strcpy(cmd0, cmd);
		}
		else {
			/* копир. только то, что на экране */
			strncpy(cmd0, cmd, (size_t)cmdsize); /*TODO: WTF*/
			cmd0[cmdsize] = 0;
		}
		pos = strlen(cmd0);
	}

	for (;;) {
	    if ( !justrun ) {
		/* редактирование командной строки */
		switch(cod) {
		case KB_HE:
		case KB_AU:
		case KB_AD:
			pos = strlen(cmd0);     /* курсор в конец */
		default:
			w_cmd(cmd0);
		}
		cod = re_str((char *)&cmd0[0], cmdsize, 0, &pos);
	    }
		switch(cod) {
		default:
			bell();
			continue;
		case KB_HE:
			cmdvew(cmd0);
			/* ДЛЯ ВОССТАНОВЛЕНИЯ ГЛАВНОГО МЕНЮ: */
			cmdrun = 1;
			break;
		case KB_AU:
			/* пред. команда */
			/* синхронизировать историю, если в буфере набираемой команды пусто */
			if (histsn && cmd0[0] == 0) cmdghist(homedir);
			if ( !cmdprv(cmd0) )
				bell();
			break;
		case KB_AD:
			/* следующая команда */
			if ( !cmdnxt(cmd0) )
				bell();
			break;
		case KB_RE:
			er_pag(); w_cmd(cmd0);
			break;
		case KB_TA:
			/* completion */
			if ((sgglist = sl_init()) != NULL) {
				old_pos = pos;
				if ((slsize = try_compl(&cmd0[0], &pos, maxco - 2)) < 0) bell();
				if (pos == old_pos && sgglist->sl_size > 0) {
					hlp_compl();
					cmdrun = 1; /*для восстановления главного меню*/
				} else {
					hlp_clr();
				}
				sgglist = sl_free(sgglist);
			}
			w_cmd(cmd0);
			break;
		case KB_NL :
			if (cmd == (char *)0) {
				/* для strncmp необходимо */
				cmd = "";
			}
			/* сохранить команду, если ее редактировали */
			if (strncmp(cmd, cmd0, strlen(cmd0)) != 0) {
				cmdput(cmd0);
				/* синхронизировать историю */
				if (histsn) {
					cmdphist(homedir);
				}
				/* заставить при повт.запуске снова сохранять: */
				cmd = "";
			}
			/* конец работы? */
			if(strcmp(cmd0, "exit")==0) {
				onexit(0); exit(0);
			}
			/* домашний каталог */
			if (strcmp(cmd0, "cd") == 0) {
				char    *homedir;
				if ((homedir=getenv("HOME")) == NULL) {
					w_emsg(
					"env HOME= undefined");
					return(0);
				}
				if (vchdir(homedir) < 0) {
					return(0);
				}
				goto fil_cd;
			}

			/* перейти в каталог? */
			if (strncmp(cmd0, "cd ", 3) == 0) {
				if (index(cmd0, ';'))
					goto std_shell;

				if (vchdir(&cmd0[3]) < 0) {
					return(0);
				}
				else    {
fil_cd:
					if (clm._y0 < y0_top)
					     y0_top = clm._y0;
					/* тут было заполнение меню */
					return(1);
				}
			}
std_shell:
			/*-----------------*/
			/* обычная команда */
			/*-----------------*/

			cmdrun = 1;

			syscod = vsystem(cmd0, cmdlbl);
			justrun = 0;
			if(syscod) {
				at_set(ERR);
				sprintf(tmpstr, "[ Exit (%d/%d) ]\r",
				cod1(syscod), cod0(syscod));
				w_str(tmpstr);
			}
			else {
				if (okwait == 1) {
					at_set(HDR);
					sprintf(tmpstr, "[ ok ]\r");
					w_str(tmpstr);
				}
			}
			at_set(CMD);
			if (okwait == 1) {
				do {
					fflush(vttout);
					fflush(stdout);
					cod = r_cod(0);
					at_set(ATT|INP);
					/*VARARGS*/
					if (cod == KB_NL) {
					  sprintf(tmpstr, " press SPACE bar or type command ");
					  w_str(tmpstr);
					}
					/*at_set(TXT); er_eol();*/
					er_eop(0);
					/*VARARGS*/
					w_str("\r");
				} while (cod == KB_NL);
			} else {
				at_set(0);
				w_str("\r");
			}
			/* проверка завершения команды sh */
			er_eol(0); fflush(vttout);
			scrlnl();
			showtime( 1 );          /* часы включить */

			switch (cod) {
			case KB_AL:
			case KB_AR:
			case KB_AD:
			case KB_AU:
			/* case KB_TA */
				/*cp_sav(); cp_set(-2, 1, ATT); w_str("ta"); cp_fet();*/
					continue;
			/* возврат в меню имен файлов */
			case ' ':       return(1);
			case KB_EX:      return(0);
			default:
				if (cod1(cod) == 0) {
					/* ввод новой команды */
					cmd0[0] = cod;
					cmd0[1] = '\0';
					pos = 1;
					continue;
				}
				goto fil_cd;
				/*break;*/
			}       /* финал проверки завершения команды sh */

		case KB_EX:
			w_msg(TXT, " ");
			w_msg(TXT, "");
			if (cmdrun) {
				scrlst(); return(1);
			}
			return(0);      /* ничего не сделано */
			/*break;*/
		/*switch(cod) -- end*/
		}
	}
}
