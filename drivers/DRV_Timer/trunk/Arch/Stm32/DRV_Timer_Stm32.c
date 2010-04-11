#include <string.h>
#include "System.h"
#include "DRV_Timer.h"
#include "DRV_Timer_Cfg.h"

/********************************************************************************************************/
/*							Define																		*/
/********************************************************************************************************/
#define PERIODIC_DEVICE_COUNT (sizeof(Periodic_StaticCfg)/sizeof(DRV_Timer_Periodic_StaticCfg_t))

/********************************************************************************************************/
/*							Macro																		*/
/********************************************************************************************************/

/********************************************************************************************************/
/*							Typedef																		*/
/********************************************************************************************************/
typedef struct
{
	char *pcName;
	uint16_t OCx;
	void (*OCxInit)(TIM_TypeDef* TIMx, TIM_OCInitTypeDef* TIM_OCInitStruct);
	void (*OCxPreloadConfig)(TIM_TypeDef* TIMx, uint16_t TIM_OCPreload);
	uint16_t OCxIT;

} DRV_Timer_Periodic_StaticCfg_t;

typedef struct
{
	const DRV_Timer_Periodic_StaticCfg_t *pCfgStatic;
	DRV_Timer_Periodic_Cfg CfgDynamic;
	DRV_Timer_Device_State eState;
}
DRV_Timer_Periodic_Data_t;


/********************************************************************************************************/
/*							Local Variables																*/
/********************************************************************************************************/
/*
 * 				Systick variables
 */
static uint32_t DRV_Timer_SysClock_Counter;

/*
 * 				Periodic timer variables
 */

static const DRV_Timer_Periodic_StaticCfg_t  Periodic_StaticCfg[]=DRV_TIMER_PERIODIC_OCX_init;
static DRV_Timer_Periodic_Data_t PeriodicData[PERIODIC_DEVICE_COUNT];

/********************************************************************************************************/
/*							Local functions	declaration													*/
/********************************************************************************************************/
static void Timer_IRQ_Handler(void *pvParam);

/********************************************************************************************************/
/*							Public functions															*/
/********************************************************************************************************/

/*
 * 				Systick functions
 */

void DRV_Timer_SysClock_Init( void )
{
	DRV_Timer_SysClock_Counter=0;
	/* SysTick end of count event each 1ms with input clock equal to 9MHz (HCLK/8, default) */
	SysTick_Config(SystemCoreClock / 1000);

}

unsigned int DRV_Timer_SysClock_Get( void )
{

	return __LDREXW(&DRV_Timer_SysClock_Counter);
}

