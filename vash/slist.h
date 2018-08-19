/*
 * slist.h
 *
 *  Created on: 19 aug. 2018 Ç.
 *      Author: vsv
 */

#ifndef VASH_SLIST_H_
#define VASH_SLIST_H_

typedef struct {
	char *sprev;	/* pointer to linked element */
	size_t ssize;	/* string size stored, index of trailing zero */
} SLIST_PTR;

typedef struct {
	void *sprev;	/* pointer to prev element */
	size_t ssize;	/* string size stored, index of trailing zero */
	char *sstr; 	/* pointer, was sstr[] - string bytes stored there, declared size does not matter */
} SLIST;

typedef struct {
	SLIST *last;
	SLIST *first;
	size_t	size;
} SLIST_HEAD;

#endif /* VASH_SLIST_H_ */
