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
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "DRV_Nand.h"
#include "LIB_Ntl_CFG.h"
/**************************************************************
						define
***************************************************************/

/**************************************************************
						Macros
***************************************************************/
#define BBM_BlockIndexFromSectorGet( sector ) (sector>>8)
#define BBM_BlockIndexFromSectorSet( sector , block ) ((sector & 0xFF) | ((((uint32_t)block)<<8)&0xFFFFFF00))
#define BBM_BlockIndexFromPage( page ) (page/kBBM_PPB())
#define BBM_BlockIndexFromPageSet( page , block ) page = ( (sector & 0x3F) | ((((uint32_t)block)<<6)&0xFFFFFFC0))

#define kBBM_PPB() (BBM_InternalData.PhysicalSizeInfo.uiPagePerBlock) /* number of page per block */
#define kBBM_PBC() (BBM_InternalData.PhysicalSizeInfo.uiBlockCount) /* Physical Block count */
#define kBBM_LBC() (BBM_InternalData.usLogicalBlockCount )/* Logical  Block count */
#define kBBM_UOOB_SIZE() (BBM_InternalData.PhysicalSizeInfo.uiOobSize  - sizeof(BBM_OobData) )
/**************************************************************
						Typedef
***************************************************************/
struct
{
	DRV_Nand_Size_Info PhysicalSizeInfo;
	uint16_t *pusBBM_Lut;
	uint8_t *pucPageBuffer;
	uint8_t *pucOobBuffer;
	uint16_t usFreeSpareBlockIndex;
	uint16_t usLogicalBlockCount;
	uint16_t usBadBlockCount;
} BBM_InternalData;

typedef struct
{
	uint16_t usLogicalBlockIndex;
} BBM_OobData;

/**************************************************************
				Local Private functions prototypes
***************************************************************/
static uint16_t BBMi_GetNewBlock( void );
static void BBMi_UpdateOobBuffer( uint16_t usLogicalBlockIndex , uint8_t  *pucUserOobBuffer);
/**************************************************************
				public functions
***************************************************************/
int BBM_Init( void )
{
	uint16_t usPhysicalBlockIndex;
	uint16_t usLogicalBlockIndex;
	DRV_Nand_Id NandId;
	int iErrorCode=0;
	BBM_OobData BbmOobData;

	/* Get physical nand info */
	if(	DRV_Nand_InfoRead( &NandId , &(BBM_InternalData.PhysicalSizeInfo) ) )
		return 1; /* Nand info read error */
	
	/* Set the logical block count, using the over provisioning rate form the config file */
	BBM_InternalData.usLogicalBlockCount=(kBBM_LBC()*( 100 - kNTL_BBM_OPR ))/100 + 1;
	BBM_InternalData.usBadBlockCount=0;
	/* Allocate structure */	
	BBM_InternalData.pusBBM_Lut = (uint16_t*) malloc( kBBM_LBC() * sizeof(uint16_t));	
	if( BBM_InternalData.pusBBM_Lut == NULL )
		return 2; /* malloc error */
	memset(BBM_InternalData.pusBBM_Lut,0xff,kBBM_LBC() * sizeof(uint16_t));
	/* Allocate Page data buffer */
	BBM_InternalData.pucPageBuffer = (uint8_t*) malloc( BBM_InternalData.PhysicalSizeInfo.uiPageSize );
	if( BBM_InternalData.pucPageBuffer == NULL )
	{
		free(BBM_InternalData.pusBBM_Lut);
		return 3; /* malloc error */
	}

	/* Allocate OOB data buffer */
	BBM_InternalData.pucOobBuffer = (uint8_t*) malloc( BBM_InternalData.PhysicalSizeInfo.uiOobSize );
	if( BBM_InternalData.pucOobBuffer == NULL )
	{
		free(BBM_InternalData.pucPageBuffer);
		free(BBM_InternalData.pusBBM_Lut);
		return 3; /* malloc error */
	}
	/* Create look up table */
	for ( usLogicalBlockIndex=0,usPhysicalBlockIndex=0; usPhysicalBlockIndex < kBBM_PBC() && usLogicalBlockIndex < kBBM_LBC() ; usPhysicalBlockIndex++)
	{
		iErrorCode = DRV_Nand_PageRead( NULL , BBM_InternalData.pucOobBuffer , usPhysicalBlockIndex*kBBM_PPB());
		memcpy( &BbmOobData , BBM_InternalData.pucOobBuffer ,  sizeof(BBM_OobData) );
		switch( iErrorCode )
		{
			case NAND_ERR_BAD_BL: /* This is a bad block*/
				BBM_InternalData.usBadBlockCount++;
				break;
			case NAND_OK: /* This is a good block*/
				if( BbmOobData.usLogicalBlockIndex == 0xFFFF  )
					BBM_InternalData.pusBBM_Lut[usLogicalBlockIndex]=usPhysicalBlockIndex;
				else
				{
					if(BbmOobData.usLogicalBlockIndex > kBBM_LBC())
					{
						free(BBM_InternalData.pucOobBuffer);
						free(BBM_InternalData.pusBBM_Lut);
						free(BBM_InternalData.pucPageBuffer);
						return 4; /* wrong value */
					}
					else 
					{
						if( BBM_InternalData.pusBBM_Lut[BbmOobData.usLogicalBlockIndex] == 0xFFFF )
							BBM_InternalData.pusBBM_Lut[BbmOobData.usLogicalBlockIndex]=usPhysicalBlockIndex;
						else
						{
							free(BBM_InternalData.pucOobBuffer);
							free(BBM_InternalData.pusBBM_Lut);
							free(BBM_InternalData.pucPageBuffer);
							return 5; /* wrong value */
						}
					}
				}
				usLogicalBlockIndex++;
				break;
			default:
				free(BBM_InternalData.pucOobBuffer);
				free(BBM_InternalData.pusBBM_Lut);
				free(BBM_InternalData.pucPageBuffer);
				return 6;

		}
	}
	/* Set spare block index */
	BBM_InternalData.usFreeSpareBlockIndex=usPhysicalBlockIndex;

	return 0;
}

