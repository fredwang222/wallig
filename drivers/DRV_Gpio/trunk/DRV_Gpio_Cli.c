#include <string.h>
#include "DRV_Gpio.h"

DRV_Gpio_Handle hPio=NULL;

char DRV_Gpio_Cli_open( int *pState , char *pcArgs , char *pcOutput , int iOutputLen)
{
	char *pcChar;

	pcChar = strchr( pcArgs , ' ');
	if(pcChar == NULL || pcChar[1]==0)
	{
		strncpy( pcOutput , "An argument is needed: the pio name\r" , iOutputLen);
		return 0;
	}
	pcChar++;
	if (DRV_Gpio_Open( pcChar ,&hPio,NULL) != GPIO_No_Error )
	{
		strncpy( pcOutput , "Error while opening\r" , iOutputLen);
	}
	else
	{
		strncpy( pcOutput , "Opening Ok\r" , iOutputLen);
	}
	return 0;
}


char DRV_Gpio_Cli_Set( int *pState , char *pcArgs , char *pcOutput , int iOutputLen)
{
	char *pcChar;

	if( hPio == NULL )
	{
		strncpy( pcOutput , "Pio not open\r" , iOutputLen);
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
		switch( pcChar[0])
		{
			case '0':
				DRV_Gpio_ValueSet(hPio, 0);
				break;
			case '1':
				DRV_Gpio_ValueSet(hPio, 1);
				break;
			default:
				strncpy( pcOutput , "Input Value Not supported\r" , iOutputLen);
		}
	}
	return 0;
}

char DRV_Gpio_Cli_Get( int *pState , char *pcArgs , char *pcOutput , int iOutputLen)
{


	if( hPio == NULL )
	{
		strncpy( pcOutput , "Pio not open\r" , iOutputLen);
	}
	else
	{
		strncpy( pcOutput , "Pio Value: " , iOutputLen);
		if( DRV_Gpio_ValueGet(hPio) )
			strcat(pcOutput , "1 \r");
		else
			strcat(pcOutput , "0 \r");

	}
	return 0;
}
