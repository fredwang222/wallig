/******************************************************************************/
/*                                                                            */
/*  HELLO.C:  Hello World Example                                             */
/*                                                                            */
/******************************************************************************/
/*  ported and extended for arm-elf-gcc / WinARM by                           */
/*  Martin Thomas, KL, .de <eversmith@heizung-thomas.de>                      */
/*  modifications Copyright Martin Thomas 2005                                */
/*                                                                            */
/*  Based on file that has been a part of the uVision/ARM development         */
/*  tools, Copyright KEIL ELEKTRONIK GmbH 2002-2004                           */
/******************************************************************************/

/* see file Abstract.txt for more info on the gcc-port */

#include <stdio.h>                          /* I/O Functions */
#include "sys/sysManager.h"
#include "dv/dvManager.h"
#include "lib/libManager.h"
#include "app/appManager.h"


/*** Main Program ***/

int main (void) 
{
	
	vFsysManager_Init();
	vFdvManager_Init();
	vFlibManager_Init();
	vFappManager_Init();
	
	while (1) 
	{                              /* Loop forever */
		vFappManager_Main();
		
	} // while
	
}
