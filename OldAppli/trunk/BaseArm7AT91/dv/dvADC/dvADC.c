#include "sys/AT91SAM7S64.h"                    /* AT91SAMT7S64 definitions */
#include "sys/lib_AT91SAM7S64.h"                    /* AT91SAMT7S64 definitions */
#include "sys/Board.h"
#include "dv/dvISR/interrupt_utils.h"


void vFdvADCInit( void )
{
	//First configure TC 0
	AT91C_BASE_TC0->TC_CCR = AT91C_TC_CLKDIS; // Disable TC clock
	AT91C_BASE_TC0->TC_IDR = 0xFFFFFFFF; // Disable interrupts
	AT91C_BASE_TC0->TC_SR; // Clear status register
	AT91C_BASE_TC0->TC_CMR = AT91C_TC_CLKS_TIMER_DIV5_CLOCK | AT91C_TC_CPCTRG | AT91C_TC_WAVE | AT91C_TC_ACPC_TOGGLE; // Set mode
	// Only really interested in interrupts when the RC happens
	AT91C_BASE_TC0->TC_IDR = 0xFF;
	AT91C_BASE_TC0->TC_IER = AT91C_TC_CPCS; // Enables the RC Compare Interrupt.
    AT91C_BASE_TC0->TC_RC = 500; // load the RC value with something


}
