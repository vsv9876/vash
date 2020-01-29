/*
 * by vsv@st.simbirsk.su
 *
 * проверка принадлежности пользователя к группе
 *
 * используется getgroups(3p)
 */

#ifdef MGROUPS

/* здесь только вариант для систем, поддерживающих POSIX или BSD */

#include <grp.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <limits.h>

/* макс. количество запрашиваемых идентификаторов группы */
#ifdef NGROUPS_MAX
#define MAXGID_ASH NGROUPS_MAX
#else
#define MAXGID_ASH 17
#endif

#ifdef Fbsd
#define __386BSD__
#endif

#ifdef __386BSD__
int   gid;
int   gidset[MAXGID_ASH];  /* кКазлы, зачем было надо, @#%$ */
#else
gid_t gid;
gid_t gidset[MAXGID_ASH];  /* так должно быть */
#endif

int
gidchk(fgid)
int fgid;
{
/*  int group_ok;       /* флаг успешного совпадения группы */
    int maxgid;         /* то, что возвращает getgroups() */
/*  int rc;             /* return code */
    int g;

    maxgid = getgroups(MAXGID_ASH, gidset);

/* это место пригодится для проверки, пока оставлено как было в ssx.c
    if (maxgid < 0) {
	perror("ERR: NGROUPS_MAX limit exsided\n");
	exit(-1);
    }
    else {
*/
	/* в общем случае проверяем по списку групп */
	for (g = 0; g < maxgid; g++) {
	    if (gidset[g] == fgid)
		return(1);
	}
/*
    }
*/
    return(0);
}

#else

int gidchk(fgid)
int fgid;
{
    if (fgid == getgid()) return(1);
    else                  return(0);
}
#endif /* MGROUPS */
