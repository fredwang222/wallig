/*
 * 	 Author : Gwendal Le Gall
 *   Date 14/08/2010
 *
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
typedef enum
{
	ETH_No_Error,    //!< OK
	ETH_Failed,      //!< generic error
	ETH_Init_Error,
	ETH_No_Rx_Data,
	ETH_Busy,
	ETH_Bad_Param,
	ETH_Err_Unknown      //!< unexpected error
} DRV_Eth_Error;

DRV_Eth_Error DRV_Eth_Init( void );
DRV_Eth_Error DRV_Eth_Send( unsigned char *pucSendBuffer , unsigned int uiLength );
DRV_Eth_Error DRV_Eth_Receive( unsigned char *pucReceiveBuffer , unsigned int *puiLength  );
