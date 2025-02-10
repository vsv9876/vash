#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include "line.h"
#include "assist.h"
#include "slist.h"

/*#define DEBUGS 1 /*debug TAB completion printout*/

/*#define BUFSMAX 4000*/
#define BUFSMAX 4*STRBUF

extern char *getenv();
/* suggestion mode */
enum sugg_mode { path_cmd, command, cd_dir, file_dir, flag1, flag2 } ;
static const char *s_debug[] = { "path_cmd", "command", "cd_dir", "file_dir", "flag1", "flag2", (char *)0 };
enum sugg_mode s_mode;

SLIST_HEAD *sgglist;
static hlp_onscreen = 0;

static char tstats[6];  /* type+mode from stat() filled in cmpl_stat */

/*#define wchar wchar_t*/


char *cmpl_stat(s_dir, s_base, s_ins)
wchar_t *s_dir;
wchar_t *s_base;
wchar_t *s_ins;
{
    struct stat statbuf;
    extern char *rwxmode();
    char  tmps_buf[U8_STRBUF];
    char *tmps;
    char  ftype;	/* file (or dir) type */
    u_short mode;
    char *s;

    tmps = tmps_buf/*alloca(U8_STRBUF)*/;
/*
	if (
			(s_mode == path_cmd ||
			s_mode == command ||
			s_mode == file_dir ||
			s_mode == cd_dir)) {
	}
*/
	if (wcscmp(s_dir, s_base) == 0) {
		sprintf(tmps, "%ls%ls", s_base, s_ins);
	} else {
		sprintf(tmps, "%ls/%ls%ls", s_dir, s_base, s_ins);
	}

	if (stat (tmps, &statbuf) < 0) {
		return NULL;
	} else {
	/* sorry, its a copypaste from tstat2() */
	/*if (1) {*/
	    /* выяснить тип файла */
	    mode = statbuf.st_mode;
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
				ftype = 'f';
			    break;
	    default:
			    ftype = '*';
			    break;
	    }
	    s = rwxmode(&statbuf);
	    sprintf(tstats, "%c%3.3s", ftype, s);
	}
	return tstats;
}

/*
 * check a candidat for completion
 */
int sgg_valid(s_dir, s_base, s_ins)
wchar_t *s_dir;
wchar_t *s_base;
wchar_t *s_ins;
{

	if (s_mode == flag1 || s_mode == flag2)
		return 1;
	if (s_mode == path_cmd)
		return 1;
	if(cmpl_stat(s_dir, s_base, s_ins) == NULL) {
		return 0;
	}
	if (s_mode == cd_dir) {
		if (tstats[0] == 'd' && tstats[1] =='r' && tstats[3] == 'x')
			return 1;
	} else if (s_mode == path_cmd || s_mode == command) {
		if (tstats[0] != 'd' && tstats[3] == 'x')
			return 1;
	} else if (s_mode == file_dir) {
		if (tstats[1] == 'r')
			return 1;
	}
	return 0;
}


void
hlp_clr() {
	int cur_co; /* расчетная позиция курсора перед показом элемента */
	int cur_li;

	if (hlp_onscreen) {
		hlp_onscreen = 0;
		cur_co = 0;
		for (cur_li = clm._y0; cur_li <= lframe->maxli; cur_li++) {
			cp_set(cur_li, cur_co, TXT);
			er_eol(TXT);
		}
	}
}

static int old_n = 0;
static int old_chk = 0;
static int n_cur_co = 0; /* n в начале последней колонки показанной в предыдущий раз*/
static int maxover = 0; /* флаг если последняя колонка вышла за пределы экрана */
int hlp_clear() {
	old_n = old_chk = n_cur_co = maxover = 0;
}

#define START_CO 1
#define GAP_CO 3
/*
 * help screen with completion suggestions list, called by <Tab> KB_TA;
 * list like /bin/ls - columns compact view
 */
