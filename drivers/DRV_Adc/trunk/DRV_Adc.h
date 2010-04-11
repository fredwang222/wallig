/*
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

/********************************************************************************************************/
/*							Define																		*/
/********************************************************************************************************/
/********************************************************************************************************/
/*							Typedef																		*/
/********************************************************************************************************/
typedef void * DRV_Adc_Handle;

typedef enum
{
	Adc_No_Error,    //!< OK
	Adc_Failed,      //!< generic error
	Adc_Device_Not_Found,
	Adc_AlreadyOpened,
	Adc_Init_Error,
	Adc_Bad_Param,
	Adc_Err_Unknown      //!< unexpected error
} DRV_Adc_Error;

typedef enum
{
	Adc_Device_Close,
	Adc_Device_Open,
} DRV_Adc_Device_State;

/********************************************************************************************************/
/*							Public functions Declarations												*/
/********************************************************************************************************/
void DRV_Adc_Init(void );
DRV_Adc_Error DRV_Adc_Open( const char *pcName , DRV_Adc_Handle *pHandle);
void DRV_Adc_Close( DRV_Adc_Handle Handle);
unsigned short DRV_Adc_Read( DRV_Adc_Handle Handle );

