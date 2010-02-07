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
#include "sysManager.h"
#include "dvManager.h"
#include "libManager.h"
#include "appManager.h"


/*** Main Program ***/

int main (void) 
{
	
	vFsysManager_Init();
	vFdvManager_Init();
	vFlibManager_Init();
	vFappManager_Init();
	
	//unsigned short q = 4;
	//iprintf("Hello from the WinARM example!  (1 2 %i %i)\r\n", 3, q);
	
	while (1) 
	{                              /* Loop forever */
		vFappManager_Main();
		
	} // while
	
}
