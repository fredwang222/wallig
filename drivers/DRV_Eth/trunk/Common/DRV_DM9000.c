/*
 * 	 Author : Gwendal Le Gall
 *   Date 02/02/2010
 *
 *    Modified from:  RT-Thread Development Team ( http://code.google.com/p/rt-thread/ )
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
#include <string.h>
#include "inc/DRV_DM9000.h"
#include "LIB_Fifo.h"
#include "DRV_Gpio.h"
#include "stm32f10x.h"
#include "stm32f10x_fsmc.h"

#define DM9000_PHY          0x40    /* PHY address 0x01 */

static void FSMC_Configuration();
void dm900_Receive_CallBack( void );
void dm900_Isr_CallBack( void );
const unsigned char tucMacAddr[6]={0x00,0x18,0xf3,0x09,0xaa,0x55};
#define ETH_FIFO_LEN 1600
#define ETH_FIFO_COUNT 4
unsigned char tucTXFifoBuffer[ETH_FIFO_LEN*ETH_FIFO_COUNT];
unsigned char tucTXFifoLen[sizeof(unsigned int)*ETH_FIFO_COUNT];
struct
{
	DRV_Gpio_Handle hExtPio;
	LIB_Fifo_Type RxFifoBuff;
	LIB_Fifo_Type RxFifoLen;
	unsigned int uiReceivBytes;
	unsigned char tucReceiveBuffer[2048];
	unsigned char ucTxBusy;
	unsigned char imr_all;
} DM9000_Data;

 void delay_ms(uint32_t ms)
{
    uint32_t len;
    for (;ms > 0; ms --)
        for (len = 0; len < 100; len++ );
}

/* Read a byte from I/O port */
uint8_t dm9000_io_read(uint16_t reg)
{
    DM9000_IO = reg;
    return (uint8_t) DM9000_DATA;
}

/* Write a byte to I/O port */
void dm9000_io_write(uint16_t reg, uint16_t value)
{
    DM9000_IO = reg;
    DM9000_DATA = value;
}


/* Read a word from phyxcer */
uint16_t phy_read(uint32_t reg)
{
    uint16_t val;

    /* Fill the phyxcer register into REG_0C */
    dm9000_io_write(DM9000_EPAR, DM9000_PHY | reg);
    dm9000_io_write(DM9000_EPCR, 0xc);	/* Issue phyxcer read command */

    delay_ms(100);		/* Wait read complete */

    dm9000_io_write(DM9000_EPCR, 0x0);	/* Clear phyxcer read command */
    val = (dm9000_io_read(DM9000_EPDRH) << 8) | dm9000_io_read(DM9000_EPDRL);

    return val;
}

/* Write a word to phyxcer */
void phy_write(uint16_t reg, uint16_t value)
{
    /* Fill the phyxcer register into REG_0C */
    dm9000_io_write(DM9000_EPAR, DM9000_PHY | reg);

    /* Fill the written data into REG_0D & REG_0E */
    dm9000_io_write(DM9000_EPDRL, (value & 0xff));
    dm9000_io_write(DM9000_EPDRH, ((value >> 8) & 0xff));
    dm9000_io_write(DM9000_EPCR, 0xa);	/* Issue phyxcer write command */

    delay_ms(500);		/* Wait write complete */

    dm9000_io_write(DM9000_EPCR, 0x0);	/* Clear phyxcer write command */
}


