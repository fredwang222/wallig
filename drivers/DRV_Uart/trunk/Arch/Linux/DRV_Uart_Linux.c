/**
	\file DRV_Uart_Linux.c

	\author Gwendal Le Gall

	\date 20/02/2010

	\section UartLinux Linux implementation of the Uart driver

	It use termios for configuration\n
	The reception is done with a thread

*/
 #include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/signal.h>
#include <pthread.h>
#include <string.h>
#include <DRV_Uart.h>
#include "Inc/DRV_Uart_private.h"

/**************************************************************
                                        Local variables
***************************************************************/

typedef struct
{
	int fd,c, res;
    struct termios oldtio,newtio;
    struct sigaction saio;           /* definition of signal action */
    pthread_t RXThread;
    pthread_mutex_t RXmutex;
} DRV_UART_ARCH_Data;

/**************************************************************
                 private Functions
***************************************************************/
void *DRV_Uart_RX_Thread( void * arg );

void DRV_Uart_SafeEnter( DRV_Uart_Devicedata *pUart )
{
	DRV_UART_ARCH_Data *pData = (DRV_UART_ARCH_Data*)pUart->pArchData;

  pthread_mutex_lock( &pData->RXmutex );
}
void DRV_Uart_SafeLeave( DRV_Uart_Devicedata *pUart )
{
	DRV_UART_ARCH_Data *pData = (DRV_UART_ARCH_Data*)pUart->pArchData;

  pthread_mutex_unlock( &pData->RXmutex );

}

DRV_Uart_Error DRV_Uart_ArchInit(void )
{

  return No_Error;
}

DRV_Uart_Error DRV_Uart_ArchTerminate(void )
{


    return No_Error;
}

DRV_Uart_Error DRV_UART_ArchOpen( DRV_Uart_Devicedata *pUart )
{
	DRV_Uart_Error tError;
	DRV_UART_ARCH_Data *pData;

	pData = (DRV_UART_ARCH_Data *) malloc( sizeof(DRV_UART_ARCH_Data));
	if(pData==NULL)
		return Init_Error;

	/*
	Open device for reading and writing and not as controlling tty
	because we don't want to get killed if line noise sends CTRL-C.
	*/
	pData->fd = open(pUart->pcDeviceName , O_RDWR | O_NOCTTY );
	if (pData->fd <0)
	{
		free(pData);
		return Init_Error;
	}
	pUart->pArchData = (void*)pData;
	tcgetattr(pData->fd,&pData->oldtio); /* save current serial port settings */
	memset(&pData->newtio, 0 , sizeof(pData->newtio)); /* clear struct for new port settings */

	pData->newtio.c_cflag = CS8| CLOCAL ;
	//Bauderate
	switch( pUart->cfg.eBaudRate)
	{
	  case BR4800:
		  pData->newtio.c_cflag |= B4800;
		  break;
	  case BR9600:
		  pData->newtio.c_cflag |= B9600;
		  break;
	  case BR115200:
		  pData->newtio.c_cflag |= B115200;
		  break;
	  default:
		  tError=  Bad_Param;
	}
	//Parity
	switch( pUart->cfg.eParity )
	{
	  case PARNone:
		  break;
	  case PAREven:
		  pData->newtio.c_cflag |= PARENB;
		  break;
	  case PAROdd:
		  pData->newtio.c_cflag |= PARENB | PARODD;
		  break;
	  default:
		  tError=  Bad_Param;
	}
	 pData->newtio.c_oflag = 0;
	 pData->newtio.c_lflag = 0;
	 pData->newtio.c_cc[VMIN]= pUart->cfg.iRXNbChar;
	 pData->newtio.c_cc[VTIME]=0;
	tcflush( pData->fd, TCIFLUSH);
	tcsetattr( pData->fd,TCSANOW,&pData->newtio);
	//Termios configuration done
	if(tError == No_Error)
	{
		//Create the RX thread
		if( pthread_create( &pData->RXThread, NULL, DRV_Uart_RX_Thread, pUart ) < 0 )
			tError=  Init_Error;
		//Init mutex for the safe sections
		pthread_mutex_init (&pData->RXmutex, NULL);
	}
	if(tError != No_Error)
	{ //cleanup allocated an open data in case of error
	  close(pData->fd);
	  free(pData);
	}
	return tError;
}
/*!
 * \brief Rx thread function call the driver call back
 */
void *DRV_Uart_RX_Thread( void * arg )
{
	unsigned char tucBuffIn[255];
	unsigned char *pucBuff=tucBuffIn;
	unsigned int uiRxChars , uiReadChars=0;
	DRV_Uart_Devicedata *pUart = (DRV_Uart_Devicedata*) arg;
	DRV_UART_ARCH_Data *pData =(DRV_UART_ARCH_Data *) pUart->pArchData ;
    //Allow the application to kill the thread
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);

    while(1)
    {
    	uiRxChars = read( pData->fd , pucBuff , sizeof(tucBuffIn)-(uiReadChars) );
    	uiReadChars = DRV_Uart_Private_Callback( pUart , tucBuffIn , uiRxChars );
    	if(uiReadChars)
		{
			//There is still some unread data in the buffer, update the FIFIO
			for( uiRxChars=0;uiRxChars<uiReadChars ; uiRxChars++ )
				tucBuffIn[uiRxChars]=tucBuffIn[uiRxChars+uiReadChars];
		}
    	pucBuff = &tucBuffIn[uiReadChars];
    }

}

DRV_Uart_Error DRV_UART_ArchClose( DRV_Uart_Devicedata *pUart )
{
	void *ret;

	DRV_UART_ARCH_Data *pData =(DRV_UART_ARCH_Data *) pUart->pArchData ;

	//kill the RX thread
    if (pthread_cancel ( pData->RXThread) != 0)
    {
      fprintf (stderr, "pthread_cancel error for thread 1\n");
      return Failed;
    }
     //wait the end of the called thread
    (void)pthread_join (pData->RXThread, &ret);
    //close the device
    close(pData->fd);
    //free the private data
    free(pData);
    pUart->pArchData=NULL;
    return No_Error;
}

DRV_Uart_Error DRV_Uart_ArchSend( DRV_Uart_Devicedata *pUart , unsigned char *pucBuffer , int iLength)
{
	DRV_UART_ARCH_Data *pData =(DRV_UART_ARCH_Data *) pUart->pArchData ;
	int iSendLen;

	iSendLen = write(pData->fd , pucBuffer , iLength);
	pUart->eTxState = TXIdle;
	if(iSendLen!=iLength)
		return RXError;
	else
		return No_Error;
}
