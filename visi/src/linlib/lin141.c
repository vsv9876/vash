/*
 * lin141.c
 *
 *  Created on: 3 апр. 2025 г.
 *      Author: vsv
 *
 *      copy&paste vsignal from lin140.c
 */

#include <unistd.h>
#include <stdio.h>
#include "line.h"

#include <signal.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <stdlib.h>

/* declared in "line.h" -- may be redeclared by user */
void vsignal(ontty)
int ontty;
{
	if (ontty == 0)
		vsig_off();
	else
		vsig_on();
}
