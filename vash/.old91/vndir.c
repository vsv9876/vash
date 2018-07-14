#include <sys/types.h>
#ifdef          SYSV
#define         u_short ushort
#endif

#include <sys/stat.h>
#define R_OK    4
#define W_OK    2
#define X_OK    1

#include <string.h>
#include <stdio.h>

/* #include <ndir.h>       /* BSD 4.2 & DEMOS/P mistake */
#include <dirent.h>

#include "line.h"
#include "assist.h"
#include "astat.h"

extern  char  Crepf[];
extern  char  Cfill[];

extern  int errno;

#define ITMCNM 255

/*NOXSTR*/
char    itmcnm[ITMCNM+1] =       /* СОХРАНЕННАЯ СТРОКА С ПУНКТОМ МЕНЮ */
			"..";
/*  YESXSTR*/

struct  stat cwdstat;   /* ДЛЯ текущего каталога */

char    cwdpath[400];   /* полное имя ТЕКУЩЕГО рабочего каталога */
char    lwdpath[400];   /* полное имя ПРЕДЫДУЩЕГО рабочего каталога */

/*
 * Вернуть строку прав доступа (rwx)
 */
char *
rwxmode(stp)
struct stat *stp;
{
	static  char rwxs[4];
	u_short umode;  /* реальные права доступа */

