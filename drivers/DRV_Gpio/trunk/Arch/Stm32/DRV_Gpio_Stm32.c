#include <string.h>
#include "stm32f10x.h"
#include "../../DRV_Gpio.h"
#include "DRV_Gpio_Cfg.h"

typedef struct
{
	char *Name;
	DRV_GpioID Id;
	DRV_Gpio_Cfg Cfg;
	DRV_GpioINT Int;

} GPIO_Data;
#define GPIO_DEVICE_COUNT ( sizeof(GpioDevicesData)/sizeof(GPIO_Data))

typedef void (*GPIO_Handler)(void);

GPIO_Handler tGpioIrqHandler[16];

GPIO_Data GpioDevicesData[]= DRV_GPIO_INIT_CFG;
//DRV_Gpio_Cfg GpioHandles[16*3];
int iPioCount;


DRV_Gpio_Error DRV_Gpio_Init( void )
{
	  iPioCount=0;
	 /* Enable GPIOB clock */
	  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA  | RCC_APB2Periph_AFIO , ENABLE);
	  /* Enable GPIOB clock */
  	  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
  	/* Enable GPIOC clock */
	  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	  memset( tGpioIrqHandler , 0 , sizeof(tGpioIrqHandler));
  	return GPIO_No_Error;

}

DRV_Gpio_Error DRV_Gpio_Terminate( void )
{
	 /* Enable GPIOB clock */
	  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, DISABLE);
	  /* Enable GPIOB clock */
	  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, DISABLE);
	  /* Enable GPIOC clock */
	  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, DISABLE);
	  return GPIO_No_Error;

}

DRV_Gpio_Error DRV_Gpio_Open( char *Name , DRV_Gpio_Handle *phDeviceHandle , DRV_GpioINT *pGpioIntCfg)
{
	int iGpioIndex;
	GPIO_Data *pGpioData;
	EXTI_InitTypeDef EXTI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	DRV_Gpio_Error eError=GPIO_Device_Not_Found;

	int iCount;
	unsigned int uiPio;
	unsigned int uiBank;

	for ( iGpioIndex = 0 ; iGpioIndex < GPIO_DEVICE_COUNT ; iGpioIndex++ )
	{
		if( !strcmp( Name , GpioDevicesData[iGpioIndex].Name) )
		{
			pGpioData = &GpioDevicesData[iGpioIndex];
			eError=GPIO_No_Error;
			break;
		}
	}
	if( eError==GPIO_Device_Not_Found )
		return eError;

	*phDeviceHandle=(DRV_Gpio_Handle)pGpioData;
	switch((u32)pGpioData->Id.Bank)
	{
		case (u32)GPIOA:
			uiBank=0;
			break;
		case (u32)GPIOB:
			uiBank=1;
			break;
		case (u32)GPIOC:
			uiBank=2;
			break;
	}

	GPIO_StructInit(&GPIO_InitStructure);
	uiPio=pGpioData->Id.Pio;
	for( iCount=0; !(uiPio&1);iCount++)
		uiPio>>=1;
	uiPio=iCount;
	GPIO_InitStructure.GPIO_Pin = pGpioData->Id.Pio;
	if( pGpioData->Cfg.Type == GPIO_Output )
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = pGpioData->Cfg.uiOption;
	GPIO_Init(pGpioData->Id.Bank, &GPIO_InitStructure);

	if( pGpioIntCfg != NULL )
		pGpioData->Int = *pGpioIntCfg;

	  /* Interrupt*/
	  if( pGpioData->Int.EventType != GPIO_INT_None && pGpioData->Int.CallBack != NULL)
	  {

		  /* Configure the EXTI0 Interrupt */
		 	  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
		 	  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
		 	  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		 	 tGpioIrqHandler[uiPio] = pGpioData->Int.CallBack;
		 	  switch( pGpioData->Id.Pio )
		 	  {
				  case GPIO_Pin_0:
					  NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
					  break;
				  case GPIO_Pin_1:
					  NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;
					  break;
				  case GPIO_Pin_2:
					  NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;
					  break;
				  case GPIO_Pin_3:
					  NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;
					  break;
				  case GPIO_Pin_4:
					  NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;
					  break;
				  case GPIO_Pin_5:
				  case GPIO_Pin_6:
				  case GPIO_Pin_7:
				  case GPIO_Pin_8:
				  case GPIO_Pin_9:
					  NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
					  break;
				  case GPIO_Pin_10:
				  case GPIO_Pin_11:
				  case GPIO_Pin_12:
				  case GPIO_Pin_13:
				  case GPIO_Pin_14:
				  case GPIO_Pin_15:
					  NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
					  break;
		 	  }
		 	  NVIC_Init(&NVIC_InitStructure);

			/* Connect PIO EXTI Line to Key Button GPIO Pin */
		    GPIO_EXTILineConfig(pGpioData->Id.Bank ,pGpioData->Id.Pio);
		    /* Configure PIO EXTI Line to generate an interrupt  */
			switch( pGpioData->Int.EventType )
			{
			  case GPIO_INT_Low2High:
				  EXTI_InitStructure.EXTI_Trigger=EXTI_Trigger_Rising;
				  break;
			  case GPIO_INT_High2Low:
				  EXTI_InitStructure.EXTI_Trigger=EXTI_Trigger_Falling;
				  break;
			  case GPIO_INT_None:
			  case GPIO_INT_Both:
				  EXTI_InitStructure.EXTI_Trigger=EXTI_Trigger_Rising_Falling;
				  break;
			}
			EXTI_InitStructure.EXTI_Line = pGpioData->Id.Pio;
			EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
			EXTI_InitStructure.EXTI_LineCmd = ENABLE;
			EXTI_Init(&EXTI_InitStructure);


	  }

	  return eError;

}

