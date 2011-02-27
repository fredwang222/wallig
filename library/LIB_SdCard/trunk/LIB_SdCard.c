/*-----------------------------------------------------------------------*/
/* MMC/SDSC/SDHC (in SPI mode) control module for STM32 Version 1.1.6    */
/* (C) Martin Thomas, 2010 - based on the AVR MMC module (C)ChaN, 2007   */
/*-----------------------------------------------------------------------*/

/* Copyright (c) 2010, Martin Thomas, ChaN
   All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:

   * Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   * Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.
   * Neither the name of the copyright holders nor the names of
     contributors may be used to endorse or promote products derived
     from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE. */

#include <stdint.h>
#include <string.h>
#include "DRV_Gpio.h"
#include "DRV_Spi.h"
#include "DRV_Timer.h"
#include "LIB_SdCard.h"
#include "inc/LIB_SdCard_p.h"
#include "LIB_SdCard_CFG.h"

/* Define */
#define SDCARD_FLAG_CARD_IN 0x0001
#define SDCARD_FLAG_WP      0x0002
#define SDCARD_FLAG_NOINIT  0x0004
/* Macros */
#define SDCARD_UNSELECT()  DRV_Gpio_ValueSet( SDCARD_Data.hCS , 1)
#define SDCARD_SELECT()    DRV_Gpio_ValueSet( SDCARD_Data.hCS , 0)
#define SDCARD_CARD_IN()   DRV_Gpio_ValueGet( SDCARD_Data.hCD )
#define SDCARD_CARD_WP()   DRV_Gpio_ValueGet( SDCARD_Data.hWP )

typedef struct
{
	char *SPIDeviceName;
	char *CSPioName;		//!< Chip select
	char *WPPioName;		//!< Write protection
	char *CDPioName;		//!< Card Detect
} SDCARD_Cfg_t;

typedef struct
{
	DRV_Spi_Handle hSpi;
	DRV_Gpio_Handle hCS;
	DRV_Gpio_Handle hWP;
	DRV_Gpio_Handle hCD;
	uint16_t u32CardState;
	uint8_t ucCardType;

} SCARD_Data_t;

SDCARD_Cfg_t SDCARD_Cfg = SDCARD_CFG_INIT;
SCARD_Data_t SDCARD_Data;

static uint8_t LIB_SdCard_Wait_Ready ( void );
static void LIB_SdCard_Release_spi (void);
static LIB_SdCard_Error LIB_SdCard_Write_datablock ( const uint8_t *pucBuff,	uint8_t ucToken	);
static LIB_SdCard_Error LIB_SdCard_ReceiveBlock( uint8_t *pBuffer , uint16_t u16Len);
static uint8_t LIB_SdCard_Send_Cmd (uint8_t ucCmd , uint32_t arg );

LIB_SdCard_Error LIB_SdCard_Init( void )
{

	SDCARD_Data.u32CardState=SDCARD_FLAG_NOINIT;
	/* open Spi device*/
	DRV_Spi_Init();
	if( DRV_Spi_Open( SDCARD_Cfg.SPIDeviceName , &SDCARD_Data.hSpi ) != Spi_No_Error )
		return SdCard_Init_Error;
	/* open Chip Select pio*/
	if ( DRV_Gpio_Open( SDCARD_Cfg.CSPioName ,  &SDCARD_Data.hCS , NULL ) != GPIO_No_Error )
		return SdCard_Init_Error;
	/* open Chip Select pio*/
	if ( DRV_Gpio_Open( SDCARD_Cfg.WPPioName ,  &SDCARD_Data.hWP , NULL ) != GPIO_No_Error )
		return SdCard_Init_Error;
	/* open Chip Select pio*/
	if ( DRV_Gpio_Open( SDCARD_Cfg.CDPioName ,  &SDCARD_Data.hCD , NULL ) != GPIO_No_Error )
		return SdCard_Init_Error;

	SDCARD_UNSELECT();
	/* Look for Card */
	if( SDCARD_CARD_IN() );
	{
		SDCARD_Data.u32CardState |= SDCARD_FLAG_CARD_IN;
		if( SDCARD_CARD_WP() )
			SDCARD_Data.u32CardState |= SDCARD_FLAG_WP;
	}

	return SdCard_No_Error;
}

