/**
	\file DRV_Uart_Stm32.c

	\author Gwendal Le Gall

	\date 20/02/2010

	\section UartLinux Stm32 implementation of the Uart driver


*/
#include <stdio.h>
#include <string.h>
#include "stm32f10x.h"
#include "stm32f10x_usart.h"
#include "../../DRV_Uart.h"

/**************************************************************
                                        Macros
***************************************************************/
#define kDRV_UART_DEVICE_COUNT (sizeof(tUART_DeviceListe)/sizeof(DRV_UART_DeviceCfg))
#define kDRV_Uart_RxSafeEnter( pUart ) USART_ITConfig(pUart->pCfg->Handle, USART_IT_RXNE, DISABLE)
#define kDRV_Uart_RxSafeLeave( pUart ) USART_ITConfig(pUart->pCfg->Handle, USART_IT_RXNE, ENABLE)
#define kDRV_Uart_TxSafeEnter( pUart ) USART_ITConfig(pUart->pCfg->Handle, USART_IT_TXE, DISABLE);
#define kDRV_Uart_TxSafeLeave( pUart ) USART_ITConfig(pUart->pCfg->Handle, USART_IT_TXE, ENABLE);

/**************************************************************
                                        Local variables
***************************************************************/
typedef enum
{
    RXClosed,
	RXEmpty,
	RXFull,
} tRxState;

typedef enum
{
    TXClosed,
	TXIdle,
	TXBusy,
} tTxState;

typedef struct
{
	char *pcName;
	USART_TypeDef* Handle;
	USART_InitTypeDef USART_InitStructure;
	struct
	{
		void( *fPeriphClockCmd)(uint32_t , FunctionalState );
		uint32_t uiPeriph;
	} UART_ClockCfg;
	struct
	{
		void( *fPeriphClockCmd)(uint32_t , FunctionalState );
		uint32_t uiPeriph;
	} UART_PioClkCfg;
	struct
	{
		GPIO_TypeDef tPort;
		uint32_t uiTxPio;
		uint32_t uiRxPio;
	};
	uint8_t ucIrqChannel;
} DRV_UART_DeviceCfg;
typedef struct
{
	DRV_UART_DeviceCfg *pCfg;
	uint32_t uiTxCount;
	uintt8_t *puCTxBuff;
	DRV_Uart_Cb CallBacks;
	tTxState TxState;
	tRxState RxState;
} DRV_UART_DeviceData;

static const DRV_UART_DeviceCfg tUART_DeviceCfgListe[]=kDRV_Uart_DEVICE_CONF;
static DRV_UART_DeviceData tUART_DeviceDataList[kDRV_UART_DEVICE_COUNT];

static void UART_Gpio_Config( uintt8_t ucUARTIndex);
/**************************************************************
                 private Functions
***************************************************************/


DRV_Uart_Error DRV_Uart_Init( void )
{
	/* Configure the NVIC Preemption Priority Bits */
	  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	  memset( tUART_DeviceDataList , 0 , sizeof(tUART_DeviceDataList));

	return UART_No_Error;
}

DRV_Uart_Error DRV_Uart_Terminate( void )
{

	memset( tUART_DeviceDataList , 0 , sizeof(tUART_DeviceDataList));
    return UART_No_Error;
}

DRV_Uart_Error DRV_Uart_Open( const char * pcDeviceName , DRV_Uart_Handle *phDeviceHandle ,  DRV_Uart_Cb *ptCb)
{
	DRV_Uart_Error tError = UART_No_Error;
	DRV_UART_DeviceData *pUart;
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	int iDeviceIndex=0;

	*phDeviceHandle=NULL;
	for( iDeviceIndex=0 ; iDeviceIndex < kDRV_UART_DEVICE_COUNT; iDeviceIndex++)
	{
		if( !strcmp( tUART_DeviceCfgListe[iDeviceIndex ].pcName , pcDeviceName) )
		{
			tUART_DeviceDataList[iDeviceIndex].pCfg = &tUART_DeviceCfgListe[iDeviceIndex ];
			pUart =  &tUART_DeviceDataList[iDeviceIndex];

		}
	}

	if( pUart == NULL )
	{
		return UART_Device_Not_Found;
	}
	/* Init UART Clock */
	pUart->pCfg.UART_ClockCfg.fPeriphClockCmd( pUart->pCfg.UART_ClockCfg.uiPeriph , ENABLE );
	/* Init Gpio Clock */
	pUart->pCfg.UART_PioClkCfg.fPeriphClockCmd(pUart->pCfg.UART_PioClkCfg.uiPeriph , ENABLE );
	/* init Uart device*/
	USART_Init( pUart->pCfg.tPort, &pUart->pCfg.USART_InitStructure);
	/* IT config */
	USART_ITConfig(pUart->pCfg.tPort , USART_IT_RXNE, ENABLE);
	USART_Cmd( pUart->pCfg.tPort , ENABLE);

	/* Configure USARTx Rx  as input floating */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Pin = pUart->pCfg.uiRxPio ;
	GPIO_Init(pUart->pCfg.tPort, &GPIO_InitStructure);

	/* Configure  Tx  as push-pull */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = pUart->pCfg.uiTxPio;
	GPIO_Init(pUart->pCfg.tPort, &GPIO_InitStructure);

	/* Enable the USART Interrupt */
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannel = pUart->pCfg.ucIrqChannel ;
	NVIC_Init(&NVIC_InitStructure);

	pUart->TxState=TXIdle;
	pUart->RxState= RXEmpty;
	/* Callback register */
	pUart->CallBacks= *ptCb;

	*phDeviceHandle = ( DRV_Uart_Handle *) pUart ;
	return tError;
}

