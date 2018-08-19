#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "line.h"
#include "assist.h"
#include "slist.h"

#define DEBUGS

#define BUFSMAX 4000

/* suggestion mode */
static enum sugg_mode { path_cmd, command, dir_cd, dirfile, flag, flaglong } ;
static char *s_debug[] = { "path_cmd", "command", "dir_cd", "dirfile", "flag", "flaglong", (char *)0 };

SLIST_HEAD *sgglist;

/* returns elemens count or -1 if error, fill sugg.string if found exact one only */
int sh_sugg(from, patt, insp)
char *from;
char *patt;
char *insp;
{
	FILE *pipe;
	char *filestr; /*[BUFSMAX+1];*/
	char *inss; /*[MAXLICO+1]; /**/
	int count;
	size_t sz, pattsz;
	char *s;
	int c, ok;

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
			s = fgets(s = filestr, BUFSMAX, pipe);
			if (s != (char *)0 && strncmp(patt, filestr, strlen(patt)) == 0) {
				count++;
				/*strncpy(inss, s + pattsz, MAXLICO);*/
				/*skip trailing LF */
				sz = strlen(s);
				if (sz > 0 && s[sz-1] == '\n') {
						s[sz-1] = '\0';
				}
				sl_add(sgglist, s);
			}
		}
		free(filestr);
		clearerr(pipe);
		pclose(pipe);
	} else {
		ok = -1; goto ret;
	}
	if (count == 1) {
		 /*workaround, TODO it in right place */
		s = sgglist->last->sstr;

		strncpy(insp, &s[pattsz], MAXLICO); /* skip common part of patt and suggestion */
	}
ret:
	if (inss != NULL) free(inss);
	return count;
}

/*
 * завершение ввода, в зависимости от режима вызова
 */
int do_compl(s_mode, insp, dirp, basep)
enum sugg_mode s_mode;
char *insp; /* pointer to suggestion string to be inserted */
char *dirp; /* pointer to dir */
char *basep; /*pointer to base */
{
	int res = 0;  /*	result: count of suggestions variants, -1 if impossible on errors */
	char *dir;
	char *bufs; /*[BUFSMAX+1]*/;

	bufs = malloc((size_t)(BUFSMAX + 1));
	sgglist = sl_init();

	if (bufs == NULL) return -1;

	*insp = '\0'; /*	default: nothing to suggest */
	if ((s_mode == dirfile)) {
#ifdef DEBUGS
		if (strcmp("dura", basep) == 0) {
			strcpy(insp, " ");
			res = 1; goto ret;
		}
#endif
		if (strcmp(dirp, basep) == 0) {
			dir = ".";
		} else {
			dir = dirp;
		}
		sprintf(bufs, "ls -f1 %s", dir);
		res = sh_sugg(bufs, basep, insp);
	}
ret:
	if(sgglist != NULL) sl_free(&sgglist);
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
    static char s_ins[MAXLICO+1] = "";		/* suggestion string to be inserted */
    		char s_dir[MAXLICO+1]; /* база (например, путь до каталога) */
    		char s_base[MAXLICO+1]; /* хвост (например, префикс имени в каталоге) */
    int argc, x_in, x_out, ins_len /*, dir_len, base_len;*/;
    int base_x, dir_x, dir_end;
    char contxt, conold;   /* cmd scaner context: 's'eparator, 'a'rg, 'n'ull, 'i'ni */
    enum sugg_mode s_mode;
    int ok;
    char **insp;

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
    	s_mode = dirfile; /* относительный путь к файлу */
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
    if (contxt == 'a') {
    	if ((ok = do_compl(s_mode, s_ins, s_dir, s_base)) < 0) {
    		bell();
    	} else if (ok == 1) {
			ins_len = strlen(s_ins);
			/*insert into buffer a completion */
			for(x_out = maxpos - ins_len, x_in = maxpos; x_in >= *curpos; x_in--, x_out--)
				cmd[x_in] = cmd[x_out];
			for(x_out=0, x_in=*curpos; x_out < ins_len; x_out++, x_in++)
				cmd[x_in] = s_ins[x_out];

			*curpos += ins_len;
		} else {
			 bell(); /*TODO visual menu of suggestions */
		}
    }
#ifdef DEBUGS
	sprintf(debugs, "ok=%d; '%c'%c' #%d %d/%d  dir='%s' base='%s' <%s> ins='%s'   ~",
			ok, conold, contxt, argc, *curpos, maxpos, s_dir, s_base, s_debug[s_mode], s_ins);
    /*cp_sav();*/ cp_set(0, 11, ATT); w_str(debugs); /*cp_fet();*/
#endif

}
