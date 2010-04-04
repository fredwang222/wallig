#include <stdio.h>
#include <DRV_SoftTimer.h>

char DRV_SoftTimer_Cli_uptime( int *pState , char *pcArgs , char *pcOutput , int iOutputLen)
{
	unsigned int uiUptime =	DRV_SoftTimer_Getsysticks();

	snprintf(pcOutput,iOutputLen,"Uptime: %d h %02d m %02d s %03d ms\r", (uiUptime/1000/60/60), (uiUptime/1000/60)%60, (uiUptime/1000)%60, uiUptime%1000);

	return 0;
}
