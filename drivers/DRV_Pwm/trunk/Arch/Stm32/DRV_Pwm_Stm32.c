#include <string.h>
#include "stm32f10x.h"
#include "../../DRV_Pwm.h"
#include "DRV_Pwm_Cfg.h"

#define PWM_DEVICE_COUNT ( sizeof(PWM_DeviceCfgListe)/sizeof(Pwm_Device_Cfg_t))

typedef struct
{
	GPIO_TypeDef* PORT;
	u16 Pin;
	uint32_t GPIO_Remap;
	FunctionalState NewState;
} DRV_Pwm_PioCfg;

typedef struct
{
	char *pcName;
	char cChannel;
	DRV_Pwm_PioCfg PioCfg;
	void (*OCxInit)(TIM_TypeDef* TIMx, TIM_OCInitTypeDef* TIM_OCInitStruct);
	void (*OCxPreloadConfig)(TIM_TypeDef* TIMx, uint16_t TIM_OCPreload);
	void (*SetCompareX)(TIM_TypeDef* TIMx, uint16_t Compare1);
}Pwm_Device_Cfg_t;
typedef struct
{
	const Pwm_Device_Cfg_t *pCfg;
	DRV_Pwm_Device_State eState;
	unsigned short usDutycycle;

} Pwm_Device_Data_t;

static const Pwm_Device_Cfg_t PWM_DeviceCfgListe[]=DRV_PWM_INIT_CFG;
static Pwm_Device_Data_t PWM_DeviceDataListe[PWM_DEVICE_COUNT];

void DRV_Pwmm_GPIO_config( const DRV_Pwm_PioCfg *pGpioCFG);
void DRV_Pwm_Init(void )
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

	int iDevicecount;

	for( iDevicecount = 0 ; iDevicecount < PWM_DEVICE_COUNT ; iDevicecount++)
	{
		PWM_DeviceDataListe[iDevicecount].pCfg = &PWM_DeviceCfgListe[iDevicecount];
		PWM_DeviceDataListe[iDevicecount].usDutycycle=0;
		PWM_DeviceDataListe[iDevicecount].eState = Pwm_Device_Close;
	}

	/* PWM_TIMER clock enable */
	PWM_TIMER_CLK_CMD(PWM_TIMER_CLOCK, ENABLE);

	/* Time base configuration */
	TIM_TimeBaseStructure.TIM_Period = PWM_DUTY_CYCLE_FULL;
	TIM_TimeBaseStructure.TIM_Prescaler = PWM_TIMER_PRESCALER;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(PWM_TIMER, &TIM_TimeBaseStructure);
	TIM_ARRPreloadConfig(PWM_TIMER, ENABLE);
	/* PWM_TIMER enable counter */
	TIM_Cmd(PWM_TIMER, ENABLE);

}

DRV_Pwm_Error DRV_Pwm_Open( const char *pcName , DRV_Pwm_Handle *pHandle ,const  unsigned short ucDutycycle)
{
	DRV_Pwm_Error eError = Pwm_Device_Not_Found;
	int iPwmIndex;
	Pwm_Device_Data_t *pPwmData;
	TIM_OCInitTypeDef  TIM_OCInitStructure;


	for ( iPwmIndex = 0 ; iPwmIndex < PWM_DEVICE_COUNT ; iPwmIndex++ )
	{
		if( !strcmp( pcName , PWM_DeviceDataListe[iPwmIndex].pCfg->pcName ) )
		{
			pPwmData = &PWM_DeviceDataListe[iPwmIndex];
			eError=Pwm_No_Error;
			break;
		}
	}
	if( eError==Pwm_Device_Not_Found )
		return eError;
	if(  pPwmData->eState != Pwm_Device_Close )
		return Pwm_AlreadyOpened;
	*pHandle=(DRV_Pwm_Handle) pPwmData;
	pPwmData->usDutycycle = (PWM_DUTY_CYCLE_FULL*ucDutycycle)/256; ;
	DRV_Pwmm_GPIO_config(&(pPwmData->pCfg->PioCfg));
	/* PWM1 Mode configuration: Channel */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = pPwmData->usDutycycle;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	pPwmData->pCfg->OCxInit(PWM_TIMER ,  &TIM_OCInitStructure);
	pPwmData->pCfg->OCxPreloadConfig(PWM_TIMER , TIM_OCPreload_Enable);

	return eError;
}

void DRV_Pwm_Enable( DRV_Pwm_Handle Handle )
{

}

void DRV_Pwm_Disable( DRV_Pwm_Handle Handle )
{

}

DRV_Pwm_Error DRV_Pwm_DutyCycleSet(const DRV_Pwm_Handle Handle , const unsigned char ucDutycycle)
{
	Pwm_Device_Data_t *pDeviceData =(Pwm_Device_Data_t *)Handle;

	if( pDeviceData != NULL )
	{
		pDeviceData->pCfg->SetCompareX(PWM_TIMER,(PWM_DUTY_CYCLE_FULL*ucDutycycle)/256);
	}
	else
		return Pwm_Bad_Param;

	return Pwm_No_Error;
}
void DRV_Pwmm_GPIO_config( const DRV_Pwm_PioCfg *pGpioCFG)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	/* Configure alternate output for GPIO*/
	GPIO_InitStructure.GPIO_Pin = pGpioCFG->Pin ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_Init(pGpioCFG->PORT , &GPIO_InitStructure);
	GPIO_PinRemapConfig(pGpioCFG->GPIO_Remap , pGpioCFG->NewState);
}
