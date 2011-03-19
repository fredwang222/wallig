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

#include <stdio.h>
#include <stdint.h>
#include "DRV_Nand.h"
/**************************************************************
						define
***************************************************************/
#define kBBM_LogicalBlockCount 512
/**************************************************************
						Macros
***************************************************************/
#define BBM_BlockIndexFromSectorGet( sector ) (sector>>8)
#define BBM_BlockIndexFromSectorSet( sector , block ) ((sector & 0xFF) | ((((uint32_t)block)<<8)&0xFFFFFF00))
#define BBM_BlockIndexFromPageGet( page ) (page>>6)
#define BBM_BlockIndexFromPageSet( page , block ) page = ( (sector & 0x3F) | ((((uint32_t)block)<<6)&0xFFFFFFC0))
/**************************************************************
						Typedef
***************************************************************/
struct
{
	uint16_t usBBM_Lut[kBBM_LogicalBlockCount];
	uint16_t uiFreeSpareBlockIndex;
} BBM_InternalData;

static void BBMi_BadBlockMark( uint32_t uiBlockIndex );
static uint32_t BBMi_GetNewBlock( void );

int BBM_Init( void )
{
	uint32_t uiPhysicalBlockIndex;
	uint32_t uiLogicalBlockIndex;
	DRV_Nand_SectorSpareData SpareArea;

	/* Create look up table */
	for ( uiLogicalBlockIndex=0,uiPhysicalBlockIndex=0;uiLogicalBlockIndex < kBBM_LogicalBlockCount &&  uiPhysicalBlockIndex < NAND_BlockCount ; uiPhysicalBlockIndex++,uiLogicalBlockIndex++)
	{
		DRV_Nand_SectorRead( NULL , &SpareArea , uiPhysicalBlockIndex*NAND_PagePerBlock*NAND_SectorPerPage);
		if( SpareArea.ucBadBlockMarker != 0xFF)
			continue;
		DRV_Nand_SectorRead( NULL , &SpareArea , uiPhysicalBlockIndex*NAND_PagePerBlock*NAND_SectorPerPage+1);
		if( SpareArea.ucBadBlockMarker == 0xFF)
		{

			BBM_InternalData.usBBM_Lut[uiLogicalBlockIndex++]=uiPhysicalBlockIndex;
		}
	}
	/* Set spare block index */
	BBM_InternalData.uiFreeSpareBlockIndex=uiPhysicalBlockIndex;

	return 0;
}

int BBM_SectorRead( uint8_t *pucDataBuffer , DRV_Nand_SectorSpareData *pSpareArea , uint32_t uiLogicalSectorIndex )
{
	uint32_t uiLogicalBlockIndex = BBM_BlockIndexFromSectorGet(uiLogicalSectorIndex);
	uint32_t uiPhysicalBlockIndex;

	if( uiLogicalBlockIndex > kBBM_LogicalBlockCount)
		return -1;

	uiPhysicalBlockIndex = BBM_InternalData.usBBM_Lut[ uiLogicalBlockIndex ];

	return BBM_SectorRead( pucDataBuffer , pSpareArea , BBM_BlockIndexFromSectorSet( uiLogicalSectorIndex , uiPhysicalBlockIndex ));
}

int BBM_sectorWrite( uint8_t *pucDataBuffer , DRV_Nand_SectorSpareData *pSpareArea , uint32_t uiLogicalSectorIndex )
{
	uint32_t uiLogicalBlockIndex = BBM_BlockIndexFromSectorGet(uiLogicalSectorIndex);
	uint32_t uiPhysicalBlockIndex;
	uint32_t uiNewBlockIndex;
	uint32_t uiPageIndex;
	int iOperationResult;

	if( uiLogicalBlockIndex > kBBM_LogicalBlockCount)
		return -1;

	uiPhysicalBlockIndex = BBM_InternalData.usBBM_Lut[ uiLogicalBlockIndex ];

	iOperationResult = DRV_Nand_sectorWrite( pucDataBuffer , pSpareArea , BBM_BlockIndexFromSectorSet( uiLogicalSectorIndex , uiPhysicalBlockIndex ) );

	if( !iOperationResult )
		return 0;

	/****************************/
	/* write Error -> bad block */
	/****************************/
	/* Get next free spare block */
	uiNewBlockIndex = BBMi_GetNewBlock() ;
	if (  uiNewBlockIndex != 0xFFFF )
			return -1; /* no more spare block on device*/
	BBM_InternalData.usBBM_Lut[ uiLogicalBlockIndex ] = uiNewBlockIndex;
	/* copy data from old to new block*/
	for( uiPageIndex= 0; uiPageIndex<NAND_PagePerBlock ; uiPageIndex++)
		DRV_Nand_PageCopy( BBM_BlockIndexFromSectorSet( uiPageIndex , uiPhysicalBlockIndex ) , BBM_BlockIndexFromSectorSet( uiPageIndex , uiNewBlockIndex ) , NULL );

	BBMi_BadBlockMark( uiPhysicalBlockIndex);

	return 0;
}

