#ifndef DVSPI_H_
#define DVSPI_H_
void vFdvSpi_Init( void );
void AT91_spiSetSpeed(BYTE speed);
BYTE AT91_spi(BYTE outgoing);
void xmit_spi(BYTE dat);
BYTE rcvr_spi(void);
void rcvr_spi_m(BYTE *dst);
BYTE wait_ready (void);
#endif /*DVSPI_H_*/
