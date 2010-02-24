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
/** @ingroup DRV_Uart_Private_Arch_grp
 *
 */
typedef struct
{
	int fd,c;
    struct termios oldtio,newtio; //!< Termios settings
    struct sigaction saio;        //!< definition of signal action
    pthread_t RXThread;
    pthread_mutex_t RXmutex;
    pthread_mutex_t TXmutex;
} DRV_UART_ARCH_Data;

/**************************************************************
                 private Functions
***************************************************************/
/*!
  * @ingroup DRV_Uart_Private_Arch_grp
 * \fn static void *DRV_Uart_RX_Thread( void * arg )
 * \brief Rx thread function which call the private driver call back
  */
static void *DRV_Uart_RX_Thread( void * arg );

void DRV_Uart_Arch_RxSafeEnter( DRV_Uart_Devicedata *pUart )
{
	DRV_UART_ARCH_Data *pData = (DRV_UART_ARCH_Data*)pUart->pArchData;

  pthread_mutex_lock( &pData->RXmutex );
}
void DRV_Uart_Arch_RxSafeLeave( DRV_Uart_Devicedata *pUart )
{
	DRV_UART_ARCH_Data *pData = (DRV_UART_ARCH_Data*)pUart->pArchData;

  pthread_mutex_unlock( &pData->RXmutex );

}

void DRV_Uart_Arch_TxSafeEnter( DRV_Uart_Devicedata *pUart )
{
	DRV_UART_ARCH_Data *pData = (DRV_UART_ARCH_Data*)pUart->pArchData;

  pthread_mutex_lock( &pData->TXmutex );
}
void DRV_Uart_Arch_TxSafeLeave( DRV_Uart_Devicedata *pUart )
{
	DRV_UART_ARCH_Data *pData = (DRV_UART_ARCH_Data*)pUart->pArchData;

  pthread_mutex_unlock( &pData->TXmutex );

}

DRV_Uart_Error DRV_Uart_ArchInit(void )
{

  return UART_No_Error;
}

DRV_Uart_Error DRV_Uart_ArchTerminate(void )
{


    return UART_No_Error;
}

DRV_Uart_Error DRV_UART_ArchOpen( DRV_Uart_Devicedata *pUart )
{
	DRV_Uart_Error tError = UART_No_Error;
	DRV_UART_ARCH_Data *pData;

	pData = (DRV_UART_ARCH_Data *) malloc( sizeof(DRV_UART_ARCH_Data));
	if(pData==NULL)
		return UART_Init_Error;

	/*
	Open device for reading and writing and not as controlling tty
	because we don't want to get killed if line noise sends CTRL-C.
	*/
	pData->fd = open(pUart->pcDeviceName , O_RDWR | O_NOCTTY );
	if (pData->fd <0)
	{
		free(pData);
		return UART_Init_Error;
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
		  tError=  UART_Bad_Param;
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
		  tError=  UART_Bad_Param;
	}
	 pData->newtio.c_oflag = 0;
	 pData->newtio.c_lflag = 0;
	 pData->newtio.c_cc[VMIN]= 1; //pUart->cfg.iRXNbChar;
	 pData->newtio.c_cc[VTIME]=0;
	tcflush( pData->fd, TCIFLUSH);
	if( tcsetattr( pData->fd,TCSANOW,&pData->newtio) )
		tError=  UART_Init_Error;
	//Termios configuration done
	if(tError == UART_No_Error)
	{
		//Create the RX thread
		if( pthread_create( &pData->RXThread, NULL, DRV_Uart_RX_Thread, pUart ) < 0 )
			tError=  UART_Init_Error;
		//Init mutex for the safe sections
		pthread_mutex_init (&pData->RXmutex, NULL);
		pthread_mutex_init (&pData->TXmutex, NULL);
	}
	if(tError != UART_No_Error)
	{ //cleanup allocated an open data in case of error
	  close(pData->fd);
	  free(pData);
	}
	return tError;
}

static void *DRV_Uart_RX_Thread( void * arg )
{
	unsigned char tucBuffIn[450];
	unsigned char *pucBuff=tucBuffIn;
	unsigned int uiRxChars , uiReadChars=0,uiRemainingChars=0;
	DRV_Uart_Devicedata *pUart = (DRV_Uart_Devicedata*) arg;
	DRV_UART_ARCH_Data *pData =(DRV_UART_ARCH_Data *) pUart->pArchData ;
    //Allow the application to kill the thread
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);

    while(1)
    {
    	uiRxChars = read( pData->fd , pucBuff , sizeof(tucBuffIn)-(uiRemainingChars) );
    	uiRemainingChars+=uiRxChars;
    	uiReadChars = DRV_Uart_Private_Callback( pUart , tucBuffIn , uiRemainingChars );
    	/* Update the remaining number of chars in the buffer*/
    	uiRemainingChars -=uiReadChars;
    	if(uiRemainingChars)
		{
			//update the FIFIO
			for( uiRxChars=0;uiRxChars<uiRemainingChars ; uiRxChars++ )
				tucBuffIn[uiRxChars]=tucBuffIn[uiRxChars+uiReadChars];
		}
    	pucBuff = &tucBuffIn[uiRemainingChars];
    }
    pthread_exit (0);

}

DRV_Uart_Error DRV_UART_ArchClose( DRV_Uart_Devicedata *pUart )
{
	void *ret;

	DRV_UART_ARCH_Data *pData =(DRV_UART_ARCH_Data *) pUart->pArchData ;

	//kill the RX thread
    if (pthread_cancel ( pData->RXThread) != 0)
    {
      fprintf (stderr, "pthread_cancel error for thread 1\n");
      return UART_Failed;
    }
     //wait the end of the called thread
    (void)pthread_join (pData->RXThread, &ret);
    //Restore old settings
    tcsetattr( pData->fd,TCSANOW,&pData->oldtio);
    //close the device
    close(pData->fd);
    //free the private data
    free(pData);
    pUart->pArchData=NULL;
    return UART_No_Error;
}

DRV_Uart_Error DRV_Uart_ArchSend( DRV_Uart_Devicedata *pUart , unsigned char *pucBuffer , int iLength)
{
	DRV_UART_ARCH_Data *pData =(DRV_UART_ARCH_Data *) pUart->pArchData ;
	int iSendLen;

	iSendLen = write(pData->fd , pucBuffer , iLength);
	pUart->eTxState = TXIdle;
	if(iSendLen!=iLength)
		return UART_RXError;
	else
		return UART_No_Error;
}

DRV_Uart_Error  DRV_Uart_ArchRXFlush( DRV_Uart_Devicedata *pUart)
{
	DRV_UART_ARCH_Data *pData =(DRV_UART_ARCH_Data *) pUart->pArchData ;

	tcflush( pData->fd, TCIFLUSH);

	return UART_No_Error;
}

DRV_Uart_Error DRV_Uart_ArchTXFlush( DRV_Uart_Devicedata *pUart)
{
	DRV_UART_ARCH_Data *pData =(DRV_UART_ARCH_Data *) pUart->pArchData ;

	tcflush( pData->fd, TCOFLUSH);

	return UART_No_Error;
}
