

vfout()
/*
 * указанные пункты подать на вывод
 */
{
	register int i;
	register char *p;

	for ( i = 0; i < itmmax; i++ ) {
		p = itms[i];
		if ( *p == '%')
			printf("%s\n", p+1);
	}
}