/* Receive a block of data from the sdcard*/
static LIB_SdCard_Error LIB_SdCard_ReceiveBlock( uint8_t *pBuffer , uint16_t u16Len)
{
	DRV_Time_TimeOut TimeOutWait;
	uint8_t ucToken;


	mDRV_TIME_TIMEOUT_INIT(TimeOutWait,100);
	do
	{							/* Wait for data packet in timeout of 100ms */
		ucToken = DRV_Spi_RW_Byte(SDCARD_Data.hSpi,0xff);
	} while ((ucToken == 0xFF) && !mDRV_TIME_TIMEOUT_END(TimeOutWait) );
	/* If not valid data token, return with error */
	if(ucToken != 0xFE)
		return SdCard_TimeOut;

	DRV_Spi_Read_Buffer(SDCARD_Data.hSpi , pBuffer, u16Len );

	DRV_Spi_RW_Byte(SDCARD_Data.hSpi,0xff);						/* Discard CRC */
	DRV_Spi_RW_Byte(SDCARD_Data.hSpi,0xff);

	return SdCard_No_Error;										/* Return with success */
}

/*-----------------------------------------------------------------------*/
/* Send a data packet to MMC                                             */
/* const uint8_t *pucBuff,	512 byte data block to be transmitted */
/* uint8_t token			Data/Stop token */
/*-----------------------------------------------------------------------*/

static LIB_SdCard_Error LIB_SdCard_Write_datablock ( const uint8_t *pucBuff,	uint8_t ucToken	)
{
	uint8_t ucResp;

	if (LIB_SdCard_Wait_Ready() != 0xFF) return SdCard_Failed;

	DRV_Spi_RW_Byte(SDCARD_Data.hSpi, ucToken);					/* Xmit data token */
	if (ucToken != 0xFD)
	{	/* Is data token */

		DRV_Spi_Write_Buffer( SDCARD_Data.hSpi , (uint8_t*)pucBuff, 512 );

		DRV_Spi_RW_Byte(SDCARD_Data.hSpi,0xff);					/* CRC (Dummy) */
		DRV_Spi_RW_Byte(SDCARD_Data.hSpi,0xff);
		ucResp =DRV_Spi_RW_Byte(SDCARD_Data.hSpi,0xff);			/* Receive data response */
		if ((ucResp & 0x1F) != 0x05)							/* If not accepted, return with error */
			return SdCard_Failed;
	}

	return SdCard_No_Error;
}

static uint8_t LIB_SdCard_Wait_Ready ( void )
{
	uint8_t u8Res;
	DRV_Time_TimeOut TimeOutWait;

	/* Wait for ready in timeout of 500ms */
	mDRV_TIME_TIMEOUT_INIT(TimeOutWait,500);
	DRV_Spi_RW_Byte(SDCARD_Data.hSpi,0xff);
	do
		u8Res = DRV_Spi_RW_Byte(SDCARD_Data.hSpi,0xff);
	while ((u8Res != 0xFF) && !mDRV_TIME_TIMEOUT_END(TimeOutWait) );

	return u8Res;
}

/*-----------------------------------------------------------------------*/
/* Deselect the card and release SPI bus                                 */
/*-----------------------------------------------------------------------*/
static void LIB_SdCard_Release_spi (void)
{
	SDCARD_UNSELECT();
	DRV_Spi_RW_Byte(SDCARD_Data.hSpi,0xff);
}


/*-----------------------------------------------------------------------*/
/* Send a command packet to MMC                                          */
/* uint8_t cmd			Command byte */
/* uint32_t arg			Argument */
/*-----------------------------------------------------------------------*/

