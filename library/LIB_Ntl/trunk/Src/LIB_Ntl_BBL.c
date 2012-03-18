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
#define BBL_BlockIndexFromPage( page ) (page/kBBL_PPB())
#define kBBL_PPB() (BBL_InternalData.PhysicalSizeInfo.uiPagePerBlock) /* number of page per block */
#define kBBL_PBC() (BBL_InternalData.PhysicalSizeInfo.uiBlockCount) /* Physical Block count */
#define kBBL_LBC() (BBL_InternalData.usLogicalBlockCount )/* Logical  Block count */
#define kBBL_UOOB_SIZE() (BBL_InternalData.PhysicalSizeInfo.uiOobSize  - sizeof(BBL_OobData) )
/**************************************************************
						Typedef
***************************************************************/
struct
{
	DRV_Nand_Size_Info PhysicalSizeInfo;
	uint16_t *pusBBL_Lut;
	uint8_t *pucPageBuffer;
	uint8_t *pucOobBuffer;
	uint16_t usFreeSpareBlockIndex;
	uint16_t usLogicalBlockCount;
	uint16_t usBadBlockCount;
} BBL_InternalData;

typedef struct
{
	uint16_t usLogicalBlockIndex;
} BBL_OobData;

/**************************************************************
				Local Private functions prototypes
***************************************************************/
static uint16_t BBLi_GetNewBlock( void );
static void BBLi_UpdateOobBuffer( uint16_t usLogicalBlockIndex , uint8_t  *pucUserOobBuffer);
/**************************************************************
				public functions
***************************************************************/
int BBL_Init( void )
{
	uint16_t usPhysicalBlockIndex;
	uint16_t usLogicalBlockIndex;
	DRV_Nand_Id NandId;
	int iErrorCode=0;
	BBL_OobData BbmOobData;

	/* Get physical nand info */
	if(	DRV_Nand_InfoRead( &NandId , &(BBL_InternalData.PhysicalSizeInfo) ) )
		return 1; /* Nand info read error */
	
	/* Set the logical block count, using the over provisioning rate form the config file */
	BBL_InternalData.usLogicalBlockCount=(kBBL_LBC()*( 100 - kNTL_BBL_OPR ))/100 + 1;
	BBL_InternalData.usBadBlockCount=0;
	/* Allocate structure */	
	BBL_InternalData.pusBBL_Lut = (uint16_t*) malloc( kBBL_LBC() * sizeof(uint16_t));	
	if( BBL_InternalData.pusBBL_Lut == NULL )
		return 2; /* malloc error */
	memset(BBL_InternalData.pusBBL_Lut,0xff,kBBL_LBC() * sizeof(uint16_t));
	/* Allocate Page data buffer */
	BBL_InternalData.pucPageBuffer = (uint8_t*) malloc( BBL_InternalData.PhysicalSizeInfo.uiPageSize );
	if( BBL_InternalData.pucPageBuffer == NULL )
	{
		free(BBL_InternalData.pusBBL_Lut);
		return 3; /* malloc error */
	}

	/* Allocate OOB data buffer */
	BBL_InternalData.pucOobBuffer = (uint8_t*) malloc( BBL_InternalData.PhysicalSizeInfo.uiOobSize );
	if( BBL_InternalData.pucOobBuffer == NULL )
	{
		free(BBL_InternalData.pucPageBuffer);
		free(BBL_InternalData.pusBBL_Lut);
		return 3; /* malloc error */
	}
	/* Create look up table */
	for ( usLogicalBlockIndex=0,usPhysicalBlockIndex=0; usPhysicalBlockIndex < kBBL_PBC() && usLogicalBlockIndex < kBBL_LBC() ; usPhysicalBlockIndex++)
	{
		iErrorCode = DRV_Nand_PageRead( NULL , BBL_InternalData.pucOobBuffer , usPhysicalBlockIndex*kBBL_PPB());
		memcpy( &BbmOobData , BBL_InternalData.pucOobBuffer ,  sizeof(BBL_OobData) );
		switch( iErrorCode )
		{
			case NAND_ERR_BAD_BL: /* This is a bad block*/
				BBL_InternalData.usBadBlockCount++;
				break;
			case NAND_OK: /* This is a good block*/
				if( BbmOobData.usLogicalBlockIndex == 0xFFFF  )
					BBL_InternalData.pusBBL_Lut[usLogicalBlockIndex]=usPhysicalBlockIndex;
				else
				{
					if(BbmOobData.usLogicalBlockIndex > kBBL_LBC())
					{
						free(BBL_InternalData.pucOobBuffer);
						free(BBL_InternalData.pusBBL_Lut);
						free(BBL_InternalData.pucPageBuffer);
						return 4; /* wrong value */
					}
					else 
					{
						if( BBL_InternalData.pusBBL_Lut[BbmOobData.usLogicalBlockIndex] == 0xFFFF )
							BBL_InternalData.pusBBL_Lut[BbmOobData.usLogicalBlockIndex]=usPhysicalBlockIndex;
						else
						{
							free(BBL_InternalData.pucOobBuffer);
							free(BBL_InternalData.pusBBL_Lut);
							free(BBL_InternalData.pucPageBuffer);
							return 5; /* wrong value */
						}
					}
				}
				usLogicalBlockIndex++;
				break;
			default:
				free(BBL_InternalData.pucOobBuffer);
				free(BBL_InternalData.pusBBL_Lut);
				free(BBL_InternalData.pucPageBuffer);
				return 6;

		}
	}
	/* Set spare block index */
	BBL_InternalData.usFreeSpareBlockIndex=usPhysicalBlockIndex;

	return 0;
}

