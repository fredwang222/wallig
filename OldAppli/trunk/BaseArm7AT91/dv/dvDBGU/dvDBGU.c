/* ----------------------------------------------------------------------------
 *         ATMEL Microcontroller Software Support
 * ----------------------------------------------------------------------------
 * Copyright (c) 2008, Atmel Corporation
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the disclaimer below.
 *
 * Atmel's name may not be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * DISCLAIMER: THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * ----------------------------------------------------------------------------
 */

//------------------------------------------------------------------------------
//      Headers
//------------------------------------------------------------------------------

#include "sys/AT91SAM7S64.h"                    /* AT91SAMT7S64 definitions */
#include "sys/lib_AT91SAM7S64.h"                    /* AT91SAMT7S64 definitions */
#include "sys/Board.h"
//------------------------------------------------------------------------------
//      Exported functions
//------------------------------------------------------------------------------

#define BR    115200                        /* Baud Rate */

#define BRD  (MCK/16/BR)                    /* Baud Rate Divisor */

//------------------------------------------------------------------------------
/// Initializes the DBGU with the given parameters, and enables both the
/// transmitter and the receiver.
/// \param mode  Operating mode to configure (see <Modes>).
/// \param baudrate  Desired baudrate.
/// \param mck  Frequency of the system master clock.
//------------------------------------------------------------------------------
void DBGU_Configure(void)
{

    AT91F_DBGU_CfgPIO();
      AT91F_US_Configure ((AT91PS_USART) AT91C_BASE_DBGU,
						  MCK,
                          AT91C_US_ASYNC_MODE,  	// mode Register to be programmed
                          BR,	// baudrate to be programmed
                          0);                   	// timeguard to be programmed
      // Enable Transmitter
      AT91F_US_EnableTx((AT91PS_USART) AT91C_BASE_DBGU);
}

//------------------------------------------------------------------------------
/// Outputs a character on the DBGU line.
/// \param c  Character to send.
//------------------------------------------------------------------------------
static void DBGU_PutChar(unsigned char c)
{
    // Wait for the transmitter to be ready
    while (!AT91F_US_TxReady((AT91PS_USART) AT91C_BASE_DBGU));
    // Send character
    AT91F_US_PutChar((AT91PS_USART) AT91C_BASE_DBGU , c );
    // Wait for the transfer to complete
    while (!AT91F_US_TxReady((AT91PS_USART) AT91C_BASE_DBGU));
}


void DBGU_PutString( char *pString)
{
	while( *pString)
	{
		DBGU_PutChar(*pString);
		pString++;
	}
}
//------------------------------------------------------------------------------
/// Reads and returns a character from the DBGU.
//------------------------------------------------------------------------------
unsigned char DBGU_GetChar()
{
    while ((AT91C_BASE_DBGU->DBGU_CSR & AT91C_US_RXRDY) == 0);
    return AT91C_BASE_DBGU->DBGU_RHR;
}

