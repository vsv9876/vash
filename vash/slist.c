/*
 * slist.c
 *
 * library for list of string,
 * based on malloc/free
 *
 *  Created on: 19 aug. 2018 Ç.
 *      Author: vsv
 */

#include <stdlib.h>
#include "slist.h"

/*
 * append elements after last one
 * returns pointer to new element
 */

SLIST *sl_add(head, str)
SLIST_HEAD *head;
char  *str;
{
	SLIST *ptr;
	SLIST *prev;
	size_t ssize;
	char  *s;

	ssize = (size_t)strlen(str);

	s   = malloc(ssize+1);
	ptr = malloc(sizeof(SLIST_PTR)); /* + ssize);*/
	if (ptr != NULL) {
		ptr->ssize = ssize;
		ptr->sstr = s;
		strncpy(ptr->sstr, str, ssize);
		prev = NULL;
		if (head->first == NULL) {
			head->first = ptr; /*special case on first element */
		} else {
			prev = head->last;
			ptr->sprev = prev->sprev;
		}
		head->last = ptr;
		head->size += 1;
	}
	return ptr;
}

/*
 * init list,
 * returns pointer to first element
 */

SLIST_HEAD *sl_init()
/*char *name; /* first element store name of the list as a string */
{
	SLIST_HEAD *head;
	SLIST *ptr;

	head = malloc(sizeof(SLIST_HEAD));
	if (ptr != NULL) {
		head->last = NULL;
		head->first = NULL;
		head->size = (size_t)0;
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
	return (curr->sprev);
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
	return head->size;
}

int sl_free(headp)
SLIST_HEAD **headp;
{
	SLIST *p;
	SLIST *prev;
	SLIST_HEAD *head;
	size_t i, count;

	head = *headp;
	i = count = head->size;
	/* from last element to head */
	for (p = head->last; i > 0 && p->sprev != NULL; i--) {
		prev = sl_prev(p);
		free(p->sstr);
		free(p);
		p = prev;
	}
	free (head); /* head itself */
	*headp = NULL; /* clear pointer to head */

	return count;
}
