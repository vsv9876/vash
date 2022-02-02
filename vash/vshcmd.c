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

/* command buffer is a string object, it is not a simple string of C/C++ */
static  u8char_t cmdbuf[4*MAXLICO + 4] = "   ";    /* size for u8sobj_t cmdo -- below */
static  int  cmdsize = 0;       /* vsize e_str -- depends of screen width */
static  int  pos = 0;           /* i     e_str    позиция в строке */
static  int old_pos = 0;		/* позиция курсора до попытки окончить ввод (completion),
								если изменилась, выполнена вставка в буфер команды */

/*static  u8sobj_t *cmdo = cmdbuf;*/
static  u8char_t *cmd0 = &cmdbuf[3]/*((u8sobj_t *)cmdbuf)->u8s*/;
static  u8sobj_t *cmdo = &cmdbuf[0];

static int done = 0;
cmdo_init()
{
	if(done == 0) {
		done = 1;
		cmdo->u8o_sig = U8O_SIG;
		cmdo->u8o_sizeh = MAXLICO / 256;
		cmdo->u8o_sizel = MAXLICO % 256;
	}
}

w_cmd(cmd)
register char *cmd;
{
	register int i;
	int cmd_li = -1 /*y0_top - 1*/; /* on bootom line or on top of scroll occupied by vash */

	cp_set(cmd_li, 0, CMD); er_eol(CMD);
	at_set(CMD|INP);
	/*if (getuid() == 0) pmtsh = ".#";*/
	w_str(pmtsh);
//	cp_set(cmd_li, pmtshsz, CMD);
//	for(i = 0; cmd[i] && i < cmdsize; i++)
//		w_chr(cmd[i]);
	cp_set(cmd_li, pmtshsz, CMD); /* point to start re_str() editor */
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
	int codexit, codsig;

	int cmdrun;             /* ФЛАГ: КОМАНДА ЗАПУСКАЛАСЬ */
	int justrun;            /* флаг: запускать, не редактировать */
	int okwait;             /* флаг: после wait() ожидать подтверждения пробелом, если OK */
	int slsize;				/* количество возможных окончаний */

	int trapcod;		/* flag: trap, no return from code exit indicator */

	cmdrun = 0;
	pmtshsz = strlen(pmtsh) /* + 1*/;
	cmdsize = lframe->maxco - 1 - pmtshsz; /*TODO size must be defined by command editor buffer*/
	cmdo_init(); /* once called */
	justrun = 0;
	okwait = 1;  /* флаг: после wait() не ожидать подтверждения пробелом, если OK */
	cod = 0;
	int atrib;

	if (cmd == (char *)0) {
	    /* используется последняя команда (уже в буфере) */
	    ;
	} else {
		if (*cmd == ':' || *cmd == ';') {
			okwait = (*cmd == ';')? 0 : 1;
			justrun = 1;
			cod = KB_NL;
			/* копируем команду полностью */
			cmd++;
			strcpy(cmd0, cmd);
		}
		else {
			/* копир. только то, что ПОМЕСТИТСЯ на экране -- TODO FTW*/
			/*strncpy(cmd0, cmd, (size_t)cmdsize); TODO: WTF
			cmd0[cmdsize] = 0;*/
			strcpy(cmd0, cmd);
		}
		pos = /*strlen*/u8slen(cmd0);
	}

	for (;;) {
	    if ( !justrun ) {
		/* редактирование командной строки */
		switch(cod) {
		case KB_HE:
		case KB_AU:
		case KB_AD:
			pos = /*strlen*/u8slen(cmd0);     /* курсор в конец */
		default:
			w_cmd(cmd0);
		}
		/*cod = re_str((char *)&cmdbuf[0], cmdsize, 0, &pos);*/
		showtime(0);
		cod = re_str(cmdo, cmdsize, 0, &pos);
		showtime(1);
	    }
		switch(cod) {
		default:
			bell();
			continue;
		case KB_HE:
			/* similar as on KB_EX */
			if (cmdvew(cmd0) >= 2) {
				w_msg(TXT, " ");
				w_msg(TXT, "");
				if (cmdrun) {
					scrlst(); /*return(1);*/
				}
				return(1);
			}
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
			er_pag(); w_cmd(cmd0); /*TODO: er_pag() covers extra space on screen - better er_eop() below y0_top */
			break;
		case KB_TA:
			/* completion */
			if ((sgglist = sl_init()) != NULL) {
				old_pos = pos;
				if ((slsize = try_compl(&cmd0[0], &pos, lframe->maxco - 2)) < 0) bell();
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
			if (strncmp(cmd, cmd0, /*strlen*/u8slen(cmd0)) != 0) {
				if (histsn) {
					cmdghist(homedir);
				}

				cmdput(cmd0);

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
			scrldo();
			if(syscod) {
				codexit = cod1(syscod); /*TODO cleanup trick, replace with stdlib.h and sys/wait.h stuff */
				codsig  = cod0(syscod);
				at_set(CMD|VEXT/*ERR*/);
				if (codsig) {
					sprintf(tmpstr, "[ exit= %d, signal= %d ]", codexit, codsig);
				} else {
					sprintf(tmpstr, "[ exit= %d ]", codexit);
				}
				/*w_str(tmpstr); cp_cret();*/
			}
			else {
				if (okwait == 1) {
					at_set(HDR);
					sprintf(tmpstr, "[ ok ]");
					/*w_str(tmpstr); cp_cret();*/
				}
			}
			cp_cret(); w_str(tmpstr); /*cp_sav();*/
			at_set(CMD);
			if (okwait == 1) {
				do {
					fflush(vttout);
					fflush(stdout);
					trapcod = 0;
					cod = r_cod(0);
					switch (cod) {
					case KB_HE:
						w_help("excode_help.lb");
						break;
					case KB_AU:
					case KB_AD:
					case KB_AL:
					case KB_AR:
					case KB_CA:
					/*case KB_EX:*/
						trapcod = 0;
						break;
					default:
						if (cod == KB_NL || cod < ' ' || ISCTL(cod)) {
							trapcod = 1;
						}
						break;
					}

					at_set(atrib = MSE|VEXT);
					if (trapcod) {
					  /*sprintf(tmpstr, "-- SPACE bar or type a command ");*/
 					  at_set(atrib); w_str(" --");
 					  at_set(CMD);     w_str(" please, type a command or <");
					  w_lh_str(":SP");
					  at_set(CMD); w_str("> to return ");
					  at_set(atrib); w_str("--");
					  at_set(CMD); w_str(" help: ");
					  w_lh_str(":HE");
					  er_eop(CMD);
					  /*cp_fet();*/ /*w_str(tmpstr);*/
					}
					er_eop(0);
					cp_cret(); /*w_str("\r");*/
					w_str(tmpstr);
				} while (trapcod);
			} else {
				at_set(0);
				cp_cret(); /*w_str("\r");*/
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
			case ' ':
			case KB_CA:
						return(1);
			case KB_EX:
						return(0);
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

		case KB_CA:
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
