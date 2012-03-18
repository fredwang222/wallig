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
					Defines
***************************************************************/
#define NAND_OK         0
#define NAND_ERR_ACCESS 1
#define NAND_ERR_BAD_BL 2
/**************************************************************
						Typedef
***************************************************************/
typedef struct
{
	uint8_t Maker_ID;
	uint8_t Device_ID;
	uint8_t Third_ID;
	uint8_t Fourth_ID;
}DRV_Nand_Id;

typedef struct
{
	uint32_t uiPageSize;    /* Page size in byte */
	uint32_t uiPagePerBlock; /* Number of page per block */
	uint32_t uiBlockCount;   /* total count of block */
	uint32_t uiOobSize;  /* out of band area size in byte*/
}DRV_Nand_Size_Info;


/**************************************************************
					Functions
***************************************************************/
void DRV_Nand_Init( void );
void DRV_Nand_Terminate( void );
int DRV_Nand_InfoRead( DRV_Nand_Id *pId , DRV_Nand_Size_Info *SizeInfo);
int DRV_Nand_PageRead( uint8_t *pucDataBuffer , uint8_t *pucSpareAreaBuffer , uint32_t uiPageIndex );
int DRV_Nand_PageWrite( uint8_t *pucDataBuffer , uint8_t *pucSpareAreaBuffer , uint32_t uiPageIndex );
int DRV_Nand_PageCopy( uint32_t uiSrcIndex , uint32_t uiDestIndex );
int DRV_Nand_BlockErase( uint32_t uiBlockIndex );
void  DRV_Nand_BadBlockSet( uint32_t uiBlockIndex );

