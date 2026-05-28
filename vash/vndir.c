/*
 * VASH - visual assistant for shell
 * Copyright (c) 1990-2025 Sergey Vovk <vsv>
 *
 * License: GPL and/or MIT,
 * see files COPYING and LICENSE
 */

#include <stddef.h>

#include <stdlib.h>
/*#define _GNU_SOURCE 1*/
#include <string.h>
#include <stdio.h>
#include <ediag.h>
#include <wchar.h>

#include <sys/types.h>
#include <sys/stat.h>

#ifndef DURA_LINUX
  #include <unistd.h>
  #include <dirent.h>
  #include <sys/stat.h>
  #define SYSV
  typedef unsigned short ushort;
#else
  #if !defined(u_short)
    #define         u_short ushort
  #endif
  #ifdef SVR4
    #define SYSV 1
  #endif
  #ifdef SYSV
    #include <dirent.h>
  #else
    #ifdef pdp11
    /* BSD 4.2 & DEMOS/P stubs */
    #include <ndir.h>
    #endif
  #endif
  #include <sys/stat.h>
  #define R_OK    4
  #define W_OK    2
  #define X_OK    1
#endif

#include "line.h"
#include "assist.h"
#include "astat.h"

/*NOXSTR*/
char    itmcnm[ITMCNM+1] =      /* Current item's name (saved) */
			"..";
int     itmci = -1;             /* Current item's index */
/*  YESXSTR*/

static struct  stat cwdstat;

char    cwdpath[U8_STRBUF];   /* CURRENT working directory full name */
char    lwdpath[U8_STRBUF];   /* LAST working directory full name */

int rescan(cmd)
const char *cmd;
{
	cwdstat.st_dev = (dev_t)0;
	cwdstat.st_ino = (ino_t)0;
/*
    cwdstat.st_mtime = (time_t)0;
    cwdstat.st_ctime = (time_t)0;
*/
    return(1);
}

/*
 * Вернуть строку прав доступа (rwx)
 */
char *
rwxmode(statp)
struct stat *statp;
{
	static  char rwxs[4];
	int umode;  /* реальные права доступа */
	unsigned short userid;  /* идентификатор пользователя */


	userid = getuid();

	umode = statp->st_mode;
	if (userid) { /* не является суперпользователем */
	    if      (statp->st_uid == userid);
/*          else if (stp->st_gid == getgid())   */
	    else if (gidchk(statp->st_gid))
		    umode <<= 3;
	    else    umode <<= 6;
	}
	/* assistant shell, будучи запущенным от root,
	 * смотрит на права владельца объекта (TODO дополнить acl)
	 * но не игнорирует их, и поступает как владелец
	 */
	rwxs[0] = ((umode & S_IRUSR/*S_IREAD*/) ? 'r' : '-');
	rwxs[1] = ((umode & S_IWUSR/*S_IWRITE*/)? 'w' : '-');
	rwxs[2] = ((umode & S_IXUSR/*S_IEXEC*/) ? 'x' : '-');
	rwxs[3] = '\0';
	return(rwxs);
}

/*
 * Функции формирования строки сравнения
 * для интерпретации команд.
 *
 * Формат выдачи:
 * tstat1():
 *      "rwx +" права каталога, наличие пометки.
 * tstat2():
 *      "rwx f filename"  права файла, его тип, имя файла.
 */

#ifdef RT11
#define TSTATS  40      /* М. БЫТЬ НАДО И БОЛЬШЕ... */
#else
#define TSTATS  400
#endif
static  char    tstats[TSTATS];         /* строка для помещения образца */

char *
tstat1()
{
	register int i;

	strcpy(tstats, rwxmode(&cwdstat));
	strcat(tstats, " -");
	/* выяснить наличие пометки */
	for (i=0; i < clm._itmmax; i++)
		if (clm._itms[i][0] == MONEY) {
			tstats[4] = '+'; break;
		}
	return(tstats);
}

