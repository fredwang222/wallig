#include "sys/AT91SAM7S64.h"                    /* AT91SAMT7S64 definitions */
#include "sys/Board.h"

AT91S_PIO * pPIO = AT91C_BASE_PIOA;         /* Global Pointer to PIO */

void vFsysManager_Init( void )
{
	// enable reset-button (mt)
	// AT91F_RSTSetMode( AT91C_BASE_RSTC , AT91C_RSTC_URSTEN );
	*AT91C_RSTC_RMR = ( 0xA5000000 | AT91C_RSTC_URSTEN );
	
	*AT91C_PMC_PCER = (1 << AT91C_ID_PIOA) |  /* Enable Clock for PIO    */
		(1 << AT91C_ID_IRQ0) |  /* Enable Clock for IRQ0   */
		(1 << AT91C_ID_US0);    /* Enable Clock for USART0 */
}
