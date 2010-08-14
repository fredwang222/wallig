/*
 * 	 Author : Gwendal Le Gall
 *   Date 02/02/2010
 *
 *    Modified from:
 *
 *    This file is part of Wallig Library and Drivers.
 *
 *    Copyright (C) 2010  Gwendal Le Gall
 *
 *    This program is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "inc/DRV_DM9000.h"
#include "DRV_Eth.h"
#include "uip.h"

DRV_Eth_Error DRV_Eth_Init( void )
{
	if( !dm9000_init() )
		return ETH_No_Error;
	else
		return ETH_Init_Error;
}

DRV_Eth_Error DRV_Eth_Send( unsigned char *pucSendBuffer , unsigned int uiLength )
{

	static u16_t i;
	  static u16_t j;
	  u8_t volatile *ptr; // general pointer for data
	  unsigned char tmpbuf[1536];

	  ptr = &pucSendBuffer[0];

	  if (pucSendBuffer[UIP_LLH_LEN + 9] == UIP_PROTO_TCP)
	  {
	    j = (UIP_LLH_LEN + UIP_TCPIP_HLEN + (((pucSendBuffer[UIP_LLH_LEN + 32] >> 4) - 5) << 2));
	  }
	  else if (pucSendBuffer[UIP_LLH_LEN + 9] == UIP_PROTO_ICMP)
	  {
	    j = (UIP_LLH_LEN + UIP_TCPIP_HLEN);
	  }
	  else if (pucSendBuffer[UIP_LLH_LEN + 9] == UIP_PROTO_UDP)
	  {
	    j = (UIP_LLH_LEN + UIP_TCPIP_HLEN);
	  }
	  else
	    j = (UIP_LLH_LEN + UIP_TCPIP_HLEN);

	  for (i = 0; i < j; i++)
	  {
	    tmpbuf[i] = *ptr++;
	  }

	  if (i < uiLength)
	  {
	    for (ptr = uip_appdata; (i < uiLength) ; i++)
	    {
				tmpbuf[i] = *ptr++;
	    }
	  }
	if( dm9000_Send(tmpbuf , uiLength ) )
		return ETH_No_Error;
	else
		return ETH_Busy;
}

DRV_Eth_Error DRV_Eth_Receive( unsigned char *pucReceiveBuffer , unsigned int *puiLength  )
{
	dm9000_Receive( pucReceiveBuffer , puiLength  );
	if( *puiLength )
		return ETH_No_Error;
	else
		return ETH_No_Rx_Data;
}