int BBL_PageRead( uint8_t *pucDataBuffer , uint8_t *pucUserOobBuffer ,   uint16_t usLogicalPageIndex )
{
	uint16_t usLogicalBlockIndex = BlockIndexFromPage(usLogicalPageIndex);
	uint16_t usPhysicalBlockIndex;
	int ErrorCode=0;

	if( usLogicalBlockIndex > kBBL_LBC())
		return -1;

	usPhysicalBlockIndex = BBL_InternalData.pusBBL_Lut[ usLogicalBlockIndex ];

	ErrorCode = DRV_Nand_PageRead( pucDataBuffer ,BBL_InternalData.pucOobBuffer , usPhysicalBlockIndex*kBBL_PPB() + usLogicalPageIndex%kBBL_PPB() );

	/* Copy Oob data */
	if( pucUserOobBuffer )
		memcpy( pucUserOobBuffer , BBL_InternalData.pucOobBuffer+sizeof(BBL_OobData),kBBL_UOOB_SIZE() );

	return ErrorCode;
}


int BBL_PageWrite( uint8_t *pucDataBuffer , uint8_t *pucUserOobBuffer ,uint16_t usLogicalPageIndex )
{
	uint16_t usLogicalBlockIndex = BlockIndexFromPage(usLogicalPageIndex);
	uint16_t usPhysicalBlockIndex;
	uint32_t uiNewBlockIndex;
	uint32_t uiPageIndex;
	int iOperationResult;

	if( usLogicalBlockIndex > kBBL_LBC())
		return -1;

	usPhysicalBlockIndex = BBL_InternalData.pusBBL_Lut[ usLogicalBlockIndex ];

	BBLi_UpdateOobBuffer( usLogicalBlockIndex , pucUserOobBuffer );

	iOperationResult = DRV_Nand_PageWrite( pucDataBuffer , BBL_InternalData.pucOobBuffer , usPhysicalBlockIndex*kBBL_PPB() + usLogicalPageIndex%kBBL_PPB() );

	if( !iOperationResult )
		return 0;

	/****************************/
	/* write Error -> bad block */
	/****************************/
	/* Get next free spare block */
	uiNewBlockIndex = BBLi_GetNewBlock() ;
	if (  uiNewBlockIndex != 0xFFFF )
			return -1; /* no more spare block on device*/
	BBL_InternalData.pusBBL_Lut[ usLogicalBlockIndex ] = uiNewBlockIndex;
	/* copy data from old to new block*/
	for( uiPageIndex= 0; uiPageIndex<kBBL_PPB() ; uiPageIndex++)
	{
		/* copy old pages */
		if( uiPageIndex != usLogicalPageIndex%kBBL_PPB() )
			DRV_Nand_PageCopy( usPhysicalBlockIndex*kBBL_PPB() + uiPageIndex , uiNewBlockIndex*kBBL_PPB() + uiPageIndex );
		else /* write new page */
		{
			BBLi_UpdateOobBuffer( usLogicalBlockIndex , pucUserOobBuffer );
			DRV_Nand_PageWrite( pucDataBuffer , BBL_InternalData.pucOobBuffer, uiNewBlockIndex*kBBL_PPB() + uiPageIndex  );
		}
	}

	return 0;
}

int BBL_BlockErase( uint16_t usLogicalBlockIndex )
{
	uint16_t usPhysicalBlockIndex;
	uint16_t usNewBlockIndex;
	int iOperationResult;

	if( usLogicalBlockIndex > kBBL_LBC())
		return -1;

	usPhysicalBlockIndex = BBL_InternalData.pusBBL_Lut[ usLogicalBlockIndex ];

	iOperationResult = DRV_Nand_BlockErase(usPhysicalBlockIndex);

	if( !iOperationResult )
	{
		/* write logical block index */
		BBLi_UpdateOobBuffer( usLogicalBlockIndex , NULL  );
		DRV_Nand_PageWrite( NULL , BBL_InternalData.pucOobBuffer, usPhysicalBlockIndex*kBBL_PPB() );
		return 0;
	}

	/****************************/
	/* Erase Error -> bad block */
	/****************************/
	/* Get next free spare block */
	usNewBlockIndex = BBLi_GetNewBlock() ;
	if (  usNewBlockIndex != 0xFFFF )
			return -1; /* no more spare block on device*/
	BBL_InternalData.pusBBL_Lut[ usLogicalBlockIndex ] = usNewBlockIndex;

	DRV_Nand_BlockErase( usNewBlockIndex );
	/* write logical block index */
	BBLi_UpdateOobBuffer( usLogicalBlockIndex , NULL  );
	DRV_Nand_PageWrite( NULL , BBL_InternalData.pucOobBuffer, usNewBlockIndex*kBBL_PPB() );
	return  0;

}

