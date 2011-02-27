/*
 * 	 Author : Gwendal Le Gall
 *   Date 02/02/2010
 *
 *    This file is part of Wallig Library and Drivers.
 *
 *    Copyright (C) 2010  Gwendal Le Gall
 *
 *    This program is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include <string.h>
#include "stm32f10x.h"
#include "../../DRV_Gpio.h"
#include "DRV_Gpio_CFG.h"
#include "Arch/DRV_Gpio_Arch.h"

#define GPIO_DEVICE_COUNT ( sizeof(GPIO_DeviceCfg)/sizeof(GPIO_Cfg_t))

/*!
 *  \brief GPIO identification
 */
typedef struct
{
	GPIO_TypeDef* PORT;
	u16 Pio;
} DRV_GpioID;

typedef struct
{
	char *pcName;
	DRV_GpioID Id;
	GPIOSpeed_TypeDef Speed;
	GPIOMode_TypeDef Mode;
	uint8_t PortSource;
} GPIO_Cfg_t;

typedef struct
{
	const GPIO_Cfg_t *pCfg;
	DRV_GpioINT Int;
	DRV_Gpio_Device_State eState;
} GPIO_Data_t;


typedef void (*GPIO_Handler_t)(void);

GPIO_Handler_t GPIO_IrqHandler[16];
static const GPIO_Cfg_t  GPIO_DeviceCfg[]= DRV_GPIO_INIT_CFG;
static GPIO_Data_t GPIO_DevicesData[GPIO_DEVICE_COUNT];

DRV_Gpio_Error DRV_Gpio_Init( void )
{
	int iDevicecount;

	for( iDevicecount = 0 ; iDevicecount < GPIO_DEVICE_COUNT ; iDevicecount++)
	{
		GPIO_DevicesData[iDevicecount].pCfg = &GPIO_DeviceCfg[iDevicecount];
		GPIO_DevicesData[iDevicecount].eState = Gpio_Device_Close;
		GPIO_DevicesData[iDevicecount].Int.CallBack=NULL;
		GPIO_DevicesData[iDevicecount].Int.EventType = GPIO_INT_None;
	}

	 /* Enable GPIOB clock */
	  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA  | RCC_APB2Periph_AFIO , ENABLE);
	  /* Enable GPIOB clock */
  	  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
  	/* Enable GPIOC clock */
	  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	  memset( GPIO_IrqHandler , 0 , sizeof(GPIO_IrqHandler));
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
	GPIO_Data_t *pGpioData;
	EXTI_InitTypeDef EXTI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	DRV_Gpio_Error eError=GPIO_Device_Not_Found;

	int iCount;
	unsigned int uiPio;
	unsigned int uiPORT;

	for ( iGpioIndex = 0 ; iGpioIndex < GPIO_DEVICE_COUNT ; iGpioIndex++ )
	{
		if( !strcmp( Name , GPIO_DevicesData[iGpioIndex].pCfg->pcName) )
		{
			pGpioData = &GPIO_DevicesData[iGpioIndex];
			eError=GPIO_No_Error;
			break;
		}
	}
	if( eError==GPIO_Device_Not_Found )
		return eError;
	if( pGpioData->eState != Gpio_Device_Close )
		return GPIO_AlreadyOpened;
	*phDeviceHandle=(DRV_Gpio_Handle)pGpioData;
	switch((u32)pGpioData->pCfg->Id.PORT)
	{
		case (u32)GPIOA:
			uiPORT=0;
			break;
		case (u32)GPIOB:
			uiPORT=1;
			break;
		case (u32)GPIOC:
			uiPORT=2;
			break;
	}

	GPIO_StructInit(&GPIO_InitStructure);
	uiPio=pGpioData->pCfg->Id.Pio;
	for( iCount=0; !(uiPio&1);iCount++)
		uiPio>>=1;
	uiPio=iCount;
	GPIO_InitStructure.GPIO_Pin = pGpioData->pCfg->Id.Pio;
	GPIO_InitStructure.GPIO_Speed = pGpioData->pCfg->Speed;
	GPIO_InitStructure.GPIO_Mode = pGpioData->pCfg->Mode;
	GPIO_Init(pGpioData->pCfg->Id.PORT, &GPIO_InitStructure);

	if( pGpioIntCfg != NULL )
		pGpioData->Int = *pGpioIntCfg;

	  /* Interrupt*/
	  if( pGpioData->Int.EventType != GPIO_INT_None && pGpioData->Int.CallBack != NULL)
	  {

		  /* Configure the EXTI0 Interrupt */
		 	  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
		 	  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
		 	  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		 	 GPIO_IrqHandler[uiPio] = pGpioData->Int.CallBack;
		 	  switch( pGpioData->pCfg->Id.Pio )
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
		    GPIO_EXTILineConfig(pGpioData->pCfg->PortSource ,pGpioData->pCfg->Id.Pio);
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
			EXTI_InitStructure.EXTI_Line = pGpioData->pCfg->Id.Pio;
			EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
			EXTI_InitStructure.EXTI_LineCmd = ENABLE;
			EXTI_Init(&EXTI_InitStructure);


	  }

	  return eError;

}

