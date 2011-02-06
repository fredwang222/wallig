/**
	\file DRV_Uart_Linux_test.c

	\author Gwendal Le Gall

	\date 02/02/2010
*/
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <DRV_Uart.h>

void *print_message_function( void *ptr );

int main()
{
	DRV_Uart_Cfg tUart0Settings;
	DRV_Uart_Handle  hUart;
	DRV_Uart_Error eError;
	int iNbCharIn;
	unsigned char tucRXBuffer[256];

	DRV_Uart_Init( );
	tUart0Settings.RXCallBack=NULL;
	tUart0Settings.eBaudRate = BR115200;
	tUart0Settings.cEndOfBuff='\r';
	tUart0Settings.eParity = PARNone;
	tUart0Settings.eSLIPModeEnable=0;
	tUart0Settings.eStopBit = SB_1;
	tUart0Settings.iRXNbChar = 0;

	eError = DRV_Uart_Open("/dev/ttyUSB0",&hUart,&tUart0Settings);
	if( eError != No_Error )
	{
		printf("Error while opening device:%d\n",eError);
	}
	else
	{
		printf("Open Ok\n");
		eError = DRV_Uart_Send(hUart,(unsigned char *) "Hello World\n",sizeof("HelloWorld\n"));
		if( eError != No_Error )
		{
			printf("Error while sending string device:%d\n",eError);
		}
		while( 1 )
		{
			usleep(1000*100);
			if(DRV_Uart_RXDataReceived(hUart))
			{
				iNbCharIn=sizeof(tucRXBuffer);
				DRV_Uart_Receive(hUart , tucRXBuffer , &iNbCharIn);
				tucRXBuffer[iNbCharIn]=0;
				printf("RX %d chars: %s\n",iNbCharIn,tucRXBuffer);
				if(tucRXBuffer[0]=='q')
				{
					printf("Quit\n");
					break;
				}
			}

		}
		DRV_Uart_Close(hUart);
	}

	DRV_Uart_Terminate( );
    exit(0);
}
