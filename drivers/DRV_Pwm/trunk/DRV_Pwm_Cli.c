#include <string.h>
#include <stdlib.h>
#include "DRV_Pwm.h"

DRV_Pwm_Handle hPwm=NULL;

char DRV_Pwm_Cli_open( int *pState , char *pcArgs , char *pcOutput , int iOutputLen)
{
	char *pcChar;

	pcChar = strchr( pcArgs , ' ');
	if(pcChar == NULL || pcChar[1]==0)
	{
		strncpy( pcOutput , "An argument is needed: the pwm name\r" , iOutputLen);
		return 0;
	}
	pcChar++;
	if (DRV_Pwm_Open( pcChar ,&hPwm) != Pwm_No_Error )
	{
		strncpy( pcOutput , "Error while opening\r" , iOutputLen);
	}
	else
	{
		strncpy( pcOutput , "Opening Ok\r" , iOutputLen);
	}
	return 0;
}


char DRV_Pwm_Cli_Duty( int *pState , char *pcArgs , char *pcOutput , int iOutputLen)
{
	char *pcChar;
	unsigned int uiValue;
	unsigned short usDuty;

	if( hPwm == NULL )
	{
		strncpy( pcOutput , "Pwm not open\r" , iOutputLen);
	}
	else
	{
		pcChar = strchr( pcArgs , ' ');
		if(pcChar == NULL || pcChar[1]==0)
		{
			strncpy( pcOutput , "An argument is needed: the pio value\r" , iOutputLen);
			return 0;
		}
		pcChar++;
		uiValue=atoi(pcChar);
		uiValue = (PWM_DUTY_CYCLE_FULL*uiValue)/100;
		usDuty=(unsigned short) uiValue;
		DRV_Pwm_DutyCycleSet(hPwm , usDuty );
	}
	return 0;
}


