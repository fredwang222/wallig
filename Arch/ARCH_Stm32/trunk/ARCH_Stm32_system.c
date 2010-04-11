#include <stdio.h>
#include "stm32f10x.h"
#include "misc.h"
#include "system_stm32f10x.h"
#include "ARCH_Stm32_system.h"
ErrorStatus HSEStartUpStatus;

/* Private function prototypes -----------------------------------------------*/
void RCC_Configuration(void);
void NVIC_Configuration(void);
void Delay(vu32 nCount);

void System_init( void )
{
	 /*  Setup the microcontroller system */
	  SystemInit();
	  /* Configure the system clocks */
	  //RCC_Configuration();

	  /* NVIC Configuration */
	  NVIC_Configuration();
}


/*******************************************************************************
* Function Name  : RCC_Configuration
* Description    : Configures the different system clocks.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
#if 0
void RCC_Configuration(void)
{
  /* RCC system reset(for debug purpose) */
  RCC_DeInit();

  /* Enable HSE */
  RCC_HSEConfig(RCC_HSE_ON);

  /* Wait till HSE is ready */
  HSEStartUpStatus = RCC_WaitForHSEStartUp();

  if(HSEStartUpStatus == SUCCESS)
  {
    /* Enable Prefetch Buffer */
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);

    /* Flash 2 wait state */
    FLASH_SetLatency(FLASH_Latency_2);

    /* HCLK = SYSCLK */
    RCC_HCLKConfig(RCC_SYSCLK_Div1);

    /* PCLK2 = HCLK */
    RCC_PCLK2Config(RCC_HCLK_Div1);

    /* PCLK1 = HCLK/2 */
    RCC_PCLK1Config(RCC_HCLK_Div2);

    /* PLLCLK = 8MHz * 9 = 72 MHz */
    RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);

    /* Enable PLL */
    RCC_PLLCmd(ENABLE);

    /* Wait till PLL is ready */
    while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
    {
    }

    /* Select PLL as system clock source */
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

    /* Wait till PLL is used as system clock source */
    while(RCC_GetSYSCLKSource() != 0x08)
    {
    }
  }
}
#endif
/*******************************************************************************
* Function Name  : NVIC_Configuration
* Description    : Configures Vector Table base location.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void NVIC_Configuration(void)
{
#ifdef  VECT_TAB_RAM
  /* Set the Vector Table base location at 0x20000000 */
  NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0);
#else  /* VECT_TAB_FLASH  */
  /* Set the Vector Table base location at 0x08000000 */
  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);
#endif
}

/*******************************************************************************
* Function Name  : Delay
* Description    : Inserts a delay time.
* Input          : nCount: specifies the delay time length.
* Output         : None
* Return         : None
*******************************************************************************/
void Delay(vu32 nCount)
{
  for(; nCount != 0; nCount--);
}


/* void * _sbrk (struct _reent * a , ptrdiff_t b)
{

}*/
/**** Locally used variables. ****/
// mt: "cleaner": extern char* end;
extern char end[];              /*  end is set in the linker command 	*/
				/* file and is the end of statically 	*/
				/* allocated data (thus start of heap).	*/

