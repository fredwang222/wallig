/*-----------------------------------------------------------------------*/
/* MMC/SDC (in SPI mode) control module  (C)ChaN, 2006                   */
/*-----------------------------------------------------------------------*/

#include <stdio.h>


#include "../diskio.h"
#include "../ff.h" /* for _BYTE_ACC */
#include "Board.h"
#include "dvSpi/dvSpi.h"
#include "mmc.h"

#define SPI_CSR_NUM      0

#define SPI_SCBR_MIN     2

#define USE_DMA 1




static volatile
DSTATUS Stat = STA_NOINIT;	/* Disk status */

// AT91: thru systime
static volatile
 BYTE Timer;			/* 100Hz decrement timer */



/*-----------------------------------------------------------------------*/
/* Module Private Functions                                              */




/*--------------------------------*/
/* Receive a data packet from MMC */

static
BOOL rcvr_datablock (
	BYTE *buff,			/* Data buffer to store received data */
	BYTE wc				/* Word count (0 means 256 words) */
)
{
	BYTE token;

#ifdef USE_DMA
	/* TODO: deuglyfy */
	static const BYTE dummy_ff_block[512] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
#endif

	Timer = 10;
	do {							/* Wait for data packet in timeout of 100ms */
		token = rcvr_spi();
	} while ((token == 0xFF) && Timer);
	//iprintf("token: %x\n", token);
	if(token != 0xFE) return FALSE;	/* If not valid data token, retutn with error */

#ifdef USE_DMA
	// enable DMA transfer
	*AT91C_SPI_RPR = (unsigned long)buff;
	*AT91C_SPI_RCR = 2 * (wc == 0 ? 256 : wc);
	*AT91C_SPI_TPR = (unsigned long)dummy_ff_block;
	*AT91C_SPI_TCR = 2 * (wc == 0 ? 256 : wc);
	*AT91C_SPI_PTCR = AT91C_PDC_RXTEN;
	*AT91C_SPI_PTCR = AT91C_PDC_TXTEN;
	/*while(i++ < 512)
	*AT91C_SPI_TDR = 0xFF; // start transfer*/
	
	while(! (*AT91C_SPI_SR & AT91C_SPI_ENDRX));	
	*AT91C_SPI_PTCR = AT91C_PDC_RXTDIS;
	*AT91C_SPI_PTCR = AT91C_PDC_TXTDIS;
#else
	do {							/* Receive the data block into buffer */
		rcvr_spi_m(buff++);
		rcvr_spi_m(buff++);
	} while (--wc);
#endif
	
	rcvr_spi();						/* Discard CRC */
	rcvr_spi();

	//puts("rcvr_datablock success");
	return TRUE;					/* Return with success */
}



/*---------------------------*/
/* Send a data packet to MMC */
static
BOOL xmit_datablock (
	const BYTE *buff,	/* 512 byte data block to be transmitted */
	BYTE token			/* Data/Stop token */
)
{
	BYTE resp, wc = 0;


	if (wait_ready() != 0xFF) return FALSE;

	xmit_spi(token);					/* Xmit data token */
	if (token != 0xFD) {	/* Is data token */
		do {							/* Xmit the 512 byte data block to MMC */
			xmit_spi(*buff++);
			xmit_spi(*buff++);
		} while (--wc);
		xmit_spi(0xFF);					/* CRC (Dummy) */
		xmit_spi(0xFF);
		resp = rcvr_spi();				/* Reveive data response */
		if ((resp & 0x1F) != 0x05)		/* If not accepted, return with error */
			return FALSE;
	}

	return TRUE;
}


/*------------------------------*/
/* Send a command packet to MMC */

static
BYTE send_cmd (
	BYTE cmd,		/* Command byte */
	DWORD arg		/* Argument */
)
{
	BYTE n, res;


	if (wait_ready() != 0xFF) return 0xFF;

	/* Send command packet */
	xmit_spi(cmd);						/* Command */
	xmit_spi((BYTE)(arg >> 24));		/* Argument[31..24] */
	xmit_spi((BYTE)(arg >> 16));		/* Argument[23..16] */
	xmit_spi((BYTE)(arg >> 8));			/* Argument[15..8] */
	xmit_spi((BYTE)arg);				/* Argument[7..0] */
	xmit_spi(0x95);						/* CRC (valid for only CMD0) */

	/* Receive command response */
	if (cmd == CMD12) rcvr_spi();		/* Skip a stuff byte when stop reading */
	n = 10;								/* Wait for a valid response in timeout of 10 attempts */
	do
		res = rcvr_spi();
	while ((res & 0x80) && --n);

	return res;			/* Return with the response value */
}




