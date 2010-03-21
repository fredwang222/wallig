#include "stm32f10x_lib.h"
#include "../../DRV_Gpio.h"

DRV_Gpio_Cfg GpioHandles[16*3];
int iPioCount;

DRV_Gpio_Error DRV_Gpio_Init( void )
{
	  iPioCount=0;
	 /* Enable GPIOB clock */
	  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	  /* Enable GPIOB clock */
  	  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
  	return GPIO_No_Error;

}

DRV_Gpio_Error DRV_Gpio_Terminate( void )
{
	 /* Enable GPIOB clock */
	  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, DISABLE);
	  /* Enable GPIOB clock */

	  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, DISABLE);
	  return GPIO_No_Error;

}

DRV_Gpio_Error DRV_Gpio_Open( DRV_Gpio_Handle *phDeviceHandle , DRV_Gpio_Cfg *ptSettings)
{
	EXTI_InitTypeDef EXTI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	DRV_Gpio_Error eError=GPIO_No_Error;

	int iCount;
	unsigned int uiPio=ptSettings->Id.Pio;
	unsigned int uiBank;
	switch((u32)ptSettings->Id.Bank)
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

	for( iCount=0; !(uiPio&1);iCount++)
		uiPio>>=1;
	uiPio=iCount;
	  *phDeviceHandle=(DRV_Gpio_Handle)&GpioHandles[uiPio+16*uiBank];

	  GPIO_InitStructure.GPIO_Pin = ptSettings->Id.Pio ;
	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	  GPIO_InitStructure.GPIO_Mode = ptSettings->uiOption;
	  GPIO_Init(ptSettings->Id.Bank, &GPIO_InitStructure);

	  /* Interrupt*/
	  if( ptSettings->Int.EventType != GPIO_INT_None )
	  {

		  /* Configure the EXTI0 Interrupt */
		 	  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
		 	  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
		 	  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		 	  switch( ptSettings->Id.Pio )
		 	  {
				  case GPIO_Pin_0:
					  NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQChannel;
					  break;
				  case GPIO_Pin_1:
					  NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQChannel;
					  break;
				  case GPIO_Pin_2:
					  NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQChannel;
					  break;
				  case GPIO_Pin_3:
					  NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQChannel;
					  break;
				  case GPIO_Pin_4:
					  NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQChannel;
					  break;
				  case GPIO_Pin_5:
				  case GPIO_Pin_6:
				  case GPIO_Pin_7:
				  case GPIO_Pin_8:
				  case GPIO_Pin_9:
					  NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQChannel;
					  break;
				  case GPIO_Pin_10:
				  case GPIO_Pin_11:
				  case GPIO_Pin_12:
				  case GPIO_Pin_13:
				  case GPIO_Pin_14:
				  case GPIO_Pin_15:
					  NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQChannel;
					  break;
		 	  }
		 	  NVIC_Init(&NVIC_InitStructure);

			/* Connect PIO EXTI Line to Key Button GPIO Pin */
		    GPIO_EXTILineConfig(uiBank ,uiPio);
		    /* Configure PIO EXTI Line to generate an interrupt  */
			switch( ptSettings->Int.EventType )
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
			EXTI_InitStructure.EXTI_Line = ptSettings->Id.Pio;
			EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
			EXTI_InitStructure.EXTI_LineCmd = ENABLE;
			EXTI_Init(&EXTI_InitStructure);
	  }

	  return eError;

}

DRV_Gpio_Error DRV_Gpio_Close( DRV_Gpio_Handle hDeviceHandle )
{
	GPIO_DeInit(((DRV_Gpio_Cfg *)hDeviceHandle)->Id.Bank);
	return GPIO_No_Error;
}

BOOL DRV_Gpio_ValueGet( DRV_Gpio_Handle hDeviceHandle )
{
 return (BOOL) GPIO_ReadOutputDataBit( ((DRV_Gpio_Cfg *)hDeviceHandle)->Id.Bank, ((DRV_Gpio_Cfg *)hDeviceHandle)->Id.Pio );
}


DRV_Gpio_Error DRV_Gpio_ValueSet( DRV_Gpio_Handle hDeviceHandle , BOOL bValue)
{
	if( bValue )
		GPIO_SetBits(((DRV_Gpio_Cfg *)hDeviceHandle)->Id.Bank, ((DRV_Gpio_Cfg *)hDeviceHandle)->Id.Pio);
	else
		GPIO_ResetBits(((DRV_Gpio_Cfg *)hDeviceHandle)->Id.Bank, ((DRV_Gpio_Cfg *)hDeviceHandle)->Id.Pio);
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
		if(GpioHandles[0].Int.CallBack)
			GpioHandles[0].Int.CallBack();
		if(GpioHandles[16].Int.CallBack)
			GpioHandles[16].Int.CallBack();
		if(GpioHandles[32].Int.CallBack)
			GpioHandles[32].Int.CallBack();
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
		if(GpioHandles[1].Int.CallBack)
			GpioHandles[1].Int.CallBack();
		if(GpioHandles[17].Int.CallBack)
			GpioHandles[17].Int.CallBack();
		if(GpioHandles[33].Int.CallBack)
			GpioHandles[33].Int.CallBack();
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
			if(GpioHandles[2].Int.CallBack)
				GpioHandles[2].Int.CallBack();
			if(GpioHandles[18].Int.CallBack)
				GpioHandles[18].Int.CallBack();
			if(GpioHandles[34].Int.CallBack)
				GpioHandles[34].Int.CallBack();
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
		if(GpioHandles[3].Int.CallBack)
			GpioHandles[3].Int.CallBack();
		if(GpioHandles[19].Int.CallBack)
			GpioHandles[19].Int.CallBack();
		if(GpioHandles[35].Int.CallBack)
			GpioHandles[35].Int.CallBack();
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
		if(GpioHandles[4].Int.CallBack)
			GpioHandles[4].Int.CallBack();
		if(GpioHandles[20].Int.CallBack)
			GpioHandles[20].Int.CallBack();
		if(GpioHandles[36].Int.CallBack)
			GpioHandles[36].Int.CallBack();
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
			if(GpioHandles[5+iCount].Int.CallBack)
				GpioHandles[5+iCount].Int.CallBack();
			if(GpioHandles[21+iCount].Int.CallBack)
				GpioHandles[21+iCount].Int.CallBack();
			if(GpioHandles[37+iCount].Int.CallBack)
				GpioHandles[37+iCount].Int.CallBack();
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
			if(GpioHandles[10+iCount].Int.CallBack)
				GpioHandles[10+iCount].Int.CallBack();
			if(GpioHandles[26+iCount].Int.CallBack)
				GpioHandles[26+iCount].Int.CallBack();
			if(GpioHandles[42+iCount].Int.CallBack)
				GpioHandles[42+iCount].Int.CallBack();
			/* Clear the Key Button EXTI line pending bit */
			EXTI_ClearITPendingBit(EXTI_LINE);
		}
	}
}