char *
tstat2(fname)
char *fname;            /* имя файла из меню */
{
	int   staterr;
	struct stat itmstat;
	short     mode;       /* права доступа и тип */
#ifdef RETRO
	char  ftypestr[4];      /* подстрока с типом файла */
#endif
	register char ftype;    /* символ типа файла */

#if defined(S_IFLNK)
	if ((staterr = stat(fname, &itmstat)) < 0) {
	       staterr = lstat(fname, &itmstat);
	}
#else
	staterr = stat(fname, &itmstat);
#endif
	if (staterr == 0) {
	    /* выяснить тип файла */
	    mode = itmstat.st_mode;
	    switch(mode & S_IFMT) {
#if defined(S_IFLNK)
	    case S_IFLNK:   ftype = 'l'; break;
#endif
#if defined(S_IFSOCK)
		case S_IFSOCK:  ftype = 's'; break;
#endif
#if defined(S_IFIFO)
	    case S_IFIFO:   ftype = 'p'; break;
#endif
	    case S_IFCHR:   ftype = 'c'; break;
	    case S_IFBLK:   ftype = 'b'; break;
	    case S_IFDIR:   ftype = 'd'; break;

	    case S_IFREG:
			    if (itmstat.st_size == (off_t)0L)
				ftype = 'E';
			    else
				ftype = filetype(fname);
			    break;
	    default:
			    ftype = '*';
			    break;
	    }
	    sprintf(tstats, "%3.3s %c ", rwxmode(&itmstat), ftype);
	}
	else {
	    sprintf(tstats, "??? ! ");
	}
	/* общий размер содержимого tstats м.быть больше TSTATS !?... */
	strncat(tstats, fname, TSTATS-7);       /* !?... */
	tstats[TSTATS-1] = '\0';
	return(tstats);
}

void
itmrestor()
{
	char    itmnnm[ITMCNM+1];   /* новое имя */
	register int i;
	int dofs;       /* delta offset increment */

	/*
	 * ВОССТАНОВИТЬ ОСМЫСЛЕННОЕ ПОЛОЖЕНИЕ КУРСОРА
	 */
	for(clm._itm = 0; clm._itm < clm._itmmax; clm._itm++) {
	    i = clm._itm;
	    cmdsub(itmnnm, "#@", clm._itm, 0, 1);
	    if (strncmp(itmcnm, itmnnm, ITMCNM) == 0) {
		goto adjust;
	    }
	}
	if (itmci >= 0) {
	    if (itmci < clm._itmmax)
		clm._itm = itmci;    /* восстановить индекс */
	    else
		clm._itm = clm._itmmax - 1;
	}
	else    {
	    /* СООТВЕТСТВИЕ НЕ УСТАНОВЛЕНО */
	    clm._itmofs = clm._itm = 0;
	    return;
	}
adjust:
	/* имя осталось(найдено) в меню, надо
	 * попытаться сохранить положение окна */
	if (clm._itm >= clm._itmofs && clm._itm <  (clm._itmofs + (clm._xx * clm._yy)))
	    return;
	if (clm._yy <= 0 || clm._xx <= 0) {
		w_emsg("yy or xx are bad:");
		printf("yy = %d, yy = %d", clm._yy, clm._xx);
	}
	dofs = (clm._xx == 1 ? clm._yy : (clm._yy * (clm._xx/2)));


	/* ПОДОБРАТЬ НОВОЕ ПОЛОЖЕНИЕ ОКНА */
	for(clm._itmofs = 0;    ;clm._itmofs += dofs)
	    if (clm._itm >= clm._itmofs && clm._itm <  (clm._itmofs + (clm._xx * clm._yy)))
		return;
#ifdef RETRO
	/* СООТВЕТСТВИЕ НЕ УСТАНОВЛЕНО */
	itmofs = itm = 0;
#endif
}

itmlwd()
{
	register int cwdlen;     /* ДЛИНА ИМЕНИ ТЕКУЩЕГО КАТАЛОГА */
	register char *p;

	cwdlen = strlen(cwdpath);

	/*
	 * ПОСТАВИТЬ КУРСОР ПРОТИВ КАТАЛОГА, ИЗ
	 * КОТОРОГО ПОДНЯЛИСЬ.
	 */

	if (prefix(cwdpath, lwdpath)) { /* ЕСЛИ ПОДНИМАЛИСЬ ВВЕРХ */
		if (cwdlen == 1 || lwdpath[cwdlen++] == '/') {
			p = itmcnm;
			while (lwdpath[cwdlen] && lwdpath[cwdlen] != '/')
				*p++ = lwdpath[cwdlen++];
#ifdef RETRO
			*p++ = '/';
#endif
			*p = '\0';
		}
	}
	else
		strcpy(itmcnm, "..");
}

