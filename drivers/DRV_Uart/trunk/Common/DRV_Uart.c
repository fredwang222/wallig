/**
	\file DRV_Uart.c

	\author Gwendal Le Gall

	\date 20/02/2010
*/
#include <stdio.h>
#include <string.h>
#include <DRV_Uart.h>
#include "Inc/DRV_Uart_private.h"
/**************************************************************
					Define
***************************************************************/
#define kDRV_SofTimer_MaxDevices 5
/**************************************************************
					Local variables
***************************************************************/
char *tpcUartDeviceName[]={kDRV_Uart_DevcenameInit};
/*!
 * 	Timers Data
 */
static struct
{
  char *tpcName[kDRV_UART_MaxDevices];
  DRV_Uart_Devicedata tsUarts[kDRV_UART_MaxDevices];
} sDRV_Uart_MainData;

/**************************************************************
                 local Functions declaration
***************************************************************/

/**************************************************************
		 Public Functions
***************************************************************/
DRV_Uart_Error DRV_Uart_Init( void )
{

	DRV_Uart_Init();
 	return No_Error;
}

DRV_Uart_Error DRV_Uart_Terminate( void )
{

	DRV_Uart_Terminate();
	return No_Error;
}

DRV_Uart_Error DRV_Uart_Open( const char * pcDeviceName , DRV_Uart_Handle *phDeviceHandle , DRV_Uart_Cfg *ptParam)
{
		DRV_Uart_Error tError = No_Error;
        DRV_Uart_Devicedata *pUart=NULL;
        int iUartIndex;

        for(iUartIndex=0;iUartIndex<kDRV_UART_MaxDevices;iUartIndex++)
        {
        	if( !strcmp(pcDeviceName, sDRV_Uart_MainData.tpcName[iUartIndex] ))
        	{
        		tError = DRV_UART_ArchOpen(pUart);
        		if( tError != No_Error)
        			return tError;
        		if( sDRV_Uart_MainData.tsUarts[iUartIndex].eRxState != RXClosed )
					return AlreadyOpened;
        		pUart = &sDRV_Uart_MainData.tsUarts[iUartIndex];
        		pUart->cfg=*ptParam;
        		pUart->eRxState = RXEmpty;
        		pUart->eTxState = TXIdle;
        		memset(pUart->tucRXBuff,0,kDRV_Uart_RXBuffSize);
        		memset(pUart->tucTXBuff,0,kDRV_Uart_TXBuffSize);
        		break;
        	}
        }
        if( pUart==NULL)
        	tError = Device_Not_Found;
	return tError;
}

DRV_Uart_Error DRV_Uart_Close( DRV_Uart_Handle hDeviceHandle )
{
        DRV_Uart_Devicedata *pUart = (DRV_Uart_Devicedata *) hDeviceHandle;

        if( pUart == NULL )
           return Input_Null;
        DRV_UART_ArchClose(pUart);

	return No_Error;
}

DRV_Uart_Error DRV_Uart_Send( DRV_Uart_Handle hDeviceHandle ,unsigned char *pucBuffer , int iLength)
{
  DRV_Uart_Devicedata *pUart = (DRV_Uart_Devicedata *) hDeviceHandle;

  if( pUart == NULL || pucBuffer == NULL)
     return Input_Null;
  DRV_Uart_SafeEnter(pUart);
  if( pUart->eTxState != TXIdle)
  {
	  DRV_Uart_SafeLeave(pUart);
	  return TXError;
  }
  //update status
  pUart->eTxState = TXBusy;
  //copy user buffer to driver buffer
  if( pUart->cfg.eSLIPModeEnable )
  {
	  DRV_Uart_Slip_Tx(pucBuffer, iLength , pUart->tucRXBuff);
  }
  else
	  memcpy( pUart->tucRXBuff , pucBuffer , iLength );
  DRV_Uart_SafeLeave(pUart);
  return DRV_Uart_ArchSend( pUart , pUart->tucRXBuff  , iLength);
 }

int DRV_Uart_TXBusy( DRV_Uart_Handle hDeviceHandle )
{
	DRV_Uart_Devicedata *pUart = (DRV_Uart_Devicedata *) hDeviceHandle;
	tTxState eState;

	DRV_Uart_SafeEnter(pUart);
	eState = pUart->eTxState;
	DRV_Uart_SafeLeave(pUart);

	if(eState  == TXIdle )
		return 0;
	else
		return 1;
}