static char *heap_ptr;		/* Points to current end of the heap.	*/
 /************************** _sbrk_r *************************************
  * Support function. Adjusts end of heap to provide more memory to
  * memory allocator. Simple and dumb with no sanity checks.

  *  struct _reent *r -- re-entrancy structure, used by newlib to
  *                      support multiple threads of operation.
  *  ptrdiff_t nbytes -- number of bytes to add.
  *                      Returns pointer to start of new heap area.
  *
  *  Note:  This implementation is not thread safe (despite taking a
  *         _reent structure as a parameter).
  *         Since _s_r is not used in the current implementation,
  *         the following messages must be suppressed.
  */
 void * _sbrk_r(
     struct _reent *_s_r,
     ptrdiff_t nbytes)
 {
 	char  *base;		/*  errno should be set to  ENOMEM on error  */

 	if (!heap_ptr) {	/*  Initialize if first time through.  */
 		heap_ptr = end;
 	}
 	base = heap_ptr;	/*  Point to end of heap.  */
 	heap_ptr += nbytes;	/*  Increase heap.  */

 	return base;		/*  Return pointer to start of new heap area.  */
 }


 typedef struct
 {
 	IrqTimerHandler_t Handler;
 	void *pvParam;

 } SYS_Timer_Irq_Data_t;

 SYS_Timer_Irq_Data_t IRQ_HandlerList[7];

 void Sys_TimerIrq_Init( void)
 {
 	int iHandlerIndex;

 	for( iHandlerIndex =0 ; iHandlerIndex < 4 ; iHandlerIndex++)
 	{
 		IRQ_HandlerList[iHandlerIndex].Handler=NULL;
 		IRQ_HandlerList[iHandlerIndex].pvParam=NULL;
 	}
 }
 void Sys_TimerIrq_Register(  uint8_t NVIC_IRQChannel , IrqTimerHandler_t Handler , void *pvParam)
 {
 	int iHandlerIndex;
 	switch( NVIC_IRQChannel )
 	{
 		case TIM1_BRK_IRQn:
 			iHandlerIndex=0;
 			break;
 		case TIM1_UP_IRQn:
 			iHandlerIndex=1;
 			break;
 		case TIM1_TRG_COM_IRQn:
 			iHandlerIndex=2;
 			break;
 		case TIM1_CC_IRQn:
 			iHandlerIndex=3;
 			break;
 		case TIM2_IRQn:
 			iHandlerIndex=4;
 			break;
 		case TIM3_IRQn:
 			iHandlerIndex=5;
 			break;
 		case TIM4_IRQn:
 			iHandlerIndex=6;
 			break;
 		default:
 			return;
 	}
 	IRQ_HandlerList[iHandlerIndex].Handler=Handler;
 	IRQ_HandlerList[iHandlerIndex].pvParam=pvParam;
 }


 void Sys_TimerIrq_Unregister(  uint8_t NVIC_IRQChannel )
 {
 	int iHandlerIndex;
 	switch( NVIC_IRQChannel )
 	{
 		case TIM1_BRK_IRQn:
 			iHandlerIndex=0;
 			break;
 		case TIM1_UP_IRQn:
 			iHandlerIndex=1;
 			break;
 		case TIM1_TRG_COM_IRQn:
 			iHandlerIndex=2;
 			break;
 		case TIM1_CC_IRQn:
 			iHandlerIndex=3;
 			break;
 		case TIM2_IRQn:
 			iHandlerIndex=4;
 			break;
 		case TIM3_IRQn:
 			iHandlerIndex=5;
 			break;
 		case TIM4_IRQn:
 			iHandlerIndex=6;
 			break;
 		default:
 			return;
 	}
 	IRQ_HandlerList[iHandlerIndex].Handler=NULL;
 	IRQ_HandlerList[iHandlerIndex].pvParam=NULL;

 }
 void TIM1_BRK_IRQHandler(void)
 {
 	if( IRQ_HandlerList[0].Handler != NULL )
 		IRQ_HandlerList[0].Handler(IRQ_HandlerList[0].pvParam);
 }

 void TIM1_UP_IRQHandler(void)
 {
 	if( IRQ_HandlerList[1].Handler != NULL )
 		IRQ_HandlerList[1].Handler(IRQ_HandlerList[1].pvParam);
 }

 void TIM1_TRG_COM_IRQHandler(void)
 {
 	if( IRQ_HandlerList[2].Handler != NULL )
 		IRQ_HandlerList[2].Handler(IRQ_HandlerList[2].pvParam);
 }

 void TIM1_CC_IRQHandler(void)
 {
 	if( IRQ_HandlerList[3].Handler != NULL )
 		IRQ_HandlerList[3].Handler(IRQ_HandlerList[3].pvParam);
 }

 void TIM2_IRQHandler(void)
 {
 	if( IRQ_HandlerList[4].Handler != NULL )
 		IRQ_HandlerList[4].Handler(IRQ_HandlerList[4].pvParam);
 }

 void TIM3_IRQHandler(void)
 {
 	if( IRQ_HandlerList[5].Handler != NULL )
 		IRQ_HandlerList[5].Handler(IRQ_HandlerList[5].pvParam);
 }

 void TIM4_IRQHandler(void)
 {
 	if( IRQ_HandlerList[6].Handler != NULL )
 		IRQ_HandlerList[6].Handler(IRQ_HandlerList[6].pvParam);
 }
