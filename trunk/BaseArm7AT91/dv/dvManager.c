#include "dvManager.h"
void vFdvUart0_init (void);
void vFdvManager_Init( void )
{
	
	vFdvTimer_init ();                            /* Initialize Timer */
//	vFdvExtInt_init();                            /* Initialize External Interrupt */
//	vFdvUart_init();                             /* Initialize Serial Interface */
	vFdvPWM_init();
	
}
