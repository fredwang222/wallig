/**
	\file DRV_Uart_Stm32.c

	\author Gwendal Le Gall

	\date 20/02/2010

	\section UartLinux Stm32 implementation of the Uart driver


*/
#include <stdio.h>
#include <string.h>
#include "stm32f10x_lib.h"
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
	char *pcName;
	USART_TypeDef* Handle;
	DRV_Uart_Devicedata *pUart;
	u32 uiTxCount;
	u8 *puCTxBuff;
} DRV_UART_ARCH_Device;

DRV_UART_ARCH_Device tDeviceListe[]={{"UART1",USART1,NULL,0 } ,{"UART2",USART2,NULL,0 },{NULL,0,NULL,0}};


static void UART_Gpio_Config( u8 ucUARTIndex);
/**************************************************************
                 private Functions
***************************************************************/

void DRV_Uart_Arch_RxSafeEnter( DRV_Uart_Devicedata *pUart )
{
	DRV_UART_ARCH_Device *pArchUART = (DRV_UART_ARCH_Device*)pUart->pArchData;
	USART_ITConfig(pArchUART->Handle, USART_IT_RXNE, DISABLE);

}
void DRV_Uart_Arch_RxSafeLeave( DRV_Uart_Devicedata *pUart )
{
	DRV_UART_ARCH_Device *pArchUART = (DRV_UART_ARCH_Device*)pUart->pArchData;
	USART_ITConfig(pArchUART->Handle, USART_IT_RXNE, ENABLE);

}

void DRV_Uart_Arch_TxSafeEnter( DRV_Uart_Devicedata *pUart )
{
	DRV_UART_ARCH_Device *pArchUART = (DRV_UART_ARCH_Device*)pUart->pArchData;
	USART_ITConfig(pArchUART->Handle, USART_IT_TXE, DISABLE);

}
void DRV_Uart_Arch_TxSafeLeave( DRV_Uart_Devicedata *pUart )
{
	DRV_UART_ARCH_Device *pArchUART = (DRV_UART_ARCH_Device*)pUart->pArchData;
	USART_ITConfig(pArchUART->Handle, USART_IT_TXE, ENABLE);

}

DRV_Uart_Error DRV_Uart_ArchInit(void )
{
	/* Configure the NVIC Preemption Priority Bits */
	  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	return UART_No_Error;
}

DRV_Uart_Error DRV_Uart_ArchTerminate(void )
{


    return UART_No_Error;
}

DRV_Uart_Error DRV_UART_ArchOpen( DRV_Uart_Devicedata *pUart )
{
	DRV_Uart_Error tError = UART_No_Error;
	USART_InitTypeDef USART_InitStructure;
	int iDeviceIndex=0;

	while(tDeviceListe[iDeviceIndex].Handle)
	{
		if( !strcmp(tDeviceListe[iDeviceIndex].pcName , pUart->pcDeviceName))
		{
			pUart->pArchData = &tDeviceListe[iDeviceIndex];
			tDeviceListe[iDeviceIndex].pUart=pUart;
			break;
		}
		iDeviceIndex++;
	}
	if( tDeviceListe[iDeviceIndex].Handle == NULL )
	{
		return UART_Device_Not_Found;
	}

	switch( (u32) tDeviceListe[iDeviceIndex].Handle )
	{
		case (u32)USART1:
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
			break;
		case (u32)USART2:
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
			break;
	}

	switch( pUart->cfg.eBaudRate )
	{
		case BR4800:
			USART_InitStructure.USART_BaudRate = 4800;
			break;
		case BR9600:
			USART_InitStructure.USART_BaudRate = 9600;
			break;
		case BR115200:
			USART_InitStructure.USART_BaudRate = 115200;
			break;
		default:
		case BRUnknown:
			return UART_Bad_Param;
	}
	switch( pUart->cfg.eParity )
	{
		case PARNone:
			USART_InitStructure.USART_Parity = USART_Parity_No;
			break;
		case PAREven:
			USART_InitStructure.USART_Parity = USART_Parity_Even;
			break;
		case PAROdd:
			USART_InitStructure.USART_Parity = USART_Parity_Odd;
			break;
		case PARUnknown:
		default:
			return UART_Bad_Param;
			break;
	}

	switch( pUart->cfg.eStopBit )
	{
		case SB_1:
			USART_InitStructure.USART_StopBits = USART_StopBits_1;
			break;
		case SB_1_5:
			USART_InitStructure.USART_StopBits = USART_StopBits_1_5;
			break;
		case SB_2:
			USART_InitStructure.USART_StopBits = USART_StopBits_2;
			break;
		default:
		case SBUnknown:
			break;
	}
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(tDeviceListe[iDeviceIndex].Handle, &USART_InitStructure);

	USART_ITConfig(tDeviceListe[iDeviceIndex].Handle, USART_IT_RXNE, ENABLE);
	USART_ITConfig(tDeviceListe[iDeviceIndex].Handle, USART_IT_TXE, ENABLE);
	 USART_Cmd(tDeviceListe[iDeviceIndex].Handle, ENABLE);
	 UART_Gpio_Config(iDeviceIndex);
	return tError;
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
	u8 ucCharIn;
	volatile u32 uiSRValue = ((volatile USART_TypeDef*)USART1)->SR;

	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	{
		ucCharIn = USART_ReceiveData(USART1);
		DRV_Uart_Private_Callback( tDeviceListe[0].pUart , &ucCharIn , 1 );
	}

	if(USART_GetITStatus(USART1, USART_IT_TXE) != RESET)
	{
		if(tDeviceListe[0].uiTxCount)
		{
			USART_SendData(USART1, *(tDeviceListe[0].puCTxBuff++));
			tDeviceListe[0].uiTxCount--;
			if(!tDeviceListe[0].uiTxCount)
				tDeviceListe[0].pUart->eTxState= TXIdle;
		}
		else
		{
			USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
		}
	}
	//NVIC_ClearIRQChannelPendingBit(NVIC_GetCurrentPendingIRQChannel());
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
	u8 ucCharIn;

	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
	{
		ucCharIn = USART_ReceiveData(USART2);
		DRV_Uart_Private_Callback( tDeviceListe[1].pUart , &ucCharIn , 1 );
	}

	if(USART_GetITStatus(USART2, USART_IT_TXE) != RESET)
	{
		if(tDeviceListe[1].uiTxCount)
		{
			USART_SendData(USART2, *(tDeviceListe[1].puCTxBuff++));
			tDeviceListe[1].uiTxCount--;
		}

	}
}
DRV_Uart_Error DRV_UART_ArchClose( DRV_Uart_Devicedata *pUart )
{
	//DRV_UART_ARCH_Data *pData =(DRV_UART_ARCH_Data *) pUart->pArchData ;
	//TODO

    pUart->pArchData=0;
    return UART_No_Error;
}