static uint8_t LIB_SdCard_Send_Cmd (uint8_t ucCmd , uint32_t arg )
{
	uint8_t ucN, ucRes;


	if (ucCmd & 0x80)
	{	/* ACMD<ucN> is the command sequence of CMD55-CMD<ucN> */
		ucCmd &= 0x7F;
		ucRes = LIB_SdCard_Send_Cmd(CMD55, 0);
		if (ucRes > 1)
			return ucRes;
	}

	/* Select the card and wait for ready */
	SDCARD_UNSELECT();
	SDCARD_SELECT();
	if (LIB_SdCard_Wait_Ready() != 0xFF)
	{
		return 0xFF;
	}

	/* Send command packet */
	DRV_Spi_RW_Byte( SDCARD_Data.hSpi , ucCmd);						/* Start + Command index */
	DRV_Spi_RW_Byte( SDCARD_Data.hSpi , (uint8_t)(arg >> 24));		/* Argument[31..24] */
	DRV_Spi_RW_Byte( SDCARD_Data.hSpi , (uint8_t)(arg >> 16));		/* Argument[23..16] */
	DRV_Spi_RW_Byte( SDCARD_Data.hSpi , (uint8_t)(arg >> 8));		/* Argument[15..8] */
	DRV_Spi_RW_Byte( SDCARD_Data.hSpi , (uint8_t)arg);				/* Argument[7..0] */
	ucN = 0x01;							/* Dummy CRC + Stop */
	if (ucCmd == CMD0)
		ucN = 0x95;			/* Valid CRC for CMD0(0) */
	if (ucCmd == CMD8)
		ucN = 0x87;			/* Valid CRC for CMD8(0x1AA) */
	DRV_Spi_RW_Byte( SDCARD_Data.hSpi , ucN);

	/* Receive command response */
	if (ucCmd == CMD12)
		DRV_Spi_RW_Byte( SDCARD_Data.hSpi , 0xff );		/* Skip a stuff byte when stop reading */

	ucN = 10;								/* Wait for a valid response in timeout of 10 attempts */
	do
		ucRes = DRV_Spi_RW_Byte( SDCARD_Data.hSpi , 0xff );
	while ((ucRes & 0x80) && --ucN);

	return ucRes;			/* Return with the response value */
}

/*-----------------------------------------------------------------------*/
/* Initialize Disk Drive                                                 */
/* uint8_t ucDrv		Physical drive number (0) */
/*-----------------------------------------------------------------------*/

LIB_SdCard_Error LIB_SdCard_Disk_initialize (	uint8_t ucDrv )
{
	LIB_SdCard_Error eError;
	uint8_t ucN, ucCmd, ucTy, tucOcr[4];
	DRV_Time_TimeOut TimeOutWait;

	if (ucDrv)
		return SdCard_Failed;			/* Supports only single drive */
	if (!(SDCARD_Data.u32CardState & SDCARD_FLAG_CARD_IN))
		return SdCard_No_Card;	/* No card in the socket */

	//TODO
	//power_on();							/* Force socket power on and initialize interface */
	DRV_Spi_Speed_Set( SDCARD_Data.hSpi , DRV_SPI_SLOW);
	for (ucN = 10; ucN; ucN--)
		DRV_Spi_RW_Byte( SDCARD_Data.hSpi , 0xff );	/* 80 dummy clocks */

	ucTy = 0;
	if (LIB_SdCard_Send_Cmd(CMD0, 0) == 1)
	{			/* Enter Idle state */
		mDRV_TIME_TIMEOUT_INIT(TimeOutWait , 1000);						/* Initialization timeout of 1000 milliseconds */
		if (LIB_SdCard_Send_Cmd(CMD8, 0x1AA) == 1)
		{	/* SDHC */
			for (ucN = 0; ucN < 4; ucN++)
				tucOcr[ucN] = DRV_Spi_RW_Byte( SDCARD_Data.hSpi , 0xff );		/* Get trailing return value of R7 response */
			if (tucOcr[2] == 0x01 && tucOcr[3] == 0xAA)
			{				/* The card can work at VDD range of 2.7-3.6V */
				while ( !mDRV_TIME_TIMEOUT_END(TimeOutWait) && LIB_SdCard_Send_Cmd(ACMD41, 1UL << 30))
					;	/* Wait for leaving idle state (ACMD41 with HCS bit) */

				if ( !mDRV_TIME_TIMEOUT_END(TimeOutWait)  && LIB_SdCard_Send_Cmd(CMD58, 0) == 0)
				{		/* Check CCS bit in the OCR */
					for (ucN = 0; ucN < 4; ucN++) tucOcr[ucN] = DRV_Spi_RW_Byte( SDCARD_Data.hSpi , 0xff );
						ucTy = (tucOcr[0] & 0x40) ? CT_SD2 | CT_BLOCK : CT_SD2;
				}
			}
		}
		else
		{							/* SDSC or MMC */
			if (LIB_SdCard_Send_Cmd(ACMD41, 0) <= 1)
			{
				ucTy = CT_SD1; ucCmd = ACMD41;	/* SDSC */
			}
			else
			{
				ucTy = CT_MMC; ucCmd = CMD1;	/* MMC */
			}
			while ( !mDRV_TIME_TIMEOUT_END(TimeOutWait) && LIB_SdCard_Send_Cmd(ucCmd, 0))
				;			/* Wait for leaving idle state */
			if (mDRV_TIME_TIMEOUT_END(TimeOutWait) || LIB_SdCard_Send_Cmd(CMD16, 512) != 0)	/* Set R/W block length to 512 */
				ucTy = 0;
		}
	}
	SDCARD_Data.ucCardType = ucTy;
	LIB_SdCard_Release_spi();

	if (ucTy)
	{			/* Initialization succeeded */
		SDCARD_Data.u32CardState &=~ SDCARD_FLAG_NOINIT;
		eError = SdCard_No_Error;
		DRV_Spi_Speed_Set( SDCARD_Data.hSpi , DRV_SPI_SLOW);
	}
	else
	{			/* Initialization failed */
		eError = SdCard_Failed;
		//power_off();
	}

	return eError;
}

