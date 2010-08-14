/*
 * 	 Author : Gwendal Le Gall
 *   Date 14/08/2010
 *
*    Modified from:
 *     - nRF24l01 driver of Neil MacMillan see: http://code.google.com/p/nrf24l01/
 *     - http://www.tinkerer.eu/AVRLib/nRF24L01

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
#ifndef DRV_TIMER_CLI_H
#define DRV_TIMER_CLI_H
char DRV_Timer_Cli_uptime( int *pState , char *pcArgs , char *pcOutput , int iOutputLen);
#define DRV_TIMER_CLI_UPTIME_COMMAND { "uptime" , "Get uptime" , DRV_Timer_Cli_uptime , 0 }
#endif