	umode = stp->st_mode;
	if      (stp->st_uid == getuid());
	else if (stp->st_gid == getgid())
		umode <<= 3;
	else    umode <<= 6;
	rwxs[0] = ((umode & S_IREAD) ? 'r' : '-');
	rwxs[1] = ((umode & S_IWRITE)? 'w' : '-');
	rwxs[2] = ((umode & S_IEXEC) ? 'x' : '-');
	rwxs[3] = 0;
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

#define TSTATS  40      /* М. БЫТЬ НАДО И БОЛЬШЕ... */
static  char    tstats[TSTATS];         /* строка для помещения образца */

char *
tstat1()
{
	register int i;

	strcpy(tstats, rwxmode(&cwdstat));
	strcat(tstats, " -");
	/* выяснить наличие пометки */
	for (i=0; i < itmmax; i++)
		if (*itms[i] == MONEY) {
			tstats[4] = MONEY; break;
		}
	return(tstats);
}

char *
tstat2(fname)
char *fname;            /* имя файла из меню */
{
	struct stat itmstat;
	u_short     mode;       /* права доступа и тип */
	char  ftypestr[4];      /* подстрока с типом файла */
	register char ftype;    /* символ типа файла */

	stat(fname, &itmstat);
		/* надо бы сделать проверку на отсутствие ошибок... */

	/* права доступа */
	strcpy(tstats, rwxmode(&itmstat));

	/* выяснить тип файла */
	mode = itmstat.st_mode;
	switch(mode & S_IFMT) {
#ifndef SYSV
	case S_IFLNK:   ftype = 'l'; break;
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
	default:        ftype = '*'; break;
	}
	/* сформировать остаток строки */
	sprintf(ftypestr, " %c ", ftype);
	strcat(tstats, ftypestr);
	strcat(tstats, fname);
	return(tstats);
}

itmrestor()
{
	register int i;
	int dofs;

	/*
	 * ВОССТАНОВИТЬ ОСМЫСЛЕННОЕ ПОЛОЖЕНИЕ КУРСОРА
	 */
	for(itm = 0; itm < itmmax; itm++) {
	    i = itm;
	    if (strcmp(itmcnm, &itms[i][2]) == 0) {

		/* ПОПЫТАТЬСЯ СОХРАНИТЬ ПОЛОЖЕНИЕ ОКНА */
		if (itm >= itmofs && itm <  (itmofs + (xx*yy)))
		    return;
		if (yy <= 0 || xx <= 0) {
			w_emsg("yy or xx are bad:");
			printf("yy = %d, yy = %d", yy, xx);
		}
		dofs = (xx == 1 ? yy : (yy*(xx/2)));


		/* ПОДОБРАТЬ НОВОЕ ПОЛОЖЕНИЕ ОКНА */
		for(itmofs = 0;    ;itmofs += dofs)
		    if (itm >= itmofs && itm <  (itmofs + (xx*yy)))
			return;
	    }
	}
	/* СООТВЕТСТВИЕ НЕ УСТАНОВЛЕНО */
	itmofs = itm = 0;
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
/*      int     aflag;     флаг: показывать все файлы */
	struct  stat sb;
	char    *fname;
	char    ftype;  /* СИМВОЛ ТИПА ФАЙЛА */
	int     ok;

	Lflag = Fflag = 0;
	if (index(Cfill, 'F')) Fflag = 1;
	if (index(Cfill, 'L')) Lflag = 1;

	len = itmlen = itmmax = 0;
	itms[itmmax] = itmbp = itmbuf;
	*itmbp++ = ' ';

    if ((dirp = opendir(Crepf)) != NULL)
        for (dp = readdir(dirp);
                  dp != NULL;
		       dp = readdir(dirp))
	{
/*              printf("%6ld %s\n", dp->d_ino, dp->d_name);
 */
			/* здесь надо бы еще просчитать (в уме), как
			   правильно учитывать len */
		len = strlen(dp->d_name);

		/* если места не хватает */
		if (&itmbuf[itmbsz] <= &itmbp[len])
			break;

		/* очередное имя занести в таблицу */
		fname = dp->d_name;

		/* определить тип файла */
#ifndef SYSV
		if (Fflag|Lflag) {
			if(Lflag)
				ok =  stat(fname, &sb);
			else    ok = lstat(fname, &sb);
#else
		if (Fflag) {
			ok =  stat(fname, &sb);
#endif
			if (ok < 0) {
				if (!Lflag) {
					w_emsg("Can't stat:");
					w_str(fname);
				}
				else    {
#ifdef RETRO
					itmbp -= len;
#endif
					continue;
				}
			}
			switch(sb.st_mode & S_IFMT) {
#ifndef SYSV
			case S_IFLNK:   ftype = '@'; break;
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
		if ( len > itmlen ) itmlen = len;
		if (itmmax >= ITMMAX)
			break;  /* НО МОЖНО И ПРОСТО ОБРЕЗАТЬ */

/*              if ((itmmax % 10) == 0) {
			w_chr('#'); fflush(vttout);
		}
 */
		itmmax++;
		itms[itmmax] = itmbp;
		*itmbp++ = ' ';
	}
        closedir(dirp);



	*itmbp++ = '\0';
	if (itmmax == 0) {
		strcpy(itmbuf, " /..");
		len = 4;
		itmmax++;
	}
	if ( len > itmlen ) itmlen = len;
	itmlen++;

/*      w_str("sort..."); fflush(vttout);
 */
	qsort(itms, itmmax, sizeof(char *), scomp);

/*      w_str("done"); fflush(vttout);
 */
}

cwdshow()
/* показать раб. каталог и др. вспомогательную информацию */
{
	char tmpstr[140];
	register int i;
	extern char *getwd();
	extern char *getenv();
	int userlen;
	char *ashlbl;

	if ((ashlbl=getenv("ASHLBL")) == (char *)0) {
#ifndef SYSV
		ashlbl = getenv("USER");
#else
		ashlbl = getenv("LOGNAME");
#endif
	}
	if (ashlbl == (char *)0)
		ashlbl = "Unknown USER";

	sprintf(tmpstr,
	"%s%s", ashlbl, (getuid() == 0 ? ":root" : ""));

	cp_set(0, 0, HDR);      /* слева вверху */
	w_str(tmpstr); userlen = strlen(tmpstr);

	if (Crepf[0] != '\0') {
		sprintf(tmpstr,
		" %s <%s>", Crepf, rwxmode(&cwdstat));
	} else {
		strcpy(tmpstr, " * ");
	}
	userlen += strlen(tmpstr);
	at_set(ATT);
	w_str(tmpstr);

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
	sprintf(tmpstr, "[ %s ]", cwdpath);
	cp_set(0, userlen, TXT); er_eol();
	i = strlen(tmpstr);
	if (i > (maxco-userlen)) {
		cp_set(0, userlen + 1, HDR);
		w_str(&tmpstr[i - maxco + userlen + 1]);
	}
	else {
		cp_set(0, maxco-i, HDR);
		w_str(tmpstr);
	}
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

	if (Crepf[0] == '\0' && Cfill[0] != ':') {
		samedir = 0;
		/* надо отключить оптимизацию чтения главного меню */
		cwdstat.st_ino = (ino_t)0;
	}
	else {
		/* надо бы добавить проверку на отсутствие ошибок... */
		stat(Crepf, &newstat);

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
	if (samedir)
		/* и здесь еще надо сохранить прежний номер itm */
		strcpy(itmcnm, &itms[itm][2]);
	else
		itmlwd();

	if (itmbuf != (char *)0)
		free(itmbuf);
	/* можно и автоматически подбирать размер itmbsz */
	if ((itmbuf=malloc(itmbsz+1)) == (char *)0) {
		w_emsg("Нет памяти для главного буфера...");
		onintr(1);
	}

	w_msg(TXT, Cfill); fflush(vttout);

	if (Cfill[0] != ':') { /* ВСТРОЕННАЯ КОМАНДА */
	/*NOSTRICT*/
		if (Cfill[0] == '\0'
		||  (fpls = popen(Cfill, "r")) == NULL) {
			fprintf(stderr,
"\nОшибка при заполнении главного меню('%s') : %d\n", Cfill, errno);
			fatal();
		}
		vfread(fpls);   /* ВНЕШНЯЯ КОМАНДА */
		pclose(fpls);
	}
	else    vls();          /* ВСТРОЕННАЯ КОМАНДА */

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
		w_emsg("Недоступен каталог:");
		w_str(nwdpath);
		return(-1);
	}
	if (chdir(nwdpath) < 0) {
		w_emsg(
		"Не могу сменить каталог на");
		w_str(nwdpath);
		return(-1);
	}
	strcpy(lwdpath, cwdpath);
	strcpy(cwdpath, nwdpath);
	return(0);
}
