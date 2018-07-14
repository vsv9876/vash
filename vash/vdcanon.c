#include <string.h>

/* взято из текстов интерпретатора csh
 * DEMOS 2.08
 */

#define eq(a, b) (strcmp(a, b) == 0)

/*
 * dcanon - canonicalize the pathname, removing excess ./ and ../ etc.
 *      we are of course assuming that the file system is standardly
 *      constructed (always have ..'s, directories have links)
 */
dcanon(cp)
	char *cp;
{
	register char *p, *sp;
	register char slash;

	if (*cp != '/')
		onintr(1);
	for (p = cp; *p; ) {            /* for each component */
		sp = p;                 /* save slash address */
		while(*++p == '/')      /* flush extra slashes */
			;
		if (p != ++sp)
			strcpy(sp, p);
		p = sp;                 /* save start of component */
		slash = 0;
		while(*++p)             /* find next slash or end of path */
			if (*p == '/') {
				slash = 1;
				*p = 0;
				break;
			}
		if (*sp == '\0')        /* if component is null */
			if (--sp == cp) /* if path is one char (i.e. /) */
				break;
			else
				*sp = '\0';
		else if (eq(".", sp)) {
			if (slash) {
				strcpy(sp, ++p);
				p = --sp;
			} else if (--sp != cp)
				*sp = '\0';
		} else if (eq("..", sp)) {
			if (--sp != cp)
				while (*--sp != '/')
					;
			if (slash) {
				strcpy(++sp, ++p);
				p = --sp;
			} else if (cp == sp)
				*++sp = '\0';
			else
				*sp = '\0';
		} else if (slash)
			*p = '/';
	}
}

prefix(sub, str)
	register char *sub, *str;
{

	for (;;) {
		if (*sub == 0)
			return (1);
		if (*str == 0)
			return (0);
		if (*sub++ != *str++)
			return (0);
	}
}
