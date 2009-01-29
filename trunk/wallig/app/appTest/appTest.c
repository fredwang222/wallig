#include <stdlib.h>
#include <stdio.h>                          /* I/O Functions */
#include <string.h>
#include "dvTimer/dvTimer.h"
#include "libTime/libTimer.h"
#include "libUart/libUart.h"
#include "libFat/ff.h"

static	tPeriodicTimer *pTimer1=NULL,*pTimer2=NULL;
unsigned char ucCpt1=0,ucCpt2=0;
static const unsigned char tucOut[]="hello world\r\n\r";


FATFS tFat;

void vFPrintString( const char *pucString)
{
	cFlibUart_SendBuff((unsigned char*)pucString,strlen((const char*)pucString));

}

void vFPrintFatResult( FRESULT tResult )
{
	switch(tResult )
	{
		case FR_OK:
			iprintf("FR_OK\n");
			break;
		case FR_NOT_READY:
			iprintf("FR_NOT_READY\n");
			break;
		case FR_NO_FILE:
			iprintf("FR_NO_FILE\n");
			break;
		case FR_NO_PATH:
			iprintf("FR_NO_PATH\n");
			break;
		case FR_INVALID_NAME:
			iprintf("FR_INVALID_NAME\n");
			break;
		case FR_INVALID_DRIVE:
			iprintf("FR_INVALID_DRIVE\n");
			break;
		case FR_DENIED:
			iprintf("FR_DENIED\n");
			break;
		case FR_EXIST:
			iprintf("FR_EXIST\n");
			break;
		case FR_RW_ERROR:
			iprintf("FR_RW_ERROR\n");
			break;
		case FR_WRITE_PROTECTED:
			iprintf("FR_WRITE_PROTECTED\n");
			break;
		case FR_NOT_ENABLED:
			iprintf("FR_NOT_ENABLED\n");
			break;
		case FR_NO_FILESYSTEM:
			iprintf("FR_NO_FILESYSTEM\n");
			break;
		case FR_INVALID_OBJECT:
			iprintf("FR_INVALID_OBJECT\n");
			break;
		case FR_MKFS_ABORTED:
			iprintf("FR_MKFS_ABORTED\n");
			break;		/* 13 */
		
	}

}

unsigned char tucBufferLecture[512];
void vFdvPWM_init( void );

void vFappTest_init( void )
{
	FRESULT tResult;
	DWORD nClust;
	FATFS *ptFat = &tFat;
	FIL tFIchier;
	FILINFO finfo;
    DIR dirs;
	UINT uiOctetLus; 
	UINT uicount=0;
	unsigned long uliTemp=0;

	//vFdvPWM_init();
	iprintf("****************Start******************\n");
	iprintf("Test FAT\n");
	iprintf("mounts : ");
	tResult = f_mount( 0 , &tFat );
	vFPrintFatResult(tResult );
	tResult = f_getfree( "/" , &nClust , &ptFat );
	vFPrintFatResult(tResult );
	iprintf("Free clust: %d\n",nClust);
	iprintf("list dir\n");
		if (f_opendir(&dirs, "/") == FR_OK) 
		{
			 while ((f_readdir(&dirs, &finfo) == FR_OK) && finfo.fname[0]) 
			 {
				 iprintf("%s\n", &finfo.fname[0]);
			 }
		}
		else
			iprintf("Error opening root dir\n");
	iprintf("open file ff006.zip \n");
	//f_open(&tFIchier , "/toto/titi.txt",FA_CREATE_NEW);
	tResult = f_open(&tFIchier , "/FF006.ZIP",FA_READ );
	if( tResult == FR_OK)
	{
		uliTemp = timeval;
		do
		{
			uicount++;
			tResult = f_read(&tFIchier ,tucBufferLecture , 512, &uiOctetLus);
		}
		while(uiOctetLus == 512 && tResult == FR_OK);
		iprintf("Nombrede lectures: %d \n",uicount);
	}
	else
		iprintf("Erreur opening: ff006.zip \n");
	vFPrintFatResult(tResult );
	iprintf("Temspde lecture: %dms\n",timeval-uliTemp);
	
	/*vFPrintFatResult(tResult );
	iprintf("Close file titi.txt\n");
	f_close(&tFIchier);
	vFPrintFatResult(tResult );
	iprintf("list dir\n");
	if (f_opendir(&dirs, "/") == FR_OK) 
	{
		 while ((f_readdir(&dirs, &finfo) == FR_OK) && finfo.fname[0]) 
		 {
			 iprintf("%s\n", &finfo.fname[0]);
		 }
	}
	else
		iprintf("Error opening root dir\n");*/

	iprintf("*****************End*******************\n");
	while(1);
	pTimer1 = ptFlibTimer_NewPeriodicTimer(1000);
	pTimer2 = ptFlibTimer_NewPeriodicTimer(2000);
	cFlibUart_SendBuff((unsigned char*)tucOut,sizeof(tucOut));	
}

void vFappTest( void )
{
	unsigned char ucIn;
	unsigned char tBuff[5];
	dvTimerwait(133);
	if( vFlibTimer_GetCount(pTimer1) )
	{
		ucCpt1++;
		iprintf("Timer1 tick: %d\n",ucCpt1);
	}
	if( ucCpt2  < 5 && vFlibTimer_GetCount(pTimer2) )
	{
		ucCpt2++;
		if( ucCpt2 < 5)
			iprintf("Timer2 tick: %d\n",ucCpt2);
		else
		{
			if( ucCpt2 == 5 )
				vFlibTimer_DeletPeriodicTimer( pTimer2 );
		}
	}
	if (cFlibUart_GetRXByte(&ucIn))
	{
		tBuff[0]='c';
		tBuff[1]=' ';
		tBuff[2]=ucIn;
		tBuff[3]='\r';
		tBuff[4]='\n';
		cFlibUart_SendBuff(tBuff,5);
	}
}
