/*
 * 	 Author : Gwendal Le Gall
 *   Date 02/02/2010
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
#ifndef DRV_GPIO_CLI_H
#define  DRV_GPIO_CLI_H
char DRV_Gpio_Cli_open( int *pState , char *pcArgs , char *pcOutput , int iOutputLen);
#define GPIO_CLI_OPEN_COMMAND { "pio_open" , "Open a pio device" , DRV_Gpio_Cli_open , 0 }

char DRV_Gpio_Cli_Set( int *pState , char *pcArgs , char *pcOutput , int iOutputLen);
#define GPIO_CLI_SET_COMMAND { "pio_set" , "Set a pio Value: 0 or 1" , DRV_Gpio_Cli_Set , 0 }

char DRV_Gpio_Cli_Get( int *pState , char *pcArgs , char *pcOutput , int iOutputLen);
#define GPIO_CLI_GET_COMMAND { "pio_get" , "Get pio Value: 0 or 1" , DRV_Gpio_Cli_Get , 0 }

#define GPIO_COMMAND_LIST GPIO_CLI_OPEN_COMMAND , GPIO_CLI_SET_COMMAND , GPIO_CLI_GET_COMMAND
#endif
