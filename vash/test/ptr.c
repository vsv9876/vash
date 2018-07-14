/*
 * тест на сравнение указателей
 */

char *a[] = {"aaa", "bbb", "ccc"};

main()
{
    unsigned i;
    for (i = 0; i < 3; i++)
	printf("0x%07lx '%s'\n", a[i], a[i]);
}
