/*
 * VASH - visual assistant for shell
 * Copyright (c) 1990-2025 Sergey Vovk <vsv>
 *
 * License: GPL and/or MIT,
 * see files COPYING and LICENSE
 */

/*
 * internal vash commands
 */

#include <stdlib.h>
#include "line.h"
#include "assist.h"
#include "vjobs.h"

static struct cmd_in {
  const wchar_t  *cmdin;
		int     (*proc)();
  const bool      ttymode;
};

struct cmd_in cmdv[] = {
	{ L"exit",	 v_exit,   0 },
	{ L"cd",     v_cd,     0 },
	{ L"jobs",   v_jobs,   1 },
	{ L"fg",     v_fg,     1 },
	{ L"bg",	 v_bg,     1 },
	{ L"disown", v_disown, 1 },
	{ L"stop",   v_stop,   1 },
	{ L"kill",   v_kill,   1 },

	{ NULL,    NULL,      0 }
};

extern  int     y0_top; /* defined in main.c */

static vtflags = 0;

/* echo command after prompter */
void tty_cmd(cmd, lbl)
wchar_t *cmd;
u8char_t *lbl;
{
	at_set(CMD); er_eop(CMD);
	if (cmd != NULL && wcscmp(cmd, L"") != 0) w_wcstr(cmd);
	else
	if (lbl != NULL && strcmp(lbl, "") != 0)  w_str(lbl);
	er_eop(0/*CMD*/);
	fflush(vttout);
}

/*
 * команды вызываются из vcmd:
 *
 *  Возвращаемые значения:
 *      -1      не найдено соответствие, экран не испорчен.
 *      -2(0)      отказ от действия, экран не испорчен.
 *      -3(1)      команда выполнена, экран испорчен.
 */
/*
 * check if internal command exist:
 * 1  non-tty, old vash style
 * 2  with tty output
 * 0  no command exists
 */
vin_chk(cmd0)
wchar_t *cmd0;
{
	struct cmd_in *v;
	int    ret;
	const wchar_t *cmdin;
	int    cmdsz;

	for (v = cmdv; v->cmdin != NULL; v++) {
		cmdin = v->cmdin;
		cmdsz = wcslen(v->cmdin);
		if ((wcsncmp(cmd0, cmdin, cmdsz) == 0)
			&& ((cmd0[cmdsz] == L'\0') || (cmd0[cmdsz] == L' ')))
		{
			if(v->ttymode)  return(2);
			else            return(1);
		}
	}
	return(0);
}

vin_do(cmd0, tmpstr)
wchar_t *cmd0;
char *tmpstr;
{
	struct cmd_in *v;
	const wchar_t *cmdin;
	int    cmdsz;

	for (v = cmdv; v->cmdin != NULL; v++) {
		cmdin = v->cmdin;
		cmdsz = wcslen(v->cmdin);
		if ((wcsncmp(cmd0, cmdin, cmdsz) == 0)
			&& ((cmd0[cmdsz] == L'\0') || (cmd0[cmdsz] == L' ')))
		{
			return(v->proc)(cmd0, tmpstr);
		}
	}
	return(-1);
}

static int fil_cd() {
	if (clm._y0 < y0_top)
		y0_top = clm._y0;
	/* тут было заполнение меню */
	return (1);
}
/*
 * main cd(chdir) command
 */
v_cd(cmd0, tmpstr)
wchar_t *cmd0;
char *tmpstr;
{
	if (/*strcmp*/wcscmp(cmd0, L"cd") == 0) {
		if (v.home == NULL) {
			w_emsg("HOME: undefined env");
			return(0);
		}
		if (vchdir(v.home) < 0) {
			return(0);
		}
		return (fil_cd());
	}
	if (/*strncmp*/wcsncmp(cmd0, L"cd ", 3) == 0) {
		if (/*index*/wcschr(cmd0, L';') != NULL) {
			return(0);
		}
		/* TODO: shell syntax substitutions there, then vchdir*/
		wcsu8s(tmpstr, &cmd0[3]);
		/**/
		if (vchdir(/*&cmd0[3]*/tmpstr) < 0) {
			return(0);
		}
		else    {
			return (fil_cd());
		}
	}
	return(0);
}

/* terminate the vash, no return from there */
v_exit(cmd0, tmpstr)
wchar_t *cmd0;
char *tmpstr;
{
	if(wcscmp(cmd0, L"exit")==0) {
		onexit(0);
		exit(0);
	}
}