int BBL_PageCopy( uint16_t usSrcLogicalPageIndex , uint16_t usDestLogicalPageIndex )
{
	uint16_t usSrcLogicalBlockIndex = usSrcLogicalPageIndex/kBBL_PPB();
	uint16_t usDestLogicalBlockIndex = usDestLogicalPageIndex/kBBL_PPB();
	uint16_t usSrcPhysicalBlockIndex , usDestPhysicalBlockIndex;
	uint16_t usNewBlockIndex;
	uint16_t usPageIndex;
	int iOperationResult;

	if( usSrcLogicalBlockIndex > kBBL_LBC())
		return -1;
	if( usDestLogicalBlockIndex > kBBL_LBC())
		return -1;

	usSrcPhysicalBlockIndex = BBL_InternalData.pusBBL_Lut[ usSrcLogicalBlockIndex ];
	usDestPhysicalBlockIndex = BBL_InternalData.pusBBL_Lut[ usDestLogicalBlockIndex ];
	

	DRV_Nand_PageRead( BBL_InternalData.pucPageBuffer ,BBL_InternalData.pucOobBuffer , usSrcPhysicalBlockIndex*kBBL_PPB() + usSrcLogicalPageIndex%kBBL_PPB() );
	BBLi_UpdateOobBuffer( usDestLogicalBlockIndex , BBL_InternalData.pucOobBuffer );
	iOperationResult = DRV_Nand_PageWrite( BBL_InternalData.pucPageBuffer , BBL_InternalData.pucOobBuffer , usDestPhysicalBlockIndex*kBBL_PPB() + usDestLogicalPageIndex%kBBL_PPB()  );

	if( !iOperationResult )
		return 0;

	/****************************/
	/* write Error -> bad block */
	/****************************/
	/* Get next free spare block */
	usNewBlockIndex = BBLi_GetNewBlock() ;
	if (  usNewBlockIndex != 0xFFFF )
			return -1; /* no more spare block on device*/
	BBL_InternalData.pusBBL_Lut[ usDestLogicalBlockIndex ] = usNewBlockIndex;
	/* copy data from old to new block*/
	for( usPageIndex= 0; usPageIndex<kBBL_PPB() ; usPageIndex++)
	{
		DRV_Nand_PageRead( BBL_InternalData.pucPageBuffer ,BBL_InternalData.pucOobBuffer , usSrcPhysicalBlockIndex*kBBL_PPB() + usSrcLogicalPageIndex%kBBL_PPB() );
		BBLi_UpdateOobBuffer( usDestLogicalBlockIndex , BBL_InternalData.pucOobBuffer );
		iOperationResult = DRV_Nand_PageWrite( BBL_InternalData.pucPageBuffer , BBL_InternalData.pucOobBuffer , usNewBlockIndex*kBBL_PPB() + usDestLogicalPageIndex%kBBL_PPB()  );
	}

	return 0;
}

/**************************************************************
				Local Private functions
***************************************************************/

static uint16_t BBLi_GetNewBlock( void )
{
	uint16_t usNewBlockIndex = BBL_InternalData.usFreeSpareBlockIndex;

	if (  usNewBlockIndex != 0xFFFF )
	{
		/* update usFreeSpareBlockIndex */
		BBL_InternalData.usFreeSpareBlockIndex++;
		if( BBL_InternalData.usFreeSpareBlockIndex >= kBBL_LBC()  )
			BBL_InternalData.usFreeSpareBlockIndex = 0xFFFF; /* The last spare block was used */
	}
	return usNewBlockIndex ;
}

static void BBLi_UpdateOobBuffer( uint16_t usLogicalBlockIndex , uint8_t  *pucUserOobBuffer)
{
	BBL_OobData BbmOobData;

	memset(BBL_InternalData.pucOobBuffer,0xff,BBL_InternalData.PhysicalSizeInfo.uiOobSize);
	BbmOobData.usLogicalBlockIndex = usLogicalBlockIndex;
	memcpy( BBL_InternalData.pucOobBuffer ,  &BbmOobData ,sizeof(BBL_OobData) );
	if( pucUserOobBuffer )
		memcpy( BBL_InternalData.pucOobBuffer + sizeof(BBL_OobData) ,  pucUserOobBuffer ,kBBL_UOOB_SIZE() );
}


