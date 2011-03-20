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
#include <stdint.h>
#include <string.h>
#include "LIB_Ntl.h"
#include "Inc/LIB_Ntl_p.h"
#include "Inc/LIB_Ntl_BBM.h"

/**************************************************************
						Constants
***************************************************************/
#define kSectorSize                   512 /* size in Byte */
#define kSectorPageCount              4    /* sector / page */
#define kPageBlockCount               64    /* page / block */
#define kBlockSize                    (kSectorSize*kSectorPageCount*kPageBlockCount) /* Block size in byte */
#define kPhysicalBlockcount           512 /* total count of block */
#define kLogicalPageCount             (((kPhysicalBlockcount-4)*kBlockSize)/(kPageBlockCount*kSectorPageCount*kSectorSize)) /* total logical page count */
#define kPhysicalPageCount            ((kPhysicalBlockcount*kBlockSize)/(kPageBlockCount*kSectorPageCount*kSectorSize))     /* total physical page count */
#define kLogicalSecteurCount	      (kLogicalPageCount*kSectorPageCount)	/* total logical sector count */
#define kDefaultPageAdress            UINT32_MAX
#define kDefaultBlockAdress           UINT32_MAX

/**************************************************************
						Macros
***************************************************************/
#define mPageFromSector(Sector)       (Sector/kSectorPageCount)				/* Get the page address from sector address */
#define mSectorOffsetValue( sector )  (sector%kSectorPageCount)             /* Offset of a sector in a page, numeric value: 0,1,2,3 */
#define mSectorOffsetMask( sector )   ( 0x01<<mSectorOffsetValue( sector )) /* Offset of a sector in a page, binary mask! 0x01,0x02,0x04,0x08*/

/**************************************************************
						Typedef
***************************************************************/
typedef struct
{
	uint32_t uiPhysicalPageAdresse; /* Physical page allocated to the logical page*/
	uint32_t uiAge;					/* Page age in operation count, mean small number -> old page*/
	uint8_t ucBlankSectorStatus;    /* b0..b3 set for blank sectors*/
} tLogicalPageInfo;

typedef struct
{
	uint8_t tucPageBlankStatus[kPageBlockCount/8]; /* bit are set to 1 if page is blank*/
	uint8_t tucPageFreeStatus[kPageBlockCount/8];  /* bit are set to 0 if page is free */
} tPhysicalBlockInfo;

typedef struct
{
	uint32_t uiLogicalIndex; /* Logical page where this sector is allocated */
	uint32_t uiAge;			 /* Sector age in operation count, mean small number -> old page*/
	uint8_t ucStatus;        /* 0x00 mean written sector*/

}  tNtl_SpareAreData;

struct
{
	tLogicalPageInfo tPageInfo[kLogicalPageCount];
	tPhysicalBlockInfo tPhysicalBlockInfo[kPhysicalBlockcount];
	uint32_t uiNextBlankPageAddress;
	uint32_t uiSpareBlockAddress;
	uint32_t uiGlobalOperationCount;
} tLIB_Ntl_InternalData;

/**************************************************************
				Local Private functions prototypes
***************************************************************/
static int LIB_Ntli_BlockErase( uint32_t uiBlockAddress );
static int LIB_Ntli_FindBlankPage( uint32_t uiBlockAddress );

/**************************************************************
				public functions
***************************************************************/

