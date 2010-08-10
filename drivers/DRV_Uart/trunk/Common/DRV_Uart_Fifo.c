#include <string.h>
#include "../DRV_Uart.h"
#include "../DRV_Uart_Fifo.h"
#include "../Inc/DRV_Uart_private.h"


#define kNumberOfUARTDevices (sizeof(tpcUartDeviceName)/sizeof(char**))
/**************************************************************
					Local variables
***************************************************************/

typedef struct
{
	LIB_Fifo_Type TX_Fifo;
	LIB_Fifo_Type RX_Fifo;
	char cSendString;
	DRV_Uart_Handle hDeviceHandle;
} UART_FIFO_Type;

static const char *tpcUartDeviceName[]={kDRV_Uart_DeviceNameList};

UART_FIFO_Type tUART_FIFO_List[ kNumberOfUARTDevices ];

void UART_FIFO_RXCallBack( DRV_Uart_Handle hDeviceHandle , unsigned char *pcBuffer , int *piLength);
void UART_FIFO_TXCallBack( DRV_Uart_Handle hDeviceHandle );
UART_FIFO_Type *DRV_Uart_Fifo_GetFifoHandle(DRV_Uart_Handle hDeviceHandle  );

DRV_Uart_Error DRV_Uart_Fifo_Open( const char * pcDeviceName , DRV_Uart_Handle *phDeviceHandle , DRV_Uart_Cfg *ptSettings , LIB_Fifo_Type *pRX_Fifo , LIB_Fifo_Type *pTX_Fifo )
{
	DRV_Uart_Error Error;
	int iUartIndex;

	for(iUartIndex=0 ; iUartIndex<kNumberOfUARTDevices ; iUartIndex++ )
	{
		if( !strcmp(pcDeviceName, tUART_DeviceList[iUartIndex].pcDeviceName ))
		{
			ptSettings->RXCallBack = UART_FIFO_RXCallBack;
			ptSettings->TXCallBack = UART_FIFO_TXCallBack;
			Error = DRV_Uart_Open(pcDeviceName,&tUART_FIFO_List[iUartIndex].hDeviceHandle,ptSettings);
			if(Error != UART_No_Error)
				return Error;
			tUART_FIFO_List[iUartIndex].RX_Fifo = *pRX_Fifo;
			tUART_FIFO_List[iUartIndex].TX_Fifo = *pTX_Fifo;
			*phDeviceHandle = (DRV_Uart_Handle)&tUART_FIFO_List[iUartIndex];
			break;
		}
	}
	 if( iUartIndex >= kNumberOfUARTDevices)
		 Error = UART_Device_Not_Found;
	 return UART_No_Error;
}

void UART_FIFO_RXCallBack( DRV_Uart_Handle hDeviceHandle , unsigned char *pcBuffer , int *piLength)
{
	DRV_Uart_Devicedata *pUart = (DRV_Uart_Devicedata *)hDeviceHandle ;
	UART_FIFO_Type *pFifoUart = DRV_Uart_Fifo_GetFifoHandle(hDeviceHandle);

	if(pFifoUart!=NULL)
	{
		if( !LIB_Fifo_Full(&pFifoUart->RX_Fifo) )
		{
			if(pFifoUart->RX_Fifo.cStringFifo )
			{
				if(*piLength<(pFifoUart->RX_Fifo.iItemSize-1))
					pcBuffer[*piLength+1]=0;
				else
					pcBuffer[pFifoUart->RX_Fifo.iItemSize-1]=0;
			}
			LIB_Fifo_Push( &pFifoUart->RX_Fifo , pcBuffer );
			*piLength=0;
		}
		pUart->eRxState = RXEmpty;
	}
}
void UART_FIFO_TXCallBack( DRV_Uart_Handle hDeviceHandle )
{
	UART_FIFO_Type *pFifoUart = DRV_Uart_Fifo_GetFifoHandle(hDeviceHandle);
	DRV_Uart_Devicedata *pUart;

	if (pFifoUart != NULL)
	{
		pUart = (DRV_Uart_Devicedata *) pFifoUart->hDeviceHandle;

		if( !LIB_Fifo_Empty(&pFifoUart->TX_Fifo) )
		{
			LIB_Fifo_Pop(&pFifoUart->TX_Fifo ,pUart->tucTXBuff);
			if(pFifoUart->TX_Fifo.cStringFifo)
				DRV_Uart_Send(pFifoUart->hDeviceHandle , pUart->tucTXBuff , strlen((char *)pUart->tucTXBuff));
			else
				DRV_Uart_Send(pFifoUart->hDeviceHandle , pUart->tucTXBuff ,pFifoUart->TX_Fifo.iItemSize );
		}
	}
}

DRV_Uart_Error DRV_Uart_Fifo_Push( DRV_Uart_Handle hDeviceHandle ,unsigned  char *pucBuffer )
{
	UART_FIFO_Type *pFifoUart = (UART_FIFO_Type *) hDeviceHandle;

		if( !LIB_Fifo_Full( &pFifoUart->TX_Fifo ))
		{

			if( !DRV_Uart_TXBusy( pFifoUart->hDeviceHandle ))
			{
				if(pFifoUart->TX_Fifo.cStringFifo)
					DRV_Uart_Send(pFifoUart->hDeviceHandle , pucBuffer , strlen((char *)pucBuffer));
				else
					DRV_Uart_Send(pFifoUart->hDeviceHandle , pucBuffer ,pFifoUart->TX_Fifo.iItemSize );
			}
			else
				LIB_Fifo_Push(&pFifoUart->TX_Fifo ,pucBuffer);
			return UART_No_Error;
		}
	return UART_TXError;
}

DRV_Uart_Error DRV_Uart_Fifo_Pop( DRV_Uart_Handle hDeviceHandle ,unsigned  char *pucBuffer )
{
	UART_FIFO_Type *pFifoUart = (UART_FIFO_Type *) hDeviceHandle;

	if( LIB_Fifo_Empty(&pFifoUart->RX_Fifo) )
		return UART_RXError;
	LIB_Fifo_Pop(&pFifoUart->RX_Fifo ,pucBuffer);
	return UART_No_Error;
}

int  DRV_Uart_Fifo_TX_Full( DRV_Uart_Handle hDeviceHandle )
{
	UART_FIFO_Type *pFifoUart = (UART_FIFO_Type *) hDeviceHandle;

	return LIB_Fifo_Full(&pFifoUart->TX_Fifo );
}

int  DRV_Uart_Fifo_RX_Epmty( DRV_Uart_Handle hDeviceHandle )
{
	UART_FIFO_Type *pFifoUart = (UART_FIFO_Type *) hDeviceHandle;

	return  LIB_Fifo_Empty(&pFifoUart->RX_Fifo);
}

UART_FIFO_Type *DRV_Uart_Fifo_GetFifoHandle(DRV_Uart_Handle hDeviceHandle  )
{
	int iUartIndex;

		for(iUartIndex=0 ; iUartIndex<kNumberOfUARTDevices ; iUartIndex++ )
		{
			if( hDeviceHandle == tUART_FIFO_List[iUartIndex].hDeviceHandle)
				return &tUART_FIFO_List[iUartIndex];
		}
		return NULL;
}