int DRV_Uart_RXDataReceived( DRV_Uart_Handle hDeviceHandle )
{
        DRV_Uart_Devicedata *pUart = (DRV_Uart_Devicedata *) hDeviceHandle;
        tRxState eState;

        DRV_Uart_SafeEnter(pUart);
        eState = pUart->eRxState;
        DRV_Uart_SafeLeave(pUart);

        if( eState == RXFull )
        	return 1;
        else
        	return 0;
}

DRV_Uart_Error DRV_Uart_Receive( DRV_Uart_Handle hDeviceHandle , unsigned char *pucBuffer , int *piLength)
{
  DRV_Uart_Devicedata *pUart = (DRV_Uart_Devicedata *) hDeviceHandle;

  if( pUart == NULL || pucBuffer == NULL )
     return Input_Null;


	DRV_Uart_SafeEnter(pUart);
	if( *piLength > pUart->iRxBuffIndex)
	  *piLength = pUart->iRxBuffIndex;
	//Copy data to user buffer
	memcpy(pucBuffer,pUart->tucRXBuff,*piLength);
	pUart->iRxBuffIndex-=*piLength;
	//update status
	if(pUart->iRxBuffIndex)
		pUart->eRxState = RXEmpty;
	DRV_Uart_SafeLeave(pUart);

    return No_Error;
}

DRV_Uart_Error DRV_Uart_RXEnable( DRV_Uart_Handle hDeviceHandle , char cFlag )
{
  DRV_Uart_Devicedata *pUart = (DRV_Uart_Devicedata *) hDeviceHandle;

    if( pUart == NULL )
       return Input_Null;

      return No_Error;
}

DRV_Uart_Error DRV_Uart_TXEnable( DRV_Uart_Handle hDeviceHandle , char cFlag )
{
  DRV_Uart_Devicedata *pUart = (DRV_Uart_Devicedata *) hDeviceHandle;

    if( pUart == NULL )
       return Input_Null;

      return No_Error;
}
DRV_Uart_Error DRV_Uart_RXFlush( DRV_Uart_Handle hDeviceHandle )
{
	DRV_Uart_Devicedata *pUart = (DRV_Uart_Devicedata *) hDeviceHandle;

	if( pUart == NULL )
	   return Input_Null;

	DRV_Uart_SafeEnter(pUart);
	pUart->iRxBuffIndex=0;
	pUart->eRxState = RXEmpty;
	DRV_Uart_SafeLeave(pUart);
	return No_Error;
}

DRV_Uart_Error DRV_Uart_TXFlush( DRV_Uart_Handle hDeviceHandle )
{
	DRV_Uart_Devicedata *pUart = (DRV_Uart_Devicedata *) hDeviceHandle;

	if( pUart == NULL )
	   return Input_Null;

	DRV_Uart_SafeEnter(pUart);
	DRV_Uart_ArchTXFlush(pUart);
	pUart->eTxState = TXIdle;
	DRV_Uart_SafeLeave(pUart);
	return No_Error;
}
/**************************************************************
                 private Functions
***************************************************************/
int DRV_Uart_Private_Callback( DRV_Uart_Devicedata *pUart ,  unsigned char *pucBuffer , int iLength )
{
	int iBufferIndex;
	DRV_Uart_SafeEnter(pUart);
	//Append data to the rx buffer
	for( iBufferIndex=0;iBufferIndex<iLength ; iBufferIndex++ )
	{
		if( pUart->cfg.eSLIPModeEnable )
		{
			pUart->iRxBuffIndex=DRV_Uart_Slip_Rx( &pUart->tSLIP , *(pucBuffer++) , pUart->tucRXBuff ,kDRV_Uart_RXBuffSize);
			if(  pUart->iRxBuffIndex )
			{   //A SLIP packet was received
				break;
			}
		}
		else
		{
			if( pUart->iRxBuffIndex < kDRV_Uart_RXBuffSize )
				pUart->tucRXBuff[pUart->iRxBuffIndex++] =*(pucBuffer++);
			else
				break;
		}

	}
	//update status
	if( pUart->iRxBuffIndex )
		pUart->eRxState = RXFull;

	DRV_Uart_SafeLeave(pUart);
	// return the number of read chars in the input buffer
	return iLength - iBufferIndex;
}

