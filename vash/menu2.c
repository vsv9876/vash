#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "line.h"
#include "assist.h"

/*char *malloc();*/

/*
 * Подпрограммы организации пользовательского меню.
 *
 * Читается файл .ashmenu в текущем, либо в домашнем,
 * либо в системном каталоге. (Лучше сделать environ PATHM).
 * Файл содержит законченные командные файлы для sh, csh и т.п.
 * Функция menu2r сканирует этот файл, запоминает строки вывесок
 * и offset для lseek.
 * Далее указанный пункт меню по индексу itm отыскивается в файле
 * (через lseek), копируется в /tmp, и запускается как обычно.
 * Ему передаются аргументами помеченные файлы.
 */

#define LBLS    30      /* количество пунктов меню */

char    *lbls[LBLS+1];          /* строки меню */
off_t    lblofs[LBLS+1];        /* положение от нач. файла */
int      lblsize[LBLS+1];       /* размер порции */
int      lblsi;                 /* индекс текущего пукта меню */
int      lblmax;                /* количество пунктов */


extern   char    *tmpflnm;      /* файл для записи и выполнения меню */
static   FILE *fpmenu;

int
menu2r(mfile)
char *mfile;
{
	char str[140];          /* очередная строка из файла */
	int c;
	register char *p;
	off_t curofs;
	/*extern FILE *afopen();*/

	/* файлы меню выбираются из текущего каталога тоже */
	if ((fpmenu = dafopen(mfile, vapath, "r")) == NULL)
		return(0);
	/*
	 * Читать строку из файла, запомнить ее начало,
	 * если есть ключевой комментарий, обработать.
	 */
	p = str; lblsi = 0;
	curofs = (off_t)0;
	while ((c = getc(fpmenu)) != EOF) {
	    curofs += (off_t)1;
	    if ((*p++ = c) == '\n') {
		*(--p) = '\0';
		if (strncmp(str, "#+", 2) == 0) {
		    if (lblsi) {
			/* закрыть предыдущий фрагмент */
			lblsize[ lblsi - 1] = (int)( curofs -
			lblofs[ lblsi - 1] - (off_t)(strlen(str) + 1));
		    }
		    lblofs[ lblsi] = curofs;

		    p = &str[2];
		    if ((lbls[ lblsi] = malloc(strlen(p) + 1)) == NULL)
			break;  /* нет места для строк */
		    else
			strcpy(lbls[ lblsi], p);

		    if (lblsi < LBLS) lblsi++;
		    else break; /* нет места для указателей */
		}
		p = str;        /* восстановить указатель */
	    }
	}
	/* последний фрагмент закрывается по EOF */
	lblmax = lblsi;
	if (lblsi) {
	    lblsi -= 1;
	    lblsize[ lblsi] = (int)(curofs - lblofs[ lblsi]);
	}
	lblsi = 0;
	return(1);
}

menu2u(cmdlbl)
/*
 * Диалог меню, запись командного файла.
 */
char *cmdlbl;   /* вывеска для показа вместо выполняемой команды */
{
	register int count;
	register LINE *vfp;
	unsigned i;
	kbcod cod;
	FILE *fptmp;
	int c;

	/* первоначальный показ на экране */
	cp_set(clm._y0, 0, TXT);
	er_eop(TXT);
	cwdshow();
	itmshow();
	w_page(clm._vf, 0);

	for ( ;; ) {

		i = clm._itm - clm._itmofs;
		vfp = clm._vf;
		vfp += i;

		cod = r_line( vfp, 0 );

		w_emsg("");

		switch (cod) {

		default:
			break;
		case KB_HE:      /* справка */
			break;

		case KB_NL:
		case ' ':
			/* записать временный файл */
			if ((fptmp = fopen(tmpflnm, "w")) != NULL) {
				if (fseek(fpmenu, lblofs[ clm._itm], 0) < 0)
					return(0);
				count = lblsize[ clm._itm];
				while(--count >= 0
				&& (c = getc(fpmenu)) != EOF) {
					putc(c, fptmp);

				}
				fclose(fptmp);
				/*VARARGS*/
				sprintf(cmdlbl, "#+%s", lbls[clm._itm]);
				/* можно выполнять ком.файл */
				return(1);
			}
			/* no break -- проваливаемся... */
		case KB_CA:
		case KB_EX:
			return(0);      /* выход без выполнения */

		case KB_RE:      /* перерисовка */
			er_pag();
			cwdshow();
			w_emsg("");
			itmshow(); w_page(clm._vf, 0);
			break;
		case KB_AL:
		case KB_AU:
		case KB_AD:
		case KB_AR:
			i = itmadj(cod);
			break;
		}
	}
}

static  LINE tmplate =
/*NOSTRICT*/
{ /*16*/MAXLICO, 0, 0, 0, MSE|INP|PMT|NED|LFASTR, 0, cvt_sp, 0, 0 };

int
menu2(tmps, cmdlbl, mfile)
char *tmps;     /* строка для команды */
char *cmdlbl;   /* вывеска для показа вместо команды */
char *mfile;    /* имя файла программы меню */
{
	extern int  y0_top;     /* определено в vshcmd */
	extern char *pmtsh;    /* --"-- */
	LINEMENU savelm;
	int ok;

	if ( !menu2r(mfile) ) {
		w_emsg("No file"); w_str(mfile);
		return(-1);
	}

	savelm = clm;
	cp_set(clm._y0 - 1, 0, TXT);   /* СОХРАНИТЬ СВИТОК, СМ. НИЖЕ */
	er_eop(TXT);

	/* инициализация меню */
	clm._itms   = lbls;          /* указатели на строки меню */
	clm._itmmax = lblmax;        /* количество пунктов */
	clm._vf     = (LINE *)0;
	clm._itmlen = lframe->maxco/2;
	clm._ltmpl  = &tmplate;

	clm._itm    = lblsi;         /* текущий пункт меню */
	clm._yy_max = 10;
	clm._itmofs = 0;
	while((clm._itm - clm._itmofs) >= clm._yy_max)
		clm._itmofs += clm._yy_max;
	itmini();
	clm._yy = 10;
	clm._x0 = clm._itmlen/2 - 1;
	pre_vf();

	/* СОХРАНИТЬ СВИТОК */
	if (y0_top > clm._y0) {
		y0_top = clm._y0;
		scrlnl();
	}

	ok = menu2u(cmdlbl);       /* выбор из меню */

	/*
	free((char *)vf); vf = (LINE *)0;
*/
	cp_set(y0_top, 0, TXT); er_eop(TXT);
	for (; lblmax >= 0; lblmax--)
	    if (lbls[ lblmax] != (char *)0) {
		free(lbls[ lblmax]); lbls[ lblmax] = (char *)0;
	}
/*      lblsi = itm;       новое значение текущего пункта меню */

	free((char *)clm._vf); clm._vf = (LINE *)0;
	fclose(fpmenu);
	clm = savelm;

	if (ok)
		sprintf(tmps, ":sh %s %c%c", tmpflnm, MONEY, MONEY);
	return(ok);
}
