/*
 * 	 Author : Gwendal Le Gall
 *   Date 19/03/2011
 *
 *
 *    This file is part of Wallig Library and Drivers.
 *
 *    Copyright (C) 2011  Gwendal Le Gall
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


int BBL_Init( void );
int BBL_Terminate( void );
int BBL_InfoRead(  DRV_Nand_Size_Info *pSizeInfo , uint16_t *pusBadBlockCount );
int BBL_PageRead( uint8_t *pucDataBuffer , uint8_t *pucUserOobBuffer ,   uint16_t usLogicalPageIndex );
int BBL_PageWrite( uint8_t *pucDataBuffer , uint8_t *pucUserOobBuffer ,uint16_t usLogicalPageIndex );
int BBL_BlockErase( uint16_t usLogicalBlockIndex );
int BBL_PageCopy( uint16_t usSrcLogicalPageIndex , uint16_t usDestLogicalPageIndex );
void BBL_PrintLut( void );