DRV_Uart_Error DRV_Uart_ArchSend( DRV_Uart_Devicedata *pUart , unsigned char *pucBuffer , int iLength)
{

	DRV_UART_ARCH_Device *pArchUART = (DRV_UART_ARCH_Device*)pUart->pArchData;

	pArchUART->puCTxBuff=pucBuffer;
	pArchUART->uiTxCount=(u32)iLength;
	USART_SendData(pArchUART->Handle, *(pArchUART->puCTxBuff++));
	USART_ITConfig(pArchUART->Handle, USART_IT_TXE, ENABLE);

	pArchUART->uiTxCount--;

	return UART_No_Error;
}

DRV_Uart_Error  DRV_Uart_ArchRXFlush( DRV_Uart_Devicedata *pUart)
{
	//DRV_UART_ARCH_Data *pData =(DRV_UART_ARCH_Data *) pUart->pArchData ;
	//TODO
	return UART_No_Error;
}

DRV_Uart_Error DRV_Uart_ArchTXFlush( DRV_Uart_Devicedata *pUart)
{
	DRV_UART_ARCH_Device *pArchUART = (DRV_UART_ARCH_Device*)pUart->pArchData;
	pArchUART->uiTxCount=0;
	return UART_No_Error;
}

static void UART_Gpio_Config( u8 ucUARTIndex)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;

#ifdef USE_STM3210B_EVAL
  /* Enable the USART2 Pins Software Remapping */
  GPIO_PinRemapConfig(GPIO_Remap_USART2, ENABLE);
#endif

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  if( !ucUARTIndex )
  {
	  /* Configure USART1 Rx (PA.10) as input floating */
	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	  GPIO_Init(GPIOA, &GPIO_InitStructure);
  }
  else
  {
	  /* Configure USART2 Rx as input floating */
	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	  GPIO_Init(GPIOA, &GPIO_InitStructure);
  }

  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  if( !ucUARTIndex )
  {
	  /* Configure USART1 Tx (PA.09) as alternate function push-pull */
	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	  GPIO_Init(GPIOA, &GPIO_InitStructure);
  }
  else
  {
	  /* Configure USART2 Tx as alternate function push-pull */
	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	  GPIO_Init(GPIOA, &GPIO_InitStructure);
  }

  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  if( !ucUARTIndex )
  {
	  /* Enable the USART1 Interrupt */
	  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQChannel;
  }
  else
  {
	  /* Enable the USART2 Interrupt */
	  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQChannel;
  }
   NVIC_Init(&NVIC_InitStructure);
}

