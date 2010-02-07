/******************************************************************************/
/*                                                                            */
/*  SERIAL.C:  Low Level Serial Routines                                      */
/*                                                                            */
/******************************************************************************/
/* 
   10/2005 by Martin Thomas <eversmith@heizung-thomas.de>
   based on an example-code from Keil GmbH
   - modified for the WinARM example 
   - changed interface to avoid potential conflicts with "stdio.h"
*/

#include "sys/AT91SAM7S64.h"                    /* AT91SAMT7S64 definitions */
#include "sys/lib_AT91SAM7S64.h"                    /* AT91SAMT7S64 definitions */
#include "sys/Board.h"
#include "dv/dvISR/interrupt_utils.h"

#ifdef ERAM   /* Fast IRQ functions Run in RAM  - see board.h */
#define ATTR RAMFUNC
#else
#define ATTR
#endif
void NACKEDFUNC SerialIsr_Wrapper( void );

#define BR    115200                        /* Baud Rate */

#define BRD  (MCK/16/BR)                    /* Baud Rate Divisor */

#define USART_INTERRUPT_LEVEL		5
AT91S_USART * pUSART = AT91C_BASE_US0;      /* Global Pointer to USART0 */
void (*pvfRXCallback)(unsigned char*);

void vFdvUart_init (void) {                   /* Initialize Serial Interface */
  /* mt: n.b: uart0 clock must be enabled to use it */

  *AT91C_PIOA_PDR = AT91C_PA5_RXD0 |        /* Enable RxD0 Pin */
                    AT91C_PA6_TXD0;         /* Enalbe TxD0 Pin */

  	// Disable interrupts
  pUSART->US_IDR = (unsigned int) -1;

    // Timeguard disabled
  pUSART->US_TTGR = 0;

  
  pUSART->US_CR = AT91C_US_RSTRX |          /* Reset Receiver      */
                  AT91C_US_RSTTX |          /* Reset Transmitter   */
                  AT91C_US_RXDIS |          /* Receiver Disable    */
                  AT91C_US_TXDIS;           /* Transmitter Disable */

  pUSART->US_MR = AT91C_US_USMODE_NORMAL |  /* Normal Mode */
                  AT91C_US_CLKS_CLOCK    |  /* Clock = MCK */
                  AT91C_US_CHRL_8_BITS   |  /* 8-bit Data  */
                  AT91C_US_PAR_NONE      |  /* No Parity   */
                  AT91C_US_NBSTOP_1_BIT;    /* 1 Stop Bit  */

  pUSART->US_BRGR = BRD;                    /* Baud Rate Divisor */

  //* Reset receiver and transmitter
      pUSART->US_CR = AT91C_US_RSTRX | AT91C_US_RSTTX ;
  
      pUSART->US_CR = AT91C_US_RXEN  |          /* Receiver Enable     */
                  AT91C_US_TXEN;            /* Transmitter Enable  */
  //* Enable USART IT AT91C_US_ENDRX
  	AT91F_US_EnableIt(AT91C_BASE_US0, AT91C_US_RXRDY);
  //* open Usart 0 interrupt
  	AT91F_AIC_ConfigureIt ( AT91C_BASE_AIC, AT91C_ID_US0, USART_INTERRUPT_LEVEL,AT91C_AIC_SRCTYPE_HIGH_LEVEL, (void (*) ())SerialIsr_Wrapper); 
  	AT91F_AIC_EnableIt (AT91C_BASE_AIC, AT91C_ID_US0);

  	pvfRXCallback=0;
  	AT91F_PDC_Open(AT91C_BASE_PDC_US0);
  	//AT91F_PDC_Close(AT91C_BASE_PDC_US0);
  	/*AT91F_PDC_DisableTx(AT91C_BASE_PDC_US0);
  	AT91F_PDC_SetNextTx(AT91C_BASE_PDC_US0, (char *) 0, 0);
  	AT91F_PDC_SetTx(AT91C_BASE_PDC_US0, (char *) 0, 0);
  	AT91F_PDC_EnableTx(AT91C_BASE_PDC_US0);*/
}

