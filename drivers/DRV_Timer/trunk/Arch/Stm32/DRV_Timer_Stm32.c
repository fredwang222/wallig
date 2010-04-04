#include "stm32f10x.h"

static uint32_t DRV_Timer_SysClock_Counter;

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
