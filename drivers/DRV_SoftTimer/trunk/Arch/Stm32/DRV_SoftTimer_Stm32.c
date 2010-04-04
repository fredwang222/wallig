/**
	\file DRV_SoftTimer_Stm32.c

	\author Gwendal Le Gall

	\date 20/03/2010

	\section SoftTimerStm32 Stm32 implementation of the main timer



*/
#include <DRV_SoftTimer.h>
#include "stm32f10x.h"
#include "Inc/DRV_SoftTimer_private.h"

/**************************************************************
                                        Local variables
***************************************************************/

/**************************************************************
                 private Functions
***************************************************************/

void DRV_SoftTimer_SafeEnter( void )
{
	// SysTick_ITConfig(DISABLE);
}
void DRV_SoftTimer_SafeLeave( void )
{
	// SysTick_ITConfig(ENABLE);

}

DRV_SoftTimer_Error DRV_SofTimer_MainInit(void )
{
	/* SysTick end of count event each 1ms with input clock equal to 9MHz (HCLK/8, default) */
	SysTick_Config(SystemCoreClock / 1000);
  return SOFTTIMER_No_Error;
}

DRV_SoftTimer_Error DRV_SofTimer_MainTerminate(void )
{
    return SOFTTIMER_No_Error;
}

static vu32 TimingDelay;

/*******************************************************************************
* Function Name  : SysTickHandler
* Description    : This function handles SysTick Handler.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SysTickHandler(void)
{
     DRV_Softimer_TimerCallBack();
	/*if (TimingDelay != 0x00)
	  {
	    TimingDelay--;
	  }*/
}
#if 0
void Delay(u32 nTime)
{
  /* Enable the SysTick Counter */
  SysTick_CounterCmd(SysTick_Counter_Enable);

  TimingDelay = nTime;

  while(TimingDelay != 0);

  /* Disable SysTick Counter */
  SysTick_CounterCmd(SysTick_Counter_Disable);
  /* Clear SysTick Counter */
  SysTick_CounterCmd(SysTick_Counter_Clear);
}
#endif
