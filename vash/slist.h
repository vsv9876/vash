/*
 * VASH - visual assistant for shell
 * Copyright (c) 1990-2025 Sergey Vovk <vsv>
 *
 * License: GPL and/or MIT,
 * see files COPYING and LICENSE
 */

/*
 * slist.h
 *
 *  Created on: 19 aug. 2018 г.
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
	wchar_t sstr[10]; 	/* starting point of string data, declared size does not matter */
} SLIST;

typedef struct {
	SLIST *sl_last;
	SLIST *sl_first;
	size_t	sl_size;
} SLIST_HEAD;

extern int sl_chk(SLIST_HEAD *);
extern wchar_t *sl_sstr(SLIST *);
extern SLIST *sl_add(SLIST_HEAD *, wchar_t *);
extern SLIST_HEAD *sl_init();
extern SLIST *sl_prev(SLIST *);
extern int sl_size(SLIST_HEAD *);
extern SLIST_HEAD *sl_free(SLIST_HEAD *);
extern int sl_find(SLIST_HEAD *, wchar_t *);

#endif /* VASH_SLIST_H_ */
