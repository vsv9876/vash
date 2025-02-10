#include <stdlib.h>
#include <sys/wait.h>
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


static  int  cmdsize = 0;       /* vsize e_str -- depends of screen width */
static  int  pos = 0;           /* i     e_str    позиция в строке */
static  int old_pos = 0;	/* позиция курсора до попытки окончить ввод
				(completion),
			если изменилась, выполнена вставка в буфер команды */

/* command buffer is a string object, it is not a simple char-type string */
static  wchar_t cmdbuf[2 + STRBUF];  /* size for wcsobj_t cmdo -- below */

static  wcsobj_t *cmdo = (wcsobj_t *)cmdbuf;
static  wchar_t *cmd0 = /*cmdo->wcs;*/ &cmdbuf[2];

static int cmdo_init_done = 0;
cmdo_init()
{
	if(cmdo_init_done == 0) {
		cmdo_init_done = 1;
		cmdo->wco_sig = WCO_SIG;
		cmdo->wco_size = STRBUF;
	}
}

void
w_cmd(cmd)
register wchar_t *cmd;
{
	register int i;
	int cmd_li = -1 /*y0_top - 1*/; /* on bootom line or on top of scroll occupied by vash */

	cp_set(cmd_li, 0, CMD); er_eol(CMD);
	at_set(CMD|INP);
	/*if (getuid() == 0) pmtsh = ".#";*/
	w_str(pmtsh);
	cp_set(cmd_li, pmtshsz, CMD); /* point to start re_str() editor */

	/* TODO: merge with e_str() code */
#if 0
	for(i = 0; cmd[i] && i < cmdsize; i++)
		w_chr(cmd[i]);
#endif
}

void
scrlst()        /* курсор к началу свитка */
{
	cp_set(y0_top, 0, CMD); er_eop(CMD);
	cp_set(y0_top - 1, 0, CMD);
	fflush(vttout);
}


static char tmpstr[STRBUF * 2];

/*
 * compare new command vs old command to be saved in history
 */
static int cmd0cmp(cmd, cmd0, size)
char *cmd;
wchar_t *cmd0;
size_t size;
{
	u8char_t new[U8_STRBUF];
	wcsu8s(new, cmd0);
	return strncmp(cmd, new, size);
}

/*
 * trim a just typed command from unescaped spaces
 */
void wcuntrim(cmd, curpos)
wcsobj_t *cmd;
int *curpos;
{
	int x, len;
	wchar_t *wcs;
	if (cmd->wco_sig == WCO_SIG)
		wcs = cmd->wcs;
	else
		wcs = (wchar_t *)cmd;
	len = wcslen(wcs);
    for (x = len - 1; x >= 0 && wcs[x] == L' '; x--) {
    	if (x > 0 && wcs[x - 1] == L'\\')
    		continue;
    	wcs[x] = L'\0';
    }
}

/*
 * выполнить команду /bin/sh
 */
int vshcmd(cmd, cmdlbl)
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

	int trapcod;		/* flag: trap active, no return from exit indicator */
	int msgat;			/*exit code message attribute*/

	u8char_t u8cmd0[U8_STRBUF + 4];	/* TODO check a type of variable - command to be executed */
	wchar_t *p;
	int		sufpos; /* suffix position and count */
	int		sufcnt;

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
			/* пропускаем первый символ, затем копируем команду полностью */
			cmd++;
		}
		/*strcpy*/u8swcs(cmd0, cmd);

		/* cp/mv commands set pos to just before last significant suffix */
		p = cmd0;
		sufpos = -1;
		while(*p++) {
			/* determining ofset from end to suffix*/
			if (p[0] == MONEY && p[1] != MONEY) {
				if (p[1] >= L'0' && p[1] <= L'9') {
					sufpos = p[1] - L'0';
					p[0] = p[1] = 0; /* terminate, clear rc instruction to offset */
				}
			}
		}
		pos = /*strlen*//*u8slen*/wcslen(cmd0);
		if (sufpos >= 0) {
			while(pos > 0) {
				if (cmd0[pos] == L' ' || cmd0[pos] == 0)
					cmd0[pos--] = 0;
				else
					break;
			}
		}
#if 0
		if (sufpos == 1) {
			/* find position of last '.' symbol in name */
			do {
				pos--;
			} while (sufpos < pos && (cmd0[pos] != L'.'));
			/* pos untouched, still at tail position */
		}