int
scomp(p1, p2)
register char **p1;
register char **p2;
{
	/* сравнивать надо только часть items, которая есть имя файла */
	return(strcmp( (*p1)+2, (*p2)+2));
}

int
vscomp(p1, p2)
register char **p1;
register char **p2;
{
	/* version comparison */
#ifndef NO_STRVERSCMP
	extern int strverscmp(const char *, const char *);
	return(strverscmp( (*p1)+2, (*p2)+2));
#else
	return(strcmp( (*p1)+2, (*p2)+2));
#endif
}

/* common flags like for ls() */
static int     Lflag;  /* ФЛАГ: НЕ РАЗЛИЧАТЬ СИМВ. ССЫЛКИ */
static int     Fflag;  /* ФЛАГ: ПОКАЗЫВАТЬ ТИП ФАЙЛА */
static int     aflag;  /* флаг: показывать все файлы */
static int     Vflag;  /* флаг: сортировать по версиям */
static int     fflag;  /* флаг: не сортировать */

int vls()
/*
 * embedded variant of ls
 * count number of items, determine VISIBLE item max length
 */
{
    DIR *dirp;
#ifdef  SYSV
    struct dirent *dp;
#else
    struct direct *dp;
#endif
    char *itmbp;  /* item buffer pointer */
    int   itmbsz;
    char *ibp_new; /* new item buffer pointer */

    ptrdiff_t ibp_ofs; /* ofset for stored items */
    int noext = 0;
    int  itmbextn = 1;  /* extent total number */
    int  i;
    short len;
    short vlen; /* length in codepoints */
    /*int     aflag;*/  /* флаг: показывать все файлы */
    char    *fname;

    /*aflag = 0;
    if (index(Cfill, 'a')) aflag = 1;*/

    len = clm._itmlen = clm._itmmax = 0;

    if ((dirp = opendir(Crepf)) != NULL) {
      for (dp = readdir(dirp); dp != NULL; dp = readdir(dirp))
      {
		/* skip hidden filenames but '..' entry will be passed */
		if (dp->d_name[0] == '.')
			if (strcmp(dp->d_name, "..") != 0)
				if (!aflag)
					continue;

		fname = dp->d_name;
		len = strlen(fname); /* internal length */
		vlen = u8vsize(fname); /* visible (on-screen) length  */

		/* initial allocation */
		if (clm._itmbuf == NULL) {
			itmbsz = clm._itmbsz;
			if ((clm._itmbuf = malloc(itmbsz)) == NULL) {
				w_emsg("malloc for main buffer: NO MEM... fatal");
				onintr(1);
			}
			clm._itms[0] = itmbp = clm._itmbuf;
		    /**itmbp++ = ' '; /* 1st placeholder */
		}
		/* try to extent item buffer if no room for current entry */
		if (&clm._itmbuf[itmbsz] <= &itmbp[len]) {
			itmbextn += 1;
			itmbsz = (clm._itmbsz * itmbextn);
			if ((ibp_new = realloc(clm._itmbuf, itmbsz)) == NULL) {
				w_emsg("No mem for all menu items");
				break;
			}
			ibp_ofs = ibp_new - clm._itmbuf;
			clm._itmbuf = ibp_new;
			/* fix stored pointers */
			if (ibp_ofs != 0)
				for (i = 0; i < clm._itmmax; i++)
					clm._itms[i] += ibp_ofs;
			itmbp += ibp_ofs;
		}
		/* store current item into the table (after 2 placeholders) */
		clm._itms[clm._itmmax] = itmbp;
	    *itmbp++ = ' '; /* 1st placeholder */
		*itmbp++ = ' '; /* 2nd placeholder */
		strcpy(itmbp, fname);
		itmbp += len;
		*itmbp++ = '\0';
		if ( vlen > clm._itmlen )
			clm._itmlen = vlen;
		clm._itmmax++;
		if (clm._itmmax >= ITMMAX)
			break;
		}
        closedir(dirp);
    }
    else {
    	return(1); /* ERROR filling main menu */
    }

    itmbsz_ex = itmbsz; /*export*/

    if ( ! fflag ) {
    	if (Vflag) qsort(clm._itms, clm._itmmax, sizeof(char *), vscomp);
    	else       qsort(clm._itms, clm._itmmax, sizeof(char *), scomp);
    }

    return(0);  /* OK */
}

