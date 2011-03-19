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
#include <stdio.h>
#include "stm32f10x.h"
#include "DRV_Nand.h"
#include "Inc/DRV_Nand_p.h"

#ifndef __IO
#define __IO
#endif
#define FSMC_Bank_NAND         FSMC_Bank2_NAND
#define Bank_NAND_ADDR         Bank2_NAND_ADDR
#define Bank2_NAND_ADDR        ((uint32_t)0x70000000)
#define NAND_Sector_Size       512
#define NAND_PageDataSize      2048
#define NAND_PageSpareAreaSize 64
#define NAND_Page PerBlock     64
#define NAND_BlockSize         ( NAND_Page PerBlock * NAND_PageSize )

#define ADDR_1st_CYCLE(ADDR)       (uint8_t)((ADDR)& 0xFF)               /* 1st addressing cycle */
#define ADDR_2nd_CYCLE(ADDR)       (uint8_t)(((ADDR)& 0xFF00) >> 8)      /* 2nd addressing cycle */
#define ADDR_3rd_CYCLE(ADDR)       (uint8_t)(((ADDR)& 0xFF0000) >> 16)   /* 3rd addressing cycle */
#define ADDR_4th_CYCLE(ADDR)       (uint8_t)(((ADDR)& 0xFF000000) >> 24) /* 4th addressing cycle */

static uint32_t DRV_Nandi_ReadStatus(void);
static uint32_t DRV_Nandi_GetStatus(void);

void DRV_Nand_Init( void )
{
	  GPIO_InitTypeDef GPIO_InitStructure;
	  FSMC_NANDInitTypeDef FSMC_NANDInitStructure;
	  FSMC_NAND_PCCARDTimingInitTypeDef  p;

	  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC, ENABLE);
	  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE |
	                         RCC_APB2Periph_GPIOF | RCC_APB2Periph_GPIOG, ENABLE);

	/*-- GPIO Configuration ------------------------------------------------------*/
	/* CLE, ALE, D0->D3, NOE, NWE and NCE2  NAND pin configuration  */
	  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_14 | GPIO_Pin_15 |
	                                 GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_4 | GPIO_Pin_5 |
	                                 GPIO_Pin_7;
	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;

	  GPIO_Init(GPIOD, &GPIO_InitStructure);

	/* D4->D7 NAND pin configuration  */
	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10;

	  GPIO_Init(GPIOE, &GPIO_InitStructure);


	/* NWAIT NAND pin configuration */
	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;

	  GPIO_Init(GPIOD, &GPIO_InitStructure);

	/* INT2 NAND pin configuration */
	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	  GPIO_Init(GPIOG, &GPIO_InitStructure);

	  /*-- FSMC Configuration ------------------------------------------------------*/
	  p.FSMC_SetupTime = 0x1;
	  p.FSMC_WaitSetupTime = 0x3;
	  p.FSMC_HoldSetupTime = 0x2;
	  p.FSMC_HiZSetupTime = 0x1;

	  FSMC_NANDInitStructure.FSMC_Bank = FSMC_Bank2_NAND;
	  FSMC_NANDInitStructure.FSMC_Waitfeature = FSMC_Waitfeature_Enable;
	  FSMC_NANDInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_8b;
	  FSMC_NANDInitStructure.FSMC_ECC = FSMC_ECC_Enable;
	  FSMC_NANDInitStructure.FSMC_ECCPageSize = FSMC_ECCPageSize_512Bytes;
	  FSMC_NANDInitStructure.FSMC_TCLRSetupTime = 0x00;
	  FSMC_NANDInitStructure.FSMC_TARSetupTime = 0x00;
	  FSMC_NANDInitStructure.FSMC_CommonSpaceTimingStruct = &p;
	  FSMC_NANDInitStructure.FSMC_AttributeSpaceTimingStruct = &p;

	  FSMC_NANDInit(&FSMC_NANDInitStructure);

	  /* FSMC NAND Bank Cmd Test */
	  FSMC_NANDCmd(FSMC_Bank2_NAND, ENABLE);
}