/*-----------------------------------------------------------------------*/
/* Public Functions                                                      */
DSTATUS disk_shutdown ( void )
{
	return 0;
}

/*-----------------------*/
/* Initialize Disk Drive */
/* (Platform dependent)  */

DSTATUS MMC_disk_initialize ( void )
{
	BYTE n;


	POWER_ON();					/* Socket power ON */
	/* MMC socket-switch init */
	// disable internal Pull-Ups if needed
	AT91C_BASE_PIOA->PIO_PPUDR = CARD_INS_PIN | CARD_WP_PIN;
	// configure as inputs
	AT91C_BASE_PIOA->PIO_ODR = CARD_INS_PIN | CARD_WP_PIN;
	// enable control of pins by PIO
	AT91C_BASE_PIOA->PIO_PER = CARD_INS_PIN | CARD_WP_PIN;
		
	vFdvSpi_Init();
	
	Stat |= STA_NOINIT;
	if (!(Stat & STA_NODISK)) 
	{
		n = 10;						/* Dummy clock */
		do
			rcvr_spi();
		while (--n);

		//iprintf("SPI prepare done\n");

		SELECT();			/* CS = L */
		if (send_cmd(CMD0, 0) == 1)
		{			/* Enter Idle state */
			Timer = 100;						/* Wait for card ready in timeout of 1 sec */
			while (Timer && send_cmd(CMD1, 0));
			if (Timer) 
				Stat &= ~STA_NOINIT;		/* When device goes ready, clear STA_NOINIT */
		}
		
		//iprintf("CMD0 done timer = %i\n", Timer);
		
		DESELECT();			/* CS = H */
		rcvr_spi();			/* Idle (Release DO) */
		
		AT91_spiSetSpeed(SPI_SCBR_MIN); 
	}

	if (Stat & STA_NOINIT)
		disk_shutdown();
	return Stat;
}







/*--------------------*/
/* Return Disk Status */

DSTATUS MMC_disk_status ( void )
{
	return Stat;
}



/*----------------*/
/* Read Sector(s) */

DRESULT MMC_disk_read (
	BYTE *buff,			/* Data buffer to store read data */
	DWORD sector,		/* Sector number (LBA) */
	BYTE count			/* Sector count (1..255) */
)
{
	if (Stat & STA_NOINIT) return RES_NOTRDY;
	if (!count) return RES_PARERR;

	sector *= 512;		/* LBA --> byte address */

	SELECT();			/* CS = L */

	if (count == 1) {	/* Single block read */
		if ((send_cmd(CMD17, sector) == 0)	/* READ_SINGLE_BLOCK */
			&& rcvr_datablock(buff, (BYTE)(512/2)))
			count = 0;
	}
	else {				/* Multiple block read */
		if (send_cmd(CMD18, sector) == 0) {	/* READ_MULTIPLE_BLOCK */
			do {
				if (!rcvr_datablock(buff, (BYTE)(512/2))) break;
				buff += 512;
			} while (--count);
			send_cmd(CMD12, 0);				/* STOP_TRANSMISSION */
		}
	}

	DESELECT();			/* CS = H */
	rcvr_spi();			/* Idle (Release DO) */

	return count ? RES_ERROR : RES_OK;
}



/*-----------------*/
/* Write Sector(s) */