int hlp_compl()
{
	SLIST *sl_n;
	int sl_size, n, nx; /* start from 1 not 0 (size 0 if empty list) */
	char tmps[40];
	wchar_t *s;
	int ssize;
	int new_chk;
	int cur_co = 0; /* расчетная позиция курсора перед показом элемента */
	int cur_li = 0;
	int maxwidth = 0; /* максимальная ширина в колонке */

	int li_start = clm._y0;
	int li_end =  lframe->maxli - 3;

	sl_size = sgglist->sl_size;
	sl_n = sgglist->sl_last;

	for (cur_li = li_start; cur_li <= li_end; cur_li++) {
		cp_set(cur_li, cur_co, TXT);
		er_eol(TXT);
	}

	if (old_chk != (new_chk = sl_chk(sgglist))) {
		old_chk = new_chk;
		maxwidth = 0;
		n = old_n = 1;
	} else {
		n = old_n;
		if (n >= sl_size && maxover == 0) {
			n = 1;
		}
		if (maxover > 0) {
			n = n_cur_co;
		}
	}
	maxover = 0;
	for (nx = 1; nx < n && sl_n != NULL; nx++) {
		sl_n = sl_prev(sl_n); /* skip the part of list already shown */
	}

	cur_li = li_start;
	maxwidth = 0;
	cur_co += START_CO;
	for (  ; n <= sl_size && sl_n != NULL && cur_li <= li_end; n++) {
		if (cur_li == li_start) {
			n_cur_co = n;
		}
		s = sl_sstr(sl_n);
		ssize = /*wcslen*/vsize(s); /* visible size is differ */
		if (maxwidth < ssize)
			maxwidth = ssize;
		if (cur_co + maxwidth /*+ START_CO*/ >= lframe->maxco) {
			maxover = 1;
			/*break;*/
		}

		cp_set(cur_li, cur_co, TXT);
		w_wcstr(s);

		cur_li++;
		if (cur_li > li_end) {
			/* start printing next column */
			cur_li = li_start;
			if (maxover == 0) {
				cur_co += maxwidth + GAP_CO;
			} else {
				if (cur_co == START_CO) {
					/*sl_n = sl_prev(sl_n);*/
					maxover = 0;
					break;
				}
				cur_co = START_CO;
				/*maxover = 0;*/
				break;
			}
			maxwidth = 0;
		}
		sl_n = sl_prev(sl_n);
	}
	old_n = n;


	sprintf(tmps, "   <%s> [%-d/%-d] ", s_debug[s_mode], n, sl_size);
	cp_set(-2, 0, TXT|INP); /*er_eop();*/
	w_str(tmps);

	if (n < sl_size) {
		cp_set(-2, 40, TXT|INP);
		w_str(">>");
	}

	hlp_onscreen = 1;
	return 1;
}
#if 0
int hlp_compl_notused()
{
	SLIST *slist;
	int slistn, n;
	char tmps[20];
	wchar_t *s;
	int ssize;
	int cur_co; /*расчетная позиция курсора перед показом элемента */
	int cur_li;

	cur_li = clm._y0; cur_co = 0;
	slistn = sgglist->sl_size;
	slist = sgglist->sl_last;

	/* show stars on 1st columns for debug TODO cleanup*/
	for (cur_li = lframe->maxli - 3; cur_li >= clm._y0; cur_li--) {
		cp_set(cur_li, cur_co, TXT); w_chr('-'); er_eol(TXT);
	}
	cur_li = clm._y0;

	cp_set(/*cur_li*/ -2, cur_co, TXT|INP); /*er_eop();*/
	sprintf(tmps, " <%s> [%d] ", s_debug[s_mode], slistn);
	w_str(tmps);

	/* cur_co += strlen(tmps) + 1;*/
	for (n = slistn; n > 0 && slist != NULL && cur_li < lframe->maxli - 2 ; n--) {
		s = sl_sstr(slist);
		ssize = wcslen(s); /*размер на экране отличается*/
		cp_set(cur_li, cur_co, TXT);
		w_wcstr(s);
		cur_co += ssize + 2;
		if (cur_co + ssize > lframe->maxco - 2) {
			/*next line, begin column*/
			cur_co = 2; cur_li++;
		}
		slist = sl_prev(slist);
	}
	hlp_onscreen = 1;
	return 1; /**/
}
#endif

