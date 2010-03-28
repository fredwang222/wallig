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
//! number of devices configured
#define kNumberOfDevices (sizeof(tpcUartDeviceName)/sizeof(char**))
/**************************************************************
					Local variables
***************************************************************/
static const char *tpcUartDeviceName[]={kDRV_Uart_DeviceNameList};
/*!
 * \brief Uart Driver Data
 *  @ingroup DRV_Uart_Public_grp
 */
DRV_Uart_Devicedata tsUartsDeviceList[kNumberOfDevices];	//!< Array of devices data

/**************************************************************
                 local Functions declaration
***************************************************************/

/**************************************************************
		 Public Functions
***************************************************************/
DRV_Uart_Error DRV_Uart_Init( void )
{
    int iUartIndex;

    for(iUartIndex=0;iUartIndex<kNumberOfDevices;iUartIndex++)
    {
    	tsUartsDeviceList[iUartIndex].pcDeviceName = (char*)tpcUartDeviceName[iUartIndex];
    	tsUartsDeviceList[iUartIndex].eRxState = RXClosed;
    	tsUartsDeviceList[iUartIndex].eTxState = TXClosed;
    }
	DRV_Uart_ArchInit();
 	return UART_No_Error;
}

DRV_Uart_Error DRV_Uart_Terminate( void )
{

	DRV_Uart_ArchTerminate();
	return UART_No_Error;
}

DRV_Uart_Error DRV_Uart_Open( const char * pcDeviceName , DRV_Uart_Handle *phDeviceHandle , DRV_Uart_Cfg *ptSettings)
{
		DRV_Uart_Error tError = UART_No_Error;
        DRV_Uart_Devicedata *pUart=NULL;
        int iUartIndex;

        for(iUartIndex=0;iUartIndex<kNumberOfDevices;iUartIndex++)
        {
        	if( !strcmp(pcDeviceName, tsUartsDeviceList[iUartIndex].pcDeviceName ))
        	{    //The device name match
        		//check if the device is not already open
				if( tsUartsDeviceList[iUartIndex].eRxState != RXClosed )
					return UART_AlreadyOpened;
				pUart = &tsUartsDeviceList[iUartIndex];
				pUart->cfg=*ptSettings;
        		tError = DRV_UART_ArchOpen(pUart);
        		if( tError != UART_No_Error)
        			return tError;
        		pUart->eRxState = RXEmpty;
        		pUart->eTxState = TXIdle;
        		pUart->iRxBuffIndex=0;
        		memset(pUart->tucRXBuff,0,kDRV_Uart_RXBuffSize);
        		memset(pUart->tucTXBuff,0,kDRV_Uart_TXBuffSize);
        		//Set the user handle
        		*phDeviceHandle = (DRV_Uart_Handle)pUart;
        		break;
        	}
        }
        if( pUart==NULL)
        	tError = UART_Device_Not_Found;
	return tError;
}

DRV_Uart_Error DRV_Uart_Close( DRV_Uart_Handle hDeviceHandle )
{
        DRV_Uart_Devicedata *pUart = (DRV_Uart_Devicedata *) hDeviceHandle;

        if( pUart == NULL )
           return UART_Input_Null;
        DRV_UART_ArchClose(pUart);
        pUart->eRxState = RXClosed;
        pUart->eTxState = TXClosed;
        pUart->iRxBuffIndex=0;

	return UART_No_Error;
}

DRV_Uart_Error DRV_Uart_Send( DRV_Uart_Handle hDeviceHandle ,unsigned char *pucBuffer , int iLength)
{
  DRV_Uart_Devicedata *pUart = (DRV_Uart_Devicedata *) hDeviceHandle;

  if( pUart == NULL || pucBuffer == NULL)
     return UART_Input_Null;
  DRV_Uart_Arch_RxSafeEnter(pUart);
  if( pUart->eTxState != TXIdle)
  {
	  DRV_Uart_Arch_RxSafeLeave(pUart);
	  return UART_TXError;
  }
  //update status
  pUart->eTxState = TXBusy;
  //copy user buffer to driver buffer
  if( pUart->cfg.eSLIPModeEnable )
  {
	  iLength = DRV_Uart_Slip_Tx(pucBuffer, iLength , pUart->tucTXBuff);
  }
  else
	  memcpy( pUart->tucTXBuff , pucBuffer , iLength );
  DRV_Uart_Arch_RxSafeLeave(pUart);
  return DRV_Uart_ArchSend( pUart , pUart->tucTXBuff  , iLength);
 }

int DRV_Uart_TXBusy( DRV_Uart_Handle hDeviceHandle )
{
	DRV_Uart_Devicedata *pUart = (DRV_Uart_Devicedata *) hDeviceHandle;
	tTxState eState;

	//DRV_Uart_Arch_RxSafeEnter(pUart);
	eState = pUart->eTxState;
	//DRV_Uart_Arch_RxSafeLeave(pUart);

	if(eState  == TXIdle )
		return 0;
	else
		return 1;
}