int DRV_Nand_IdRead( DRV_Nand_Id *pId )
{
	uint32_t data = 0;

	/* Send Command to the command area */
	*(__IO uint8_t *)(Bank_NAND_ADDR | CMD_AREA) = 0x90;
	*(__IO uint8_t *)(Bank_NAND_ADDR | ADDR_AREA) = 0x00;

	/* Sequence to read ID from NAND flash */
	data = *(__IO uint32_t *)(Bank_NAND_ADDR | DATA_AREA);

	pId->Maker_ID   = ADDR_1st_CYCLE (data);
	pId->Device_ID  = ADDR_2nd_CYCLE (data);
	pId->Third_ID   = ADDR_3rd_CYCLE (data);
	pId->Fourth_ID  = ADDR_4th_CYCLE (data);
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
	uint32_t uiSectorIndex,uiIndex,status;

	 /* Page write command and address */
	*(__IO uint8_t *)(Bank_NAND_ADDR | CMD_AREA) = NAND_CMD_AREA_A;

	*(__IO uint8_t *)(Bank_NAND_ADDR | ADDR_AREA) = 0x00; //Column address
	*(__IO uint8_t *)(Bank_NAND_ADDR | ADDR_AREA) = 0x00; //Column address
	*(__IO uint8_t *)(Bank_NAND_ADDR | ADDR_AREA) = ((uint8_t)uiSrcPageIndex ); //Row address
	*(__IO uint8_t *)(Bank_NAND_ADDR | ADDR_AREA) = ((uint8_t)uiSrcPageIndex )>>8; //Row address
	*(__IO uint8_t *)(Bank_NAND_ADDR | CMD_AREA) = NAND_CMD_READ_COPY_BACK ;
	/* Set destination page address*/
	*(__IO uint8_t *)(Bank_NAND_ADDR | CMD_AREA) = NAND_CMD_WRITE_RANDOM;
	*(__IO uint8_t *)(Bank_NAND_ADDR | ADDR_AREA) = 0x00; //Column address
	*(__IO uint8_t *)(Bank_NAND_ADDR | ADDR_AREA) = 0x00; //Column address
	*(__IO uint8_t *)(Bank_NAND_ADDR | ADDR_AREA) = ((uint8_t)uiDestPageIndex ); //Row address
	*(__IO uint8_t *)(Bank_NAND_ADDR | ADDR_AREA) = ((uint8_t)uiDestPageIndex )>>8; //Row address
	/* update spare area data if needed*/
	if( pSpareArea == NULL )
	{
		/* For the 4 sectors of the page */
		for(uiSectorIndex=0;uiSectorIndex<4;uiSectorIndex++)
		{
			*(__IO uint8_t *)(Bank_NAND_ADDR | CMD_AREA) = NAND_CMD_WRITE_RANDOM;
			*(__IO uint8_t *)(Bank_NAND_ADDR | ADDR_AREA) = uiSectorIndex<<4; //Column address
			*(__IO uint8_t *)(Bank_NAND_ADDR | ADDR_AREA) = 0x08; //Column address -> spare area

			/* write Spare area data */
			for( uiIndex=4 ; uiIndex < sizeof(DRV_Nand_SectorSpareData); uiIndex ++)
			{
			  *(__IO uint8_t *)(Bank_NAND_ADDR | DATA_AREA) = ((uint8_t *)pSpareArea)[uiIndex];
			}
		}
	}
	/* Send write data  command*/
	*(__IO uint8_t *)(Bank_NAND_ADDR | CMD_AREA) = NAND_CMD_WRITE_TRUE1;
	 /* Check status for successful operation */
	status = DRV_Nandi_GetStatus();

	if( status == NAND_READY )
		return 0;
	else
		return 1;
}

int DRV_Nand_BlockErase( uint32_t uiBlockIndex )
{
	uint32_t status ;

	*(__IO uint8_t *)(Bank_NAND_ADDR | CMD_AREA) = NAND_CMD_ERASE0;

	*(__IO uint8_t *)(Bank_NAND_ADDR | ADDR_AREA) = (uint8_t)uiBlockIndex&0xff;
	*(__IO uint8_t *)(Bank_NAND_ADDR | ADDR_AREA) = (uint8_t)((uiBlockIndex>>8)&0xff);
	*(__IO uint8_t *)(Bank_NAND_ADDR | ADDR_AREA) = (uint8_t)((uiBlockIndex>>16)&0xff);

	*(__IO uint8_t *)(Bank_NAND_ADDR | CMD_AREA) = NAND_CMD_ERASE1;

	/* Check status for successful operation */
	status = DRV_Nandi_GetStatus();

	if( status == NAND_READY )
		return 0;
	else
		return 1;
}

