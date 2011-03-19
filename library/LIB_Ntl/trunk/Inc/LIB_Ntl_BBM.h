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
/**************************************************************
						Typedef
***************************************************************/

/**************************************************************
					Functions
***************************************************************/

int BBM_SectorRead( uint8_t *pucDataBuffer , BBM_SectorSpareData *pSpareArea , uint32_t uiLogicalSectorIndex );
int BBM_sectorWrite( uint8_t *pucDataBuffer , BBM_SectorSpareData *pSpareArea , uint32_t uiLogicalSectorIndex );
int BBM_BlockErase( uint32_t uiBlockIndex );
int BBM_PageCopy( uint32_t uiSrcLogicalPageIndex , uint32_t uiDestLogicalIndex , DRV_Nand_SectorSpareData *pSpareArea);


