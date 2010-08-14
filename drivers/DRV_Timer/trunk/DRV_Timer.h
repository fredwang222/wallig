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
#ifndef DRV_TIMER_H
#define DRV_TIMER_H

typedef unsigned int  DRV_Time_TimeOut;
typedef unsigned int  DRV_Time_Count;
typedef void * DRV_Timer_PeriodicTimer;

typedef enum
{
	Timer_No_Error,    //!< OK
	Timer_Failed,      //!< generic error
	Timer_Device_Not_Found,
	Timer_AlreadyOpened,
	Timer_Init_Error,
	Timer_Bad_Param,
	Timer_Err_Unknown      //!< unexpected error
} DRV_Timer_Error;

typedef enum
{
	Timer_Device_Close,
	Timer_Device_Open,
} DRV_Timer_Device_State;

typedef struct
{
	unsigned short usPeriod;
	void (*CallBack)(void);
} DRV_Timer_Periodic_Cfg;

#define  mDRV_TIME_TIMEOUT_INIT( TimeOut , uiValue)  TimeOut =DRV_Timer_SysClock_Get()+uiValue
#define  mDRV_TIME_TIMEOUT_END( TimeOut ) (TimeOut<DRV_Timer_SysClock_Get())
#define  mDRV_TIME_DELAY( uiValue ) { unsigned int TimeOut= DRV_Timer_SysClock_Get()+uiValue; while(TimeOut>=DRV_Timer_SysClock_Get()); }
#define  mDRV_TIME_COUNT_INIT( Count ) Count=DRV_Timer_SysClock_Get()
#define  mDRV_TIME_COUNT_GET( Count ) (DRV_Timer_SysClock_Get() - Count)
void DRV_Timer_SysClock_Init( void );
unsigned int DRV_Timer_SysClock_Get( void );
void DRV_Timer_Periodic_Init( void );
DRV_Timer_Error DRV_Timer_Periodic_Open( char *pcName , DRV_Timer_PeriodicTimer *pHandle,DRV_Timer_Periodic_Cfg *pCfg );
DRV_Timer_Error DRV_Timer_Periodic_Start(  DRV_Timer_PeriodicTimer Handle );
DRV_Timer_Error DRV_Timer_Periodic_Stop(  DRV_Timer_PeriodicTimer Handle );
#endif
