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
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "DRV_Nand.h"
#include "Inc/DRV_Nand_p.h"
#include "DRV_Nand_CFG.h"

#define kNAND_BLOCK_SIZE ((kNAND_PAGE_SIZE+kNAND_OOB_SIZE)*kNAND_PAGE_COUNT)
#define kNAND_TOTAL_SIZE (kNAND_BLOCK_SIZE*kNAND_BLOCK_COUNT)
#define kNAND_UOOB_SIZE  ( kNAND_OOB_SIZE - sizeof(DRV_Nand_OobData) )
#define kNANd_FIRST_PAGE( uiPageIndex )  (uiPageIndex-uiPageIndex%kNAND_PAGE_COUNT )*(kNAND_PAGE_SIZE+kNAND_OOB_SIZE)
typedef struct
{
	uint8_t ucBadBlockMarker;
	uint8_t tucEcc[12];
} DRV_Nand_OobData;

uint8_t *pucSimuNandBuffer=NULL;

void DRV_Nand_Init( void )
{
	int NandFileDesc;
	printf("Read Nand simulation file:%s\n",kNAND_FILE_NAME);
	NandFileDesc = open( kNAND_FILE_NAME , O_RDWR);
	if(  NandFileDesc == -1 )
	{
		printf("File not found create it\n");
		NandFileDesc = open( kNAND_FILE_NAME , O_RDWR|O_CREAT,S_IWUSR|S_IRUSR);
		if(  NandFileDesc == -1 )
		{
			printf("Enable to create file, exit\n");	
			exit(1);
		}
		printf("Allocate Nand simulation buffer, size=%dkB\n",kNAND_TOTAL_SIZE/1024);
		pucSimuNandBuffer = malloc(kNAND_TOTAL_SIZE);
		memset(pucSimuNandBuffer,0xff,kNAND_TOTAL_SIZE);
		write(NandFileDesc, pucSimuNandBuffer,kNAND_TOTAL_SIZE);
	}
	else
	{
		printf("Allocate Nand simulation buffer, size=%dkB\n",kNAND_TOTAL_SIZE/1024);
		pucSimuNandBuffer = malloc(kNAND_TOTAL_SIZE);
		printf("File found, copy it to buffer\n");
		read(NandFileDesc,pucSimuNandBuffer,kNAND_TOTAL_SIZE);
	}
	printf("Close simulation file\n");
	close(NandFileDesc);
}


void DRV_Nand_Terminate( void )
{
	int NandFileDesc;

	NandFileDesc = open( kNAND_FILE_NAME , O_RDWR );
	if(  NandFileDesc != -1 )
	{
		printf("Save Nand simulation file:%s\n",kNAND_FILE_NAME);
		write(NandFileDesc, pucSimuNandBuffer,kNAND_TOTAL_SIZE);
		close(NandFileDesc);
		
	}
	else
	{
		printf("Error while opening nand file,can't save data\n");
	}
	
	printf("Free Nand simulation buffer\n");
	if( pucSimuNandBuffer)
		free(pucSimuNandBuffer);
}

int DRV_Nand_InfoRead( DRV_Nand_Id *pId , DRV_Nand_Size_Info *pSizeInfo)
{
	pSizeInfo->uiPageSize = kNAND_PAGE_SIZE;
	pSizeInfo->uiPagePerBlock = kNAND_PAGE_COUNT;
	pSizeInfo->uiBlockCount = kNAND_BLOCK_COUNT;
	pSizeInfo->uiOobSize = kNAND_OOB_SIZE-sizeof(DRV_Nand_OobData);
	return NAND_OK;
}