/*
 * встроенная команда (аналог cat), понимает и учитывает vexdir
 * сделана копипастой из vls()
 */
int vcat(file)
char *file;
{
	FILE *fp;
	int ch;

    register char *itmbp;
    size_t len;

    unsigned char    *fname; /* item name, from main menu */
    /*char	*file;*/ /* name of file to be read */
    char    *s;
    char   itmname[U8_STRBUF]; /**/
    char  *ip;

    /*if (index(Cfill, 'a')) aflag = 1;*/
	/* in progress:TODO: автоматически подбирать размер itmbsz */
	/*
	 * this malloc/realloc must be inside of vls() and/or vfread();
	 */

	if ((clm._itmbuf = malloc(clm._itmbsz + 1)) == (char *)0) {
		w_emsg("malloc a main buffer: no mem... fatal");
		onintr(1);
	}

    len = clm._itmlen = clm._itmmax = 0;
    clm._itms[clm._itmmax] = itmbp = clm._itmbuf;
    *itmbp++ = ' '; /* 1st placeholder */


    /*file = &Cfill[1];*/
    while(*file != '\0' && *file == ' ') {/*найти имя файла в Cfill после ключа '_'*/
    	file++;
    }
    s = file;
    while(*s != '\0' && *s > ' ') {/*strip a string tailing garbage */
    	s++;
    }
    *s = '\0';
	if ((fp = dafopen(file, vexdir, "r")) == NULL) {
    	return(1); /* ERROR filling main menu */
	} else {
		ip = &itmname[0];
		while ((ch = fgetc(fp)) != EOF)	{
			if (ch != '\n') {
				  *ip++ = ch;
			} else {
				*ip++ = '\0';
				ip = &itmname[0]; /*для накопления следующей строки*/
				/*строка накоплена в d_name*/
				len = strlen(itmname);

				if (&clm._itmbuf[clm._itmbsz] <= &itmbp[len]) {
					w_emsg("FATAL: vfill(): clm.itmbuf overflow");
					fclose(fp);
					return(1);
				}

				*itmbp++ = ' '; /* 2nd placeholder */
				/* store current item into the table (after 2 placeholders) */
				strcpy(itmbp, itmname);
				itmbp += len;
				*itmbp++ = '\0';
				if ( len > clm._itmlen ) clm._itmlen = len;
				if (clm._itmmax >= ITMMAX)
					break;
				clm._itmmax++;
				clm._itms[clm._itmmax] = itmbp;
				*itmbp++ = ' ';
			}
		} /*while*/
		fclose(fp);
	}
    *itmbp++ = '\0';
/*
    *itmbp++ = '\0';
    if (clm._itmmax == 0) {
	    strcpy(clm._itmbuf, " /..");
	    len = 4;
	    clm._itmmax++;
    }
    if ( len > clm._itmlen ) clm._itmlen = len;
    clm._itmlen++;

    qsort(clm._itms, clm._itmmax, sizeof(char *), scomp);
*/
    return(0);
}