int DRV_Nand_SectorRead( uint8_t *pucDataBuffer , DRV_Nand_SectorSpareData *pSpareArea , uint32_t uiSectorIndex )
{
	uint32_t uiIndex , uiEcc ;

    /* Page write command and address */
    *(__IO uint8_t *)(Bank_NAND_ADDR | CMD_AREA) = NAND_CMD_AREA_A;

    *(__IO uint8_t *)(Bank_NAND_ADDR | ADDR_AREA) = 0x00; //Column address
    *(__IO uint8_t *)(Bank_NAND_ADDR | ADDR_AREA) = ((uint8_t)(uiSectorIndex & 0x03))<<1; //Column address
    *(__IO uint8_t *)(Bank_NAND_ADDR | ADDR_AREA) = ((uint8_t)uiSectorIndex )>>2; //Row address
    *(__IO uint8_t *)(Bank_NAND_ADDR | ADDR_AREA) = ((uint8_t)uiSectorIndex )>>10; //Row address

    *(__IO uint8_t *)(Bank_NAND_ADDR | CMD_AREA) = NAND_CMD_AREA_TRUE1;

    if( pucDataBuffer != NULL )
    {
		/*reset ECC */
		FSMC_NANDECCCmd( FSMC_Bank_NAND , DISABLE );
		FSMC_NANDECCCmd( FSMC_Bank_NAND , ENABLE );
		/* Get Data into Buffer */
		for(uiIndex=0; uiIndex < NAND_Sector_Size; uiIndex++)
		{
			pucDataBuffer[uiIndex]= *(__IO uint8_t *)(Bank_NAND_ADDR | DATA_AREA);
		}
    }
	/* Get Spare Area */
	*(__IO uint8_t *)(Bank_NAND_ADDR | CMD_AREA) = NAND_CMD_READ_RANDOM0;
	*(__IO uint8_t *)(Bank_NAND_ADDR | ADDR_AREA) = ((uint8_t)(uiSectorIndex & 0x03))<<4; //Column address
	*(__IO uint8_t *)(Bank_NAND_ADDR | ADDR_AREA) = 0x08; //Column address -> spare area
	*(__IO uint8_t *)(Bank_NAND_ADDR | CMD_AREA) = NAND_CMD_READ_RANDOM1;
	for(uiIndex=0; uiIndex < sizeof(DRV_Nand_SectorSpareData); uiIndex++)
	{
		((uint8_t *)pSpareArea)[uiIndex]= *(__IO uint8_t *)(Bank_NAND_ADDR | DATA_AREA);
	}
	if( pucDataBuffer != NULL )
	{
		/* Get Computed ECC */
		uiEcc = FSMC_GetECC(FSMC_Bank_NAND);
		/* compare computed and red ECC */
	   if( (((uint8_t)uiEcc)&0xff)       != pSpareArea->tucEcc[0] ||
		   ((((uint8_t)uiEcc)&0xff)>>8)  != pSpareArea->tucEcc[1] ||
		   ((((uint8_t)uiEcc)&0xff)>>16) != pSpareArea->tucEcc[2] )
			   return -1;
	}
   return 0;

}

