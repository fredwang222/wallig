/*
 * DRV_Vcp.c
 *
 *  Created on: 5 févr. 2011
 *      Author: cara
 */
#include <stdint.h>
#include <stdlib.h>
#include "DRV_Vcp_p.h"

tDRV_Vcp_Data DRV_Vcp_Data;

void DRV_Vcp_Init( void )
{
	DRV_Vcp_Data.callback=NULL;
}

void DRV_Vcp_CallbackRegister( void (*callback)(uint8_t*, uint8_t))
{
	DRV_Vcp_Data.callback= callback;
}