int BBM_PageRead( uint8_t *pucDataBuffer , uint8_t *pucUserOobBuffer ,   uint16_t usLogicalPageIndex )
{
	uint16_t usLogicalBlockIndex = BlockIndexFromPage(usLogicalPageIndex);
	uint16_t usPhysicalBlockIndex;
	int ErrorCode=0;

	if( usLogicalBlockIndex > kBBM_LBC())
		return -1;

	usPhysicalBlockIndex = BBM_InternalData.pusBBM_Lut[ usLogicalBlockIndex ];

	ErrorCode = DRV_Nand_PageRead( pucDataBuffer ,BBM_InternalData.pucOobBuffer , usPhysicalBlockIndex*kBBM_PPB() + usLogicalPageIndex%kBBM_PPB() );

	/* Copy Oob data */
	memcpy( pucUserOobBuffer , BBM_InternalData.pucOobBuffer+sizeof(BBM_OobData),kBBM_UOOB_SIZE() );

	return ErrorCode;
}


int BBM_PageWrite( uint8_t *pucDataBuffer , uint8_t *pucUserOobBuffer ,uint16_t usLogicalPageIndex )
{
	uint16_t usLogicalBlockIndex = BlockIndexFromPage(usLogicalPageIndex);
	uint16_t usPhysicalBlockIndex;
	uint32_t uiNewBlockIndex;
	uint32_t uiPageIndex;
	int iOperationResult;

	if( usLogicalBlockIndex > kBBM_LBC())
		return -1;

	usPhysicalBlockIndex = BBM_InternalData.pusBBM_Lut[ usLogicalBlockIndex ];

	BBMi_UpdateOobBuffer( usLogicalBlockIndex , pucUserOobBuffer );

	iOperationResult = DRV_Nand_PageWrite( pucDataBuffer , BBM_InternalData.pucOobBuffer , usPhysicalBlockIndex*kBBM_PPB() + usLogicalPageIndex%kBBM_PPB() );

	if( !iOperationResult )
		return 0;

	/****************************/
	/* write Error -> bad block */
	/****************************/
	/* Get next free spare block */
	uiNewBlockIndex = BBMi_GetNewBlock() ;
	if (  uiNewBlockIndex != 0xFFFF )
			return -1; /* no more spare block on device*/
	BBM_InternalData.pusBBM_Lut[ usLogicalBlockIndex ] = uiNewBlockIndex;
	/* copy data from old to new block*/
	for( uiPageIndex= 0; uiPageIndex<kBBM_PPB() ; uiPageIndex++)
	{
		/* copy old pages */
		if( uiPageIndex != usLogicalPageIndex%kBBM_PPB() )
			DRV_Nand_PageCopy( usPhysicalBlockIndex*kBBM_PPB() + uiPageIndex , uiNewBlockIndex*kBBM_PPB() + uiPageIndex );
		else /* write new page */
		{
			BBMi_UpdateOobBuffer( usLogicalBlockIndex , pucUserOobBuffer );
			DRV_Nand_PageWrite( pucDataBuffer , BBM_InternalData.pucOobBuffer, uiNewBlockIndex*kBBM_PPB() + uiPageIndex  );
		}
	}

	return 0;
}