#endif
		if (sufpos == 0) {
			/* find position of 1st '.' symbol in name (last from tail) */
			sufpos = pos;
			do {
				if (cmd0[pos] == L'.')
					sufpos = pos;
				pos--;
			} while (cmd0[pos] != L' ');
			pos = sufpos;
		} else if (sufpos == 1 || sufpos == 2) {
			/* find position of 1th or 2nd '.' symbol from tail */
			sufcnt = sufpos;
			do {
				if (cmd0[pos] == L'.') {
					sufpos = pos;
					sufcnt--;
				}
				pos--;
			} while (sufcnt > 0 && cmd0[pos] != L' ');
			pos = sufpos;
		} else if (sufpos > 2) {
			while (sufpos < pos && (cmd0[pos] == L' ' || cmd0[pos] == 0)) {
				cmd0[pos] = 0;
				pos--;
			}
			pos++;
			pos -= sufpos;
		}

	}

	for (;;) {
	    if ( !justrun ) {
		/* редактирование командной строки */
		switch(cod) {
		case KB_HE:
		case KB_AU:
		case KB_AD:
			pos = /*strlen*//*u8slen*/wcslen(cmd0);     /* курсор в конец */
			/*NO BREAK*/
		default:
			w_cmd(cmd0);
		}
		/*cod = re_str((char *)&cmdbuf[0], cmdsize, 0, &pos);*/
		showtime(0);
		showitem(1);

		cod = re_str(cmdo, cmdsize, 0, &pos);
	    wcuntrim(cmdo, &pos);

	    showitem(0);
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
			if (vashflag.histsn && cmd0[0] == 0) cmdghist();
			if (!cmdprv(cmd0))
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

				if ((slsize = try_compl(&cmd0[0], &pos, STRBUF/*lframe->maxco - 2*/)) < 0)
					bell();

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
			cmdhreset();
			if (cmd == (char *)0) {
				/* для strncmp необходимо */
				cmd = "";
			}
			/* сохранить команду, если ее редактировали */
			if (cmd0cmp(cmd, cmd0, /*strlen*//*u8slen*/wcslen(cmd0)) != 0) {
			/*if (strncmp(cmd, cmd0, strlenu8slenwcslen(cmd0)) != 0) {*/
				if (vashflag.histsn) {
					cmdghist();
				}

				wcsu8s(tmpstr, cmd0);
				cmdput(tmpstr/*cmd0*/);

				if (vashflag.histsn) {
					cmdphist();
				}
				/* заставить при повт.запуске снова сохранять: */
				cmd = "";
			}
			/* конец работы? */
			if(/*strcmp*/wcscmp(cmd0, L"exit")==0) {
				onexit(0); exit(0);
			}
			/* домашний каталог */
			if (/*strcmp*/wcscmp(cmd0, L"cd") == 0) {
				char    *homedir;
				if ((homedir=getenv("HOME")) == NULL) {
					w_emsg(
					"undefined: env HOME= ");
					return(0);
				}
				if (vchdir(homedir) < 0) {
					return(0);
				}
				goto fil_cd;
			}

			/* перейти в каталог? */
			if (/*strncmp*/wcsncmp(cmd0, L"cd ", 3) == 0) {
				if (/*index*/wcschr(cmd0, L';') != NULL) {
					goto std_shell;
				}

				/* TODO: shell syntax substitutions there, then vchdir*/
				wcsu8s(tmpstr, &cmd0[3]);

				if (vchdir(/*&cmd0[3]*/tmpstr) < 0) {
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

			wcsu8s(u8cmd0, cmd0);

			syscod = vsystem(u8cmd0, cmdlbl);
			justrun = 0;
			scrldo();
			if(syscod) {
				msgat = ERR;
#if 0
				codexit = cod1(syscod); /*TODO cleanup trick, replace with stdlib.h and sys/wait.h stuff */
				codsig  = cod0(syscod);
				if (codsig) {
					sprintf(tmpstr, "[ exit= %d, signal= %d ]", codexit, codsig);
				} else {
					sprintf(tmpstr, "[ exit= %d ]", codexit);
				}
				/*w_str(tmpstr); cp_cret();*/
#else
				if (WIFEXITED(syscod)) {
					sprintf(tmpstr, "[ exit= %d ]", WEXITSTATUS(syscod));
				} else if (WIFSIGNALED(syscod)) {
                    sprintf(tmpstr, "[ signal= %d ]", WTERMSIG(syscod));
#if VASH_NO_SUPPORT_JobControl
				} else if (WIFSTOPPED(syscod)) {
                    sprintf(tmpstr, "[ stopped by signal %d ]", WSTOPSIG(syscod));
                } else if (WIFCONTINUED(syscod)) {
                    sprintf(tmpstr, "[ continued ]");
#endif
                }
#endif
			}
			else {
				if (okwait == 1) {
					if (vashflag.exittrap) {
						msgat = HDR/*ATT/*VEXT|MSE*//*HDR*/;
						sprintf(tmpstr, "[ ok ]");
					} else {
						msgat = CMD|INP;
						sprintf(tmpstr, pmtsh);
						/*w_str(tmpstr); cp_cret();*/
					}
				}
			}
			/*cp_cret();*/
			/*at_set(CMD|INP); w_str(pmtsh);*/
			at_set(msgat); w_str(tmpstr); /*cp_sav();*/
			at_set(CMD);
			if (okwait == 1) {
				do {
					fflush(vttout);
					fflush(stdout);
					trapcod = 0;
					cod = r_cod(0);
					switch (cod) {
					case KB_HE:
						w_help((LINE *)"excode_help.lb");
						break;
					case KB_AU:
					case KB_AD:
					case KB_AL:
					case KB_AR:
					case KB_CA:
						trapcod = 0;
						break;
					case KB_EX:
						scrlnl();
						onexit(0); exit(0);
						break;
					default:
						if (vashflag.exittrap) {
							if (cod == KB_NL || cod < ' ' || ISCTL(cod)) {
								trapcod = 1;
							}
						} else {
							if (/*cod == KB_NL || */cod < ' ' || ISCTL(cod)) {
								trapcod = 1;
							}
						}
						break;
					}
					/*sprintf(tmpstr, pmtsh);*/
					/*at_set(msgat = (CMD|INP));*/
					at_set(msgat);
					if ( ! vashflag.exittrap && cod == KB_NL) {
						er_eol(CMD);
						/*trapcod = 1;*/
						if (syscod) {
							cp_cret()/*w_str("\r")*/;
							syscod = 0; /* show exitcode only once */
						} else {
							w_str("\n");
						}
						at_set(CMD); er_eol(CMD);
						at_set(msgat = (CMD|INP));
						sprintf(tmpstr, pmtsh);
					}
#if 0
					if (vashflag.exittrap) {
						at_set(atrib = HDR);
					}
#endif
					if (trapcod) {
						if (vashflag.novice) {
							at_set(TXT);
							w_str(" main view: click ");
							w_lh_str(":SP");
							at_set(TXT);
							w_str(" or ");
							w_lh_str(":CA");
							at_set(TXT);
							w_str("               help: ");
							w_lh_str(":HE");
							/*at_set(TXT);*/
							er_eol(TXT);
						}
					}
					er_eop(CMD);
					cp_cret(); /*w_str("\r");*/
					/*at_set(CMD|INP); w_str(pmtsh);*/
					at_set(msgat); w_str(tmpstr);
				} while (trapcod);
			} else {
				cp_cret(); /*w_str("\r");*/
				at_set(CMD);
			}
			cp_cret(); /*er_eop(TXT);*/
			/* проверка завершения команды sh */
			er_eol(CMD); fflush(vttout);

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
#if 0
						if (vashflag.exittrap && cod == ' ')
							continue;
#endif
						return(1);
			case KB_EX:
						return(0);
			default:
				if (cod1(cod) == 0) {
					/* ввод новой команды */
					cmd0[0] = cod;
					cmd0[1] = L'\0';
					pos = 1;
					continue;
				}
				goto fil_cd;
				/*break;*/
			}       /* финал проверки завершения команды sh */

		/*case KB_CA:*/
		case KB_EX:
			if ( ! cmd0[0] ) {
				if (cmdrun)
					scrlst();
				onexit(0); exit(0);
			}
			/*NO BREAK*/
		case KB_CA:
			w_msg(TXT, " ");
			w_msg(TXT, "");
			if (cmdrun) {
				scrlnl(); return(1);
			}
			return(0);      /* ничего не сделано */
			/*break;*/
		/*switch(cod) -- end*/
		}
	}
	return 0;
}