char vlstype(fname)
char *fname;
/*
 * returns one symbol (sorry, cast to char/wchar)
 */
{
    struct  stat sb;
    char    ftype;  /* СИМВОЛ ТИПА ФАЙЛА */
    int     ok;

	/* определить тип файла */
#if defined(S_IFLNK)
	if (Fflag|Lflag) {
		if(Lflag) {
			ok =  stat(fname, &sb);
			if (ok < 0)
			/* пустые симлинки тоже пытаться показывать */
				   ok = lstat(fname, &sb);
		}
		else {
			ok = lstat(fname, &sb);
		}
#else
		if (Fflag) {
			ok =  stat(fname, &sb);
#endif
			if (ok < 0) {
				if (!Lflag) {
#if defined (S_IFLNK)
					if (lstat(fname, &sb) < 0) {
						   w_emsg("lstat() error on: "); w_str(fname);
					}
#else
					w_emsg("stat() error on: "); w_str(fname);
#endif
				}
			}
#if !defined (S_IFLNK)
		}
#endif
		switch(sb.st_mode & S_IFMT) {
#if defined(S_IFLNK)
		case S_IFLNK:   ftype = '@'; break;
#endif
#if defined(S_IFIFO)
		case S_IFIFO:   ftype = '|'/*'='*/; break;
#endif
		case S_IFCHR:   ftype = '"'; break;
		case S_IFBLK:   ftype = ':'; break;
		case S_IFDIR:   ftype = '/'; break;
		case S_IFREG:
			if ((sb.st_mode & S_IEXEC) == S_IEXEC)
				ftype = '*';
			else    ftype = ' ';
			break;
#if defined(S_IFSOCK)
		case S_IFSOCK:  ftype = '='; break;
#endif
		default:        ftype = '?'; break;
		}
	} else {
		ftype = ' ';
	}

    return(ftype);  /* OK */
}



vlstag() {
/*маркировать во втором байте тип файла, подобно ls -F */
	int i;

	char *fname;
	char *s;

	/*hint: do it Crepf[] points to directory, so items are files */
	if (Crepf[0] != '\0') {
		for (i = 0; i < clm._itmmax; i++) {
			fname = &clm._itms[i][2]; /* file name there */

			s = clm._itms[i];
			/*type of file symbol placed there*/
			s[1] = (char) vlstype(fname);
		}
	}
}

void binpwd()
{
#ifdef  pdp11
	if (cwdpath[0] == '\0' && getwd(cwdpath) == 0) {
		fprintf(stderr, "getwd() == 0\n");
		fatal();
	}
#endif
#ifdef SYSV
	if (cwdpath[0] == '\0') {
		FILE *wdf;
		char *s;
		if ((wdf=popen("/bin/pwd", "r")) == NULL) {
			fprintf(stderr, "Can't exec /bin/pwd\n");
			fatal();
		}
		fgets(cwdpath, U8_STRBUF-1, wdf); /* TODO WTF */
		for (s=cwdpath; *s != '\n'; s++) ; *s = '\0';
		pclose(wdf);
	}
#endif
}