/*******************************************************************************
* Function Name  : SysTickHandler
* Description    : This function handles SysTick Handler.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SysTickHandler(void)
{
	DRV_Timer_SysClock_Counter++;
}


void DRV_Timer_Periodic_Init( void )
{
	int iTimerIndex;
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef TimerInit = DRV_TIMER_PERIODIC_MAIN_INIT;

	for (iTimerIndex=0 ; iTimerIndex<PERIODIC_DEVICE_COUNT ; iTimerIndex++)
	{
		PeriodicData[iTimerIndex].pCfgStatic = &Periodic_StaticCfg[iTimerIndex];
		PeriodicData[iTimerIndex].CfgDynamic.CallBack = NULL;
		PeriodicData[iTimerIndex].CfgDynamic.usPeriod=0;
		PeriodicData[iTimerIndex].eState = Timer_Device_Close;
	}

	/* TIM clock enable */
	DRV_TIMER_PERIODIC_TIMER_CLK_CMD(DRV_TIMER_PERIODIC_TIMER_CLK_ID, ENABLE);
	/* TIM initialization*/
	TIM_TimeBaseInit(DRV_TIMER_PERIODIC_TIMER, &TimerInit);
	/* Prescaler configuration */
	TIM_PrescalerConfig(DRV_TIMER_PERIODIC_TIMER, DRV_TIMER_PERIODIC_TIMER_PRESCALER, TIM_PSCReloadMode_Immediate);
	/* Register the timer IRQ handler*/
	Sys_TimerIrq_Register(DRV_TIMER_PERIODIC_TIMER_IRQ_ID,Timer_IRQ_Handler,NULL);
	/* Enable the TIM global Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = DRV_TIMER_PERIODIC_TIMER_IRQ_ID;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	TIM_Cmd(DRV_TIMER_PERIODIC_TIMER, ENABLE);
}

DRV_Timer_Error DRV_Timer_Periodic_Open( char *pcName , DRV_Timer_PeriodicTimer *pHandle,DRV_Timer_Periodic_Cfg *pCfg )
{
	DRV_Timer_Error eError=Timer_Device_Not_Found;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	DRV_Timer_Periodic_Data_t *pTimerData;
	int iTimerIndex;

	for ( iTimerIndex = 0 ; iTimerIndex < PERIODIC_DEVICE_COUNT ; iTimerIndex++ )
	{
		if( !strcmp( pcName , PeriodicData[iTimerIndex].pCfgStatic->pcName) )
		{
			pTimerData = &PeriodicData[iTimerIndex];
			eError=Timer_No_Error;
			break;
		}
	}
	if( eError==Timer_Device_Not_Found )
		return eError;
	if( pTimerData->eState == Timer_Device_Open)
		return Timer_AlreadyOpened;
	if(pCfg!= NULL)
		pTimerData->CfgDynamic = *pCfg;
	*pHandle = (DRV_Timer_PeriodicTimer)pTimerData;
	pTimerData->eState = Timer_Device_Open;
	/* Output Compare Timing Mode configuration: Channel1 */
	TIM_OCInitStructure.TIM_OCMode 		= TIM_OCMode_Timing;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse 		= pTimerData->CfgDynamic.usPeriod;
	TIM_OCInitStructure.TIM_OCPolarity 	= TIM_OCPolarity_High;

	TIM_OC1Init(DRV_TIMER_PERIODIC_TIMER, &TIM_OCInitStructure);
	TIM_OC1PreloadConfig(DRV_TIMER_PERIODIC_TIMER, TIM_OCPreload_Disable);

	return eError;
}

DRV_Timer_Error DRV_Timer_Periodic_Start(  DRV_Timer_PeriodicTimer Handle )
{
	DRV_Timer_Periodic_Data_t *pTimerData = (DRV_Timer_Periodic_Data_t *) Handle;
	DRV_Timer_Error eError=Timer_Bad_Param;

	if (( pTimerData != NULL ) && ( pTimerData->CfgDynamic.CallBack != NULL ) )
	{
		TIM_ITConfig(DRV_TIMER_PERIODIC_TIMER, pTimerData->pCfgStatic->OCx , ENABLE);
		eError=Timer_No_Error;
	}

	return eError;
}

DRV_Timer_Error DRV_Timer_Periodic_Stop(  DRV_Timer_PeriodicTimer Handle )
{
	DRV_Timer_Periodic_Data_t *pTimerData = (DRV_Timer_Periodic_Data_t *) Handle;
	DRV_Timer_Error eError=Timer_Bad_Param;

	if (( pTimerData != NULL ) && ( pTimerData->CfgDynamic.CallBack != NULL ) )
	{
		TIM_ITConfig(DRV_TIMER_PERIODIC_TIMER, pTimerData->pCfgStatic->OCx , DISABLE);
		eError=Timer_No_Error;
	}

	return eError;
}

/********************************************************************************************************/
/*							Local functions																*/
/********************************************************************************************************/

static void Timer_IRQ_Handler(void *pvParam)
{
	int iTimerIndex;
	uint16_t usCapture = 0;

	for (iTimerIndex=0 ; iTimerIndex<PERIODIC_DEVICE_COUNT ; iTimerIndex++)
	{

		if (TIM_GetITStatus(DRV_TIMER_PERIODIC_TIMER, PeriodicData[iTimerIndex].pCfgStatic->OCxIT) != RESET)
		{
			TIM_ClearITPendingBit(DRV_TIMER_PERIODIC_TIMER, PeriodicData[iTimerIndex].pCfgStatic->OCxIT);
			usCapture = TIM_GetCapture1(DRV_TIMER_PERIODIC_TIMER);
			TIM_SetCompare1(DRV_TIMER_PERIODIC_TIMER, usCapture + PeriodicData[iTimerIndex].CfgDynamic.usPeriod);
			if( PeriodicData[iTimerIndex].CfgDynamic.CallBack != NULL )
				PeriodicData[iTimerIndex].CfgDynamic.CallBack();
		}
	}
}