DRESULT MMC_disk_write (
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Sector number (LBA) */
	BYTE count			/* Sector count (1..255) */
)
{
	if (Stat & STA_NOINIT) return RES_NOTRDY;
	if (Stat & STA_PROTECT) return RES_WRPRT;
	if (!count) return RES_PARERR;
	sector *= 512;		/* LBA --> byte address */

	SELECT();			/* CS = L */

	if (count == 1) {	/* Single block write */
		if ((send_cmd(CMD24, sector) == 0)	/* WRITE_BLOCK */
			&& xmit_datablock(buff, 0xFE))
			count = 0;
	}
	else {				/* Multiple block write */
		if (send_cmd(CMD25, sector) == 0) {	/* WRITE_MULTIPLE_BLOCK */
			do {
				if (!xmit_datablock(buff, 0xFC)) break;
				buff += 512;
			} while (--count);
			if (!xmit_datablock(0, 0xFD))	/* STOP_TRAN token */
				count = 1;
		}
	}

	DESELECT();			/* CS = H */
	rcvr_spi();			/* Idle (Release DO) */

	return count ? RES_ERROR : RES_OK;
}



/*--------------------------*/
/* Miscellaneous Functions  */

DRESULT MMC_disk_ioctl (
	BYTE ctrl,		/* Control code */
	void *buff		/* Buffer to send/receive data block */
)
{
	DRESULT res;
	BYTE n, csd[16], *ptr = buff;
	WORD csm, csize;


	if (Stat & STA_NOINIT) return RES_NOTRDY;

	SELECT();		/* CS = L */

	res = RES_ERROR;
	switch (ctrl) {
		case GET_SECTOR_COUNT :	/* Get number of sectors on the disk (unsigned long) */
			if ((send_cmd(CMD9, 0) == 0) && rcvr_datablock(csd, 16/2)) {
				/* Calculate disk size */
				csm = 1 << (((csd[10] & 128) >> 7) + ((csd[9] & 3) << 1) + 2);
				csize = ((WORD)(csd[8] & 3) >> 6) + (WORD)(csd[7] << 2) + ((WORD)(csd[6] & 3) << 10) + 1;
#ifdef _BYTE_ACC
				ST_DWORD(ptr, (DWORD)csize * csm);
#else
				*(DWORD*)ptr = (DWORD)csize * csm;
#endif
				res = RES_OK;
			}
			break;

		case MMC_GET_CSD :	/* Receive CSD as a data block (16 bytes) */
			if ((send_cmd(CMD9, 0) == 0)	/* READ_CSD */
				&& rcvr_datablock(ptr, 16/2))
				res = RES_OK;
			break;

		case MMC_GET_CID :	/* Receive CID as a data block (16 bytes) */
			if ((send_cmd(CMD10, 0) == 0)	/* READ_CID */
				&& rcvr_datablock(ptr, 16/2))
				res = RES_OK;
			break;

		case MMC_GET_OCR :	/* Receive OCR as an R3 resp (4 bytes) */
			if (send_cmd(CMD58, 0) == 0) {	/* READ_OCR */
				for (n = 0; n < 4; n++)
					*ptr++ = rcvr_spi();
				res = RES_OK;
			}
			break;

		default:
			res = RES_PARERR;
	}

	DESELECT();			/* CS = H */
	rcvr_spi();			/* Idle (Release DO) */

	return res;
}



/*---------------------------------------*/
/* Device timer interrupt procedure      */
/* This must be called in period of 10ms */
/* (Platform dependent)                  */

void disk_timerproc (void)
{
	static BYTE pv;
	BYTE n, s;
	
	static DWORD pv_d;
	DWORD n_d;
	
	n = Timer;						/* 100Hz decrement timer */
	if (n) Timer = --n;

	// n = pv;
	// pv = SOCKPORT & (SOCKWP | SOCKINS);	/* Sapmle socket switch */
	
	n_d = pv_d;
	pv = AT91C_BASE_PIOA->PIO_PDSR;

	if (n == pv) {					/* Have contacts stabled? */
		s = Stat;

		if (pv & SOCKWP)			/* WP is H (write protected) */
			s |= STA_PROTECT;
		else						/* WP is L (write enabled) */
			s &= ~STA_PROTECT;

		if (pv & SOCKINS)			/* INS = H (Socket empty) */
			s |= (STA_NODISK | STA_NOINIT);
		else						/* INS = L (Card inserted) */
			s &= ~STA_NODISK;

		Stat = s;
	}
}

/* 31-25: Year(0-127 org.1980), 24-21: Month(1-12), 20-16: Day(1-31) */
/* 15-11: Hour(0-23), 10-5: Minute(0-59), 4-0: Second(0-29 *2) */
DWORD get_fattime (void)	
{
	//Warning debug
	return 0;
}
							