int BBM_BlockErase( uint32_t uiLogicalBlockIndex )
{
	uint32_t uiPhysicalBlockIndex;
	uint32_t uiNewBlockIndex;
	int iOperationResult;

	if( uiLogicalBlockIndex > kBBM_LogicalBlockCount)
		return -1;

	uiPhysicalBlockIndex = BBM_InternalData.usBBM_Lut[ uiLogicalBlockIndex ];

	iOperationResult = DRV_Nand_BlockErase(uiPhysicalBlockIndex);

	if( !iOperationResult )
		return 0;

	/****************************/
	/* Erase Error -> bad block */
	/****************************/
	/* Get next free spare block */
	uiNewBlockIndex = BBMi_GetNewBlock() ;
	if (  uiNewBlockIndex != 0xFFFF )
			return -1; /* no more spare block on device*/
	BBM_InternalData.usBBM_Lut[ uiLogicalBlockIndex ] = uiNewBlockIndex;

	BBMi_BadBlockMark( uiPhysicalBlockIndex);

	DRV_Nand_BlockErase( uiNewBlockIndex );

	return  0;

}

int BBM_PageCopy( uint32_t uiSrcLogicalPageIndex , uint32_t uiDestLogicalPageIndex , DRV_Nand_SectorSpareData *pSpareArea)
{
	uint32_t uiSrcLogicalBlockIndex = BBM_BlockIndexFromSectorGet(uiSrcLogicalPageIndex);
	uint32_t uiDestLogicalBlockIndex = BBM_BlockIndexFromSectorGet(uiDestLogicalPageIndex);
	uint32_t uiSrcPhysicalBlockIndex , uiDestPhysicalBlockIndex;
	uint32_t uiNewBlockIndex;
	uint32_t uiPageIndex;
	int iOperationResult;

	if( uiSrcLogicalBlockIndex > kBBM_LogicalBlockCount)
		return -1;
	if( uiDestLogicalBlockIndex > kBBM_LogicalBlockCount)
		return -1;

	uiSrcPhysicalBlockIndex = BBM_InternalData.usBBM_Lut[ uiSrcLogicalBlockIndex ];
	uiDestPhysicalBlockIndex = BBM_InternalData.usBBM_Lut[ uiDestLogicalBlockIndex ];

	iOperationResult = DRV_Nand_PageCopy( BBM_BlockIndexFromSectorSet( uiSrcLogicalPageIndex , uiSrcPhysicalBlockIndex ) ,
			                              BBM_BlockIndexFromSectorSet( uiDestLogicalPageIndex , uiDestPhysicalBlockIndex ) , pSpareArea );

	if( !iOperationResult )
		return 0;

	/****************************/
	/* write Error -> bad block */
	/****************************/
	/* Get next free spare block */
	uiNewBlockIndex = BBMi_GetNewBlock() ;
	if (  uiNewBlockIndex != 0xFFFF )
			return -1; /* no more spare block on device*/
	BBM_InternalData.usBBM_Lut[ uiDestLogicalBlockIndex ] = uiNewBlockIndex;
	/* copy data from old to new block*/
	for( uiPageIndex= 0; uiPageIndex<NAND_PagePerBlock ; uiPageIndex++)
	{
		if( uiPageIndex == (uiSrcLogicalPageIndex&0x3F) )
			DRV_Nand_PageCopy( BBM_BlockIndexFromSectorSet( uiPageIndex , uiSrcLogicalBlockIndex ) , BBM_BlockIndexFromSectorSet( uiPageIndex , uiNewBlockIndex ) , pSpareArea );
		else
			DRV_Nand_PageCopy( BBM_BlockIndexFromSectorSet( uiPageIndex , uiDestLogicalBlockIndex ) , BBM_BlockIndexFromSectorSet( uiPageIndex , uiNewBlockIndex ) , NULL );
	}

	BBMi_BadBlockMark( uiDestLogicalBlockIndex );

	return 0;
}

static uint32_t BBMi_GetNewBlock( void )
{
	uint32_t uiNewBlockIndex = BBM_InternalData.uiFreeSpareBlockIndex;

	if (  uiNewBlockIndex != 0xFFFF )
	{
		/* update uiFreeSpareBlockIndex */
		BBM_InternalData.uiFreeSpareBlockIndex++;
		if( BBM_InternalData.uiFreeSpareBlockIndex >= kBBM_LogicalBlockCount  )
			BBM_InternalData.uiFreeSpareBlockIndex = 0xFFFF; /* The last spare block was used */
	}
	return uiNewBlockIndex ;
}

static void BBMi_BadBlockMark( uint32_t uiBlockIndex )
{
	DRV_Nand_SectorSpareData SpareData;

	/*Mark old block as bad in first 2 pages */
	SpareData.ucBadBlockMarker=0;
	DRV_Nand_sectorWrite( NULL , &SpareData , BBM_BlockIndexFromSectorSet( 0 , uiBlockIndex ) );
	DRV_Nand_sectorWrite( NULL , &SpareData , BBM_BlockIndexFromSectorSet( 4 , uiBlockIndex ) );

}
