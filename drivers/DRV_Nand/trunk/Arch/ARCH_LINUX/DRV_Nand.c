/*
 * 	 Author : Gwendal Le Gall
 *   Date 19/03/2011
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
#include <stdint.h>
#include <stdio.h>
#include "DRV_Nand.h"
#include "Inc/DRV_Nand_p.h"

void DRV_Nand_Init( void )
{

}

int DRV_Nand_IdRead( DRV_Nand_Id *pId )
{
return 0;
}


int DRV_Nand_PageRead( uint8_t *pucDataBuffer , uint8_t *pucSpareAreaBuffer , uint32_t uiPageIndex )
{
	return 0;
}


int DRV_Nand_PageWrite( uint8_t *pucDataBuffer , uint8_t *pucSpareAreaBuffer , uint32_t uiPageIndex )
{
	return 0;
}

int DRV_Nand_PageCopy( uint32_t uiSrcPageIndex , uint32_t uiDestPageIndex , DRV_Nand_SectorSpareData *pSpareArea)
{
}

int DRV_Nand_BlockErase( uint32_t uiBlockIndex )
{
}

int DRV_Nand_SectorRead( uint8_t *pucDataBuffer , DRV_Nand_SectorSpareData *pSpareArea , uint32_t uiSectorIndex )
{
   return 0;

}

int DRV_Nand_sectorWrite( uint8_t *pucDataBuffer , DRV_Nand_SectorSpareData *pSpareArea , uint32_t uiSectorIndex )
{
	return 0;
}



