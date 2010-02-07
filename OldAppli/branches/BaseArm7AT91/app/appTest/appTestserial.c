#include <stdlib.h>
#include <stdio.h>                          /* I/O Functions */
#include <string.h>
#include "dv/dvTimer/dvTimer.h"
#include "lib/libTime/libTimer.h"
#include "lib/libUart/libUart.h"
#include "lib/libFat/ff.h"


void vFPrintString( const char *pucString)
{
	cFlibUart_SendBuff((unsigned char*)pucString,strlen((const char*)pucString));

}

unsigned char tucBufferLecture[512];


void vFappTest_init( void )
{

}

void vFappTest( void )
{

}