/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/* uint8_t ucDrv,			Physical drive number (0) */
/*	uint8_t *pucBuff,		Pointer to the data buffer to store read data */
/*	uint32_t uiSector,		Start uiSector number (LBA) */
/*	uint8_t ucCount			Sector ucCount (1..255) */
/*-----------------------------------------------------------------------*/
LIB_SdCard_Error LIB_SdCard_Disk_read (uint8_t ucDrv, uint8_t *pucBuff, uint32_t uiSector,uint8_t ucCount)
{
	if (ucDrv || !ucCount)
		return SdCard_Bad_Param;
	if (SDCARD_Data.u32CardState & SDCARD_FLAG_NOINIT)
		return SdCard_Not_Ready;

	if (!(SDCARD_Data.ucCardType & CT_BLOCK))
		uiSector *= 512;	/* Convert to byte address if needed */

	if (ucCount == 1)
	{	/* Single block read */
		if (LIB_SdCard_Send_Cmd(CMD17, uiSector) == 0)
		{ /* READ_SINGLE_BLOCK */
			if (LIB_SdCard_ReceiveBlock(pucBuff, 512)==SdCard_No_Error)
			{
				ucCount = 0;
			}
		}
	}
	else
	{	/* Multiple block read */
		if (LIB_SdCard_Send_Cmd(CMD18, uiSector) == 0)
		{	/* READ_MULTIPLE_BLOCK */
			do {
				if (LIB_SdCard_ReceiveBlock(pucBuff, 512)!=SdCard_No_Error)
				{
					break;
				}
				pucBuff += 512;
			} while (--ucCount);
			LIB_SdCard_Send_Cmd(CMD12, 0);				/* STOP_TRANSMISSION */
		}
	}
	LIB_SdCard_Release_spi();

	return ucCount ? SdCard_Failed : SdCard_No_Error;
}

