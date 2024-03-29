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
/**
*	\file DRV_Gpio.h
*
*	\author Gwendal Le Gall
*
*	\date 02/02/2010
*	\brief Public declaration for the driver
*
*/
/*!
 * \page svn_log SVN log
 *  \include svn_log.txt
 */
/*!
 * \if standalone_DRV_Gpio
*	\mainpage GPIO Driver
*    \else
*    \page GPIO GPIO Driver
*    \endif
*   \author Gwendal Le Gall
*
*	\date 13/03/2010
*
*	\section Description Description
*	GPIO Hardware Abstraction layer
*	\section Configuration Configuration
*
*	\section arch architecture specific
*
*	\subsection ARM7AT91NoOs ARM7AT91 NoOs
*
*	\ref GPIOLinux
*
*	\section history History
*	\ref svn_log
*/
/** @defgroup DRV_Gpio_Public_grp Public interface for DRV_Gpio driver
 *  This is the first group
 *  @{
 */
#ifndef DRV_GPIO_H
#define  DRV_GPIO_H

#define BOOL unsigned char
/**************************************************************
						Typedef
***************************************************************/
/*!
 *  \brief Handle for device
 */
typedef void * DRV_Gpio_Handle;

/*!
 *  \brief Returned error for driver
 *  \public
 */
typedef enum
{
	GPIO_No_Error,    //!< OK
	GPIO_Failed,      //!< generic error
	GPIO_Device_Not_Found,
	GPIO_AlreadyOpened,
	GPIO_Init_Error,
	GPIO_Bad_Param,
	GPIO_Err_Unknown      //!< unexpected error
} DRV_Gpio_Error;

typedef enum
{
	Gpio_Device_Close,
	Gpio_Device_Open,
} DRV_Gpio_Device_State;

/** @defgroup DRV_Gpio_Settings PIO basics settings
 *  @ingroup DRV_Gpio_Public_grp
 *  @{
 */


/*!
 *  \brief Type
 *  \public
 */
typedef enum
{
        GPIO_INT_None,         //!< No interrupt for this pio
        GPIO_INT_Low2High,     //!< Interrupt on low to high transition
        GPIO_INT_High2Low,     //!< Interrupt on high to low transition
        GPIO_INT_Both,			//!< Interrupt on any level transition
} DRV_Gpio_IntType;

/*!
 *  \brief GPIO identification
 */
typedef struct
{
	DRV_Gpio_IntType EventType;
	void (*CallBack)(void);
} DRV_GpioINT;





/** @} */ //
/**************************************************************
					Functions
***************************************************************/
/*! \fn DRV_Gpio_Error DRV_Gpio_Init( void )
 *  \brief Initialization the driver.
 *  \return Driver error.
 */
DRV_Gpio_Error DRV_Gpio_Init( void );

/*! \fn DRV_Gpio_Error DRV_Gpio_Terminate( void )
 *  \brief Terminate the driver.
 *  \return Driver error.
 */
DRV_Gpio_Error DRV_Gpio_Terminate( void );
/*! \fn DRV_Gpio_Error DRV_Gpio_Open( DRV_Gpio_Handle *phDeviceHandle, DRV_Gpio_Cfg *ptSettings)
 *  \brief Open an instance of a GPIO.
 *  \param phDeviceHandle Handle of the new instance.
 *  \param ptSettings device settings
 *  \return Driver error.
 */
DRV_Gpio_Error DRV_Gpio_Open( char *Name , DRV_Gpio_Handle *phDeviceHandle , DRV_GpioINT *pGpioIntCfg);

/*! \fn DRV_Gpio_Error DRV_Gpio_Close( DRV_Gpio_Handle hDeviceHandle );
 *  \brief Close an instance of the driver.
 *  \param hDeviceHandle Handle of the instance to close.
 *  \return Driver error.
 */
DRV_Gpio_Error DRV_Gpio_Close( DRV_Gpio_Handle hDeviceHandle );

/*! \fn BOOL DRV_Gpio_Send( DRV_Gpio_Handle hDeviceHandle);
 *  \brief Getthe value of a PIO.
 *  \param hDeviceHandle Handle of the instance to close.
 *  \return Driver error.
 */
BOOL DRV_Gpio_ValueGet( DRV_Gpio_Handle hDeviceHandle );


/*! \fn DRV_Gpio_Error DRV_Gpio_Send( DRV_Gpio_Handle hDeviceHandle , BOOL bValue);
 *  \brief Set a PIO.
 *  \param hDeviceHandle Handle of the instance to close.
 *  \return Driver error.
 */
DRV_Gpio_Error DRV_Gpio_ValueSet( DRV_Gpio_Handle hDeviceHandle , BOOL bValue);

/*! \fn DRV_Gpio_Error DRV_Gpio_Send( DRV_Gpio_Handle hDeviceHandle ,unsigned char *pucBuffer , int iLength);
 *  \brief Set the pio type
 *  \param hDeviceHandle Handle of the instance to close.
 *  \param eType new type
 *  \return Driver error.
 */
DRV_Gpio_Error DRV_Gpio_TypeSet( DRV_Gpio_Handle hDeviceHandle , char  eType);

/*! \fn DRV_Gpio_Error DRV_Gpio_Send( DRV_Gpio_Handle hDeviceHandle ,unsigned char *pucBuffer , int iLength);
 *  \brief Set an option
 *  \param hDeviceHandle Handle of the instance to close.
 *  \param uiOption Option mask to set
 *  \return Driver error.
 */
DRV_Gpio_Error DRV_Gpio_OptionSet( DRV_Gpio_Handle hDeviceHandle , unsigned int uiOption);

/*! \fn DRV_Gpio_Error DRV_Gpio_Send( DRV_Gpio_Handle hDeviceHandle ,unsigned char *pucBuffer , int iLength);
 *  \brief reset an option
 *  \param hDeviceHandle Handle of the instance to close.
 *  \param uiOption Option mask to reset
 *  \return Driver error.
 */
DRV_Gpio_Error DRV_Gpio_OptionReSet( DRV_Gpio_Handle hDeviceHandle , unsigned int uiOption);

/** @} */ // end of DRV_Gpio_Public_grp
#endif
