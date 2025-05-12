/*
 * VISI(LINLIB)
 * Copyright (c) 1986-1990 Sergey Vovk and the team of RIAR, Dimitrovgrad, USSR
 * Copyright (c) 2017-2025 Sergey Vovk
 *
 * VISI -- a visual interactive simple interface for non-GUI terminals
 * LINLIB -- library for video terminals
 *
 * This is free software, 
 * please keep applied LICENSE file and copyright notice above
 *
 */

/*
 * LBP - linlib build pages
 * data structures for external build pages
*/

/*
 * link between names from page and program internal names (addresses)
 */
typedef struct {
	const char *in_name; /* name in external desciption */
	void       *in_addr; /* program address */
	} IN_PORTS;

/*
 * header of external description of page
 */
typedef struct {
	int     lh_magic;       /* id of external description */
	int     lh_len;
	char    lh_name[6];
	int     lh_free[16];   /* not used yet */
	linptr_t lh_lines;
	linptr_t lh_heaps;
	linptr_t lh_names;
	} LINE_H;

#define  LH_MAGIC 000477

extern  const char   *v_dir;
extern  const char   *phelp0;         /* file name of common help page */
extern  IN_PORTS in_help[];     /* LINLIB library ports */

extern LINE	   *b_page(const char *, char *, const IN_PORTS *);
