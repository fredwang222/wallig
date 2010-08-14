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
#define DRV_SPI_SLOW 0
#define DRV_SPI_FAST 1
/********************************************************************************************************/
/*							Typedef																		*/
/********************************************************************************************************/
typedef void * DRV_Spi_Handle;

typedef enum
{
	Spi_No_Error,    //!< OK
	Spi_Failed,      //!< generic error
	Spi_Device_Not_Found,
	Spi_Device_Busy,
	Spi_AlreadyOpened,
	Spi_Init_Error,
	Spi_Bad_Param,
	Spi_Err_Unknown      //!< unexpected error
} DRV_Spi_Error;

typedef enum
{
	Spi_Device_Close,
	Spi_Device_Open,
} DRV_Spi_Device_State;

/********************************************************************************************************/
/*							Public functions Declarations												*/
/********************************************************************************************************/
void DRV_Spi_Init(void );
DRV_Spi_Error DRV_Spi_Open( const char *pcName , DRV_Spi_Handle *pHandle);
void DRV_Spi_Close( DRV_Spi_Handle Handle);
DRV_Spi_Error DRV_Spi_Speed_Set( DRV_Spi_Handle Handle , uint16_t u16Speed);
unsigned short DRV_Spi_RW_Byte( DRV_Spi_Handle Handle , unsigned char ucOut);
unsigned short DRV_Spi_Read_Buffer( DRV_Spi_Handle Handle , unsigned char *pcBuffer , unsigned int uiLen);
unsigned short DRV_Spi_Write_Buffer( DRV_Spi_Handle Handle , unsigned char *pcBuffer , unsigned int uiLen);
DRV_Spi_Error DRV_Spi_LockSet( DRV_Spi_Handle Handle  );
DRV_Spi_Error DRV_Spi_LockRelease( DRV_Spi_Handle Handle  );
