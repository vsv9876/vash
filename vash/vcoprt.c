#include <stdio.h>
#include "line.h"

#ifdef RETRO
static LINE helpk[] = {
{  5,-1,64, 0, LTXT, 0,0,0, "Help:" },
/*NOSTRICT*/
{  8,-1,70, 0, ATT|MID|PAD, 0, cvt_lh, 0, ":HE" },
{ 0 }, };
#endif

/*NOXSTR*/
char    coprts[] =
"1990,1996,2020 (c) Sergey Vovk";
/*YESXSTR*/
