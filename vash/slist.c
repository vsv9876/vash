/*
 * slist.c
 *
 * library for list of string,
 * based on malloc/free
 *
 *  Created on: 19 aug. 2018 г.
 *      Author: vsv
 *
 */

#include <wchar.h>
#include <stdlib.h>
#include <string.h>
#include "slist.h"

/*
 * return ptr to string stored in this list element
 */
wchar_t *sl_sstr(slist)
SLIST *slist;
{
	wchar_t *s;

	/*s += sizeof(SLIST_PTR);*/
	s = slist->sstr;
	return s;
}

/*
 * append elements after last one
 * returns pointer to new element
 */
SLIST *sl_add(head, str)
SLIST_HEAD *head;
wchar_t  *str;
{
	SLIST *ptr;
	SLIST *prev;
	size_t ssize;
	wchar_t  *s;

	ssize = wcslen(str);

	ptr   = malloc(sizeof(SLIST_PTR) + (sizeof(wchar_t) * (ssize + 2)));

	if (ptr != NULL) {
		ptr->ssize = ssize;
		s = sl_sstr(ptr);
		wcsncpy(s, str, ssize);
		s[ssize] = '\0'; /**/
		prev = NULL;
		if (head->sl_first == NULL) {
			head->sl_first = ptr; /*special case on first element */
		} else {
			prev = head->sl_last;
			ptr->sl_prev = prev;/*->sl_prev;*/
		}
		head->sl_last = ptr;
		head->sl_size += 1;
	}
	return ptr;
}

/*
 * init list,
 * returns pointer to first element
 */
SLIST_HEAD *sl_init()
{
	SLIST_HEAD *head;
	SLIST *ptr;

	head = malloc(sizeof(SLIST_HEAD));
	if (head != NULL) {
		head->sl_last = NULL;
		head->sl_first = NULL;
		head->sl_size = (size_t)0;
	}
	return head;
}

/*
 * iterator,
 *
 * returns pointer to next element, NULL on end
 */
SLIST *sl_prev(curr)
SLIST *curr;
{
	return (curr->sl_prev);
}

/*static char last_defined = slist_init("");
*/

int sl_size(head)
SLIST_HEAD *head;
{
	SLIST *p;
	SLIST *prev;
	int count;

/*
	count = 0;
	 from last element to head
	for (p = head->last; p != head; ) {
		count++;
		prev = sl_prev(p);
		free(p);
		p = prev;
	}
*/
	return head->sl_size;
}

/*
 * return non-zero if pattern already stored
 */
int sl_find(head, patt)
SLIST_HEAD *head;
wchar_t *patt;
{
	SLIST *p;
	SLIST *prev;
	/*SLIST_HEAD *head;*/
	size_t i, count;

	/*head = headp;*/
	i = count = head->sl_size;
	/* from last element to head */
	for (p = head->sl_last; i > 0 && p->sl_prev != NULL; i--) {
		prev = sl_prev(p);
		/*free(p->sstr);*/
		if (wcscmp(sl_sstr(p), patt) == 0)
			return 1;
		p = prev;
	}
	return 0;
}

static int chk(s)
wchar_t *s;
{
	int sum;
	sum = 0;
	while( *s != L'\0') {
		sum = sum + *s++;
	}
	return sum;
}

int sl_chk(head)
SLIST_HEAD *head;
{
	SLIST *p;
	SLIST *prev;
	size_t n, i, count;
	int sum, schk;

	sum = n = count = head->sl_size;
	/*return sum;*/
	/* from last element to head */
	for (p = head->sl_last, i = 0;
			i < n && sl_prev(p) != NULL;
			i++) {
		schk = chk(sl_sstr(p));
		sum += schk;
		prev = sl_prev(p);
		p = prev;
	}

	return sum;
}

SLIST_HEAD *sl_free(head)
SLIST_HEAD *head;
{
	SLIST *p;
	SLIST *prev;
	size_t i, count;

	i = count = head->sl_size;
	/* from last element to head */
	for (p = head->sl_last; i > 0 && p->sl_prev != NULL; i--) {
		prev = sl_prev(p);
		/*free(p->sstr);*/
		free(p);
		p = prev;
	}
	free (head); /* head itself */
	/*headp = NULL; /* clear pointer to head */

	return NULL;
}
