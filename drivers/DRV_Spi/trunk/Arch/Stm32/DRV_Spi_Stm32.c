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
#include <string.h>
#include "stm32f10x.h"
#include "DRV_Spi.h"
#include "DRV_Spi_Cfg.h"


#define SPI_DEVICE_COUNT ( sizeof(tSPI_DeviceCfgList)/sizeof(Spi_Device_Cfg))

typedef struct
{
	char *pcName;
	SPI_TypeDef *pDevice;
	uint16_t speed[2];
	struct
	{
		void (*Cmd)(uint32_t , FunctionalState );
		uint32_t Clock;
	} PeriphClock;
	struct
	{
		GPIO_TypeDef* PORT;
		u16 SCK_Pin;
		u16 MISO_Pin;
		u16 MOSI_Pin;
	} PIO;
	struct
	{
		struct
		{
			DMA_Channel_TypeDef *Channel;
			uint32_t FlagTc;
		} RX;
		struct
		{
			DMA_Channel_TypeDef *Channel;
			uint32_t FlagTc;
		} TX;
	} DMA;
} Spi_Device_Cfg;

typedef struct
{
	Spi_Device_Cfg *pCfg;
	uint16_t *SpeedCfg;
	DRV_Spi_Device_State eState;
} Spi_Device_Data;

const Spi_Device_Cfg tSPI_DeviceCfgList[]=SPI_INIT_CFG;
Spi_Device_Data tSPI_DeviceDataList[SPI_DEVICE_COUNT];

void stm32_dma_transfer(	Spi_Device_Data *pSpiData , char receive, const uint8_t *buff, uint16_t btr  );

void DRV_Spi_Init(void )
{
	int iDevicecount;

	for( iDevicecount = 0 ; iDevicecount < SPI_DEVICE_COUNT ; iDevicecount++)
	{
		tSPI_DeviceDataList[iDevicecount].pCfg = (Spi_Device_Cfg*)&tSPI_DeviceCfgList[iDevicecount];
		tSPI_DeviceDataList[iDevicecount].eState = Spi_Device_Close;
	}
}

DRV_Spi_Error DRV_Spi_Open( char *pcName , DRV_Spi_Handle *pHandle)
{
	DRV_Spi_Error eError = Spi_Device_Not_Found;
	int iSpiIndex;
	Spi_Device_Data *pSpiData;
	SPI_InitTypeDef  SPI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	volatile unsigned char ucDummyread;

	for ( iSpiIndex = 0 ; iSpiIndex < SPI_DEVICE_COUNT ; iSpiIndex++ )
	{
		if( !strcmp( pcName , tSPI_DeviceDataList[iSpiIndex].pCfg->pcName) )
		{
			pSpiData = &tSPI_DeviceDataList[iSpiIndex];
			eError=Spi_No_Error;
			break;
		}
	}
	if( eError==Spi_Device_Not_Found )
		return eError;
	if( pSpiData->eState == Spi_Device_Open)
		return Spi_AlreadyOpened;
	*pHandle =(DRV_Spi_Handle) pSpiData;
	/* Enable SPI clock */
	pSpiData->pCfg->PeriphClock.Cmd(pSpiData->pCfg->PeriphClock.Clock,ENABLE);
	/* Configure SPI pins: SCK and MOSI with default alternate function (not remapped) push-pull */
	GPIO_InitStructure.GPIO_Pin   = pSpiData->pCfg->PIO.SCK_Pin | pSpiData->pCfg->PIO.MOSI_Pin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
	GPIO_Init(pSpiData->pCfg->PIO.PORT, &GPIO_InitStructure);
	/* Configure MISO as Input with internal pull-up */
	GPIO_InitStructure.GPIO_Pin   = pSpiData->pCfg->PIO.MISO_Pin;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;
	GPIO_Init(pSpiData->pCfg->PIO.PORT, &GPIO_InitStructure);
	/* SPI configuration */
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler =  pSpiData->pCfg->speed[DRV_SPI_SLOW]; // 72000kHz/256=281kHz < 400kHz
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;

	SPI_Init(pSpiData->pCfg->pDevice, &SPI_InitStructure);
	SPI_CalculateCRC(pSpiData->pCfg->pDevice, DISABLE);
	SPI_Cmd(pSpiData->pCfg->pDevice, ENABLE);

	/* drain SPI */
	while (SPI_I2S_GetFlagStatus(pSpiData->pCfg->pDevice, SPI_I2S_FLAG_TXE) == RESET) { ; }
	ucDummyread = SPI_I2S_ReceiveData(pSpiData->pCfg->pDevice);

	return eError;
}

void DRV_Spi_Close( DRV_Spi_Handle Handle)
{

}

DRV_Spi_Error DRV_Spi_Speed_Set( DRV_Spi_Handle Handle , uint16_t u16Speed)
{
	Spi_Device_Data *pSpiData = (Spi_Device_Data *) Handle ;
	uint32_t tmp;

	tmp = pSpiData->pCfg->pDevice->CR1;
	if ( u16Speed == DRV_SPI_SLOW ) {
		/* Set slow clock (100k-400k) */
		tmp = ( tmp | pSpiData->pCfg->speed[DRV_SPI_SLOW] );
	} else {
		/* Set fast clock (depends on tRxhe CSD) */
		tmp = ( tmp & ~pSpiData->pCfg->speed[DRV_SPI_SLOW]) | pSpiData->pCfg->speed[DRV_SPI_FAST];
	}
	pSpiData->pCfg->pDevice->CR1= tmp;

	return Spi_No_Error;
}

