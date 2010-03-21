/**
	\file DRV_SoftTimer_Stm32.c

	\author Gwendal Le Gall

	\date 20/03/2010

	\section SoftTimerStm32 Stm32 implementation of the main timer



*/
#include <DRV_SoftTimer.h>
#include "stm32f10x_lib.h"
#include "Inc/DRV_SoftTimer_private.h"

/**************************************************************
                                        Local variables
***************************************************************/

/**************************************************************
                 private Functions
***************************************************************/
void *MaintTimer( void * arg );

void DRV_SoftTimer_SafeEnter( void )
{
	 SysTick_ITConfig(DISABLE);
}
void DRV_SoftTimer_SafeLeave( void )
{
	 SysTick_ITConfig(ENABLE);

}

DRV_SoftTimer_Error DRV_SofTimer_MainInit(void )
{
	/* SysTick end of count event each 1ms with input clock equal to 9MHz (HCLK/8, default) */
	  SysTick_SetReload(9000);
	  SysTick_ITConfig(ENABLE);
	  /* Enable SysTick interrupt */
	  SysTick_ITConfig(ENABLE);
  return SOFTTIMER_No_Error;
}

DRV_SoftTimer_Error DRV_SofTimer_MainTerminate(void )
{
    return SOFTTIMER_No_Error;
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

       //test the end of the thread
      DRV_Softimer_TimerCallBack();
}
