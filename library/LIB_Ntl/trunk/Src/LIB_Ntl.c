/*
 * LIB_Ntl.c
 *
 *  Created on: 19 mars 2011
 *      Author: cara
 */
#include <stdint.h>
#include <string.h>
#include "LIB_Ntl.h"
#include "Inc/LIB_Ntl_p.h"
#include "Inc/LIB_Ntl_BBM.h"


#define kSectorSize                   512 /* size in Byte */
#define kSectorPageCount              4    /* sector / page */
#define kPageBlockCount               64    /* page / block */
#define kBlockSize                    (kSectorSize*kSectorPageCount*kPageBlockCount)
#define kPhysicalBlockcount           512
#define kLogicalPageCount             (((kPhysicalBlockcount-4)*kBlockSize)/(kPageBlockCount*kSectorPageCount*kSectorSize))
#define kPhysicalPageCount            ((kPhysicalBlockcount*kBlockSize)/(kPageBlockCount*kSectorPageCount*kSectorSize))
#define kLogicalSecteurCount	      (kLogicalPageCount*kSectorPageCount)
#define kPageFromSector(Sector)       (Sector/kSectorPageCount)
#define kSectorOffsetValue( sector ) (sector%kSectorPageCount)
#define kSectorOffsetMask( sector )   ( 0x01<<kSectorOffsetValue( sector ))
#define kNTL_DefaultPageIndex   UINT32_MAX
#define kNTL_DefaultBlockIndex   UINT32_MAX

typedef struct
{
	uint32_t uiPhysicalPageAdresse;
	uint32_t uiAge;
	uint8_t ucStatus;
} tLogicalPageInfo;

typedef struct
{
	uint8_t tucPageBlankStatus[kPageBlockCount/8];
	uint8_t tucPageFreeStatus[kPageBlockCount/8];
} tPhysicalBlockInfo;

typedef struct
{
	uint32_t uiLogicalIndex;
	uint32_t uiAge;
	uint8_t ucStatus;

}  tNtl_SpareAreData;

struct
{
	tLogicalPageInfo tPageInfo[kLogicalPageCount];
	tPhysicalBlockInfo tPhysicalBlockInfo[kPhysicalBlockcount];
	uint32_t uiNextBlankPageAddress;
	uint32_t uiSpareBlockAddress;
	uint32_t uiGlobalOperationCount;
} tLIB_Ntl_InternalData;