void usart_c_irq_handler( void )
{
	//volatile unsigned int status;
	volatile unsigned char ucByteIn;
	volatile unsigned int uiStatus;
	
	uiStatus = AT91C_BASE_US0->US_CSR;
	uiStatus &= AT91C_BASE_US0->US_IMR;
	//* get Usart status register
	//if ( AT91F_US_RxReady(AT91C_BASE_US0)&AT91C_US_RXRDY )
	 if ( uiStatus&AT91C_US_RXRDY )
	{
		ucByteIn = AT91F_US_GetChar( AT91C_BASE_US0 );
		if( pvfRXCallback )
			pvfRXCallback((unsigned char*)&ucByteIn);
	}
	
	//* Reset the satus bit
	 AT91C_BASE_US0->US_CR = AT91C_US_RSTSTA;
	AT91F_AIC_AcknowledgeIt(AT91C_BASE_AIC);
}

void NACKEDFUNC SerialIsr_Wrapper( void )
{
	/* Save the context of the interrupted task. */
	ISR_ENTRY();

	/* Call the handler to do the work.  This must be a separate
	function to ensure the stack frame is set up correctly. */
	usart_c_irq_handler();

	/* Restore the context of whichever task will execute next. */
	ISR_EXIT();
}

void vFdvUart_SetRXCallback( void (*callback)(unsigned char*))
{
	pvfRXCallback = callback;

}

void vFdvUart_WaitTXFree(void)
{
	while (!(pUSART->US_CSR & AT91C_US_TXRDY));   /* Wait for Empty Tx Buffer */
}

int uart0_putc(int ch) 
{
	while (!(pUSART->US_CSR & AT91C_US_TXRDY));   /* Wait for Empty Tx Buffer */
	return (pUSART->US_THR = ch);                 /* Transmit Character */
}	

int uart0_putchar (int ch) {                      /* Write Character to Serial Port */

  if (ch == '\n')  {                            /* Check for LF */
    uart0_putc( '\r' );                         /* Output CR */
  }
  return uart0_putc( ch );                     /* Transmit Character */
}

int uart0_puts ( char* s )
{
	int i = 0;
	while ( *s ) {
		uart0_putc( *s++ );
		i++;
	}
	return i;
}

int uart0_prints ( char* s )
{
	int i = 0;
	while ( *s ) {
		uart0_putchar( *s++ );
		i++;
	}
	return i;
}

int iFdvUart_kbhit( void ) /* returns true if character in receive buffer */
{
	if ( pUSART->US_CSR & AT91C_US_RXRDY) {
		return 1;
	}
	else {
		return 0;
	}
}

int uart0_getc ( void )  /* Read Character from Serial Port */
{    
  while (!(pUSART->US_CSR & AT91C_US_RXRDY));   /* Wait for Full Rx Buffer */
  return (pUSART->US_RHR);                      /* Read Character */
}


unsigned char ucFdvUart_GetByte(void)
{
	return (pUSART->US_RHR);                      /* Read Character */
}



void vFdvUartSendPS2Byte( unsigned char );
//static unsigned char uc1,uc2;