DRV_Gpio_Error DRV_Gpio_Close( DRV_Gpio_Handle hDeviceHandle )
{
	GPIO_DeInit(((GPIO_Data *)hDeviceHandle)->Id.Bank);
	return GPIO_No_Error;
}

BOOL DRV_Gpio_ValueGet( DRV_Gpio_Handle hDeviceHandle )
{
 return (BOOL) GPIO_ReadInputDataBit( ((GPIO_Data *)hDeviceHandle)->Id.Bank, ((GPIO_Data *)hDeviceHandle)->Id.Pio );
}


DRV_Gpio_Error DRV_Gpio_ValueSet( DRV_Gpio_Handle hDeviceHandle , BOOL bValue)
{
	if( bValue )
		GPIO_SetBits(((GPIO_Data*)hDeviceHandle)->Id.Bank, ((GPIO_Data*)hDeviceHandle)->Id.Pio);
	else
		GPIO_ResetBits(((GPIO_Data*)hDeviceHandle)->Id.Bank, ((GPIO_Data*)hDeviceHandle)->Id.Pio);
	return GPIO_No_Error;
}

DRV_Gpio_Error DRV_Gpio_TypeSet( DRV_Gpio_Handle hDeviceHandle , DRV_Gpio_Type eType)
{
	return GPIO_No_Error;
}

DRV_Gpio_Error DRV_Gpio_OptionSet( DRV_Gpio_Handle hDeviceHandle , unsigned int uiOption)
{
	return GPIO_No_Error;
}

DRV_Gpio_Error DRV_Gpio_OptionReSet( DRV_Gpio_Handle hDeviceHandle , unsigned int uiOption)
{
	return GPIO_No_Error;
}

/*******************************************************************************
* Function Name  : EXTI0_IRQHandler
* Description    : This function handles External interrupt Line 0 request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EXTI0_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line0) != RESET)
	{
		if(tGpioIrqHandler[0] != NULL )
		tGpioIrqHandler[0]();
		/* Clear the Key Button EXTI line pending bit */
		EXTI_ClearITPendingBit(EXTI_Line0);
	}
}

/*******************************************************************************
* Function Name  : EXTI1_IRQHandler
* Description    : This function handles External interrupt Line 1 request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EXTI1_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line1) != RESET)
	{
		if(tGpioIrqHandler[1] != NULL )
			tGpioIrqHandler[1]();
		/* Clear the Key Button EXTI line pending bit */
		EXTI_ClearITPendingBit(EXTI_Line1);
	}
}
/*******************************************************************************
* Function Name  : EXTI2_IRQHandler
* Description    : This function handles External interrupt Line 2 request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EXTI2_IRQHandler(void)
{
		if(EXTI_GetITStatus(EXTI_Line2) != RESET)
		{
			if(tGpioIrqHandler[2] != NULL )
				tGpioIrqHandler[2]();
			/* Clear the Key Button EXTI line pending bit */
			EXTI_ClearITPendingBit(EXTI_Line2);
		}
}

/*******************************************************************************
* Function Name  : EXTI3_IRQHandler
* Description    : This function handles External interrupt Line 3 request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EXTI3_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line3) != RESET)
	{
		if(tGpioIrqHandler[3] != NULL )
			tGpioIrqHandler[3]();
		/* Clear the Key Button EXTI line pending bit */
		EXTI_ClearITPendingBit(EXTI_Line3);
	}
}


/*******************************************************************************
* Function Name  : EXTI4_IRQHandler
* Description    : This function handles External interrupt Line 4 request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EXTI4_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line4) != RESET)
	{
		if(tGpioIrqHandler[4] != NULL )
			tGpioIrqHandler[4]();
		/* Clear the Key Button EXTI line pending bit */
		EXTI_ClearITPendingBit(EXTI_Line4);
	}
}

/*******************************************************************************
* Function Name  : EXTI9_5_IRQHandler
* Description    : This function handles External lines 9 to 5 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EXTI9_5_IRQHandler(void)
{
	int iCount;
	u32 EXTI_LINE;
	for( iCount = 0 ; iCount<5;iCount++)
	{
		EXTI_LINE=1<<(5+iCount);
		if(EXTI_GetITStatus(EXTI_LINE) != RESET)
		{
			if(tGpioIrqHandler[5+iCount] != NULL )
				tGpioIrqHandler[5+iCount]();
			/* Clear the Key Button EXTI line pending bit */
			EXTI_ClearITPendingBit(EXTI_LINE);
		}
	}
}


/*******************************************************************************
* Function Name  : EXTI15_10_IRQHandler
* Description    : This function handles External lines 15 to 10 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EXTI15_10_IRQHandler(void)
{
	int iCount;
	u32 EXTI_LINE;
	for( iCount = 0 ; iCount<6;iCount++)
	{
		EXTI_LINE=1<<(10+iCount);
		if(EXTI_GetITStatus(EXTI_LINE) != RESET)
		{
			if(tGpioIrqHandler[10+iCount] != NULL )
				tGpioIrqHandler[10+iCount]();
			/* Clear the Key Button EXTI line pending bit */
			EXTI_ClearITPendingBit(EXTI_LINE);
		}
	}
}
