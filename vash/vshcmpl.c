#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include "line.h"
#include "assist.h"
#include "slist.h"

#define DEBUGS 1

#define BUFSMAX 4000

extern char *getenv();
/* suggestion mode */
static enum sugg_mode { path_cmd, command, dir_cd, dir_file, flag, flaglong } ;
static char *s_debug[] = { "path_cmd", "command", "dir_cd", "dirfile", "flag", "flaglong", (char *)0 };

SLIST_HEAD *sgglist;

/* returns elemens count or -1 if error, fill sugg.string if found exact one only */
int sh_sugg(from, patt/*, insp*/)
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
	pattsz = strlen(patt);

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
			if (s != NULL && strncmp(patt, filestr, strlen(patt)) == 0) {
				count++;
				/*strncpy(inss, s + pattsz, MAXLICO);*/
				/*skip trailing LF */
				sz = strlen(filestr);
				if (sz > 0 && filestr[sz-1] == '\n') {
						filestr[sz-1] = '\0';
				}
				slist = sl_add(sgglist, filestr);
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
	char  **sggstr; /* массив указателей на строки sgglist*/
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
 * селектор режима завершения ввода (выбор алгоритма подбора вставки)
 * пока поддерживается dirfile
 *
 * заполняет список подходящих вариантов.
 * возвращает количество найденных вариантов,
 */
int do_compl(s_mode, /*insp,*/ dirp, basep)
enum sugg_mode s_mode;
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
	int n, maxnpath;

	tmps = malloc((size_t)(BUFSMAX + 1));
	bufs = malloc((size_t)(BUFSMAX + 1));
/*	sgglist = sl_init();*/

	if (bufs == NULL) return -1;

/*	*insp = '\0'; 	default: nothing to suggest */
	if ((s_mode == dir_file) ||
			s_mode == dir_cd || s_mode == command ) {
#ifdef DEBUGS_DURA
		if (strcmp("dura", basep) == 0) {
			strcpy(insp, " ");
			res = 1; goto ret;
		}
#endif
		if (strcmp(dirp, basep) == 0) {	dir = ".";
		} else {						dir = dirp; }
		sprintf(bufs, "ls -A1 %s", dir);
		res = sh_sugg(bufs, basep);
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
			for(n = 0; n < maxnpath; n++) {
				sprintf(tmps, "ls -A1 %s", pathdir[n]);
				res = sh_sugg(tmps, basep);
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
    char tmps[(MAXLICO * 3) + 3];
    char s_dir[MAXLICO+1]; /* база (например, путь до каталога) */
	char s_base[MAXLICO+1]; /* хвост (например, префикс имени в каталоге) */
    int argc, x_in, x_out, ins_len /*, dir_len, base_len;*/;
    int base_x, dir_x, dir_end;
    char contxt, conold;   /* cmd scaner context: 's'eparator, 'a'rg, 'n'ull, 'i'ni */
    enum sugg_mode s_mode;
    int ok;
    char *s;
    int   c;
    char *s_ins;
    size_t pattsz;
    struct stat statbuf;

    s_dir[0] = s_base[0] = '\0';
    dir_x = dir_end = base_x = 0;
    /* учесть нули за концом команды и поставить курсор перед ними */
    conold = 's'; contxt = 'n';
    argc = 0;
    for (x_in = 0; x_in < *curpos && x_in < maxpos; x_in++) {
    	/* determine current context */
    	/* skip leading spaces and separators between args */
    	if (cmd[x_in] == ' ') /* || cmd[x_in] == '\0') */ {
    		contxt = 's'; /* todo '\ ' which is not separator */
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
    	s_mode = dir_file; /* относительный путь к файлу */
        if (strncmp(&cmd[0], "cd ", 3) == 0) {
        	/* директория для cd */
        	s_mode = dir_cd;
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
    /* найти и выполнить подстановку, если однозначно; иначе сигнал (или показать список TODO)*/
    s_ins = "";
    if (contxt == 'a') {
    	if ((ok = do_compl(s_mode, /*s_ins,*/ s_dir, s_base)) < 0) {
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
    	if (ins_len != 0) {
			/*ins_len = strlen(s_ins); s_ins[ins_len++] = ' '; /* expand one space on success*/
			c = ' '; /* default with space, for dir '/' */
    		if (ok == 1 && (
    				s_mode == path_cmd ||
					s_mode == command ||
					s_mode == dir_file ||
					s_mode == dir_cd)) {
				if (strcmp(s_dir, s_base) == 0) {
					sprintf(tmps, "%s%s", s_base, s_ins);
				} else {
					sprintf(tmps, "%s/%s%s", s_dir, s_base, s_ins);
				}
				stat (tmps, &statbuf);
				if ((statbuf.st_mode & S_IFMT) == S_IFDIR) {
					c = '/';
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
	sprintf(debugs, "ok=%d; '%c'%c' #%d %d/%d  dir='%s' base='%s' <%s> ins='%s'   ~",
			ok, conold, contxt, argc, *curpos, maxpos, s_dir, s_base, s_debug[s_mode], s_ins);
      /*cp_sav();*/ cp_set(y0, 1, ATT); w_str(debugs); er_eol(); /*cp_fet();*/
#endif

  	return ok;
}