void LIB_Ntl_init( void )
{
	uint32_t uiPhysicalPageAdresse;
	uint32_t uiSectorIndexInPage;
	uint32_t uiBlockAddress;
	uint8_t ucBlockIsFree=1,ucBlockIsBlank=1,ucPageisBlank=1,ucIndex;
	DRV_Nand_SectorSpareData NandSpareArea;
	tNtl_SpareAreData *pNtlSpareData;


	memset(tLIB_Ntl_InternalData,0xffff,sizeof(tLIB_Ntl_InternalData));
	tLIB_Ntl_InternalData.uiGlobalOperationCount=0;

	/* Read sector state in flash*/
	for( uiPhysicalPageAdresse=0;uiPhysicalPageAdresse<kPhysicalPageCount;uiPhysicalPageAdresse++)
	{
		if( !(uiPhysicalPageAdresse%kPageBlockCount) )
		{
			uiBlockAddress=(uiPhysicalPageAdresse-1)/kPageBlockCount;
			if( uiPhysicalPageAdresse && ucBlockIsFree && !ucBlockIsBlank )
			{
				LIB_Ntli_BlockErase(uiBlockAddress);
				ucBlockIsBlank = 1;
			}
			if( uiPhysicalPageAdresse && ucBlockIsBlank &&  tLIB_Ntl_InternalData.uiSpareBlockAddress == kDefaultBlockAdress)
			{
				/* The last block is blank use it as spare block  */
				tLIB_Ntl_InternalData.uiSpareBlockAddress = uiBlockAddress;
			}
			ucBlockIsFree = 1;
			ucBlockIsBlank = 1;
		}
		ucPageisBlank=1;
		/* Read sector spare data in the page */
		for( uiSectorIndexInPage=0  ;uiSectorIndexInPage<kSectorPageCount ; uiSectorIndexInPage++)
		{
			BBM_SectorRead(NULL,&SpareArea,uiPhysicalPageAdresse*kSectorPageCount+uiSectorIndexInPage);

			pNtlSpareData = &SpareArea.tucData[0];
			/* check is sector is used */
			if( pNtlSpareData->ucStatus != 0xFF)
			{
				ucBlockIsBlank =0; /* This block is not blank */
				ucPageisBlank=0; /* This page is not blank*/
				if( pNtlSpareData->uiLogicalIndex < kLogicalPageCount )
				{
					uint32_t uiPreviousAllocatedPhysicalPage = kDefaultPageAdress;
					/* if the logical sector is already allocate save the physical page */
					if( tLIB_Ntl_InternalData.tPageInfo[pNtlSpareData->uiLogicalIndex].uiPhysicalPageAdresse != kDefaultBlockAdress )
					{
						uiPreviousAllocatedPhysicalPage = tLIB_Ntl_InternalData.tPageInfo[pNtlSpareData->uiLogicalIndex].uiPhysicalPageAdresse;

						if( uiPreviousAllocatedPhysicalPage != kDefaultBlockAdress  && uiPreviousAllocatedPhysicalPage != uiPhysicalPageAdresse )
						{	/* compare ages of the 2 pages, get only the youngest */
							if( tLIB_Ntl_InternalData.tPageInfo[pNtlSpareData->uiLogicalIndex].uiAge < pNtlSpareData->uiAge )
							{
								/* The previous allocated page is free */
								tLIB_Ntl_InternalData.tPhysicalBlockInfo[ uiPreviousAllocatedPhysicalPage/kPageBlockCount].tucPageFreeStatus[(uiPreviousAllocatedPhysicalPage%kPageBlockCount)/8] &= ~(0x01 << ((uiPreviousAllocatedPhysicalPage%kPageBlockCount)%8));
								/* This page is not free */
								uiPreviousAllocatedPhysicalPage = kDefaultPageAdress;
							}
							else
							{
								/* This page is free */
								tLIB_Ntl_InternalData.tPhysicalBlockInfo[ uiPhysicalPageAdresse/kPageBlockCount].tucPageFreeStatus[(uiPhysicalPageAdresse%kPageBlockCount)/8] &= ~(0x01 << ((uiPhysicalPageAdresse%kPageBlockCount)%8));
							}
						}
					}
					/* The physical sector is valid if the logical sector is set not allocated */
					if( uiPreviousAllocatedPhysicalPage == kDefaultPageAdress  )
					{
						/* This block is not free */
						ucBlockIsFree=0;
						/* Sector in use*/
						tLIB_Ntl_InternalData.tPageInfo[pNtlSpareData->uiLogicalIndex].ucBlankSectorStatus &= ~mSectorOffsetMask(uiSectorIndexInPage);
						/* Set physical address of the page*/
						tLIB_Ntl_InternalData.tPageInfo[pNtlSpareData->uiLogicalIndex].uiPhysicalPageAdresse = uiPhysicalPageAdresse;
						/* Set age of the page */
						tLIB_Ntl_InternalData.tPageInfo[pNtlSpareData->uiLogicalIndex].uiAge = pNtlSpareData->uiAge;
						/* Update global age */
						if ( pNtlSpareData->uiAge > tLIB_Ntl_InternalData.uiGlobalOperationCount )
							tLIB_Ntl_InternalData.uiGlobalOperationCount=pNtlSpareData->uiAge;
					}

				}
				else
					return -1;
			}
		}
		if( !ucPageisBlank )
		{
			/* Reset Physical Page status bit */
			tLIB_Ntl_InternalData.tPhysicalBlockInfo[uiPhysicalPageAdresse/kPageBlockCount].tucPageBlankStatus[(uiPhysicalPageAdresse%kPageBlockCount)/8] &= ~(0x01 << ((uiPhysicalPageAdresse%kPageBlockCount)%8));
		}

	}
	/* find first blank page*/
	for( uiBlockAddress=0;uiBlockAddress < kPhysicalBlockcount && tLIB_Ntl_InternalData.uiNextBlankPageAddress == kDefaultPageAdress ;uiBlockAddress++)
	{
		uiPhysicalPageAdresse = LIB_Ntli_FindBlankPage(kPhysicalBlockcount);
		if( kDefaultPageAdress != uiPhysicalPageAdresse )
		{
			uiPhysicalPageAdresse += (uiBlockAddress * kPageBlockCount);
			tLIB_Ntl_InternalData.uiNextBlankPageAddress=uiPhysicalPageAdresse;
		}
	}
	if( tLIB_Ntl_InternalData.uiNextBlankPageAddress == kDefaultPageAdress  || tLIB_Ntl_InternalData.uiSpareBlockAddress == kDefaultBlockAdress )
		return -2;
}

