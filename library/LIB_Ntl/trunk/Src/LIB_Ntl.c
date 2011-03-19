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


#define kSectorSize             512 /* size in Byte */
#define kSectorPageCount        4    /* sector / page */
#define kPageBlockCount         64    /* page / block */
#define kBlockSize              (kSectorSize*kSectorPageCount*kPageBlockCount)
#define kPhysicalBlockcount     512
#define kLogicalPageCount       (((kPhysicalBlockcount-4)*kBlockSize)/(kPageBlockCount*kSectorPageCount*kSectorSize))
#define kPhysicalPageCount      ((kPhysicalBlockcount*kBlockSize)/(kPageBlockCount*kSectorPageCount*kSectorSize))
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
	uint32_t uiNextFreePageAddress;
	uint32_t uiSpareBlockAddress;
	uint32_t uiGlobalOperationCount;
} tLIB_Ntl_InternalData;

static int LIB_Ntli_BlockErase( uint32_t uiBlockAddress );
static int LIB_Ntli_FindFreePage( uint32_t uiBlockAddress );

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
	for( uiBlockAddress=0;uiBlockAddress < kPhysicalBlockcount && tLIB_Ntl_InternalData.uiNextFreePageAddress == kNTL_DefaultPageIndex ;uiBlockAddress++)
	{
		uiPhysicalPageAdresse = LIB_Ntli_FindFreePage(kPhysicalBlockcount);
		if( kNTL_DefaultPageIndex != uiPhysicalPageAdresse )
		{
			uiPhysicalPageAdresse += (uiBlockAddress * kPageBlockCount);
			tLIB_Ntl_InternalData.uiNextFreePageAddress=uiPhysicalPageAdresse;
		}
	}
	if( tLIB_Ntl_InternalData.uiNextFreePageAddress == kNTL_DefaultPageIndex  || tLIB_Ntl_InternalData.uiSpareBlockAddress == kNTL_DefaultBlockIndex )
		return -2;
}

int LIB_Ntl_SectorWrite( uint8_t *pucData , uint32_t uiSectorAddress)
{

}

int LIB_Ntl_SectorRead( uint8_t *pucData , uint32_t uiSectorAddress)
{

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

static int LIB_Ntli_FindFreePage( uint32_t uiBlockAddress )
{
	uint8_t ucIndex,ucPageindex,ucData , ucMask=0x1;
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
