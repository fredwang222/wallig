/*
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
#include <string.h>
#include "stm32f10x.h"
#include "DRV_Spi.h"
#include "DRV_Spi_Cfg.h"


/********************************************************************************************************/
/*							Define																		*/
/********************************************************************************************************/
#define SPI_DEVICE_COUNT ( sizeof(SPI_DeviceCfgList)/sizeof(Spi_Device_Cfg))

/********************************************************************************************************/
/*							Macro																		*/
/********************************************************************************************************/

/********************************************************************************************************/
/*							Typedef																		*/
/********************************************************************************************************/
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

/********************************************************************************************************/
/*							Local Variables																*/
/********************************************************************************************************/
static const Spi_Device_Cfg SPI_DeviceCfgList[]=SPI_INIT_CFG;
static Spi_Device_Data SPI_DeviceDataList[SPI_DEVICE_COUNT];

/********************************************************************************************************/
/*							Local functions	declaration													*/
/********************************************************************************************************/
void Spi_Dma_Transfer(	Spi_Device_Data *pSpiData , char receive, const uint8_t *buff, uint16_t btr  );

/********************************************************************************************************/
/*							Public functions															*/
/********************************************************************************************************/
void DRV_Spi_Init(void )
{
	int iDevicecount;

	for( iDevicecount = 0 ; iDevicecount < SPI_DEVICE_COUNT ; iDevicecount++)
	{
		SPI_DeviceDataList[iDevicecount].pCfg = (Spi_Device_Cfg*)&SPI_DeviceCfgList[iDevicecount];
		SPI_DeviceDataList[iDevicecount].eState = Spi_Device_Close;
	}
}

DRV_Spi_Error DRV_Spi_Open( const char *pcName , DRV_Spi_Handle *pHandle)
{
	DRV_Spi_Error eError = Spi_Device_Not_Found;
	int iSpiIndex;
	Spi_Device_Data *pSpiData;
	SPI_InitTypeDef  SPI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	volatile unsigned char ucDummyread;

	for ( iSpiIndex = 0 ; iSpiIndex < SPI_DEVICE_COUNT ; iSpiIndex++ )
	{
		if( !strcmp( pcName , SPI_DeviceDataList[iSpiIndex].pCfg->pcName) )
		{
			pSpiData = &SPI_DeviceDataList[iSpiIndex];
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
	SPI_InitStructure.SPI_BaudRatePrescaler =  pSpiData->pCfg->speed[DRV_SPI_SLOW];
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;

	SPI_Init(pSpiData->pCfg->pDevice, &SPI_InitStructure);
	SPI_CalculateCRC(pSpiData->pCfg->pDevice, DISABLE);
	SPI_Cmd(pSpiData->pCfg->pDevice, ENABLE);

	/* drain SPI */
	while (SPI_I2S_GetFlagStatus(pSpiData->pCfg->pDevice, SPI_I2S_FLAG_TXE) == RESET)
		;
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

	if( pSpiData == NULL)
		return Spi_Bad_Param;

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

	if( pSpiData == NULL)
		return 0;

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

	if( pSpiData == NULL)
		return 0;

	Spi_Dma_Transfer(pSpiData , TRUE , pcBuffer , uiLen);
	return 0;
}

unsigned short DRV_Spi_Write_Buffer( DRV_Spi_Handle Handle , unsigned char *pcBuffer , unsigned int uiLen)
{
	Spi_Device_Data *pSpiData = (Spi_Device_Data *) Handle ;

	if( pSpiData == NULL)
		return 0;

	Spi_Dma_Transfer(pSpiData , FALSE , pcBuffer , uiLen);
	return 0;
}

/********************************************************************************************************/
/*							Local functions																*/
/********************************************************************************************************/
/* BOOL receive,		FALSE for buff->SPI, TRUE for SPI->buff               */
/*	const BYTE *buff,	receive TRUE  : 512 byte data block to be transmitted */
/*						receive FALSE : Data buffer to store received data    */
/*	UINT btr 			receive TRUE  : Byte count (must be multiple of 2)    */
/*						receive FALSE : Byte count (must be 512)              */
void Spi_Dma_Transfer(	Spi_Device_Data *pSpiData , char receive, const uint8_t *buff, uint16_t btr  )
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

	if ( receive )
	{
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
	while (DMA_GetFlagStatus(pSpiData->pCfg->DMA.RX.FlagTc) == RESET)
		;
	// same w/o function-call:
	// while ( ( ( DMA1->ISR ) & DMA_FLAG_SPI_SD_TC_RX ) == RESET ) { ; }

	/* Disable DMA RX Channel */
	DMA_Cmd(pSpiData->pCfg->DMA.RX.Channel, DISABLE);
	/* Disable DMA TX Channel */
	DMA_Cmd(pSpiData->pCfg->DMA.TX.Channel, DISABLE);

	/* Disable SPI RX/TX request */
	SPI_I2S_DMACmd(pSpiData->pCfg->pDevice, SPI_I2S_DMAReq_Rx | SPI_I2S_DMAReq_Tx, DISABLE);
}

