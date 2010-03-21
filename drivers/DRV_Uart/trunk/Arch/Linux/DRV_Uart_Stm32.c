/**
	\file DRV_Uart_Stm32.c

	\author Gwendal Le Gall

	\date 20/02/2010

	\section UartLinux Stm32 implementation of the Uart driver


*/
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
	int dummy;
} DRV_UART_ARCH_Data;

/**************************************************************
                 private Functions
***************************************************************/

void DRV_Uart_Arch_RxSafeEnter( DRV_Uart_Devicedata *pUart )
{
	DRV_UART_ARCH_Data *pData = (DRV_UART_ARCH_Data*)pUart->pArchData;
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

}
void DRV_Uart_Arch_RxSafeLeave( DRV_Uart_Devicedata *pUart )
{
	DRV_UART_ARCH_Data *pData = (DRV_UART_ARCH_Data*)pUart->pArchData;
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

}

void DRV_Uart_Arch_TxSafeEnter( DRV_Uart_Devicedata *pUart )
{
	DRV_UART_ARCH_Data *pData = (DRV_UART_ARCH_Data*)pUart->pArchData;
	USART_ITConfig(USART1, USART_IT_TXE, ENABLE);

}
void DRV_Uart_Arch_TxSafeLeave( DRV_Uart_Devicedata *pUart )
{
	DRV_UART_ARCH_Data *pData = (DRV_UART_ARCH_Data*)pUart->pArchData;
	USART_ITConfig(USART1, USART_IT_TXE, ENABLE);

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
	USART_InitTypeDef USART_InitStructure;


	USART_InitStructure.USART_BaudRate = 9600;
	  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	  USART_InitStructure.USART_StopBits = USART_StopBits_1;
	  USART_InitStructure.USART_Parity = USART_Parity_No;
	  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	  USART_Init(USART1, &USART_InitStructure);

	  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	    USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
	    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

	return tError;
}

/*******************************************************************************
* Function Name  : USART1_IRQHandler
* Description    : This function handles USART1 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USART1_IRQHandler(void)
{
	 if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	  {
	    /* Read one byte from the receive data register */
	    RxBuffer1[RxCounter1++] = USART_ReceiveData(USART1);

	    if(RxCounter1 == NbrOfDataToRead1)
	    {
	      /* Disable the USART1 Receive interrupt */
	      USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);
	    }
	  }

	  if(USART_GetITStatus(USART1, USART_IT_TXE) != RESET)
	  {
	    /* Write one byte to the transmit data register */
	    USART_SendData(USART1, TxBuffer1[TxCounter1++]);

	    if(TxCounter1 == NbrOfDataToTransfer1)
	    {
	      /* Disable the USART1 Transmit interrupt */
	      USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
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
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
	  {
	    /* Read one byte from the receive data register */
	    RxBuffer2[RxCounter2++] = USART_ReceiveData(USART2);

	    if(RxCounter2 == NbrOfDataToRead1)
	    {
	      /* Disable the USART2 Receive interrupt */
	      USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);
	    }
	  }

	  if(USART_GetITStatus(USART2, USART_IT_TXE) != RESET)
	  {
	    /* Write one byte to the transmit data register */
	    USART_SendData(USART2, TxBuffer2[TxCounter2++]);

	    if(TxCounter2 == NbrOfDataToTransfer2)
	    {
	      /* Disable the USART2 Transmit interrupt */
	      USART_ITConfig(USART2, USART_IT_TXE, DISABLE);
	    }
	  }
}
DRV_Uart_Error DRV_UART_ArchClose( DRV_Uart_Devicedata *pUart )
{
	void *ret;

	DRV_UART_ARCH_Data *pData =(DRV_UART_ARCH_Data *) pUart->pArchData ;

    pUart->pArchData=0;
    return UART_No_Error;
}

DRV_Uart_Error DRV_Uart_ArchSend( DRV_Uart_Devicedata *pUart , unsigned char *pucBuffer , int iLength)
{
	DRV_UART_ARCH_Data *pData =(DRV_UART_ARCH_Data *) pUart->pArchData ;
	return UART_No_Error;
}

DRV_Uart_Error  DRV_Uart_ArchRXFlush( DRV_Uart_Devicedata *pUart)
{
	DRV_UART_ARCH_Data *pData =(DRV_UART_ARCH_Data *) pUart->pArchData ;

	return UART_No_Error;
}

DRV_Uart_Error DRV_Uart_ArchTXFlush( DRV_Uart_Devicedata *pUart)
{
	DRV_UART_ARCH_Data *pData =(DRV_UART_ARCH_Data *) pUart->pArchData ;
	return UART_No_Error;
}