/* returns elements count or -1 if error;
 * fill sugg.string if found exact one candidat
 * NOTE: patterns stored without escapes
 */
int sh_sugg(dirp, patt, from/*, insp*/)
wchar_t *dirp;
wchar_t *patt;
char *from;	/* shell command to get context of suggestion */
/*char *insp;*/
{
	FILE *pipe0;
	char *filestr; /* utf-8 file string after fgets() */
	wchar_t *inss; /* element of list */
	int count;
	size_t sz, pattsz;
	char *s;
	int c, ok;
	SLIST *slist;

	count = 0;
	pattsz = wcslen(patt); /*тут нужен именно strlen*/

	if ((inss = malloc(U8_STRBUF)) == NULL) {
		ok = -2; goto ret;
	}

	pipe0 = popen(from, "r");
	if (pipe0 != NULL) {
		if ((filestr = malloc((U8_STRBUF))) == NULL) {
			ok = -2; goto ret; /*in hope this never happen*/
		}
		while ( NULL != (s = fgets((s = filestr), BUFSMAX, pipe0))) {
			/*s = fgets((s = filestr), BUFSMAX, pipe);*/
			u8swcs(inss, filestr);
			if (wcsncmp(patt, inss, pattsz) == 0) {
				/*strncpy(inss, s + pattsz, STRBUF);*/
				/*skip trailing LF */
				sz = wcslen(inss); /*тут нужен именно strlen*/
				if (sz > 0 && inss[sz - 1] == '\n') {
					inss[sz - 1] = '\0';
				}
				if (sl_find(sgglist, inss) == 0) {
					if (sgg_valid(dirp, patt, &inss[pattsz])) {
						slist = sl_add(sgglist, inss);
						count++;
					}
				}
			}
		}
		free(filestr);
		clearerr(pipe0);
		pclose(pipe0);
	} else {
		ok = -1; goto ret;
	}
ret:
	if (inss != NULL) free(inss);
	return count;
}

/*
 * попытка подобрать общую часть, если найдено больше одного подходящего продолжения
 * возвращает строку общей части;
 *
 * сравнение начинается после последнего символа в s_base,
 * потому что все строки в sgglist уже имеют общую часть s_base
 *
 */
wchar_t *sgg_ext(s_base)
wchar_t *s_base;
{
	static  wchar_t s_buf[STRBUF] = L"";
	SLIST  *slist;
	size_t	slist_sz;
	wchar_t  **sggstr; /* массив указателей на строки sgglist */
	wchar_t  *s;
	wchar_t  *str;
	int i, k, x_ext; /* длина образца, расширенная */
	int	all;

	s_buf[0] = '\0'; /* sure clean on exit */

	x_ext = wcslen(s_base);

	slist_sz = sgglist->sl_size;
	if (slist_sz == 0) return s_buf;

	slist    = sgglist->sl_last;
	sggstr = (wchar_t **)malloc(sizeof(wchar_t *) * slist_sz);
	if (sggstr == NULL) return s_buf; /* nothing to do */
	i = slist_sz;
	while(i > 0) {
		i -= 1;
		s = sl_sstr(slist);
		sggstr[i] = &s[x_ext];
		slist = sl_prev(slist);
	}

	/*расширять образец сравнения на один символ из первого (попавшегося) кандидата*/
	str = sggstr[0];
	i = 0;
	while (i < STRBUF && str[i] != '\0') {
		/*расширить образец на один символ */
		s_buf[i] = str[i];
		s_buf[++i] = '\0';

		/* поочередно сравнить образец со строками sgglist */
		all = 1;
		for (k = 0; k < slist_sz; k++) {
			if (wcsncmp(s_buf, sggstr[k], i) != 0)
				all = 0;
		}
		if (!all) {
			s_buf[--i] = '\0'; /* затереть последний символ, он не подошел */
			break;
		}
	}
	free(sggstr);
	return s_buf;
}

/*
 * deduplicate array of pointers to string:
 *   if content equal, leave 1st one then squeeze;
 * return number of elements in array when squeeze done
 */