/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/* uint8_t ucDrv,			 Physical drive number (0) */
/*	uint8_t *pucBuff,		Pointer to the data to be written */
/*	uint32_t uiSector,		Start uiSector number (LBA) */
/*	uint8_t ucCount			Sector count (1..255) */
/*-----------------------------------------------------------------------*/
LIB_SdCard_Error LIB_SdCard_Disk_write (uint8_t ucDrv, uint8_t *pucBuff, uint32_t uiSector,	 uint8_t ucCount )
{
	if (ucDrv || !ucCount)
		return SdCard_Bad_Param;
	if (SDCARD_Data.u32CardState & SDCARD_FLAG_NOINIT)
		return SdCard_Not_Ready;
	if (SDCARD_Data.u32CardState & SDCARD_FLAG_WP)
		return SdCard_WriteProtected;

	if (!(SDCARD_Data.ucCardType & CT_BLOCK))
		uiSector *= 512;	/* Convert to byte address if needed */

	if (ucCount == 1)
	{	/* Single block write */
		if ((LIB_SdCard_Send_Cmd(CMD24, uiSector) == 0)	/* WRITE_BLOCK */
			&& LIB_SdCard_Write_datablock(pucBuff, 0xFE))
			ucCount = 0;
	}
	else
	{				/* Multiple block write */
		if (SDCARD_Data.ucCardType & CT_SDC)
			LIB_SdCard_Send_Cmd(ACMD23, ucCount);
		if (LIB_SdCard_Send_Cmd(CMD25, uiSector) == 0)
		{	/* WRITE_MULTIPLE_BLOCK */
			do {
				if (LIB_SdCard_Write_datablock(pucBuff, 0xFC)!=SdCard_No_Error)
					break;
				pucBuff += 512;
			} while (--ucCount);

			if (LIB_SdCard_Write_datablock(0, 0xFD)!=SdCard_No_Error)	/* STOP_TRAN token */
				ucCount = 1;
		}
	}
	LIB_SdCard_Release_spi();

	return ucCount ? SdCard_Failed : SdCard_No_Error;
}