int LIB_Ntl_SectorWrite( uint8_t *pucData , uint32_t uiSectorAddress)
{
	uint32_t uiPhysicalPageAdress;
	uint32_t uiNewPageAdress;
	uint32_t LogicalPageAdress = mPageFromSector(uiSectorAddress);
	int iReturn=0;

	if( uiSectorAddress >= kLogicalSecteurCount )
		return -1;

	if (InternalData.tPageInfo[ LogicalPageAdress ].ucBlankSectorStatus & mSectorOffsetMask(uiSectorAddress))
	{
		/* The Sector is blank*/
		uiPhysicalPageAdress = tLIB_Ntl_InternalData.tPageInfo[ LogicalPageAdress ].uiPhysicalPageAdresse;
		if( uiPhysicalPageAdress != kDefaultPageAdress )
		{
			/* The page is allocated write into the blank sector*/
			LIb_Ntli_WritePhysicalSector( pucData , uiPhysicalPageAdress*kSectorPageCount + mSectorOffsetValue(uiSectorAddress) , LogicalPageAdress );
		}
		else
		{
			/* The logical page is not allocated */
			uiNewPageAdress = LIB_Ntli_GetFreePage();
			if( uiNewPageAdress != kDefaultPageAdress )
			{
				iReturn = LIb_Ntli_WritePhysicalSector( pucData , uiNewPageAdress *kSectorPageCount + mSectorOffsetValue(uiSectorAddress) , LogicalPageAdress );
			}
			else
				iReturn=-1;
		}

	}
	else
	{
		uint8_t ucSectorIndex;
		uint8_t tucPageReadBuffer[kSectorSize];

		/* The physical sector is not blank, move to a new physical page */
		uiNewPageAdress = LIB_Ntli_GetFreePage();
		if( uiNewPageAdress != kDefaultPageAdress )
		{
			/* The new page is valid*/
			for (ucSectorIndex = 0; ucSectorIndex < kSectorPageCount && iReturn == 0x00 ; ++ucSectorIndex)
			{
				/* Copy other sector or write current sector ? */
				if( ucSectorIndex != mSectorOffsetValue(uiSectorAddress) )
				{
					/*copy other sector only if it's not blank */
					if( !( InternalData.tPageInfo[ LogicalPageAdress ].ucucBlankSectorStatus & mSectorOffsetMask(ucSectorIndex) ) )
					{
						/* Read Sector in old place */
						BBM_SectorRead( tucPageReadBuffer , NULL , uiPhysicalPageAdress * kSectorPageCount + ucSectorIndex );
						/* Write sector in new place */
						iReturn = LIb_Ntli_WritePhysicalSector( tucPageReadBuffer , uiNewPageAdress *kSectorPageCount + ucSectorIndex , LogicalPageAdress );
					}
				}
				else /* Write current sector in new place */
					iReturn = LIb_Ntli_WritePhysicalSector( pucData , uiNewPageAdress *kSectorPageCount + ucSectorIndex , LogicalPageAdress );

			}
			/* The old page is now free */
			tLIB_Ntl_InternalData.tPhysicalBlockInfo[ uiPhysicalPageAdress/kPageBlockCount].tucPageFreeStatus[(uiPhysicalPageAdress%kPageBlockCount)/8] &= ~(0x01 << ((uiPhysicalPageAdress%kPageBlockCount)%8));
		}
	}

	return iReturn;
}