int sdedup(pathp, maxnpath)
char *pathp[];
int maxnpath;
{
	int n, n2, ndup; /* n2 for deduplicate path dirs */

	/* deduplicate pathdir */
	for (n = 0; n < maxnpath; n++ ) {
		for (n2 = n + 1; n2 < maxnpath; n2++) {
			if (strcmp(pathp[n], pathp[n2]) == 0) {
				while(n2 < maxnpath) {
					/* remove duplicated element and squeeze array */
					pathp[n2] = pathp[n2 + 1]; n2++;
				}
				pathp[n2] = '\0'; maxnpath -= 1;
			}
		}
	}
	return n;
}

/*
 * селектор режима завершения ввода (выбор алгоритма подбора вставки)
 * пока поддерживается dirfile
 *
 * заполняет список подходящих вариантов.
 * возвращает количество найденных вариантов,
 */
int do_compl(/*s_mode, *//*insp,*/ dirp, basep, av0)
/*char *insp; /* pointer to suggestion string to be inserted */
wchar_t *dirp; /* pointer to dir */
wchar_t *basep; /*pointer to base */
wchar_t *av0;	/* name of command, argv0 */
{
	int res = 0;  /*	result: count of suggestions variants, -1 if impossible on errors */
	wchar_t *dir;
	char *bufs; char *tmps;	/* couple of string buffers for sprintf */;

	wchar_t *pathbin;
#define MAXNPATH 50
	char *pathdir[MAXNPATH];
	char *path;		/* env PATH element iterator like */
	int n, maxnpath, n2, ndup; /* n2 for deduplicate path dirs */

	tmps = malloc((size_t)(BUFSMAX + 1));
	bufs = malloc((size_t)(BUFSMAX + 1));
/*	sgglist = sl_init();*/

	if (bufs == NULL) return -1;

/*	*insp = '\0'; 	default: nothing to suggest */
	if ((s_mode == file_dir) ||
			s_mode == cd_dir || s_mode == command ) {
#ifdef DEBUGS_DURA
		if (strcmp("dura", basep) == 0) {
			strcpy(insp, " ");
			res = 1; goto ret;
		}
#endif
		if (wcscmp(dirp, basep) == 0) {
			dir = L".";
		} else {
			dir = dirp;
		}
		sprintf(bufs, "ls -A1r %ls 2>/dev/null", dir);
		res = sh_sugg(dirp, basep, bufs);
	} else if (s_mode == path_cmd) {
		/* для каждого каталога в PATH просмотреть файлы и построить общее содержимое в sgglist */
		path = getenv("PATH");
		if (path != NULL) {
			strncpy(bufs, path, BUFSMAX+1);
			path = bufs; path[BUFSMAX+1] = '\0';

			for (n = 0; n < MAXNPATH && *path != '\0'; n++) {
				pathdir[n] = path;
				for (; *path != '\0'; path++) {
					if (*path == ':') {
						*path++ = '\0'; break;
					}
				}
				if (strlen(pathdir[n]) == 0) {
					pathdir[n] = ".";
				}
			}
			maxnpath = n;
			maxnpath = sdedup(pathdir, maxnpath);
			/*prepare suggestion list*/
			for(n = 0; n < maxnpath && pathdir[n] != NULL; n++) {
				sprintf(tmps, "ls -A1r %s 2>/dev/null", pathdir[n]);
				res = sh_sugg(dirp, basep, tmps);
			}
		}
	} else if (s_mode == flag1 || s_mode == flag2) {
		/* найти ключи в выдаче man argv0 */
		sprintf(bufs,
				"man %ls 2> /dev/null | sed -e 's/ /\\n/g' -e 's/|/\\n/g' |"
				"grep -e '^%ls' | sed -e 's/^-$//' -e 's/[.]$//' |"
				"sed -e 's/[*,)}]$//g' -e 's/=.*/=/' -e 's/\\[=.*//' |"
				"sort -u ", /* | less */
					av0, basep);
		res = sh_sugg(dirp, basep, bufs);
	}
ret:
/*	if(sgglist != NULL) sgglist = sl_free(sgglist);*/
	free(tmps);
	free(bufs);
	return res;
	/* return -1; /* if error */
}

