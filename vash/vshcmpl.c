#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include "line.h"
#include "assist.h"
#include "slist.h"

/*#define DEBUGS 1 /*debug TAB completion printout*/

#define BUFSMAX 4000

extern char *getenv();
/* suggestion mode */
static enum sugg_mode { path_cmd, command, cd_dir, file_dir, flag, flaglong } ;
static char *s_debug[] = { "path_cmd", "command", "dir_cd", "file_dir", "flag", "flaglong", (char *)0 };
enum sugg_mode s_mode;

SLIST_HEAD *sgglist;
static hlp_onscreen = 0;

static char tstats[6];

char *cmpl_stat(s_dir, s_base, s_ins)
char *s_dir;
char *s_base;
char *s_ins;
{
    struct stat statbuf;
    extern char *rwxmode();
    char tmps[(MAXLICO * 3) + 3];
    char  ftype;	/* file (or dir) type */
    u_short mode;
    char *s;

/*
	if (
			(s_mode == path_cmd ||
			s_mode == command ||
			s_mode == file_dir ||
			s_mode == cd_dir)) {
	}
*/
	if (strcmp(s_dir, s_base) == 0) {
		sprintf(tmps, "%s%s", s_base, s_ins);
	} else {
		sprintf(tmps, "%s/%s%s", s_dir, s_base, s_ins);
	}

	if (stat (tmps, &statbuf) < 0) {
		return NULL;
	} else {
	/*копипаста из tstat2() */
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
 * проверить, уместно ли рассматривать кандидата на завершение команды
 */
int sgg_valid(s_dir, s_base, s_ins)
char *s_dir;
char *s_base;
char *s_ins;
{
	char *s;
	s = tstats;

	if (s_mode == path_cmd) return 1;
	if(cmpl_stat(s_dir, s_base, s_ins) == NULL) {
		return 0;
	}
	if (s_mode == cd_dir) {
		if (s[0] == 'd' && s[1] =='r' && s[3] == 'x')
			return 1;
	} else if (s_mode == path_cmd || s_mode == command) {
		if (s[0] != 'd' && s[3] == 'x')
			return 1;
	} else if (s_mode == file_dir) {
		if (s[1] == 'r')
			return 1;
	}
	return 0;
}


int hlp_clr() {
	int cur_co; /*рассчетная позиция курсора перед показом элемента */
	int cur_li;

	if (hlp_onscreen) {
		hlp_onscreen = 0;
		cur_co = 0;
		for (cur_li = clm._y0; cur_li <= maxli; cur_li++) {
			cp_set(cur_li, cur_co, TXT); er_eol(TXT);
		}
	}
}
int hlp_compl()
{
	SLIST *slist;
	int slistn, n;
	char tmps[20];
	char *s;
	int ssize;
	int cur_co; /*расчетная позиция курсора перед показом элемента */
	int cur_li;

	cur_li = clm._y0; cur_co = 0;
	slistn = sgglist->sl_size;
	slist = sgglist->sl_last;

	for (cur_li = maxli - 3; cur_li >= clm._y0; cur_li--) {
		cp_set(cur_li, cur_co, TXT); w_chr('*'); er_eol(TXT);
	}
	cur_li = clm._y0;
	cp_set(cur_li, cur_co, TXT|INP); /*er_eop();*/
	sprintf(tmps, " <%s> [%d] ", s_debug[s_mode], slistn);
	w_str(tmps);
	cur_co += strlen(tmps) + 1;
	for (n = slistn; n > 0 && slist != NULL && cur_li < maxli - 2 ; n--) {
		s = sl_sstr(slist);
		ssize = strlen(s); /*размер на экране отличается*/
		cp_set(cur_li, cur_co, TXT);
		w_str(s);
		cur_co += ssize + 2;
		if (cur_co + ssize > maxco - 2) {
			/*next line, begin column*/
			cur_co = 2; cur_li++;
		}
		slist = sl_prev(slist);
	}
	hlp_onscreen = 1;
	return 1; /**/
}

/* returns elements count or -1 if error; fill sugg.string if found exact one candidat */
int sh_sugg(dirp, patt, from/*, insp*/)
char *dirp;
char *from;
char *patt;
/*char *insp;*/
{
	FILE *pipe;
	char *filestr; /*[BUFSMAX+1];*/
	char *inss; /*[MAXLICO+1]; /**/
	int count;
	size_t sz, pattsz;
	char *s;
	int c, ok;
	SLIST *slist;

	count = 0;
	pattsz = strlen(patt); /*тут нужен именно strlen*/

	if ((inss = malloc((size_t)MAXLICO + 1)) == NULL) {
		ok = -2; goto ret;
	}

	pipe = popen(from, "r");
	if (pipe != NULL) {
		if ((filestr = malloc((size_t)MAXLICO + 1)) == NULL) {
			ok = -2; goto ret; /*in hope this never happen*/
		}
		while(!feof(pipe)) {
			s = fgets((s = filestr), BUFSMAX, pipe);
			if (s != NULL && strncmp(patt, filestr, /*strlen(patt)*/pattsz) == 0) { /*тут нужен именно strlen*/
					/*strncpy(inss, s + pattsz, MAXLICO);*/
					/*skip trailing LF */
					sz = strlen(filestr); /*тут нужен именно strlen*/
					if (sz > 0 && filestr[sz-1] == '\n') {
							filestr[sz-1] = '\0';
					}
					if (sl_chkdup(sgglist, filestr) == 0) {
						if (sgg_valid(dirp, patt, filestr+/*strlen(patt)*/pattsz)) {
						slist = sl_add(sgglist, filestr);
						count++;
					}
				}
			}
		}
		free(filestr);
		clearerr(pipe);
		pclose(pipe);
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
char *sgg_ext(s_base)
char *s_base;
{
	static  char s_buf[MAXLICO + 1] = "";
	SLIST  *slist;
	size_t	slist_sz;
	char  **sggstr; /* массив указателей на строки sgglist */
	char  *s;
	char  *str;
	int i, k, x_ext; /* длина образца, расширенная */
	int	all;

	s_buf[0] = '\0'; /* sure clean on exit */

	x_ext = strlen(s_base);

	slist_sz = sgglist->sl_size;
	if (slist_sz == 0) return s_buf;

	slist    = sgglist->sl_last;
	sggstr = (char **)malloc(sizeof(char *) * slist_sz);
	if (sggstr == NULL) return s_buf; /* nothing to do */
	i = slist_sz;
	while(i > 0) {
		i -= 1;
		s = sl_sstr(slist);
		sggstr[i] = s + x_ext; slist = sl_prev(slist);
	}

	/*расширять образец сравнения на один символ из первого (попавшегося) кандидата*/
	str = sggstr[0];
	i = 0;
	while (i < MAXLICO && str[i] != '\0') {
		/*расширить образец на один символ */
		s_buf[i] = str[i];
		s_buf[++i] = '\0';

		/* поочередно сравнить образец со строками sgglist */
		all = 1;
		for (k = 0; k < slist_sz; k++) {
			if (strncmp(s_buf, sggstr[k], i) != 0) all = 0;
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
 * deduplicate array of pointers to string, if content equal, leave firts one and squieeze
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
int do_compl(/*s_mode, *//*insp,*/ dirp, basep)
/*char *insp; /* pointer to suggestion string to be inserted */
char *dirp; /* pointer to dir */
char *basep; /*pointer to base */
{
	int res = 0;  /*	result: count of suggestions variants, -1 if impossible on errors */
	char *dir;
	char *bufs; /*[BUFSMAX+1]*/;
	char *tmps;
	char *pathbin;
#define MAXNPATH 50
	char *pathdir[MAXNPATH];
	char *path;
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
		if (strcmp(dirp, basep) == 0) {	dir = ".";
		} else {						dir = dirp; }
		sprintf(bufs, "ls -A1 %s 2>/dev/null", dir);
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
				sprintf(tmps, "ls -A %s 2>/dev/null", pathdir[n]);
				res = sh_sugg(dirp, basep, tmps);
			}
		}
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
try_compl(cmd, curpos, maxpos)
char *cmd; /* буфер строки набираемой команды */
int  *curpos; /* текущая позиция курсора в буфере */
int maxpos; /* максимальное значение позиции в буфере строки */
{
#ifdef DEBUGS
    static char debugs[MAXLICO+1];			/* completion string */
#endif
/*	char s_ins[MAXLICO+1] = "";		/* suggestion string to be inserted */
    char s_dir[MAXLICO+1]; /* база (например, путь до каталога) */
	char s_base[MAXLICO+1]; /* хвост (например, префикс имени в каталоге) */
    int argc, x_in, x_out, ins_len /*, dir_len, base_len;*/;
    int base_x, dir_x, dir_end;
    char contxt, conold;   /* cmd scaner context: 's'eparator, 'a'rg, 'n'ull, 'i'ni */
    /*enum sugg_mode s_mode;*/
    int ok;
    char *s;
    int   c;
    char *s_ins;
    /*char *stats;*/
    size_t pattsz;

    s_dir[0] = s_base[0] = '\0';
    dir_x = dir_end = base_x = 0;
    /* учесть нули за концом команды и поставить курсор перед ними */
    conold = 's'; contxt = 'n';
    argc = 0;
    /*
     * хак: после '|', '&', ';' попытаться трактовать как команду,
     * сбрасывая сканер до argc=0 при обнаружении этих символов
     */
    for (x_in = 0; x_in < *curpos && x_in < maxpos; x_in++) {
    	/* determine current context */
    	/* skip leading spaces and separators between args */
    	if (cmd[x_in] == ' ') /* || cmd[x_in] == '\0') */ {
    		contxt = 's'; /* TODO '\ ' which is not separator */
    	} else if (cmd[x_in] == ';'
    			|| cmd[x_in] == '|'
				|| cmd[x_in] == '&'
				   ) {
    		contxt = 'n'; conold = 's'; argc = 0;
    	} else {
    		contxt = 'a';
    	}
    	if (conold != contxt) {
        	/* context changed just now */
    		if (contxt == 's') {
    			dir_x = dir_end = base_x = 0;
    			s_dir[dir_x] = s_base[base_x] = '\0';
    			if (conold == 'a') argc++;
    			//goto the_moon;
    		}
    		if (contxt == 'a') {
				dir_x = base_x = 0;
			}
    	}
    	if (contxt == 's') goto the_moon;
		if (contxt == 'a') {
			if (cmd[x_in] == '/') {
				base_x = 0;
				s_dir[dir_x++] = cmd[x_in];
				dir_end = dir_x;
				//s_base[base_x] = '\0';
			} else {
				s_dir[dir_x++] = cmd[x_in];
				s_base[base_x++] = cmd[x_in];
			}
			/* terminate resulting substrings */
			s_dir[dir_x] = s_base[base_x] = '\0';
		}
		the_moon:
		conold = contxt; /* remember current context for next hope */
    }
    if (dir_end != 0) s_dir[dir_end] = '\0'; /* terminate if differ */

    /* выбрать тип подстановки - текущий каталог, команда, параметр команды */
    if (argc == 0) {
    	if (strcmp(s_dir, s_base) == 0) {
        	/* команда из PATH, не содержит '/'; base и dir одинаковы */
    		s_mode = path_cmd;
    	} else {
            /* команда из текущего каталога или полное имя команды */
    		s_mode = command;
    	}
    } else if (argc > 0) {
    	s_mode = file_dir; /* относительный путь к файлу */
        if (strncmp(&cmd[0], "cd ", 3) == 0) {
        	/* директория для cd */
        	s_mode = cd_dir;
        } else {
        	if (s_dir[0] == '-') {
        		if (s_dir[1] == '-')
        			s_mode = flaglong;
        		else
        			s_mode = flag;
        	}
        }
    }

    x_in = strlen(cmd);
    if (x_in < *curpos) *curpos = x_in; /* вернуть курсор к концу набираемой строки */

    ok = -2;
    /* найти и выполнить подстановку, если однозначно;
     * иначе сигнал TODO (или показать список)
     * */
    s_ins = "";
    if (contxt == 'a' || (contxt == 's' && argc == 0)) {
    	if ((ok = do_compl(/*s_mode,*/ /*s_ins,*/ s_dir, s_base)) < 0) {
    		bell();
    	} else if (ok == 1) {
			if (sgglist->sl_size == 1) {
				s_ins = sl_sstr(sgglist->sl_last);
				pattsz = strlen(s_base);
				/* skip common part of patt and suggestion */
				s_ins += pattsz;
			}
		} else {
			/* find common part from several suggestions */
			s_ins = sgg_ext(s_base);
			ins_len = strlen(s_ins);
			if (ins_len == 0) {
				bell(); /*TODO visual menu of suggestions there */
			}
		}
    	ins_len = strlen(s_ins);
    	/* insert a completion just found into cmd buffer */
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
				s_ins[ins_len] = c; ins_len += 1; s_ins[ins_len] = '\0';

    		}
			for(x_out = maxpos - ins_len, x_in = maxpos; x_in >= *curpos; x_in--, x_out--)
				cmd[x_in] = cmd[x_out];
			for(x_out=0, x_in=*curpos; x_out < ins_len; x_out++, x_in++)
				cmd[x_in] = s_ins[x_out];
			*curpos += ins_len;
    	}
    }

#ifdef DEBUGS
	sprintf(debugs, "ok=%d'%c%c #%d %d/%d dir=%s' base=%s' <%s> ins=%s'  %4s~",
			ok, conold, contxt, argc, *curpos, maxpos, s_dir, s_base, s_debug[s_mode], s_ins, tstats);
      /*cp_sav();*/ cp_set(/*y0-1*/ -2, 0, ATT); w_str(debugs); er_eol(); /*cp_fet();*/
#endif

  	return ok;
}
