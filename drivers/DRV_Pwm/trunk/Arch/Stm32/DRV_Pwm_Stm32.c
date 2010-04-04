#include <string.h>
#include "stm32f10x.h"
#include "../../DRV_Pwm.h"
#include "DRV_Pwm_Cfg.h"

#define PWM_DEVICE_COUNT ( sizeof(tPWM_DeviceData)/sizeof(DRV_Pwm_Device_Data))

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
	unsigned short usDutycycle;

} DRV_Pwm_Device_Data;

static DRV_Pwm_Device_Data tPWM_DeviceData[] = DRV_PWM_INIT_DATA;
void DRV_Pwmm_GPIO_config( DRV_Pwm_PioCfg *pGpioCFG);
void DRV_Pwm_Init(void )
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

	/* PWM_TIMER clock enable */
	RCC_APB1PeriphClockCmd(PWM_TIMER_CLOCK, ENABLE);

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

DRV_Pwm_Error DRV_Pwm_Open( char *pcName , DRV_Pwm_Handle *pHandle)
{
	DRV_Pwm_Error eError = Pwm_Device_Not_Found;
	int iPwmIndex;
	DRV_Pwm_Device_Data *pPwmData;
	TIM_OCInitTypeDef  TIM_OCInitStructure;


	for ( iPwmIndex = 0 ; iPwmIndex < PWM_DEVICE_COUNT ; iPwmIndex++ )
	{
		if( !strcmp( pcName , tPWM_DeviceData[iPwmIndex].pcName) )
		{
			pPwmData = &tPWM_DeviceData[iPwmIndex];
			eError=Pwm_No_Error;
			break;
		}
	}
	if( eError==Pwm_Device_Not_Found )
		return eError;
	*pHandle=(DRV_Pwm_Handle) pPwmData;
	DRV_Pwmm_GPIO_config(&pPwmData->PioCfg);
	/* PWM1 Mode configuration: Channel */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = pPwmData->usDutycycle;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

	switch( pPwmData->cChannel)
	{
		case 1:
			TIM_OC1Init(PWM_TIMER, &TIM_OCInitStructure);
			TIM_OC1PreloadConfig(PWM_TIMER, TIM_OCPreload_Enable);
			break;
		case 2:
			TIM_OC2Init(PWM_TIMER, &TIM_OCInitStructure);
			TIM_OC2PreloadConfig(PWM_TIMER, TIM_OCPreload_Enable);
			break;
		case 3:
			TIM_OC3Init(PWM_TIMER, &TIM_OCInitStructure);
			TIM_OC3PreloadConfig(PWM_TIMER, TIM_OCPreload_Enable);
			break;
		case 4:
			TIM_OC4Init(PWM_TIMER, &TIM_OCInitStructure);
			TIM_OC4PreloadConfig(PWM_TIMER, TIM_OCPreload_Enable);
			break;
		default:
			;
	}


	return eError;

}

void DRV_Pwm_Enable( DRV_Pwm_Handle Handle )
{

}

void DRV_Pwm_Disable( DRV_Pwm_Handle Handle )
{

}

void DRV_Pwm_DutyCycleSet( DRV_Pwm_Handle Handle , unsigned short usDutyCycle)
{
	DRV_Pwm_Device_Data *pDeviceData =(DRV_Pwm_Device_Data *)Handle;

	switch( pDeviceData->cChannel)
	{
		case 1:
			TIM_SetCompare1( PWM_TIMER , usDutyCycle);
			break;
		case 2:
			TIM_SetCompare2( PWM_TIMER , usDutyCycle);
			break;
		case 3:
			TIM_SetCompare3( PWM_TIMER , usDutyCycle);
			break;
		case 4:
			TIM_SetCompare4( PWM_TIMER , usDutyCycle);
			break;
		default:
			;
	}
}
void DRV_Pwmm_GPIO_config( DRV_Pwm_PioCfg *pGpioCFG)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	/* Configure alternate output for GPIO*/
	GPIO_InitStructure.GPIO_Pin = pGpioCFG->Pin ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_Init(pGpioCFG->PORT , &GPIO_InitStructure);
	GPIO_PinRemapConfig(pGpioCFG->GPIO_Remap , pGpioCFG->NewState);
}