/* RT-Thread Device Interface */
/* initialize the interface */
 int dm9000_init( void )
{
	DRV_GpioINT PIOINITData;
    int i, oft, lnk , iMode;
    uint32_t value;

    FSMC_Configuration();

    /* RESET device */
    dm9000_io_write(DM9000_NCR, NCR_RST);
    delay_ms(1000);		/* delay 1ms */

    /* identfy DM9000 */
    value  = dm9000_io_read(DM9000_VIDL);
    value |= dm9000_io_read(DM9000_VIDH) << 8;
    value |= dm9000_io_read(DM9000_PIDL) << 16;
    value |= dm9000_io_read(DM9000_PIDH) << 24;
    if (value != DM9000_ID)
    {
        return -1;
    }
    /* GPIO0 on pre-activate PHY */
    /* GPIO0 on pre-activate PHY */
       dm9000_io_write(DM9000_GPR, 0x00);	            	/* REG_1F bit0 activate phyxcer */
       dm9000_io_write(DM9000_GPCR, GPCR_GEP_CNTL);    		/* Let GPIO0 output */
       dm9000_io_write(DM9000_GPR, 0x00);                 	/* Enable PHY */

    /* Program operating register */
	dm9000_io_write(DM9000_NCR, 0x0);						/* only intern phy supported by now */
	dm9000_io_write(DM9000_TCR, 0);	    					/* TX Polling clear */
	dm9000_io_write(DM9000_BPTR, 0x3f);						/* Less 3Kb, 200us */
	dm9000_io_write(DM9000_FCTR, FCTR_HWOT(3) | FCTR_LWOT(8));	/* Flow Control : High/Low Water */
	dm9000_io_write(DM9000_FCR, 0x0);						/* SH FIXME: This looks strange! Flow Control */
	dm9000_io_write(DM9000_SMCR, 0);						/* Special Mode */
	dm9000_io_write(DM9000_NSR, NSR_WAKEST | NSR_TX2END | NSR_TX1END);	/* clear TX status */
	dm9000_io_write(DM9000_ISR, 0x0f);						/* Clear interrupt status */

	for (i = 0, oft = 0x10; i < 6; i++, oft++)
		dm9000_io_write(oft,  tucMacAddr[i]);
	for (i = 0, oft = 0x16; i < 8; i++, oft++)
		dm9000_io_write(oft, 0xff);
	/* Activate DM9000 */
	dm9000_io_write(DM9000_RCR, RCR_DIS_LONG | RCR_DIS_CRC | RCR_RXEN);	/* RX enable */
	dm9000_io_write(DM9000_IMR, IMR_PAR);	/* Enable TX/RX interrupt mask */
	i = 0;
	while (!(phy_read(1) & 0x20)) {	/* autonegation complete bit */
		delay_ms(1000);
		i++;
		if (i == 10000) {
			//printf("could not establish link\n");
			return -1;
		}
	}

	 /* see what we've got */
	    lnk = phy_read(17) >> 12;
	    switch (lnk)
	    {
	    case 1:
	        //rt_kprintf("10M half duplex ");
	    	iMode=1;
	        break;
	    case 2:
	        //rt_kprintf("10M full duplex ");
	        iMode=2;
	        break;
	    case 4:
	        //rt_kprintf("100M half duplex ");
	    	iMode=4;
	        break;
	    case 8:
	        //rt_kprintf("100M full duplex ");
	    	iMode=8;
	        break;
	    default:
	        //rt_kprintf("unknown: %d ", lnk);
	    	iMode=0;
	        break;
	    }

	    LIB_Fifo_Init(&DM9000_Data.RxFifoBuff, ETH_FIFO_LEN, ETH_FIFO_COUNT , tucTXFifoBuffer , 0 );
	    LIB_Fifo_Init(&DM9000_Data.RxFifoLen, sizeof(unsigned int), ETH_FIFO_COUNT , tucTXFifoLen , 0 );

	    DM9000_Data.ucTxBusy = 0;
	    DM9000_Data.imr_all = IMR_PAR | IMR_PTM | IMR_PRM;
	    PIOINITData.EventType = GPIO_INT_High2Low;
		PIOINITData.CallBack = dm900_Isr_CallBack;
		DRV_Gpio_Open("DM9000_IRQ", &(DM9000_Data.hExtPio), &PIOINITData);
		dm9000_io_write(DM9000_IMR, IMR_PAR | IMR_PTM | IMR_PRM);
	    return 0;
}

 unsigned char dm9000_Send( unsigned char *pucBuffer , unsigned int uiLength )
 {
	 uint16_t *puiData =(uint16_t *) pucBuffer;
	 unsigned int uiIndex , uiDataBuffLen=(uiLength+1)/2;

	 if( DM9000_Data.ucTxBusy )
		 return 0;

	 DM9000_Data.ucTxBusy=1;
	 /* disable dm9000a interrupt */
	     dm9000_io_write(DM9000_IMR, IMR_PAR);
		/* Move data to DM9000 TX RAM */
		DM9000_outb(DM9000_IO_BASE, DM9000_MWCMD);

		for (uiIndex = 0; uiIndex < uiDataBuffLen ; uiIndex++,puiData++)
			DM9000_outw(DM9000_DATA_BASE,*puiData);

		/* Set TX length to DM9000 */
		dm9000_io_write(DM9000_TXPLL, uiLength & 0xff);
		dm9000_io_write(DM9000_TXPLH, (uiLength >> 8) & 0xff);

		/* Issue TX polling command */
		dm9000_io_write(DM9000_TCR, TCR_TXREQ);	/* Cleared after TX complete */

		dm9000_io_write(DM9000_IMR, IMR_PAR | IMR_PTM | IMR_PRM);
		/* wait for end of transmission */
		return 1;
 }

 void dm9000_Receive( unsigned char *pucBuffer , unsigned int *puiLength )
 {

	 if(!LIB_Fifo_Empty(&DM9000_Data.RxFifoBuff) )
	 {
		 dm9000_io_write(DM9000_IMR, IMR_PAR);

		 LIB_Fifo_Pop(&DM9000_Data.RxFifoBuff,pucBuffer);
		 LIB_Fifo_Pop(&DM9000_Data.RxFifoLen,(unsigned char*)puiLength);
		 //*puiLength=DM9000_Data.uiReceivBytes;
		 //memcpy(pucBuffer , DM9000_Data.tucReceiveBuffer,*puiLength);
		 //DM9000_Data.uiReceivBytes=0;
		 dm9000_io_write(DM9000_IMR, IMR_PAR | IMR_PTM | IMR_PRM);
	 }

 }


 void dm900_Isr_CallBack( void )
 {
	 uint16_t int_status;
	 unsigned int uiLocalStatus=0;
	 uint16_t last_io;

	 last_io = DM9000_IO;
	/* Disable all interrupts */
	dm9000_io_write(DM9000_IMR, IMR_PAR);
	/* Got DM9000 interrupt status */
	int_status = dm9000_io_read(DM9000_ISR);               /* Got ISR */
	dm9000_io_write(DM9000_ISR, int_status);    /* Clear ISR status */

	/* receive overflow */
	    if (int_status & ISR_ROS)
	    {
	        //rt_kprintf("overflow\n");
	    	uiLocalStatus=1;
	    }

	    if (int_status & ISR_ROOS)
	    {
	    	//rt_kprintf("overflow counter overflow\n");
	    	uiLocalStatus|=2;
	    }

	    /* Received the coming packet */
		if (int_status & ISR_PRS)
		{
			uiLocalStatus|=4;
			/* disable receive interrupt */
			//DM9000_Data.imr_all = IMR_PAR | IMR_PTM ;
			/* a frame has been received */
			dm900_Receive_CallBack();
		}

		/* Transmit Interrupt check */
		if (int_status & ISR_PTS)
		{

			/* transmit done */
			int tx_status = dm9000_io_read(DM9000_NSR);    /* Got TX status */
			uiLocalStatus|=8;

			if (tx_status & (NSR_TX2END | NSR_TX1END))
			{
				uiLocalStatus|=0x10;
				DM9000_Data.ucTxBusy=0;
			}

		}

	    /* Re-enable interrupt mask */
	    dm9000_io_write(DM9000_IMR, DM9000_Data.imr_all);
	    DM9000_IO = last_io;
 }

 void dm900_Receive_CallBack( void )
 {
	 unsigned char ucRxByte;
	 uint16_t usRxStatus , usRxLen,*puiData;
	 unsigned int uiIndex , uiDataBuffLen;

		/* Check packet ready or not */
		 dm9000_io_read(DM9000_MRCMDX);	/* Dummy read */
		ucRxByte = DM9000_inb(DM9000_DATA_BASE);	/* Got most updated data */
		if (ucRxByte == 0)
		{
		        /* restore receive interrupt */
				DM9000_Data.imr_all = IMR_PAR | IMR_PTM | IMR_PRM;
		        dm9000_io_write(DM9000_IMR, DM9000_Data.imr_all);
	    }


		/* Status check: this byte must be 0 or 1 */
		if (ucRxByte > 1)
		{
			dm9000_io_write(DM9000_RCR, 0x00);	/* Stop Device */
			dm9000_io_write(DM9000_ISR, 0x80);	/* Stop INT request */
		}

		if( DM9000_Data.uiReceivBytes )
			DM9000_Data.uiReceivBytes=0;

		/* A packet ready now  & Get status/length */
		 DM9000_outb(DM9000_IO_BASE, DM9000_MRCMD);

		usRxStatus = DM9000_inw(DM9000_DATA_BASE);
		usRxLen = DM9000_inw(DM9000_DATA_BASE);

		DM9000_Data.uiReceivBytes = usRxLen;
		if( DM9000_Data.uiReceivBytes>255)
			uiIndex=255;
		uiDataBuffLen=(usRxLen+1)/2;
		puiData = (uint16_t*)DM9000_Data.tucReceiveBuffer;
		for (uiIndex = 0; uiIndex < uiDataBuffLen ; uiIndex++)
			puiData[ uiIndex ] = DM9000_inw(DM9000_DATA_BASE);

		if ((usRxStatus & 0xbf00) || (usRxLen < 0x40)|| (usRxLen > DM9000_PKT_MAX))
		{
				if (usRxStatus & 0x100) {
					//printf("rx fifo error\n");
					return ;
				}
				if (usRxStatus & 0x200) {
					//printf("rx crc error\n");
					return ;
				}
				if (usRxStatus & 0x8000) {
					//printf("rx length error\n");
					return ;
				}
				if (usRxLen > DM9000_PKT_MAX) {
					//printf("rx length too big\n");
					/* RESET device */
					dm9000_io_write(DM9000_NCR, NCR_RST);
					return ;
					//dm9000_reset();
				}
		}
		LIB_Fifo_Push(&DM9000_Data.RxFifoBuff , DM9000_Data.tucReceiveBuffer);
		LIB_Fifo_Push(&DM9000_Data.RxFifoLen , (unsigned char *)&DM9000_Data.uiReceivBytes);

 }

static void FSMC_Configuration()
{
	FSMC_NORSRAMInitTypeDef FSMC_NORSRAMInitStructure;
	FSMC_NORSRAMTimingInitTypeDef p;
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOG | RCC_APB2Periph_GPIOE |
	                     RCC_APB2Periph_GPIOF, ENABLE);

	/*-- GPIO Configuration ------------------------------------------------------*/
	/* NE4 configuration */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_Init(GPIOG, &GPIO_InitStructure);

	/*-- FSMC Configuration ------------------------------------------------------*/
	p.FSMC_AddressSetupTime = 0;
	p.FSMC_AddressHoldTime = 0;
	p.FSMC_DataSetupTime = 2;
	p.FSMC_BusTurnAroundDuration = 0;
	p.FSMC_CLKDivision = 0;
	p.FSMC_DataLatency = 0;
	p.FSMC_AccessMode = FSMC_AccessMode_A;

	FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM4;
	FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
	FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_SRAM;
	FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
	FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
	FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
	FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
	FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
	FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
	FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
	FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;
	FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
	FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &p;
	FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &p;

	FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);

	/* Enable FSMC Bank1_SRAM Bank4 */
	FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM4, ENABLE);
}
