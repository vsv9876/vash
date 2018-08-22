/*
 * slist.h
 *
 *  Created on: 19 aug. 2018 Ç.
 *      Author: vsv
 */

#ifndef VASH_SLIST_H_
#define VASH_SLIST_H_

typedef struct {
	char *sl_prev;	/* pointer to linked element */
	size_t ssize;	/* string size stored, index of trailing zero */
} SLIST_PTR;

typedef struct {
	void *sl_prev;	/* pointer to prev element */
	size_t ssize;	/* string size stored, index of trailing zero */
	char sstr[10]; 	/* starting point of string data, declared size does not matter */
} SLIST;

typedef struct {
	SLIST *sl_last;
	SLIST *sl_first;
	size_t	sl_size;
} SLIST_HEAD;

extern char *sl_sstr();
extern SLIST *sl_add();
extern SLIST_HEAD *sl_init();
extern SLIST *sl_prev();
extern int sl_size();
extern SLIST_HEAD *sl_free();

#endif /* VASH_SLIST_H_ */