int DRV_Nand_sectorWrite( uint8_t *pucDataBuffer , DRV_Nand_SectorSpareData *pSpareArea , uint32_t uiSectorIndex )
{
	uint32_t uiIndex , uiEcc , status;
	/* Page write command and address */
	*(__IO uint8_t *)(Bank_NAND_ADDR | CMD_AREA) = NAND_CMD_AREA_A;
	*(__IO uint8_t *)(Bank_NAND_ADDR | CMD_AREA) = NAND_CMD_WRITE0;

	*(__IO uint8_t *)(Bank_NAND_ADDR | ADDR_AREA) = 0x00;
	*(__IO uint8_t *)(Bank_NAND_ADDR | ADDR_AREA) = ((uint8_t)(uiSectorIndex & 0x03))<<1;
	*(__IO uint8_t *)(Bank_NAND_ADDR | ADDR_AREA) = ((uint8_t)uiSectorIndex )>>2;
	*(__IO uint8_t *)(Bank_NAND_ADDR | ADDR_AREA) = ((uint8_t)uiSectorIndex )>>10;

	if( pucDataBuffer != NULL )
	{
		/*reset ECC */
		FSMC_NANDECCCmd( FSMC_Bank_NAND , DISABLE );
		FSMC_NANDECCCmd( FSMC_Bank_NAND , ENABLE );

		 /* Write data */
		for( uiIndex=0 ; uiIndex < NAND_Sector_Size; uiIndex ++)
		{
		  *(__IO uint8_t *)(Bank_NAND_ADDR | DATA_AREA) = pucDataBuffer[uiIndex];
		}
		/* Get Computed ECC */
		uiEcc = FSMC_GetECC(FSMC_Bank_NAND);
		pSpareArea->tucEcc[0] = (((uint8_t)uiEcc)&0xff);
		pSpareArea->tucEcc[1] = ((((uint8_t)uiEcc)&0xff)>>8);
		pSpareArea->tucEcc[2] = ((((uint8_t)uiEcc)&0xff)>>16);
	}

	/* Set Spare Area Address*/
	*(__IO uint8_t *)(Bank_NAND_ADDR | CMD_AREA) = NAND_CMD_WRITE_RANDOM;
	*(__IO uint8_t *)(Bank_NAND_ADDR | ADDR_AREA) = ((uint8_t)(uiSectorIndex & 0x03))<<4; //Column address
	*(__IO uint8_t *)(Bank_NAND_ADDR | ADDR_AREA) = 0x08; //Column address-> spare area
	/* write Spare area*/
	for( uiIndex=0 ; uiIndex < sizeof(DRV_Nand_SectorSpareData); uiIndex ++)
	{
	  *(__IO uint8_t *)(Bank_NAND_ADDR | DATA_AREA) = ((uint8_t *)pSpareArea)[uiIndex];
	}
	*(__IO uint8_t *)(Bank_NAND_ADDR | CMD_AREA) = NAND_CMD_WRITE_TRUE1;

	 /* Check status for successful operation */
	status = DRV_Nandi_GetStatus();

	if( status == NAND_READY )
		return 0;
	else
		return 1;
}



/******************************************************************************
* Function Name  : FSMC_NAND_GetStatus
* Description    : Get the NAND operation status
* Input          : None
* Output         : None
* Return         : New status of the NAND operation. This parameter can be:
*                   - NAND_TIMEOUT_ERROR: when the previous operation generate
*                     a Timeout error
*                   - NAND_READY: when memory is ready for the next operation
*******************************************************************************/
static uint32_t DRV_Nandi_GetStatus(void)
{
  uint32_t timeout = 0x1000000, status = NAND_READY;

  status = DRV_Nandi_ReadStatus();

  /* Wait for a NAND operation to complete or a TIMEOUT to occur */
  while ((status != NAND_READY) &&( timeout != 0x00))
  {
     status = DRV_Nandi_ReadStatus();
     timeout --;
  }

  if(timeout == 0x00)
  {
    status =  NAND_TIMEOUT_ERROR;
  }

  /* Return the operation status */
  return (status);
}

/******************************************************************************
* Function Name  : FSMC_NAND_ReadStatus
* Description    : Reads the NAND memory status using the Read status command
* Input          : None
* Output         : None
* Return         : The status of the NAND memory. This parameter can be:
*                   - NAND_BUSY: when memory is busy
*                   - NAND_READY: when memory is ready for the next operation
*                   - NAND_ERROR: when the previous operation gererates error
*******************************************************************************/
static uint32_t DRV_Nandi_ReadStatus(void)
{
  uint32_t data = 0x00, status = NAND_BUSY;

  /* Read status operation ------------------------------------ */
  *(__IO uint8_t *)(Bank_NAND_ADDR | CMD_AREA) = NAND_CMD_STATUS;
  data = *(__IO uint8_t *)(Bank_NAND_ADDR);

  if((data & NAND_ERROR) == NAND_ERROR)
  {
    status = NAND_ERROR;
  }
  else if((data & NAND_READY) == NAND_READY)
  {
    status = NAND_READY;
  }
  else
  {
    status = NAND_BUSY;
  }

  return (status);
}