DRV_Gpio_Error DRV_Gpio_Close( DRV_Gpio_Handle hDeviceHandle )
{
	GPIO_DeInit(((GPIO_Data_t *)hDeviceHandle)->pCfg->Id.PORT);
	return GPIO_No_Error;
}

BOOL DRV_Gpio_ValueGet( DRV_Gpio_Handle hDeviceHandle )
{
 return (BOOL) GPIO_ReadInputDataBit( ((GPIO_Data_t *)hDeviceHandle)->pCfg->Id.PORT, ((GPIO_Data_t *)hDeviceHandle)->pCfg->Id.Pio );
}


DRV_Gpio_Error DRV_Gpio_ValueSet( DRV_Gpio_Handle hDeviceHandle , BOOL bValue)
{
	if( bValue )
		GPIO_SetBits(((GPIO_Data_t*)hDeviceHandle)->pCfg->Id.PORT, ((GPIO_Data_t*)hDeviceHandle)->pCfg->Id.Pio);
	else
		GPIO_ResetBits(((GPIO_Data_t*)hDeviceHandle)->pCfg->Id.PORT, ((GPIO_Data_t*)hDeviceHandle)->pCfg->Id.Pio);
	return GPIO_No_Error;
}

DRV_Gpio_Error DRV_Gpio_TypeSet( DRV_Gpio_Handle hDeviceHandle , char eType)
{
	return GPIO_No_Error;
}

DRV_Gpio_Error DRV_Gpio_OptionSet( DRV_Gpio_Handle hDeviceHandle , unsigned int Mode)
{
	return GPIO_No_Error;
}

DRV_Gpio_Error DRV_Gpio_OptionReSet( DRV_Gpio_Handle hDeviceHandle , unsigned int Mode)
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
		/* Clear the Key Button EXTI line pending bit */
		EXTI_ClearITPendingBit(EXTI_Line0);

		if(GPIO_IrqHandler[0] != NULL )
		GPIO_IrqHandler[0]();
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
		/* Clear the Key Button EXTI line pending bit */
		EXTI_ClearITPendingBit(EXTI_Line1);
		if(GPIO_IrqHandler[1] != NULL )
			GPIO_IrqHandler[1]();

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
			/* Clear the Key Button EXTI line pending bit */
			EXTI_ClearITPendingBit(EXTI_Line2);

			if(GPIO_IrqHandler[2] != NULL )
				GPIO_IrqHandler[2]();
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
		/* Clear the Key Button EXTI line pending bit */
		EXTI_ClearITPendingBit(EXTI_Line3);
		if(GPIO_IrqHandler[3] != NULL )
			GPIO_IrqHandler[3]();

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
		/* Clear the Key Button EXTI line pending bit */
		EXTI_ClearITPendingBit(EXTI_Line4);
		if(GPIO_IrqHandler[4] != NULL )
			GPIO_IrqHandler[4]();

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
			/* Clear the Key Button EXTI line pending bit */
			EXTI_ClearITPendingBit(EXTI_LINE);
			if(GPIO_IrqHandler[5+iCount] != NULL )
				GPIO_IrqHandler[5+iCount]();

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
			/* Clear the Key Button EXTI line pending bit */
			EXTI_ClearITPendingBit(EXTI_LINE);
			if(GPIO_IrqHandler[10+iCount] != NULL )
				GPIO_IrqHandler[10+iCount]();

		}
	}
}