int LIb_Ntli_WritePhysicalSector(  uint8_t *pucData , uint32_t uiPhysicalSectorAddress , uint32_t uiLogicalPageAdress )
{
	DRV_Nand_SectorSpareData NandSpareArea;
	tNtl_SpareAreData *pNtlSpareData = &(NandSpareArea.tucData[0]);
	uint32_t uiPhysicalPageAdress = uiPhysicalSectorAddress/kSectorPageCount;

	/* Sector in use*/
	tLIB_Ntl_InternalData.tPageInfo[ uiLogicalPageAdress ].ucBlankSectorStatus&=~( mSectorOffsetMask(uiPhysicalSectorAddress));
	/* Set physical address of the page*/
	tLIB_Ntl_InternalData.tPageInfo[ uiLogicalPageAdress ].uiPhysicalPageAdresse = uiPhysicalPageAdress;
	/* Set age of the page */
	tLIB_Ntl_InternalData.tPageInfo[ uiLogicalPageAdress ].uiAge = tLIB_Ntl_InternalData.uiGlobalOperationCount++;
	/* Update page blank satuts */
	tLIB_Ntl_InternalData.tPhysicalBlockInfo[ uiPhysicalPageAdress/kPageBlockCount].tucPageBlankStatus[(uiPhysicalPageAdress%kPageBlockCount)/8] &= ~(0x01 << ((uiPhysicalPageAdress%kPageBlockCount)%8));
	/* Set Spare area data */
	pNtlSpareData->ucStatus=0x00;
	pNtlSpareData->uiAge=tLIB_Ntl_InternalData.uiGlobalOperationCount++;
	pNtlSpareData->uiLogicalIndex=mPageFromSector(uiLogicalPageAdress);

	return BBM_sectorWrite( pucData , &NandSpareArea ,uiPhysicalSectorAddress  );
}

int LIB_Ntl_SectorRead( uint8_t *pucData , uint32_t uiSectorAddress)
{
	uint32_t uiPhysicalPageAdress;
	uint32_t LogicalPageAdress = mPageFromSector(uiSectorAddress);
	int iReturn=0;

	if( uiSectorAddress >= kLogicalSecteurCount )
		return -1;

	if (tLIB_Ntl_InternalData.tPageInfo[ LogicalPageAdress ].ucBlankSectorStatus & mSectorOffsetMask(uiSectorAddress))
		memset(pucData,0xff,kSectorSize); /* Sector is blank */
	else
	{
		/* Sector is not blank */
		uiPhysicalPageIndex = tLIB_Ntl_InternalData.tPageInfo[ LogicalPageAdress ].uiPhysicalPageAdresse;
		if( uiPhysicalPageIndex < kPhysicalPageCount )
			iReturn=BBM_SectorRead( pucData , NULL , uiPhysicalPageIndex * kSectorPageCount + mSectorOffsetValue(uiSectorAddress));
		else
			iReturn=-1;
	}

	return iReturn;
}

/**************************************************************
				Local Private functions
***************************************************************/
static int LIB_Ntli_BlockErase( uint32_t uiBlockAddress )
{
	uint8_t ucIndex;
	/* The last block is free and not blank, erase it */
	BBM_BlockErase(uiBlockAddress);

	for (ucIndex = 0; ucIndex < kPageBlockCount/8; ++ucIndex)
	{
		tLIB_Ntl_InternalData.tPhysicalBlockInfo[uiBlockAddress].tucPageBlankStatus[ucIndex]=0xFF;
	}

}

static int LIB_Ntli_FindBlankPage( uint32_t uiBlockAddress )
{
	uint8_t ucIndex,ucPageindex,ucData , ucMask=0x1;

	if (uiBlockAddress == tLIB_Ntl_InternalData.uiSpareBlockAddress )
		return kDefaultPageAdress;

	for (ucIndex = 0; ucIndex < kPageBlockCount/8; ++ucIndex)
	{
		ucData = tLIB_Ntl_InternalData.tPhysicalBlockInfo[uiBlockAddress].tucPageBlankStatus[ucIndex];
		if( ucData != 0x00 )
		{
			ucPageindex=ucIndex*8;
			while(  !(ucData & ucMask) )
			{
				ucMask<<1;
				ucPageindex++;
			}
			return ucPageindex;
		}
	}
	return kDefaultPageAdress;
}
