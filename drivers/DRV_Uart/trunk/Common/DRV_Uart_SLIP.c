/**
	\file DRV_Uart.c

	\author Gwendal Le Gall

	\date 20/02/2010
*/
#include "DRV_Uart_SLIP.h"
/**************************************************************
					Define
***************************************************************/
 /* SLIP special character codes
*/
#define END             0300    /* indicates end of packet */
#define ESC             0333    /* indicates byte stuffing */
#define ESC_END         0334    /* ESC ESC_END means END data byte */
#define ESC_ESC         0335    /* ESC ESC_ESC means ESC data byte */
#define kSLIP_default 0
#define kSLIP_Previous_ESC 1

int DRV_Uart_Slip_Rx( tSLIP_Data *ptData , unsigned char ucInput , unsigned char *pucOutPutBuff, int iOutputMaxLen)
{
	int iReturnLen=0;

	switch( ptData->iSlipState)
	{
		default:
			ptData->iSlipState = kSLIP_default;
		case kSLIP_default:
			switch(ucInput)
			{
				/* if it's an END character then we're done with the packet	*/
				case END:
					if(ptData->iReceived)
					{
						iReturnLen = ptData->iReceived;
						ptData->iReceived=0;
					}
					break;
				/* if it's the same code as an ESC character, wait and get another character
				* and then figure out what to store in the packet based on that. */
				case ESC:
					ptData->iSlipState = kSLIP_Previous_ESC;
					break;
				default:
					if( ptData->iReceived < iOutputMaxLen )
						pucOutPutBuff[ptData->iReceived++] = ucInput;
			}
			break;
		case kSLIP_Previous_ESC:
			/* if "c" is not one of these two, then we have a protocol violation.  The best
			* bet seems to be to leave the byte alone and just stuff it into the packet	*/
			switch(ucInput)
			{
				case ESC_END:
					ucInput = END;
					break;
				case ESC_ESC:
					ucInput = ESC;
					break;
			}
			if( ptData->iReceived < iOutputMaxLen )
				pucOutPutBuff[ptData->iReceived++] = ucInput;
			ptData->iSlipState = kSLIP_default;
			break;
	}
	return iReturnLen;
}

/**
 * \todo there is no checking of the max output buff length
 */
void DRV_Uart_Slip_Tx(unsigned char *pucInputBuff, int iLength ,unsigned char *pucOutputBuff)
{
	/* send an initial END character to flush out any data that may
	* have accumulated in the receiver due to line noise
	*/
	*(pucOutputBuff++) = END;

	/* for each byte in the packet, send the appropriate character
	* sequence
	*/
	while(iLength--)
	{
		switch(*pucInputBuff)
		{
			/* if it's the same code as an END character, we send a
			* special two character code so as not to make the
			* receiver think we sent an END
			*/
			case END:
				*(pucOutputBuff++) = (ESC);
				*(pucOutputBuff++) = (ESC_END);
			   break;

			/* if it's the same code as an ESC character,
			* we send a special two character code so as not
			* to make the receiver think we sent an ESC
			*/
			case ESC:
			   *(pucOutputBuff++) = (ESC);
			   *(pucOutputBuff++) = (ESC_ESC);
			   break;

			/* otherwise, we just send the character
			*/
			default:
				*(pucOutputBuff++) = (*pucInputBuff);
		  }
		pucInputBuff++;
	}
    /* tell the receiver that we're done sending the packet	*/
	*(pucOutputBuff++) = (END);
}

