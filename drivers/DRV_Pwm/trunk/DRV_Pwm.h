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
/*							Typedef																		*/
/********************************************************************************************************/

typedef void * DRV_Pwm_Handle;

typedef enum
{
	Pwm_No_Error,    //!< OK
	Pwm_Failed,      //!< generic error
	Pwm_Device_Not_Found,
	Pwm_AlreadyOpened,
	Pwm_Init_Error,
	Pwm_Bad_Param,
	Pwm_Err_Unknown      //!< unexpected error
} DRV_Pwm_Error;

typedef enum
{
	Pwm_Device_Close,
	Pwm_Device_Open,
} DRV_Pwm_Device_State;

/********************************************************************************************************/
/*							Public functions Declarations												*/
/********************************************************************************************************/
void DRV_Pwm_Init(void );
DRV_Pwm_Error DRV_Pwm_Open( const char *pcName , DRV_Pwm_Handle *pHandle , unsigned short ucDutycycle);
void DRV_Pwm_Close( DRV_Pwm_Handle Handle);
void DRV_Pwm_Enable( DRV_Pwm_Handle Handle );
void DRV_Pwm_Disable( DRV_Pwm_Handle Handle );
DRV_Pwm_Error DRV_Pwm_DutyCycleSet(const DRV_Pwm_Handle Handle ,  unsigned char ucDutycycle);

