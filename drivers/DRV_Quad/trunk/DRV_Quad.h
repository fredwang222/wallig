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
typedef void * DRV_Quad_Handle;

typedef enum
{
	Quad_No_Error,    //!< OK
	Quad_Failed,      //!< generic error
	Quad_Device_Not_Found,
	Quad_AlreadyOpened,
	Quad_Init_Error,
	Quad_Bad_Param,
	Quad_Err_Unknown      //!< unexpected error
} DRV_Quad_Error;

typedef enum
{
	Quad_Device_Close,
	Quad_Device_Open,
} DRV_Quad_Device_State;

/********************************************************************************************************/
/*							Public functions Declarations												*/
/********************************************************************************************************/
void DRV_Quad_Init(void );
DRV_Quad_Error DRV_Quad_Open( char *pcName , DRV_Quad_Handle *pHandle);
void DRV_Quad_Close( DRV_Quad_Handle Handle);
int DRV_Quad_Position_Get( DRV_Quad_Handle Handle );
DRV_Quad_Error DRV_Quad_Position_Reset( DRV_Quad_Handle Handle );