DRV_Uart_Error DRV_Uart_Close( DRV_Uart_Handle hDeviceHandle )
{
	DRV_UART_DeviceData *pUart = (DRV_UART_DeviceData *)hDeviceHandle; ;
	pUart->TxState=TXClosed;
	pUart->RxState= RXClosed;

    return UART_No_Error;
}

DRV_Uart_Error DRV_Uart_Send( DRV_Uart_Handle hDeviceHandle ,unsigned  char *pucBuffer , int iLength)
{

	DRV_UART_DeviceData *pUart = (DRV_UART_DeviceData *)hDeviceHandle;

	if( !DRV_Uart_TXBusy( hDeviceHandle ) )
	{
		pUart->puCTxBuff=pucBuffer;
		pUart->uiTxCount=(uint32_t)iLength;
		pUart->uiTxCount--;
		pUart->TxState=TXBusy;
		USART_SendData(pUart->pCfg->Handle, *(pUart->puCTxBuff++));
		USART_ITConfig(pUart->pCfg->Handle, USART_IT_TXE, ENABLE);
		return UART_No_Error;
	}

	return UART_RXError;
}


int DRV_Uart_TXBusy( DRV_Uart_Handle hDeviceHandle )
{
	DRV_UART_DeviceData *pUart = (DRV_UART_DeviceData *) hDeviceHandle;
	tTxState eState;

	kDRV_Uart_RxSafeEnter(pUart);
	eState = pUart->TxState;
	kDRV_Uart_RxSafeLeave(pUart);

	if(eState  == TXIdle )
		return 0;
	else
		return 1;
}

DRV_Uart_Error DRV_Uart_TXFlush( DRV_Uart_Handle hDeviceHandle )
{
	DRV_UART_DeviceData *pUart = (DRV_UART_DeviceData *) hDeviceHandle;

	if( pUart == NULL )
	   return UART_Input_Null;

	DRV_Uart_RxSafeEnter(pUart);
	pUart->uiTxCount = 0;
	pUart->TxState = TXIdle;
	DRV_Uart_RxSafeLeave(pUart);
	return UART_No_Error;
}

/*******************************************************************************
* Function Name  : USART1_IRQHandler
* Description    : This function handles USART1 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void __attribute__((__interrupt__)) USART1_IRQHandler(void)
{
	uintt8_t ucCharIn;

	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	{
		ucCharIn = USART_ReceiveData(USART1);
		if( tUART_DeviceDataList[0].CallBacks.RXCallBack )
			tUART_DeviceDataList[0].CallBacks.RXCallBack(( DRV_Uart_Handle) &tUART_DeviceDataList[0] , &ucCharIn , 1);
	}

	if(USART_GetITStatus(USART1, USART_IT_TXE) != RESET)
	{
		if(tUART_DeviceDataList[0].uiTxCount)
		{
			USART_SendData(USART1, *(tUART_DeviceDataList[0].puCTxBuff++));
			tUART_DeviceDataList[0].uiTxCount--;
		}
		else
		{
			USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
			tUART_DeviceDataList[0].TxState= TXIdle;
			if( tUART_DeviceDataList[0].CallBacks.TXCallBack )
				tUART_DeviceDataList[0].CallBacks.TXCallBack(( DRV_Uart_Handle) &tUART_DeviceDataList[0] );
		}
	}
}

/*******************************************************************************
* Function Name  : USART2_IRQHandler
* Description    : This function handles USART2 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USART2_IRQHandler(void)
{
	uintt8_t ucCharIn;

	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	{
		ucCharIn = USART_ReceiveData(USART1);
		if( tUART_DeviceDataList[1].CallBacks.RXCallBack )
		tUART_DeviceDataList[1].CallBacks.RXCallBack(( DRV_Uart_Handle) &tUART_DeviceDataList[1] , &ucCharIn , 1);
	}

	if(USART_GetITStatus(USART1, USART_IT_TXE) != RESET)
	{
		if(tUART_DeviceDataList[1].uiTxCount)
		{
			USART_SendData(USART1, *(tUART_DeviceDataList[1].puCTxBuff++));
			tUART_DeviceDataList[1].uiTxCount--;
		}
		else
		{
			USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
			tUART_DeviceDataList[1].TxState= TXIdle;
			if( tUART_DeviceDataList[1].CallBacks.TXCallBack )
				tUART_DeviceDataList[1].CallBacks.TXCallBack(( DRV_Uart_Handle) &tUART_DeviceDataList[1] );
		}
	}
}
