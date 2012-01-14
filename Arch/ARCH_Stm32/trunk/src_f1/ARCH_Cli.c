#include "stm32f10x.h"

char ARCH_Cli_Reset( int *pState , char *pcArgs , char *pcOutput , int iOutputLen)
{
	NVIC_SystemReset();
	return 0;
}
