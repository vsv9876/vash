/*
 *      $Header: ajobs.h,v 3.2 90/01/11 10:17:52 vsv Rel $
 *
 *      $Log:	ajobs.h,v $
 * Revision 3.2  90/01/11  10:17:52  vsv
 * קועףיס V32
 * 
 * Revision 3.1  89/08/29  16:21:17  vsv
 * קועףיס LINLIB_3
 * 
 * Revision 3.0  87/12/21  11:42:23  vsv
 * נעוהקבעיפולרמשך קשנץףכ
 * 
 */

/*
 * נןההועצכב בףימטעןממשט תבהבמיך
 */

struct lnk_l {
	 char           siz_l;         /* [ 0] size in bytes           */
	 char           typ_l;         /* [ 1] element type            */
	 unsigned       nxt_l;         /* [ 2] next element            */
	 unsigned       prv_l;         /* [ 4] previous element        */
	 char           nof_l;         /* [ 6] nof code                */
	 char           pof_l;         /* [ 7] pof code                */
	} ;

struct jcb {
	 struct lnk_l   lnk_j;         /*      inter job link          */
	 unsigned  (*job_j)();         /* [10] job start address       */
	 unsigned       srt_j;         /* [12] priority sort value     */
	 char           sem_j;         /* [14] semaphore               */
	 char           pri_j;         /* [15] priority                */
	 unsigned        sp_j;         /* [16] stack pointer           */
	 unsigned       flg_j;         /* [20] flag                    */
	 unsigned       grp_j;         /* [22] group                   */
	 unsigned      *bos_j;         /* [24] pntr to bottom of stack */
	 unsigned      *tos_j;         /* [24] pntr to top    of stack */
	 } ;

typedef struct jcb JCB;

#define JCBSIZ sizeof(JCB)