/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/* uint8_t ucDrv,		Physical drive number (0) */
/*	uint8_t ctrl,		Control code */
/*	void *pBuff		 	Buffer to send/receive control data */
/*-----------------------------------------------------------------------*/
LIB_SdCard_Error LIB_SdCard_Disk_ioctl ( uint8_t ucDrv,	 uint8_t ctrl, void *pBuff )
{
	LIB_SdCard_Error eError;
	uint8_t ucN, tucCsd[16], *pPtr = pBuff;
	uint16_t usSize;

	if (ucDrv)
		return SdCard_Bad_Param;

	eError = SdCard_Failed;

	if (ctrl == CTRL_POWER)
	{
		switch (*pPtr)
		{
			case 0:		/* Sub control code == 0 (POWER_OFF) */
				//TODO
				//if (chk_power())
				//	power_off();		/* Power off */
				eError = SdCard_No_Error;
				break;
			case 1:		/* Sub control code == 1 (POWER_ON) */
				//TODO
				//power_on();				/* Power on */
				eError = SdCard_No_Error;
				break;
			case 2:		/* Sub control code == 2 (POWER_GET) */
				//TODO
				//*(pPtr+1) = (BYTE)chk_power();
				*(pPtr+1) = 1;
				eError = SdCard_No_Error;
				break;
			default :
				eError = SdCard_Bad_Param;
		}
	}
	else
	{
		if (SDCARD_Data.u32CardState & SDCARD_FLAG_NOINIT)
			return SdCard_Not_Ready;

		switch (ctrl)
		{
			case CTRL_SYNC :		/* Make sure that no pending write process */
				SDCARD_SELECT();
				if (LIB_SdCard_Wait_Ready() == 0xFF)
					eError = SdCard_No_Error;
				break;

			case GET_SECTOR_COUNT :	/* Get number of sectors on the disk (uint32_t) */
				if ((LIB_SdCard_Send_Cmd(CMD9, 0) == 0) && ( LIB_SdCard_ReceiveBlock(tucCsd, 16)==SdCard_No_Error ))
				{
					if ((tucCsd[0] >> 6) == 1)
					{	/* SDC version 2.00 */
						usSize = tucCsd[9] + ((uint16_t)tucCsd[8] << 8) + 1;
						*(uint32_t*)pBuff= (uint32_t)usSize << 10;
					}
					else
					{					/* SDC version 1.XX or MMC*/
						ucN = (tucCsd[5] & 15) + ((tucCsd[10] & 128) >> 7) + ((tucCsd[9] & 3) << 1) + 2;
						usSize = (tucCsd[8] >> 6) + ((uint16_t)tucCsd[7] << 2) + ((uint16_t)(tucCsd[6] & 3) << 10) + 1;
						*(uint32_t*)pBuff= (uint32_t)usSize << (ucN - 9);
					}
					eError = SdCard_No_Error;
				}
				break;

			case GET_SECTOR_SIZE :	/* Get R/W sector size (uint16_t) */
				*(uint16_t*)pBuff= 512;
				eError = SdCard_No_Error;
				break;

			case GET_BLOCK_SIZE :	/* Get erase block size in unit of sector (uint32_t) */
				if (SDCARD_Data.ucCardType & CT_SD2)
				{	/* SDC ver 2.00 */
					if (LIB_SdCard_Send_Cmd(ACMD13, 0) == 0) {	/* Read SD status */
						DRV_Spi_RW_Byte( SDCARD_Data.hSpi , 0xff );
						if (LIB_SdCard_ReceiveBlock(tucCsd, 16)==SdCard_No_Error )
						{				/* Read partial block */
							for (ucN = 64 - 16; ucN; ucN--)
								DRV_Spi_RW_Byte( SDCARD_Data.hSpi , 0xff );	/* Purge trailing data */
							*(uint32_t*)pBuff= 16UL << (tucCsd[10] >> 4);
							eError = SdCard_No_Error;
						}
					}
				}
				else
				{					/* SDC ver 1.XX or MMC */
					if ((LIB_SdCard_Send_Cmd(CMD9, 0) == 0) && (LIB_SdCard_ReceiveBlock(tucCsd, 16)==SdCard_No_Error ))
					{	/* Read CSD */
						if (SDCARD_Data.ucCardType & CT_SD1)
						{	/* SDC ver 1.XX */
							*(uint32_t*)pBuff= (((tucCsd[10] & 63) << 1) + ((uint16_t)(tucCsd[11] & 128) >> 7) + 1) << ((tucCsd[13] >> 6) - 1);
						}
						else
						{					/* MMC */
							*(uint32_t*)pBuff= ((uint16_t)((tucCsd[10] & 124) >> 2) + 1) * (((tucCsd[11] & 3) << 3) + ((tucCsd[11] & 224) >> 5) + 1);
						}
						eError = SdCard_No_Error;
					}
				}
				break;

			case MMC_GET_TYPE :	/* Get card type flags (1 byte) */
				*pPtr = SDCARD_Data.ucCardType;
				eError = SdCard_No_Error;
				break;

			case MMC_GET_CSD :		/* Receive CSD as a data block (16 bytes) */
				if (LIB_SdCard_Send_Cmd(CMD9, 0) == 0		/* READ_CSD */
					&& (LIB_SdCard_ReceiveBlock(pPtr, 16) ==SdCard_No_Error ))
					eError = SdCard_No_Error;
				break;

			case MMC_GET_CID :		/* Receive CID as a data block (16 bytes) */
				if (LIB_SdCard_Send_Cmd(CMD10, 0) == 0		/* READ_CID */
					&& (LIB_SdCard_ReceiveBlock(pPtr, 16)==SdCard_No_Error ))
					eError = SdCard_No_Error;
				break;

			case MMC_GET_OCR :		/* Receive OCR as an R3 resp (4 bytes) */
				if (LIB_SdCard_Send_Cmd(CMD58, 0) == 0)
				{	/* READ_OCR */
					for (ucN = 4; ucN; ucN--)
						*pPtr++ = DRV_Spi_RW_Byte( SDCARD_Data.hSpi , 0xff );
					eError = SdCard_No_Error;
				}
				break;

			case MMC_GET_SDSTAT :	/* Receive SD status as a data block (64 bytes) */
				if (LIB_SdCard_Send_Cmd(ACMD13, 0) == 0)
				{	/* SD_STATUS */
					DRV_Spi_RW_Byte( SDCARD_Data.hSpi , 0xff );
					if (LIB_SdCard_ReceiveBlock(pPtr, 64)==SdCard_No_Error )
						eError = SdCard_No_Error;
				}
				break;

			default:
				eError = SdCard_Bad_Param;
		}

		LIB_SdCard_Release_spi();
	}

	return eError;
}