size_t cwd_fmt(char *ctmps, char *cpath, int room)
{
	int cwd_vx;
	int v_size, last_size; /* visible size of path, vsize of last element*/
	int nlast, n, i; /*slash: total, current, index in ctmps*/
	u8char_t *s;
	u8char_t *snext;
	wchar_t  wc;
	wchar_t *ws;
	wchar_t *w_path;
	wchar_t *w_tmps;

	v_size = u8vsize(cpath);
	last_size = 0;

	if (v_size <= room) {
		strcpy(ctmps, cpath);
		return (v_size);
	} else {
		w_path = calloc(U8_STRBUF, sizeof(wchar_t));
		w_tmps = calloc(U8_STRBUF, sizeof(wchar_t));
		/*count elements, convert cpath*/
		nlast = 0;
		ws = w_path;
		for(s = cpath; *s != 0; s = snext, ws++) {
			if (*s == '/') {
				nlast++;
			}
			snext = u8pxx(s, ws);
		}
		/*
		 *  /home/user/project/subproject/subdir.d
		 *  /home/user/p* /s* /s*r.d
		 */
		for (n = i = 0, ws = w_path; /*i < room && */*ws != 0; ws++) {
			if (i >= room/**ws != L'\0'*/) {
				/*i--;*/
				w_tmps[i++] = L'>';
				break;
			}
			if (*ws == L'/') {
				n++;
			}
			if (n < nlast) {
				if (n < 3) {
					w_tmps[i++] = *ws;
					continue;
				}
				else {
					if (*ws == L'/') {
						if ((last_size = vsize(ws)) <= room - i) {
							wcscpy(&w_tmps[i], ws);
							i += last_size;
							break;
						}
						last_size = 0;
						w_tmps[i++] = *ws++;  /*  '/' */
						w_tmps[i++] = *ws;    /*  1st symbol of path element */
						/* about 2nd symbol */
						if (*ws == 0) {
							break;
						} else {
							ws++;
							if (ws[1] == L'/' || ws[1] == 0) {
								w_tmps[i++] = *ws;	/* 2nd symbol is last one in element */
							} else {
								w_tmps[i++] = L'*'; /* path element consists of 3 symbols and more*/
							}
						}
					}
					continue;
				}
			}
			else {
				/*last element expected to be as verbose as possible*/
				if (*ws == L'/') {
					if ((last_size = vsize(ws)) <= room - i) {
						wcscpy(&w_tmps[i], ws);
						i += last_size;
						break;
					}
					w_tmps[i++] = *ws++; /*  '/' */
					w_tmps[i++] = *ws; /*  1st symbol */
					/* about 2nd symbol */
					if (*ws == 0) {
						break;
					} else {
						ws++;
						if (ws[1] == L'/' || ws[1] == 0) {
							w_tmps[i++] = *ws;	/* 2nd symbol */
						} else {
							w_tmps[i++] = L'*'; /* 3rd symbol exists */
						}
					}
				}
				if (last_size > 0) {
					if (vsize(ws) > room - i) {
						continue;
					}
					w_tmps[i++] = *ws; /* visible tail */
					/*if (i >= room) {
						w_tmps[i++] = L'>';
						break;
					}*/
				}
			}

		}
	}
	w_tmps[i++] = L'\0';
	wcstombs(ctmps, w_tmps, U8_STRBUF);
	return (vsize(w_tmps));
}

/*
 * current working directory show (and other related info)
 * if vashflag.xtermf, use xterm escapes '\E]0;'....'^G'
 * if vashflag.panelf is in use
 */
void
cwdshow()
{
	char cwd_tmpstr[U8_STRBUF]; /* cwdpath fraction to be shown */
	char lbl_tmpstr[U8_STRBUF];
	register int x;
	int cwd_x;			/* visible width of cwd_tmpstr */
	int cwd_room;		/* space to display cwd_tmpstr */
	int     showli;     /* строка показа */
	int     deltco;     /* если в последней строке экрана, то == 1 */
	/*int     cwdirf = 1;		/* flag: cwdshow in console; TODO global flag via setup */
	extern char *getwd();
	extern char *getenv();
	extern int mailf2;  /* см. chckmail */
	int lblen; /* length of string about user */
	char *ashlbl; /* env(VASH_LABEL) or  */
	char *usrlbl; /* username/logname if VASH_LABEL is empty/notset*/

	char mode_tmpstr[40]; /* mode and permissions fraction */

	mailf2 = 1;

	if (vflag.panelf) {
		if (clm._y0 > y0_top) {
			showli = y0_top-1;
			cp_set(y0_top-1, 0, TXT);
		}
		else {
			showli = clm._y0 - 1;
			cp_set(clm._y0 - 1, 0, TXT);
		}
		er_eop(TXT);
		/*showli = y0_top-1 clm._y0 -1;*/
		deltco = 0;
	} else {
	    showli = lframe->maxli - 1;
	    deltco = 1;
	}

	if ((ashlbl=getenv("VASH_LABEL")) == (char *)0) {
		ashlbl = "vash -- :";
	}
#ifdef SYSV
	usrlbl = getenv("LOGNAME");
#else
	usrlbl = getenv("USER");
#endif
	if (usrlbl == NULL) {
		usrlbl = "";
	}

	sprintf(lbl_tmpstr, "%s", ashlbl);
	lblen = /*strlen*/u8vsize(lbl_tmpstr);

	if (Crepf[0] != '\0') {
		sprintf(mode_tmpstr, " %s <%s> %s ",
				Crepf, rwxmode(&cwdstat), usrlbl);
	} else {
		strcpy(mode_tmpstr, " * ");
	}
	lblen += /*strlen*/u8vsize(mode_tmpstr);

	binpwd();

/*	sprintf(tmpstr, "[ %s ]", cwdpath); */
	if (vflag.xtermf) {
		w_raw("\033]0;");
		w_str(lbl_tmpstr);

		w_str(cwdpath);
		/*w_str(" "); w_str(mode_tmpstr); w_str(":");*/

		w_raw("\007"); /* terminate escape sequence for xterm window title */
	}
	if (vflag.whodirf) {
		cp_set(showli, 0, HDR);
		w_str(lbl_tmpstr);

		cwd_room = lframe->maxco - lblen - 2 - deltco;

		x = 0;
		x += deltco;
		cwd_x = 0;
		if (NULL != index(ashlbl, ':')) {
			cwd_x = cwd_fmt(cwd_tmpstr, cwdpath, cwd_room);
			w_str(cwd_tmpstr);
		}
		for (x = cwd_x; x <= cwd_room; x++) {
			w_chr(' ');
		}
		cp_set(showli, lframe->maxco - u8vsize(mode_tmpstr) - deltco, HDR);
		w_str(mode_tmpstr);
	}
#ifdef DEBUG
	sprintf(cwd_tmpstr, "0x%07lx", (unsigned long)vf);
	cp_set(-2, 0, TXT); w_str(cwd_tmpstr);
#endif
}