/*
 * попытка завершить ввод
 *
 * ввод символов выполняется прямо в буфер набираемой строки,
 * при подстановке проверяется попытка выйти за границы буфера;
 * символом пустого промежутка м.быть только пробел (' ')
 *
 * другие функции вызываются для подбора вариантов, но не для ввода
 */
int
try_compl(cmd0, curpos, maxpos)
wchar_t  *cmd0; /* буфер строки набираемой команды */
int  *curpos; /* текущая позиция курсора в буфере */
int maxpos; /* максимальное значение позиции в буфере строки */
{
#ifdef DEBUGS
    static char debugs[U8_STRBUF];			/* completion string */
#endif
/*	char s_ins[STRBUF] = "";		/* suggestion string to be inserted */
    wchar_t s_argv0[STRBUF];	/* собственно имя команды */
    wchar_t *av0;
    wchar_t s_dir[STRBUF];      /* база (например, путь до каталога) */
	wchar_t s_base[STRBUF];     /* хвост (например, префикс имени в каталоге) */
    int argc, x_in, x_out, ins_len /*, dir_len, base_len;*/;
    int base_x, dir_x, dir_end;
    /* cmd scaner context: 's'eparator, 'a'rg, 'n'ull, //'i'ni */
    char contxt, conold;
    int escaped;		/* символ экранирован */
    /*enum sugg_mode s_mode;*/
    int ok;
    char *s;
    int   c;
    wchar_t *s_ins; /* the suggestion word to be inserted */
    wchar_t  s_ins_buf[STRBUF];
    wchar_t *s_ins_esc;
    /*char *stats;*/
    size_t pattsz;

    s_ins_esc = s_ins_buf/*alloca(U8_STRBUF)*/;
    s_dir[0] = s_base[0] = '\0';
    dir_x = dir_end = base_x = 0;
    /* учесть нули за концом команды и поставить курсор перед ними */
    conold = 's'; contxt = 'n';
    argc = 0;
    /*
     * хак: после '|', '&', ';' попытаться трактовать как команду,
     * сбрасывая сканер до argc=0 при обнаружении этих символов
     */
	av0 = &s_argv0[0];
	*av0 = L'\0';
	for (x_in = 0; x_in < *curpos && x_in < maxpos; x_in++) {
    	/* determine current context */
		if (cmd0[x_in] == L'\\' && !escaped) {
			escaped = 1;
			continue;
		}
		if (escaped) {
			conold = contxt;
		}
		/* skip leading spaces and separators between args */
		if (cmd0[x_in] == L' ') {
			if (!escaped) {
				contxt = 's'; /* definitely separator */
				/*escaped = 1;*/
			} else {
				contxt = 'a'; /* part of argument */
				escaped = 0;
			}
		} else if (!escaped && (
				cmd0[x_in] == L';' ||
				cmd0[x_in] == L'|' ||
				cmd0[x_in] == L'&')) {
			contxt = 'n';
			conold = 's';
			argc = 0;
			av0 = &s_argv0[0];
			*av0 = L'\0';
			escaped = 0;
		} else {
			contxt = 'a';
			escaped = 0;
		}
		if (conold != contxt) {
			/* context changed just now */
			if (contxt == 's') {
				dir_x = dir_end = base_x = 0;
				s_dir[dir_x] = s_base[base_x] = '\0';
				if (conold == 'a')
					argc++;
				/*goto the_moon;*/
			}
			if (contxt == 'a') {
				dir_x = base_x = 0;
			}
		}
		if (contxt == 's')
			goto the_moon;
		if (contxt == 'a') {
			if (argc == 0) {
				*av0 = cmd0[x_in];
				av0++;
				*av0 = L'\0';
			}
			if (cmd0[x_in] == '/') {
				base_x = 0;
				s_dir[dir_x++] = cmd0[x_in];
				dir_end = dir_x;
				/*s_base[base_x] = '\0';*/
			} else {
				s_dir[dir_x++] = cmd0[x_in];
				s_base[base_x++] = cmd0[x_in];
			}
			/* terminate resulting substrings */
			s_dir[dir_x] = s_base[base_x] = '\0';
		}
the_moon:
		conold = contxt; /* remember current context for next hope */
    }
    if (dir_end != 0) s_dir[dir_end] = '\0'; /* terminate if differ */

    /*
     *  выбрать тип подстановки - текущий каталог, команда, параметр команды, TODO: ключ команды
     *  */
    if (argc == 0) {
    	if (wcscmp(s_dir, s_base) == 0) {
        	/* команда из PATH, не содержит '/'; base и dir одинаковы */
    		s_mode = path_cmd;
    	} else {
            /* команда из текущего каталога или полное имя команды */
    		s_mode = command;
    	}
    } else if (argc > 0) {
		s_mode = file_dir; /* относительный путь к файлу */
		if (wcsncmp(&cmd0[0], L"cd ", 3) == 0) {
			/* директория для cd */
			s_mode = cd_dir;
		} else {
			if (s_dir[0] == '-') {
				if (s_dir[1] == '-')
					s_mode = flag2;
				else
					s_mode = flag1;
			}
		}
    }

    /*x_in = wcslen(cmd0);*/
    if (x_in < *curpos) *curpos = x_in; /* вернуть курсор к концу набираемой строки */

    ok = -2;

    /* найти и выполнить подстановку, если однозначно;
     * иначе сигнал (или TODO: показать список)
     */
    s_ins = L"";
    if (contxt == 'a' || (contxt == 's' && argc == 0)) {
    	if ((ok = do_compl(s_dir, s_base, &s_argv0[0]/*av0*/)) < 0) {
    		;/*bell();*/
    	} else if (ok == 1) {
			if (sgglist->sl_size == 1) {
				s_ins = sl_sstr(sgglist->sl_last);
				pattsz = wcslen(s_base);
				/* skip common part of patt and suggestion */
				s_ins += pattsz;
			}
		} else {
			/* find common part from several suggestions */
			s_ins = sgg_ext(s_base);
			ins_len = wcslen(s_ins);
			if (ins_len == 0) {
				;/*bell();*/ /*TODO visual menu of suggestions there */
			}
		}
    	sh_wcesc(s_ins_esc, s_ins, 0); /* */
    	ins_len = wcslen(s_ins_esc);
		/*
		 * insert a completion just found into cmd buffer
		 */
    	if (ins_len >= 0) {
			/* advance one symbol on success: '/' on dir, ' ' otherwise */
    		if (sgglist->sl_size == 1) {
    			if (cmpl_stat(s_dir, s_base, s_ins) != NULL) {
					if (tstats[0] == 'd') {
						c = '/';
					} else {
						c = ' ';
					}
    			} else if (s_mode == path_cmd) {
    				c = ' ';
    			}
				s_ins_esc[ins_len] = c; ins_len += 1; s_ins_esc[ins_len] = '\0';

    		}
    		/* растолкать место для вставки */
			for(x_out = maxpos - ins_len, x_in = maxpos; x_in >= *curpos; x_in--, x_out--)
				cmd0[x_in] = cmd0[x_out];

			/* insert into command editor buffer */
			/*u8swcs(wcstmp, s_ins);*/
			for(x_out=0, x_in=*curpos; x_out < ins_len; x_out++, x_in++)
				cmd0[x_in] = s_ins_esc[x_out];

			*curpos += ins_len;
    	}
    }

#ifdef DEBUGS
	sprintf(debugs, "av0'%ls' \\%d ok=%d'%c%c #%d %d/%d dir'%ls' base'%ls' <%s> ins'%ls'  %4s~",
			&s_argv0[0], escaped, ok, conold, contxt, argc,
			*curpos, maxpos, s_dir, s_base, s_debug[s_mode], s_ins_esc, tstats);
    cp_set( clm._y0-1 /*-2*/, 0, ATT); w_str(debugs); er_eol(); /*cp_fet();*/
#endif

  	return ok;
}