void vFdvUart0_init (void) 
{                   /* Initialize Serial Interface */
	  /* mt: n.b: uart0 clock must be enabled to use it */
	volatile unsigned int uiCount,uiDummy;
	

	  *AT91C_PIOA_PDR = AT91C_PA5_RXD0 ;        /* Enable RxD0 & SCK0 Pin */

	  	// Disable interrupts
	  pUSART->US_IDR = (unsigned int) -1;

	    // Timeguard disabled
	  pUSART->US_TTGR = 0;

	  
	  pUSART->US_CR = AT91C_US_RSTRX |          /* Reset Receiver      */
	                  AT91C_US_RSTTX |          /* Reset Transmitter   */
	                  AT91C_US_RXDIS |          /* Receiver Disable    */
	                  AT91C_US_TXDIS;           /* Transmitter Disable */

	  pUSART->US_MR = AT91C_US_USMODE_NORMAL |  /* Normal Mode */
	  					AT91C_US_CLKS_EXT    |  /* Externalclock */
	                  AT91C_US_CHRL_8_BITS   |  /* 8-bit Data  */
	                  AT91C_US_PAR_ODD      |  /* Parity  Odd */
	                  AT91C_US_NBSTOP_1_BIT  | /* 1 Stop Bit  */
	                  AT91C_US_SYNC;    	 /*synchonous mode*/


	  	//* Reset receiver and transmitter
	      pUSART->US_CR = AT91C_US_RSTRX | AT91C_US_RSTTX ;
	  
//
	      vFdvUartSendPS2Byte(0xF4);
	      
  	      for (uiCount=0;uiCount<500;uiCount++)
  	      	uiDummy++;
  	      

	      while( 1)
	    	  ;
}


void vFdvUartSendPS2Byte( unsigned char ucData )
{
	volatile unsigned int uiCount,uiDummy;
	unsigned char ucParityCount = 0;
	
	 pUSART->US_CR =  AT91C_US_RXDIS ;          /* Receiver Disable    */
      //set clock line low: configure pio tx has pio
      AT91F_PIO_CfgOutput(AT91C_BASE_PIOA,AT91C_PA2_SCK0);
      AT91C_BASE_PIOA->PIO_MDER = AT91C_PA2_SCK0;
      AT91F_PIO_ClearOutput(AT91C_BASE_PIOA,AT91C_PA2_SCK0);
      //wait 
      for (uiCount=0;uiCount<500;uiCount++)
      	uiDummy++;
      AT91F_PIO_CfgOutput(AT91C_BASE_PIOA,AT91C_PIO_PA6);
      AT91C_BASE_PIOA->PIO_MDER = AT91C_PIO_PA6;
      AT91F_PIO_ClearOutput(AT91C_BASE_PIOA,AT91C_PIO_PA6);
      AT91F_PIO_SetOutput(AT91C_BASE_PIOA,AT91C_PA2_SCK0);
      AT91F_PIO_CfgInput(AT91C_BASE_PIOA,AT91C_PA2_SCK0);
      
      for (uiCount=0;uiCount<8;uiCount++)
      {
    	  //wait clock line low
    	  while( (AT91C_BASE_PIOA->PIO_PDSR)&AT91C_PA2_SCK0)
    		  ;
    	  //set data line level
    	  if( ucData & 0x1 )
    	  {
    		ucParityCount++;
    		AT91C_BASE_PIOA->PIO_SODR = AT91C_PA6_TXD0;
    	  }
    	  else
    		AT91C_BASE_PIOA->PIO_CODR = AT91C_PA6_TXD0;
    	  ucData>>=1;
    	//wait clock line high
    	  while( !((AT91C_BASE_PIOA->PIO_PDSR)&AT91C_PA2_SCK0))
    		  ;
      }
      //wait clock line low
      while( (AT91C_BASE_PIOA->PIO_PDSR)&AT91C_PA2_SCK0)
		  ;
      //parity: 
      if( ucParityCount & 0x1 )
    	  AT91C_BASE_PIOA->PIO_CODR = AT91C_PA6_TXD0;
	  else
    	  AT91C_BASE_PIOA->PIO_SODR = AT91C_PA6_TXD0;
	  //wait clock line high
	  while( !((AT91C_BASE_PIOA->PIO_PDSR)&AT91C_PA2_SCK0))
  		  ;
	  //wait clock line low
	  while( (AT91C_BASE_PIOA->PIO_PDSR)&AT91C_PA2_SCK0)
		  ;
      //stop bit:level 1 <=> cfg input
	  AT91F_PIO_CfgInput(AT91C_BASE_PIOA,AT91C_PA6_TXD0);
	  /* Receiver Enable     */
      pUSART->US_CR = AT91C_US_RXEN ;         
}
