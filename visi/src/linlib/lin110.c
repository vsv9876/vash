/*
**      +----------+    БИБЛИОТЕКА ВВОДА-ВЫВОДА
**     (c) linlib  !    ДЛЯ АЛФАВИТНО-ЦИФРОВЫХ
**      +----------+    ВИДЕОТЕРМИНАЛОВ
**/


/*
 * library replacements
 */

#include <stdlib.h>
#include "line.h"
#include "linebp.h"

char *strcp(dest, src)
 char *dest;
 char *src;
{
	 size_t i;

	 for(i = 0; src[i] != '\0'; i++) {
		 dest[i] = src[i];
	 }
	 dest[i] = '\0';

	 return dest;
}

char *strncp(dest, src, size)
 char *dest;
 char *src;
 size_t size;
{
	 size_t i;

	 for(i = 0; i < size && src[i] != '\0'; i++)
		 dest[i] = src[i];
	 for(;i < size; i++) /* do not terminate oversized destination buffer, similar to strncpy() */
		 dest[i] = '\0';

	 return dest;
}