int DRV_Uart_RXDataReceived( DRV_Uart_Handle hDeviceHandle )
{
        DRV_Uart_Devicedata *pUart = (DRV_Uart_Devicedata *) hDeviceHandle;
        tRxState eState;

        // DRV_Uart_Arch_RxSafeEnter(pUart);
        eState = pUart->eRxState;
       // DRV_Uart_Arch_RxSafeLeave(pUart);

        if( eState == RXFull )
        	return 1;
        else
        	return 0;
}

DRV_Uart_Error DRV_Uart_Receive( DRV_Uart_Handle hDeviceHandle , unsigned char *pucBuffer , int *piLength)
{
  DRV_Uart_Devicedata *pUart = (DRV_Uart_Devicedata *) hDeviceHandle;

  if( pUart == NULL || pucBuffer == NULL )
     return UART_Input_Null;


	DRV_Uart_Arch_RxSafeEnter(pUart);
    *piLength = pUart->iRxBuffIndex;
	//Copy data to user buffer
	memcpy(pucBuffer,pUart->tucRXBuff,*piLength);
	pUart->iRxBuffIndex-=*piLength;
	//update status
	if( !pUart->iRxBuffIndex)
		pUart->eRxState = RXEmpty;
	DRV_Uart_Arch_RxSafeLeave(pUart);

    return UART_No_Error;
}

DRV_Uart_Error DRV_Uart_RXEnable( DRV_Uart_Handle hDeviceHandle , char cFlag )
{
  DRV_Uart_Devicedata *pUart = (DRV_Uart_Devicedata *) hDeviceHandle;

    if( pUart == NULL )
       return UART_Input_Null;

      return UART_No_Error;
}

DRV_Uart_Error DRV_Uart_TXEnable( DRV_Uart_Handle hDeviceHandle , char cFlag )
{
  DRV_Uart_Devicedata *pUart = (DRV_Uart_Devicedata *) hDeviceHandle;

    if( pUart == NULL )
       return UART_Input_Null;

      return UART_No_Error;
}
DRV_Uart_Error DRV_Uart_RXFlush( DRV_Uart_Handle hDeviceHandle )
{
	DRV_Uart_Devicedata *pUart = (DRV_Uart_Devicedata *) hDeviceHandle;

	if( pUart == NULL )
	   return UART_Input_Null;

	DRV_Uart_Arch_RxSafeEnter(pUart);
	pUart->iRxBuffIndex=0;
	pUart->eRxState = RXEmpty;
	DRV_Uart_Arch_RxSafeLeave(pUart);
	return UART_No_Error;
}

DRV_Uart_Error DRV_Uart_TXFlush( DRV_Uart_Handle hDeviceHandle )
{
	DRV_Uart_Devicedata *pUart = (DRV_Uart_Devicedata *) hDeviceHandle;

	if( pUart == NULL )
	   return UART_Input_Null;

	DRV_Uart_Arch_RxSafeEnter(pUart);
	DRV_Uart_ArchTXFlush(pUart);
	pUart->eTxState = TXIdle;
	DRV_Uart_Arch_RxSafeLeave(pUart);
	return UART_No_Error;
}
/**************************************************************
                 private Functions
***************************************************************/
int DRV_Uart_Private_Callback( DRV_Uart_Devicedata *pUart ,  unsigned char *pucBuffer , int iLength )
{
	int iBufferIndex=0;
	DRV_Uart_Arch_RxSafeEnter(pUart);
	//if(pUart->eRxState = RXEmpty)
	{
		//Append data to the rx buffer
		for( iBufferIndex=0;iBufferIndex<iLength ; iBufferIndex++ )
		{
			if( pUart->cfg.eSLIPModeEnable )
			{
				if(pUart->eRxState == RXEmpty)
				{
					pUart->iRxBuffIndex=DRV_Uart_Slip_Rx( &pUart->tSLIP , *(pucBuffer++) , pUart->tucRXBuff ,kDRV_Uart_RXBuffSize);
					if(  pUart->iRxBuffIndex )
					{   //A SLIP packet was received
						pUart->eRxState = RXFull;
						break;
					}
				}
				else
				{
					iBufferIndex=0;
					break ;
				}
			}
			else
			{
				if( pUart->iRxBuffIndex < kDRV_Uart_RXBuffSize )
				{

					pUart->tucRXBuff[pUart->iRxBuffIndex] =*(pucBuffer++);
					//checking end of packet
					if( pUart->cfg.cEndOfBuff )
					{
						if(pUart->tucRXBuff[pUart->iRxBuffIndex++] == pUart->cfg.cEndOfBuff )
						{
							pUart->eRxState = RXFull;
							break;
						}
					}
					else if( pUart->cfg.iRXNbChar )
					{
						if( pUart->cfg.iRXNbChar > pUart->iRxBuffIndex++ )
						{
							pUart->eRxState = RXFull;
							break;
						}
					}
					else
					{
						pUart->iRxBuffIndex++;
						pUart->eRxState = RXFull;
						break;
					}

				}
				else
					break;
			}

		}
	}
	DRV_Uart_Arch_RxSafeLeave(pUart);
	// return the number of read chars in the input buffer
	return iBufferIndex;
}

