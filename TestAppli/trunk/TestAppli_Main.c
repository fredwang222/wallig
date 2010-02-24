#include <unistd.h>
#include "DRV_Uart.h"
#include "DRV_SoftTimer.h"

void IpStack_Thread_init( void );
 int IpStack_Thread_Call( void );

int main()
{
	//Drivers init
	DRV_Uart_Init( );
	DRV_SoftTimer_Init();
	IpStack_Thread_init();

	while(1)
	{
		IpStack_Thread_Call();
		usleep(2*1000);
	}
	return 0;

}