static int fil_first = 1; /*hint for 1st invocation - do it silently*/

extern void onintr(int);

/*
 * Заполнить главное меню.
 * Команда для заполнения указана в Cfill,
 * если нач. с ':', значит встроенная команда ls.
 * ПЕРЕСМОТРЕНО в 2023:
 * если нач. с ':' или '-', то выполняется мвркировка типа файла
 * если после ':' есть только флаги, выполняется встроенная команда ls
 *
 * Если реперный файл не указан, внешняя команда заполнения
 * выполняется всегда.
 *
 * Возвращает 1, если заполнено новое меню, иначе 0.
 */
fil_vf(newflag)
int newflag;    /* если 0, то только обновить каталог */
{
	struct  stat newstat;
	FILE *fpls;
	int samedir;    /* ФЛАГ: ТОТ ЖЕ КАТАЛОГ */
	char *p;		/**/
	int  fpopen = 1; /* extra command required flag */
	char tmpbuf[ITMCNM+2]; /* for substitution in Cfill */

	/*samedir = 1;*/
	if (Crepf[0] == '\0' /*&& Cfill[0] != ':' && Cfill[0] != '-'*/) {
		/*
		 * отключить оптимизацию
		 * чтения главного меню
		 */
		cwdstat.st_dev = (dev_t)0;
		cwdstat.st_ino = (ino_t)0;
		samedir = 0;
	}
	else {
		stat(Crepf, &newstat);
		/* надо бы добавить проверку на отсутствие ошибок stat()... */
		samedir = 0;
		if (       cwdstat.st_dev   == newstat.st_dev
				&& cwdstat.st_ino   == newstat.st_ino)
			samedir = 1;

		if (newflag == 0 && samedir)
			if (cwdstat.st_mtime == newstat.st_mtime
					/*workaround for bug storing cwdstat.st_mtime*/
					|| cwdstat.st_ctime == newstat.st_ctime) {
				/*cwdstat = newstat;*/
				return (0);
			}
		cwdstat = newstat;
	}
#ifdef  RETRO
	cwdshow();      /* в принципе здесь это лишнее... */
#endif
	if (samedir) {
		/* сохранить прежний номер и содержимое пункта меню */
		itmci = clm._itm;
		strncpy(itmcnm, &clm._itms[clm._itm][2], ITMCNM-1);
		itmcnm[ITMCNM-1] = '\0';
/*      TODO:          cmdsub(itmcnm, "#@", itm, 0, 1);     */
	}
	else    {
		itmci = -1;
		itmlwd();
	}
	if (clm._itmbuf != (char *)0)
		free(clm._itmbuf);
	clm._itmbuf = (char *)0;

	if (fil_first) {
		fil_first = 0;
	} else {
		w_msg(TXT, Cfill); w_str(" -- "); fflush(vttout);
	}

	/* флаги ls (для встроенной и для внешней команды) */
    Lflag = Fflag = aflag = Vflag = fflag = 0;

    p = Cfill;
    if(*p == '<') { /* список в файле (в пути) vexdir */
		if ( vcat(&p[1]) ) { /* возвращает 0, если все хорошо */
			w_emsg(&p[1]); w_str(": vcat() failed");
			return(0);
		}
		fpopen = 0;
	}
	else if (*p == ':' || *p == '-') {
	    for (p = &Cfill[1]; *p != '\0' && *p != ' '; p++) {
	    	switch(*p) {
	    	case 'F': Fflag = 1; break;
#if defined(S_IFLNK)
	    	case 'L': Lflag = 1; break;
#endif
	    	case 'a': aflag = 1; break;
	    	case 'v': Vflag = 1; break;
	    	case 'f': fflag = 1; break;
	    	default:
	    		w_emsg("Cfill: flag unsupported: '");
	    		w_chr(*p); w_chr('\'');
	    		return(0);
	    		break;
	    	}
	    	continue;
	    }
	    if (NULL == strchr(Cfill, ' ')) {
	    	fpopen = 0;
	    	if ( vls() ) {        /* ВСТРОЕННАЯ КОМАНДА */
	    		w_emsg(Cfill); w_str(" -- vls() failed");
	    		return(0);
	    	}
	    } else {
	    	p = strchr(Cfill, ' ');
	    	p++;
	    }
	}

    conduit(NULL);

    if (fpopen) {
    	/*TODO substitution */
		cmdsub(tmpbuf, p, 0, 0, 1);
		/*NOSTRICT*/
		if (/*Cfill[0] == '\0' ||*/ (fpls = popen(tmpbuf, "r")) == NULL) {
			w_emsg(p); w_str(" -- command failed: ");
			w_str(tmpbuf);
			return(0);
		}
		if (Cfill[0] == '_') {
			/*TODO: читать заголовок из команды (1st string)*/
		}
		vfread(fpls);   /* ВНЕШНЯЯ КОМАНДА */
		pclose(fpls);
	}
	w_emsg("");

	vlstag();		/* маркировать (items)файлы подобно ls -F */

	itmini();       /* ПОСЧИТАТЬ ГАБАРИТЫ МЕНЮ */
	itmrestor();
	pre_vf();       /* create main view page in initial state
					СОЗДАТЬ СТРАНИЦУ LINLIB ДЛЯ МЕНЮ */
	return(1);
}