unsigned short DRV_Spi_RW_Byte( DRV_Spi_Handle Handle , unsigned char ucOut)
{
	Spi_Device_Data *pSpiData = (Spi_Device_Data *) Handle ;
	/* Send byte through the SPI peripheral */
	SPI_I2S_SendData(pSpiData->pCfg->pDevice, ucOut);

	/* Wait to receive a byte */
	while (SPI_I2S_GetFlagStatus(pSpiData->pCfg->pDevice, SPI_I2S_FLAG_RXNE) == RESET) { ; }

	/* Return the byte read from the SPI bus */
	return SPI_I2S_ReceiveData(pSpiData->pCfg->pDevice);
}

unsigned short DRV_Spi_Read_Buffer( DRV_Spi_Handle Handle , unsigned char *pcBuffer , unsigned int uiLen)
{
	Spi_Device_Data *pSpiData = (Spi_Device_Data *) Handle ;
	stm32_dma_transfer(pSpiData , TRUE , pcBuffer , uiLen);
	return 0;
}

unsigned short DRV_Spi_Write_Buffer( DRV_Spi_Handle Handle , unsigned char *pcBuffer , unsigned int uiLen)
{
	Spi_Device_Data *pSpiData = (Spi_Device_Data *) Handle ;
	stm32_dma_transfer(pSpiData , FALSE , pcBuffer , uiLen);
	return 0;
}

/* BOOL receive,		FALSE for buff->SPI, TRUE for SPI->buff               */
/*	const BYTE *buff,	receive TRUE  : 512 byte data block to be transmitted */
/*						receive FALSE : Data buffer to store received data    */
/*	UINT btr 			receive TRUE  : Byte count (must be multiple of 2)    */
/*						receive FALSE : Byte count (must be 512)              */
void stm32_dma_transfer(	Spi_Device_Data *pSpiData , char receive, const uint8_t *buff, uint16_t btr  )
{
	DMA_InitTypeDef DMA_InitStructure;
	uint16_t rw_workbyte[] = { 0xffff };

	/* shared DMA configuration values */
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&(pSpiData->pCfg->pDevice->DR));
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_BufferSize = btr;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;

	DMA_DeInit(pSpiData->pCfg->DMA.RX.Channel );
	DMA_DeInit(pSpiData->pCfg->DMA.TX.Channel );

	if ( receive ) {

		/* DMA1 channel2 configuration SPI1 RX ---------------------------------------------*/
		/* DMA1 channel4 configuration SPI2 RX ---------------------------------------------*/
		DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)buff;
		DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
		DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
		DMA_Init(pSpiData->pCfg->DMA.RX.Channel, &DMA_InitStructure);

		/* DMA1 channel3 configuration SPI1 TX ---------------------------------------------*/
		/* DMA1 channel5 configuration SPI2 TX ---------------------------------------------*/
		DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)rw_workbyte;
		DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
		DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;
		DMA_Init(pSpiData->pCfg->DMA.TX.Channel, &DMA_InitStructure);

	}
	else
	{
		/* DMA1 channel2 configuration SPI1 RX ---------------------------------------------*/
		/* DMA1 channel4 configuration SPI2 RX ---------------------------------------------*/
		DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)rw_workbyte;
		DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
		DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;
		DMA_Init(pSpiData->pCfg->DMA.RX.Channel, &DMA_InitStructure);

		/* DMA1 channel3 configuration SPI1 TX ---------------------------------------------*/
		/* DMA1 channel5 configuration SPI2 TX ---------------------------------------------*/
		DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)buff;
		DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
		DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
		DMA_Init(pSpiData->pCfg->DMA.TX.Channel, &DMA_InitStructure);


	}

	/* Enable DMA RX Channel */
	DMA_Cmd(pSpiData->pCfg->DMA.RX.Channel, ENABLE);
	/* Enable DMA TX Channel */
	DMA_Cmd(pSpiData->pCfg->DMA.TX.Channel, ENABLE);

	/* Enable SPI TX/RX request */
	SPI_I2S_DMACmd(pSpiData->pCfg->pDevice , SPI_I2S_DMAReq_Rx | SPI_I2S_DMAReq_Tx, ENABLE);

	/* Wait until DMA1_Channel 3 Transfer Complete */
	/// not needed: while (DMA_GetFlagStatus(DMA_FLAG_SPI_SD_TC_TX) == RESET) { ; }
	/* Wait until DMA1_Channel 2 Receive Complete */
	while (DMA_GetFlagStatus(pSpiData->pCfg->DMA.RX.FlagTc) == RESET) { ; }
	// same w/o function-call:
	// while ( ( ( DMA1->ISR ) & DMA_FLAG_SPI_SD_TC_RX ) == RESET ) { ; }

	/* Disable DMA RX Channel */
	DMA_Cmd(pSpiData->pCfg->DMA.RX.Channel, DISABLE);
	/* Disable DMA TX Channel */
	DMA_Cmd(pSpiData->pCfg->DMA.TX.Channel, DISABLE);

	/* Disable SPI RX/TX request */
	SPI_I2S_DMACmd(pSpiData->pCfg->pDevice, SPI_I2S_DMAReq_Rx | SPI_I2S_DMAReq_Tx, DISABLE);
}

