/*
 * 	 Author : Gwendal Le Gall
 *   Date 02/02/2010
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
#ifndef DRV_GPIO_ARCH_H
#define  DRV_GPIO_ARCH_H

#ifdef ARCH_STM32
#include "stm32f10x.h"

typedef GPIO_TypeDef* tPIO_BANK;
typedef u16 tPIO_Pin;

#define  GPIO_INPUT (int) GPIO_Mode_IN_FLOATING
#define  GPIO_OUPUT_OD (int) GPIO_Mode_Out_OD
#define  GPIO_OUPUT_PP (int) GPIO_Mode_Out_PP
#endif
#endif