static int LIB_Ntli_BlockErase( uint32_t uiBlockAddress );
static int LIB_Ntli_FindBlankPage( uint32_t uiBlockAddress );

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
			if( uiPhysicalPageAdresse && ucBlockIsBlank &&  tLIB_Ntl_InternalData.uiSpareBlockAddress == kNTL_DefaultBlockIndex)
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

			if( pNtlSpareData->ucStatus != 0xFF)
			{

				ucBlockIsBlank =0; /* This block is not blank */
				ucPageisBlank=0; /* This page is not blank*/
				if( pNtlSpareData->uiLogicalIndex < kLogicalPageCount )
				{
					/* The physical sector is valid if the logical sector is set as not valid or is older */
					if( tLIB_Ntl_InternalData.tPageInfo[pNtlSpareData->uiLogicalIndex].ucStatus == 0xff ||
						tLIB_Ntl_InternalData.tPageInfo[pNtlSpareData->uiLogicalIndex].uiAge < pNtlSpareData->uiAge )
					{
						/* This block is not free */
						ucBlockIsFree=0;
						/* Sector in use*/
						tLIB_Ntl_InternalData.tPageInfo[pNtlSpareData->uiLogicalIndex].ucStatus=0;
						/* Set physical address of the page*/
						tLIB_Ntl_InternalData.tPageInfo[pNtlSpareData->uiLogicalIndex].uiPhysicalPageAdresse = uiPhysicalPageAdresse;
						/* Set age of the page */
						tLIB_Ntl_InternalData.tPageInfo[pNtlSpareData->uiLogicalIndex].uiAge = pNtlSpareData->uiAge;
						/* Update global age */
						if ( pNtlSpareData->uiAge > tLIB_Ntl_InternalData.uiGlobalOperationCount )
							tLIB_Ntl_InternalData.uiGlobalOperationCount=pNtlSpareData->uiAge;
					}
					else
					{   /* The older page is free */
						tLIB_Ntl_InternalData.tPhysicalBlockInfo[ uiPhysicalPageAdress/kPageBlockCount].tucPageFreeStatus[(uiPhysicalPageAdress%kPageBlockCount)/8] &= ~(0x01 << ((uiPhysicalPageAdress%kPageBlockCount)%8));
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
	for( uiBlockAddress=0;uiBlockAddress < kPhysicalBlockcount && tLIB_Ntl_InternalData.uiNextBlankPageAddress == kNTL_DefaultPageIndex ;uiBlockAddress++)
	{
		uiPhysicalPageAdresse = LIB_Ntli_FindBlankPage(kPhysicalBlockcount);
		if( kNTL_DefaultPageIndex != uiPhysicalPageAdresse )
		{
			uiPhysicalPageAdresse += (uiBlockAddress * kPageBlockCount);
			tLIB_Ntl_InternalData.uiNextBlankPageAddress=uiPhysicalPageAdresse;
		}
	}
	if( tLIB_Ntl_InternalData.uiNextBlankPageAddress == kNTL_DefaultPageIndex  || tLIB_Ntl_InternalData.uiSpareBlockAddress == kNTL_DefaultBlockIndex )
		return -2;
}

int LIB_Ntl_SectorWrite( uint8_t *pucData , uint32_t uiSectorAddress)
{
	uint32_t uiPhysicalPageAdress;
	uint32_t uiNewPageAdress;
	uint32_t LogicalPageAdress = kPageFromSector(uiSectorAddress);
	int iReturn=0;

	if( uiSectorAddress >= kLogicalSecteurCount )
		return -1;

	if (InternalData.tPageInfo[ LogicalPageAdress ].ucStatus & kSectorOffsetMask(uiSectorAddress))
	{
		/* The Sector is blank*/
		uiPhysicalPageAdress = tLIB_Ntl_InternalData.tPageInfo[ LogicalPageAdress ].uiPhysicalPageAdresse;
		if( uiPhysicalPageAdress != kNTL_DefaultPageIndex )
		{
			/* The page is allocated write into the blank sector*/
			LIb_Ntli_WritePhysicalSector( pucData , uiPhysicalPageAdress*kSectorPageCount + kSectorOffsetValue(uiSectorAddress) , LogicalPageAdress );
		}
		else
		{
			/* The logical page is not allocated */
			uiNewPageAdress = LIB_Ntli_GetFreePage();
			if( uiNewPageAdress != kNTL_DefaultPageIndex )
			{
				iReturn = LIb_Ntli_WritePhysicalSector( pucData , uiNewPageAdress *kSectorPageCount + kSectorOffsetValue(uiSectorAddress) , LogicalPageAdress );
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
		if( uiNewPageAdress != kNTL_DefaultPageIndex )
		{
			/* The new page is valid*/
			for (ucSectorIndex = 0; ucSectorIndex < kSectorPageCount && iReturn == 0x00 ; ++ucSectorIndex)
			{
				/* Copy other sector or write current sector ? */
				if( ucSectorIndex != kSectorOffsetValue(uiSectorAddress) )
				{
					/*copy other sector only if it's not blank */
					if( !( InternalData.tPageInfo[ LogicalPageAdress ].ucStatus & kSectorOffsetMask(ucSectorIndex) ) )
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
	tLIB_Ntl_InternalData.tPageInfo[ uiLogicalPageAdress ].ucStatus&=~( kSectorOffsetMask(uiPhysicalSectorAddress));
	/* Set physical address of the page*/
	tLIB_Ntl_InternalData.tPageInfo[ uiLogicalPageAdress ].uiPhysicalPageAdresse = uiPhysicalPageAdress;
	/* Set age of the page */
	tLIB_Ntl_InternalData.tPageInfo[ uiLogicalPageAdress ].uiAge = tLIB_Ntl_InternalData.uiGlobalOperationCount++;
	/* Update page blank satuts */
	tLIB_Ntl_InternalData.tPhysicalBlockInfo[ uiPhysicalPageAdress/kPageBlockCount].tucPageBlankStatus[(uiPhysicalPageAdress%kPageBlockCount)/8] &= ~(0x01 << ((uiPhysicalPageAdress%kPageBlockCount)%8));
	/* Set Spare area data */
	pNtlSpareData->ucStatus=0x00;
	pNtlSpareData->uiAge=tLIB_Ntl_InternalData.uiGlobalOperationCount++;
	pNtlSpareData->uiLogicalIndex=kPageFromSector(uiLogicalPageAdress);

	return BBM_sectorWrite( pucData , &NandSpareArea ,uiPhysicalSectorAddress  );
}

int LIB_Ntl_SectorRead( uint8_t *pucData , uint32_t uiSectorAddress)
{
	uint32_t uiPhysicalPageAdress;
	uint32_t LogicalPageAdress = kPageFromSector(uiSectorAddress);
	int iReturn=0;

	if( uiSectorAddress >= kLogicalSecteurCount )
		return -1;

	if (InternalData.tPageInfo[ LogicalPageAdress ].ucStatus == OxFF & kSectorOffsetMask(uiSectorAddress))
		memset(pucData,0xff,kSectorSize); /* Sector is blank */
	else
	{
		/* Sector is not blank */
		uiPhysicalPageIndex = tLIB_Ntl_InternalData.tPageInfo[ LogicalPageAdress ].uiPhysicalPageAdresse;
		if( uiPhysicalPageIndex < kPhysicalPageCount )
			iReturn=BBM_SectorRead( pucData , NULL , uiPhysicalPageIndex * kSectorPageCount + kSectorOffsetValue(uiSectorAddress));
		else
			iReturn=-1;
	}

	return iReturn;
}

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
		return kNTL_DefaultPageIndex;

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
	return kNTL_DefaultPageIndex;
}