/*
 * СМЕНИТЬ КАТАЛОГ, ВЫДАТЬ ДИАГНОСТИКУ, НАСТРОИТЬ cwd, lwd;
 */
int
vchdir(cdarg)
const char *cdarg;
{
	extern int dcanon(); /*workaround from BSD code*/
	extern void sh_unesc();
	char  nwdpath[U8_STRBUF];     /* new working directory path */
	char  tmppath[U8_STRBUF];
	register char *p;

	p = nwdpath;
	if (*cdarg != '/') {
		strcpy(nwdpath, cwdpath);
		if (strcmp(nwdpath,"/")!=0)	strcat(nwdpath, "/");
		strcat(nwdpath, cdarg);
	}
	else
		strcpy(nwdpath, cdarg);
#ifdef TRY_SH_UNESCAPE
		/*strcat(nwdpath, cdarg);*/
		while (*p++);
	}
#endif
	sh_unesc(tmppath, nwdpath); /*sh_esc() reverse*/
	strcpy(nwdpath, tmppath);

	dcanon(nwdpath);        /* canonicalize new path */

	if (access(nwdpath, R_OK|X_OK)) {
		w_emsg("no access: ");  w_str("'");
		w_str(nwdpath);         w_str("'");
		return(-1);
	}
	if (chdir(nwdpath) < 0) {
		w_emsg("can't chdir() to ");  w_str("'");
		w_str(nwdpath);               w_str("'");
		return(-1);
	}
	strcpy(lwdpath, cwdpath);
	strcpy(cwdpath, nwdpath);
	if (vflag.histcd && (strcmp(nwdpath, lwdpath) != 0)) {
		cmdhget(1);
	}
	return(0);
}