int DRV_Nand_PageRead( uint8_t *pucDataBuffer , uint8_t *pucUserOobBuffer , uint32_t uiPageIndex )
{
	uint8_t *pucNandBuffer=pucSimuNandBuffer+kNANd_FIRST_PAGE(uiPageIndex); /* first page of the block */


	if( pucDataBuffer )
		memcpy( pucDataBuffer, pucSimuNandBuffer+uiPageIndex*(kNAND_PAGE_SIZE+kNAND_OOB_SIZE),kNAND_PAGE_SIZE);
	if( pucUserOobBuffer )
		memcpy( pucUserOobBuffer, pucSimuNandBuffer+uiPageIndex*(kNAND_PAGE_SIZE+kNAND_OOB_SIZE)+kNAND_PAGE_SIZE+sizeof(DRV_Nand_OobData),kNAND_UOOB_SIZE);

	/* Read bad block marker */
	if( pucNandBuffer[kNAND_PAGE_SIZE] != 0xff  ) 
		return NAND_ERR_BAD_BL;
	else
		return NAND_OK;
}


int DRV_Nand_PageWrite( uint8_t *pucDataBuffer , uint8_t *pucUserOobBuffer , uint32_t uiPageIndex )
{
	unsigned int uiByteIndex=0;
	uint8_t *pucNandBuffer=pucSimuNandBuffer+kNANd_FIRST_PAGE(uiPageIndex); /* first page of the block */

	/* Read bad block marker */
	if( pucNandBuffer[kNAND_PAGE_SIZE] != 0xff  ) 
		return NAND_ERR_BAD_BL;
	/* get page offset */
	pucNandBuffer=pucSimuNandBuffer+uiPageIndex*(kNAND_PAGE_SIZE+kNAND_OOB_SIZE);
	for( uiByteIndex=0 ; uiByteIndex<kNAND_PAGE_SIZE ; uiByteIndex++)
	{
		if( pucDataBuffer )
			(*pucNandBuffer) &= pucDataBuffer[uiByteIndex];
		pucNandBuffer++;
	}
	/* skip drv oob data */
	pucNandBuffer+=sizeof(DRV_Nand_OobData);
	/* write user oob */
	for( uiByteIndex=0 ; uiByteIndex<kNAND_UOOB_SIZE ; uiByteIndex++)
	{
		if( pucUserOobBuffer )
			(*pucNandBuffer) &= pucUserOobBuffer[uiByteIndex];
		pucNandBuffer++;
	}
	return NAND_OK;
}

int DRV_Nand_PageCopy( uint32_t uiSrcPageIndex , uint32_t uiDestPageIndex )
{
	uint8_t *pucDestNandBuffer=pucSimuNandBuffer+kNANd_FIRST_PAGE(uiDestPageIndex); /* first page of the block */
	uint8_t *pucSrcNandBuffer=pucSimuNandBuffer+uiSrcPageIndex*(kNAND_PAGE_SIZE+kNAND_OOB_SIZE);

	/* Read bad block marker */
	if( pucDestNandBuffer[kNAND_PAGE_SIZE] != 0xff  ) 
		return NAND_ERR_BAD_BL;

	pucDestNandBuffer=pucSimuNandBuffer+uiDestPageIndex*(kNAND_PAGE_SIZE+kNAND_OOB_SIZE);

	memcpy( pucDestNandBuffer , pucSrcNandBuffer ,  kNAND_PAGE_SIZE+kNAND_OOB_SIZE );

	return NAND_OK;
}

int DRV_Nand_BlockErase( uint32_t uiBlockIndex )
{
	uint8_t *pucNandBuffer=pucSimuNandBuffer+uiBlockIndex*kNAND_PAGE_COUNT*(kNAND_PAGE_SIZE+kNAND_OOB_SIZE); /* first page of the block */
	/* Read bad block marker */
	if( pucNandBuffer[kNAND_PAGE_SIZE] != 0xff  ) 
		return NAND_ERR_BAD_BL;

	memset( pucSimuNandBuffer+ kNAND_BLOCK_SIZE*uiBlockIndex,0xff,kNAND_BLOCK_SIZE);

	return NAND_OK;
}


void  DRV_Nand_BadBlockSet( uint32_t uiBlockIndex )
{
	uint8_t *pucNandBuffer=pucSimuNandBuffer+uiBlockIndex*kNAND_PAGE_COUNT*(kNAND_PAGE_SIZE+kNAND_OOB_SIZE);

	pucNandBuffer[kNAND_PAGE_SIZE] = 0;
}

