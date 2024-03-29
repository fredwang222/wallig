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
extern uint8_t  USB_Tx_State;

void DRV_Vcp_Init( void )
{
	DRV_Vcp_Data.Rxcallback=NULL;
	DRV_Vcp_Data.Txcallback=NULL;
}

void DRV_Vcp_CallbackRegister( void (*Rxcallback)(uint8_t*, uint8_t),void (*Txcallback)(void))
{
	DRV_Vcp_Data.Rxcallback =Rxcallback;
	DRV_Vcp_Data.Txcallback =Txcallback;
}

uint8_t DRV_Vcp_TxBusy( void )
{
 return USB_Tx_State;
}
