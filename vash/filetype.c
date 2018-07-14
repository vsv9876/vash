#include <sys/types.h>
#include <a.out.h>

#ifdef  SYSV
#define PORTAR 1
#include <string.h>
#endif
#include <ar.h>

#include <ctype.h>
#include <stdio.h>

#ifdef pdp11
#define     BSDmagic
#endif

/*
 * Выяснить тип регулярного файла по
 * содержимому первых 512 байт.
 * Вернуть символ типа непустого файла.
 * 'D'  Data (unknown)  неопознанные данные
 * 'T'  Text (source) текстовый файл
 * 'A'  Archive (ar)  архивный формат
 * 'O'  Object (a.out, ELF)
 */
int
filetype(fname)
char *fname;
{
	int texttype;
	int ftype;
	FILE *fp;
	union ub {
#ifdef  BSDmagic
		struct exec exe;
#endif
#ifdef  SYSV
		AOUTHDR exe;
#endif
		char buf[512];
		short  magic;
	} un;
	register char *s;
	register int count;
	unsigned int c;

	ftype = '-';
	if ((fp = fopen(fname, "r")) == NULL) {
		return (ftype);
	}
	s = un.buf;
	count = fread(s, 1, sizeof(un), fp);
	if (strncmp(un.buf, "From ", 5) == 0)
		/* почтовый ящик */
		ftype = 'M';
#ifdef  BSDmagic
	else if (count >= sizeof(un.exe) && N_BADMAG(un.exe) == 0)
			ftype = 'O';
	else if (count >= sizeof(int)
	       && un.magic == ARMAG)
			ftype = 'A';
#endif
#ifdef  SYSV
	else if (count >= sizeof(un.exe) && ISCOFF(un.exe.magic))
			ftype = 'O';
	else if (count >= SARMAG && strncmp(un.buf, ARMAG, SARMAG) == 0)
			ftype = 'A';
#endif
#ifdef SVR4
	else if (strncmp(un.buf, "\177ELF", 4) == 0)
		/* это, конечно, надо делать умнее - elf_kind(3e) */
		ftype = 'O';
#endif
	else {
		texttype = 1;   /* презумпция текстовости файла */
		while (count) {
			c = s[--count];
#ifdef DURA
			if (iscntrl(c)) {
				if (c >= 0300)  /* KOI-8 CYRILL */
					continue;
#else
			if (c >= 0300)  /* KOI-8 CYRILL */
				continue;
			if (c >= '\0' && c < ' ') {
#endif
				switch(c) {
				case '\n':      /* текстовые символы */
				case '\t':
				case '\f':
				case '\b':
				case '\r':
				case '\007':    /* bell */
				case '\033':    /* escape (for ansi) */
					continue;
				default:
					texttype = 0;
					count = 0;
				}
			}
		}
		if (texttype)
			ftype = 'T';
		else    ftype = 'D';
	}
	fclose(fp);
	return(ftype);
}