int BBM_BlockErase( uint16_t usLogicalBlockIndex )
{
	uint16_t usPhysicalBlockIndex;
	uint16_t usNewBlockIndex;
	int iOperationResult;

	if( usLogicalBlockIndex > kBBM_LBC())
		return -1;

	usPhysicalBlockIndex = BBM_InternalData.pusBBM_Lut[ usLogicalBlockIndex ];

	iOperationResult = DRV_Nand_BlockErase(usPhysicalBlockIndex);

	if( !iOperationResult )
		return 0;

	/****************************/
	/* Erase Error -> bad block */
	/****************************/
	/* Get next free spare block */
	usNewBlockIndex = BBMi_GetNewBlock() ;
	if (  usNewBlockIndex != 0xFFFF )
			return -1; /* no more spare block on device*/
	BBM_InternalData.pusBBM_Lut[ usLogicalBlockIndex ] = usNewBlockIndex;

	DRV_Nand_BlockErase( usNewBlockIndex );

	return  0;

}

int BBM_PageCopy( uint16_t usSrcLogicalPageIndex , uint16_t usDestLogicalPageIndex )
{
	uint16_t usSrcLogicalBlockIndex = usSrcLogicalPageIndex/kBBM_PPB();
	uint16_t usDestLogicalBlockIndex = usDestLogicalPageIndex/kBBM_PPB();
	uint16_t usSrcPhysicalBlockIndex , usDestPhysicalBlockIndex;
	uint16_t usNewBlockIndex;
	uint16_t usPageIndex;
	int iOperationResult;

	if( usSrcLogicalBlockIndex > kBBM_LBC())
		return -1;
	if( usDestLogicalBlockIndex > kBBM_LBC())
		return -1;

	usSrcPhysicalBlockIndex = BBM_InternalData.pusBBM_Lut[ usSrcLogicalBlockIndex ];
	usDestPhysicalBlockIndex = BBM_InternalData.pusBBM_Lut[ usDestLogicalBlockIndex ];
	

	DRV_Nand_PageRead( BBM_InternalData.pucPageBuffer ,BBM_InternalData.pucOobBuffer , usSrcPhysicalBlockIndex*kBBM_PPB() + usSrcLogicalPageIndex%kBBM_PPB() );
	BBMi_UpdateOobBuffer( usDestLogicalBlockIndex , BBM_InternalData.pucOobBuffer );
	iOperationResult = DRV_Nand_PageWrite( BBM_InternalData.pucPageBuffer , BBM_InternalData.pucOobBuffer , usDestPhysicalBlockIndex*kBBM_PPB() + usDestLogicalPageIndex%kBBM_PPB()  );

	if( !iOperationResult )
		return 0;

	/****************************/
	/* write Error -> bad block */
	/****************************/
	/* Get next free spare block */
	usNewBlockIndex = BBMi_GetNewBlock() ;
	if (  usNewBlockIndex != 0xFFFF )
			return -1; /* no more spare block on device*/
	BBM_InternalData.pusBBM_Lut[ usDestLogicalBlockIndex ] = usNewBlockIndex;
	/* copy data from old to new block*/
	for( usPageIndex= 0; usPageIndex<kBBM_PPB() ; usPageIndex++)
	{
		DRV_Nand_PageRead( BBM_InternalData.pucPageBuffer ,BBM_InternalData.pucOobBuffer , usSrcPhysicalBlockIndex*kBBM_PPB() + usSrcLogicalPageIndex%kBBM_PPB() );
		BBMi_UpdateOobBuffer( usDestLogicalBlockIndex , BBM_InternalData.pucOobBuffer );
		iOperationResult = DRV_Nand_PageWrite( BBM_InternalData.pucPageBuffer , BBM_InternalData.pucOobBuffer , usNewBlockIndex*kBBM_PPB() + usDestLogicalPageIndex%kBBM_PPB()  );
	}

	return 0;
}

/**************************************************************
				Local Private functions
***************************************************************/

static uint16_t BBMi_GetNewBlock( void )
{
	uint16_t usNewBlockIndex = BBM_InternalData.usFreeSpareBlockIndex;

	if (  usNewBlockIndex != 0xFFFF )
	{
		/* update usFreeSpareBlockIndex */
		BBM_InternalData.usFreeSpareBlockIndex++;
		if( BBM_InternalData.usFreeSpareBlockIndex >= kBBM_LBC()  )
			BBM_InternalData.usFreeSpareBlockIndex = 0xFFFF; /* The last spare block was used */
	}
	return usNewBlockIndex ;
}

static void BBMi_UpdateOobBuffer( uint16_t usLogicalBlockIndex , uint8_t  *pucUserOobBuffer)
{
	BBM_OobData BbmOobData;

	BbmOobData.usLogicalBlockIndex = usLogicalBlockIndex;
	memcpy( BBM_InternalData.pucOobBuffer ,  &BbmOobData ,sizeof(BBM_OobData) );
	memcpy( BBM_InternalData.pucOobBuffer + sizeof(BBM_OobData) ,  pucUserOobBuffer ,kBBM_UOOB_SIZE() );
}


