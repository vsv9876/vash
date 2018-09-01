#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ediag.h>

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

extern  char  Crepf[];
extern  char  Cfill[];

#define ITMCNM 255      /* must be in "assist.h" */

/*NOXSTR*/
char    itmcnm[ITMCNM+1] =      /* Current item's name (saved) */
			"..";
int     itmci = -1;             /* Current item's index */
/*  YESXSTR*/

struct  stat cwdstat;

char    cwdpath[400];   /* CURRENT working directory full name */
char    lwdpath[400];   /* LAST working directory full name */

int rescan()
{
/*     cwdstat.st_ino = (ino_t)0;   */
    cwdstat.st_mtime = (time_t)0L;
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
	 */
	rwxs[0] = ((umode & S_IREAD) ? 'r' : '-');
	rwxs[1] = ((umode & S_IWRITE)? 'w' : '-');
	rwxs[2] = ((umode & S_IEXEC) ? 'x' : '-');
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
		if (*clm._itms[i] == MONEY) {
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
	u_short     mode;       /* права доступа и тип */
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
	/* надо бы сделать проверку на отсутствие ошибок... */
	/* это она и есть */
	if (staterr == 0) {
	    /* выяснить тип файла */
	    mode = itmstat.st_mode;
	    switch(mode & S_IFMT) {
#if defined(S_IFLNK)
	    case S_IFLNK:   ftype = 'l'; break;
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
	    cmdsub(itmnnm, "#@", clm._itm);
	    if (strcmp(itmcnm, itmnnm) == 0) {
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
	    return 0;
	}
adjust:
	/* имя осталось в меню, надо
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

vls()
/*
 * ВСТРОЕННАЯ КОМАНДА ls.
 * посчитать пункты, определить макс. длину пункта
 */
{
    DIR *dirp;
#ifdef  SYSV
    struct dirent *dp;
#else
    struct direct *dp;
#endif
    register char *itmbp;
    short len;
    int     Lflag;  /* ФЛАГ: НЕ РАЗЛИЧАТЬ СИМВ. ССЫЛКИ */
    int     Fflag;  /* ФЛАГ: ПОКАЗЫВАТЬ ТИП ФАЙЛА */
    int     aflag;  /* флаг: показывать все файлы */
    struct  stat sb;
    char    *fname;
    char    ftype;  /* СИМВОЛ ТИПА ФАЙЛА */
    int     ok;

    Lflag = Fflag = aflag = 0;
    if (index(Cfill, 'F')) Fflag = 1;
#if defined(S_IFLNK)
    if (index(Cfill, 'L')) Lflag = 1;
#endif
    if (index(Cfill, 'a')) aflag = 1;

    len = clm._itmlen = clm._itmmax = 0;
    clm._itms[clm._itmmax] = itmbp = clm._itmbuf;
    *itmbp++ = ' ';

    if ((dirp = opendir(Crepf)) != NULL) {
      for (dp = readdir(dirp); dp != NULL; dp = readdir(dirp))
	{
/*              printf("%6ld %s\n", dp->d_ino, dp->d_name);
 */
			/* здесь надо бы еще просчитать (в уме), как
			   правильно учитывать len */
		len = strlen(dp->d_name);

		if ( !aflag && dp->d_name[0] == '.'
			    && strcmp(dp->d_name,"..") != 0)
			/* скрытые имена */
			continue;

		/* если места не хватает */
		if (&clm._itmbuf[clm._itmbsz] <= &itmbp[len]) {
			w_emsg(ediag("No mem for all menu items",
				     "Нет места для всех пунктов меню"));
			break;
		}
		/* очередное имя занести в таблицу */
		fname = dp->d_name;

		/* определить тип файла */
#if defined(S_IFLNK)
		if (Fflag|Lflag) {
			if(Lflag) {
				ok =  stat(fname, &sb);
				if (ok < 0)
				/* пустые симлинки тоже пытаться показывать */
				       ok = lstat(fname, &sb);
			}
			else    ok = lstat(fname, &sb);
#else
		if (Fflag) {
			ok =  stat(fname, &sb);
#endif
			if (ok < 0) {
				if (!Lflag) {
#if defined (S_IFLNK)
					if (lstat(fname, &sb) < 0) {
					       w_emsg("Can't stat:");
					       w_str(fname);
					}
#else
					w_emsg("Can't stat:");
					w_str(fname);
#endif
				}
				else    {
#ifdef RETRO
					itmbp -= len;
#endif
					continue;
				}
			}
			switch(sb.st_mode & S_IFMT) {
#if defined(S_IFLNK)
			case S_IFLNK:   ftype = '@'; break;
#endif
#if defined(S_IFIFO)
			case S_IFIFO:   ftype = '='; break;
#endif
			case S_IFCHR:   ftype = '"'; break;
			case S_IFBLK:   ftype = ':'; break;
			case S_IFDIR:   ftype = '/'; break;
			case S_IFREG:
				if ((sb.st_mode & S_IEXEC) == S_IEXEC)
					ftype = '*';
				else    ftype = ' ';
				break;
			default:        ftype = '?'; break;
			}
			*itmbp++ = ftype;
		}
		else    *itmbp++ = ' ';
		/* ОЧЕРЕДНОЕ ИМЯ ЗАНЕСТИ В ТАБЛИЦУ */
		strcpy(itmbp, fname);
		itmbp += len;
		*itmbp++ = '\0';
		if ( len > clm._itmlen ) clm._itmlen = len;
		if (clm._itmmax >= ITMMAX)
			break;  /* НО МОЖНО И ПРОСТО ОБРЕЗАТЬ */
/*              if ((itmmax % 10) == 0) {
			w_chr('#'); fflush(vttout);
		}
 */
		clm._itmmax++;
		clm._itms[clm._itmmax] = itmbp;
		*itmbp++ = ' ';
	}
        closedir(dirp);
    }
    else {
	return(1); /* ERROR filling main menu */
    }
    *itmbp++ = '\0';
    if (clm._itmmax == 0) {
	    strcpy(clm._itmbuf, " /..");
	    len = 4;
	    clm._itmmax++;
    }
    if ( len > clm._itmlen ) clm._itmlen = len;
    clm._itmlen++;
/*  w_str("sort..."); fflush(vttout);
 */
    qsort(clm._itms, clm._itmmax, sizeof(char *), scomp);
/*  w_str("done"); fflush(vttout);
 */
    return(0);  /* OK */
}
/*
 * current working directory show (and other related info)
 * if xtermf, use xterm escapes '\E]0;'....'^G'
 * if panelf use
 */
cwdshow()
{
	char cwd_tmpstr[140]; /* cwdpath fraction to be shown, TODO change constant to some reasonable */
	char user_tmpstr[140];
	register int i;
	int     showli;     /* строка показа */
	int     deltco;     /* если в последней строке экрана, то == 1 */
	/*int     cwdirf = 1;		/* flag: cwdshow in console; TODO global flag via setup */
	extern char *getwd();
	extern char *getenv();
	extern int mailf2;  /* см. chckmail */
	int userlen; /* length of string about user */
	char *userstr; /* env(ASHLBL) or username/logname if empty/notset */

	char mode_tmpstr[40]; /* mode and permissions fraction */

	mailf2 = 1;

	if (panelf) {
	    showli = 0;         deltco = 0;
	} else {
	    showli = maxli-1;   deltco = 1;
	}

	if ((userstr=getenv("ASHLBL")) == (char *)0) {
#ifdef SYSV
		userstr = getenv("LOGNAME");
#else
		userstr = getenv("USER");
#endif
	}
	if (userstr == (char *)0)
		userstr = "Unknown USER";
	sprintf(user_tmpstr,
	"%s%s", userstr, (getuid() == 0 ? "(su)" : ""));
	userlen = strlen(user_tmpstr);

	if (Crepf[0] != '\0') {
		sprintf(mode_tmpstr,
		" %s <%s>", Crepf, rwxmode(&cwdstat));
	} else {
		strcpy(mode_tmpstr, " * ");
	}
	userlen += strlen(mode_tmpstr);

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
		fgets(cwdpath, 400, wdf);
		for (s=cwdpath; *s != '\n'; s++) ; *s = '\0';
		pclose(wdf);
	}
#endif
/*	sprintf(tmpstr, "[ %s ]", cwdpath); */
	if (xtermf) {
		w_raw("\033]0;");
		w_str(user_tmpstr);

		w_str(":");
		w_str(mode_tmpstr);
		w_str(":");

		w_str(cwdpath);
		w_raw("\007"); /* terminate escape sequence for xterm window title */
	}
	if (whodirf) {
		cp_set(showli, 0, HDR);
		w_str(user_tmpstr);
		w_str(" ");

		at_set(ATT);
		w_str(mode_tmpstr);

		sprintf(cwd_tmpstr, "[ %s ]", cwdpath);
		cp_set(showli, userlen, TXT); er_eol();
		i = strlen(cwd_tmpstr);
		i += deltco;
		if (i > (maxco-userlen)) {
			cp_set(showli, userlen + 1, HDR);
			w_str(&cwd_tmpstr[i - maxco + userlen + 1 + deltco]);
		}
		else {
			cp_set(showli, maxco - i - deltco, HDR);
			w_str(cwd_tmpstr);
		}
	}
#ifdef DEBUG
	sprintf(cwd_tmpstr, "0x%07lx", (unsigned long)vf);
	cp_set(-2, 0, TXT); w_str(cwd_tmpstr);
#endif
}

/*
 * Заполнить главное меню.
 * Команда для заполнения указана в Cfill,
 * если нач. с ':', значит встроенная команда ls.
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
	char tmpbuf[120]; /* for substitution in Cfill */

	if (Crepf[0] == '\0' && Cfill[0] != ':') {
		samedir = 0;
		/*
		 * надо отключить оптимизацию
		 * чтения главного меню
		 */
		cwdstat.st_ino = (ino_t)0;
	}
	else {
		stat(Crepf, &newstat);
		/* надо бы добавить проверку на отсутствие ошибок stat... */

		samedir = (cwdstat.st_dev   == newstat.st_dev
			&& cwdstat.st_ino   == newstat.st_ino);

		if (newflag == 0)
			if (cwdstat.st_mtime == newstat.st_mtime && samedir) {
				cwdstat = newstat;
				return(0);
			}
		cwdstat = newstat;
	}
#ifdef  RETRO
	cwdshow();      /* в принципе здесь это лишнее... */
#endif
	if (samedir) {
		/* сохранить прежний номер и содержимое пункта меню */
		itmci = clm._itm;
		strcpy(itmcnm, &clm._itms[clm._itm][2]);
/*                cmdsub(itmcnm, "#@", itm);     */
	}
	else    {
		itmci = -1;
		itmlwd();
	}
	if (clm._itmbuf != (char *)0)
		free(clm._itmbuf);
	/* можно и автоматически подбирать размер itmbsz */
	if ((clm._itmbuf = malloc(clm._itmbsz + 1)) == (char *)0) {
		w_emsg("No mem for main buffer...");
		onintr(1);
	}

	w_msg(TXT, Cfill); fflush(vttout);

	if (Cfill[0] != ':') { /* ВСТРОЕННАЯ КОМАНДА */
	/*TODO substitution */
		cmdsub(tmpbuf, Cfill, 0);
	/*NOSTRICT*/
		if (Cfill[0] == '\0'
		||  (fpls = popen(tmpbuf, "r")) == NULL) {
			cp_set(-1, 0, TXT);
			io_set(IO_TTYPE);
			fprintf(vttout, "\n");
			fprintf(stdout, /* stderr */
"\nMain menu build failed with error ('%s')\n", tmpbuf);/*Cfill);*/
			perror(Cfill);
			fatal();
		}
		vfread(fpls);   /* ВНЕШНЯЯ КОМАНДА */
		pclose(fpls);
	}
	else {
	    if ( vls() ) {        /* ВСТРОЕННАЯ КОМАНДА */
		w_emsg("Can't read '.' - use cd manually");
		return(0);
	    }
	}
	w_emsg("");

	itmini();       /* ПОСЧИТАТЬ ГАБАРИТЫ МЕНЮ */
	itmrestor();
	pre_vf();       /* СОЗДАТЬ СТРАНИЦУ LINLIB ДЛЯ МЕНЮ */
	return(1);
}

/*
 * СМЕНИТЬ КАТАЛОГ, ВЫДАТЬ ДИАГНОСТИКУ, НАСТРОИТЬ cwd, lwd;
 */
vchdir(cdarg)
char *cdarg;
{
	char  nwdpath[400];     /* НОВЫЙ КАТАЛОГ */

	if (*cdarg != '/') {
		strcpy(nwdpath, cwdpath);
		strcat(nwdpath, "/");
		strcat(nwdpath, cdarg);
	}
	else
		strcpy(nwdpath, cdarg);

	dcanon(nwdpath);        /* УБРАТЬ МУСОР ИЗ ПОНОГО ИМЕНИ */

	if (access(nwdpath, R_OK|X_OK)) {
		w_emsg("directory unaccessible:");
		w_str(nwdpath);
		return(-1);
	}
	if (chdir(nwdpath) < 0) {
		w_emsg(
		"Can't change directory to ");
		w_str(nwdpath);
		return(-1);
	}
	strcpy(lwdpath, cwdpath);
	strcpy(cwdpath, nwdpath);
	return(0);
}
