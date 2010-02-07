/*-----------------------------------------------------------------------*/
/* MMC/SDC (in SPI mode) control module  (C)ChaN, 2006                   */
/*-----------------------------------------------------------------------*/
#include "sys/AT91SAM7S64.h"                    /* AT91SAMT7S64 definitions */
#include "sys/lib_AT91SAM7S64.h"                    /* AT91SAMT7S64 definitions */
#include "sys/Board.h"

#ifndef _INTEGER

/* These types must be 16-bit, 32-bit or larger integer */
typedef int				INT;
typedef unsigned int	UINT;

/* These types must be 8-bit integer */
typedef signed char		CHAR;
typedef unsigned char	UCHAR;
typedef unsigned char	BYTE;

/* These types must be 16-bit integer */
typedef short			SHORT;
typedef unsigned short	USHORT;
typedef unsigned short	WORD;

/* These types must be 32-bit integer */
typedef long			LONG;
typedef unsigned long	ULONG;
typedef unsigned long	DWORD;

/* Boolean type */
typedef enum { FALSE = 0, TRUE } BOOL;

#define _INTEGER
#endif
static AT91PS_PIO  pPIOA = AT91C_BASE_PIOA; 
static AT91PS_SPI  pSPI  = AT91C_BASE_SPI;

#define SPI_CSR_NUM      0

#define SPI_SCBR_MIN     2


// AT91: thru systime
static volatile
 BYTE Timer;			/* 100Hz decrement timer */


void AT91_spiSetSpeed(BYTE speed)
{
	DWORD reg;

	if ( speed < SPI_SCBR_MIN ) 
		speed = SPI_SCBR_MIN;
	if ( speed > 1 ) 
		speed &= 0xFE;

	reg = pSPI->SPI_CSR[SPI_CSR_NUM];
	reg = ( reg & ~(AT91C_SPI_SCBR) ) | ( (DWORD)speed << 8 );
	pSPI->SPI_CSR[SPI_CSR_NUM] = reg;
}

BYTE AT91_spi(BYTE outgoing)
{
	BYTE incoming;

	while( !( pSPI->SPI_SR & AT91C_SPI_TDRE ) ); // transfer compl. wait
	pSPI->SPI_TDR = (WORD)( outgoing );
	while( !( pSPI->SPI_SR & AT91C_SPI_RDRF ) ); // wait for char
	incoming = (BYTE)( pSPI->SPI_RDR );

	return incoming;
}

/*--------------------------------*/
/* Transmit a byte to MMC via SPI */
/* (Platform dependent)           */

void xmit_spi(BYTE dat)
{
	AT91_spi(dat);
}

/*---------------------------------*/
/* Receive a byte from MMC via SPI */
/* (Platform dependent)            */

BYTE rcvr_spi(void)
{
	return AT91_spi(0xff);
}

/* Alternative "macro" (not at AT91 so far) to receive data fast */
void rcvr_spi_m(BYTE *dst)
{
	*dst = rcvr_spi();
}


/*---------------------*/
/* Wait for card ready */
BYTE wait_ready ()
{
	BYTE res;
	
	Timer = 50;			/* Wait for ready in timeout of 500ms */
	rcvr_spi();
	do
		res = rcvr_spi();
	while ((res != 0xFF) && Timer);
	return res;
}






/*-----------------------------------------------------------------------*/
/* Public Functions                                                      */


/*-----------------------*/
/* Initialize Disk Drive */
/* (Platform dependent)  */

void vFdvSpi_Init( void )
{
	// disable PIO from controlling MOSI, MISO, SCK (=hand over to SPI)
	// keep CS untouched - used as GPIO pin during init
	pPIOA->PIO_PDR = AT91C_PA12_MISO | AT91C_PA13_MOSI | AT91C_PA14_SPCK|AT91C_PA11_NPCS0; //  | NCPS_PDR_BIT;
	// set pin-functions in PIO Controller
	pPIOA->PIO_ASR = AT91C_PA12_MISO | AT91C_PA13_MOSI | AT91C_PA14_SPCK|AT91C_PA11_NPCS0; /// not here: | NCPS_ASR_BIT;
	

	// enable peripheral clock for SPI ( PID Bit 5 )
	AT91F_PMC_EnablePeriphClock(AT91C_BASE_PMC,1 << AT91C_ID_SPI  );
	//pPMC->PMC_PCER = ( (DWORD) 1 << AT91C_ID_SPI ); // n.b. IDs are just bit-numbers
	
	// SPI enable and reset
	AT91F_SPI_Reset(AT91C_BASE_SPI);
	AT91F_SPI_Enable(AT91C_BASE_SPI);
	//pSPI->SPI_CR = AT91C_SPI_SPIEN | AT91C_SPI_SWRST;
	
	// SPI mode: master, FDIV=0, fault detection disabled
	AT91F_SPI_CfgMode(AT91C_BASE_SPI,AT91C_SPI_MSTR | AT91C_SPI_MODFDIS);
	//pSPI->SPI_MR  = AT91C_SPI_MSTR | AT91C_SPI_MODFDIS;
	
	// set chip-select-register
	// 8 bits per transfer, CPOL=1, ClockPhase=0, DLYBCT = 0
	AT91F_SPI_CfgCs(AT91C_BASE_SPI ,SPI_CSR_NUM, AT91C_SPI_CPOL | AT91C_SPI_BITS_8);
	//pSPI->SPI_CSR[SPI_CSR_NUM] = AT91C_SPI_CPOL | AT91C_SPI_BITS_8;
	
	// slow during init
	AT91_spiSetSpeed(0xFE); 
	
	// enable
	pSPI->SPI_CR = AT91C_SPI_SPIEN;

}



