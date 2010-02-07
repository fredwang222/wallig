#include "dvUart/dvUart.h"
#include "cfg_libUart/libUartConfig.h"

static struct
{
	unsigned char tucInputBuffer[kLIBUART_INPUT_BUFFER_SIZE];
	unsigned int uiIndexIn;
	unsigned int uiIndexOut;
	unsigned int uiByteCount;
}tLibUartData;

void vFlibUart_SetRXByte( unsigned char *);

void vFlibUart_Init( void )
{
	tLibUartData.uiIndexIn = 0;
	tLibUartData.uiIndexOut = 0;
	tLibUartData.uiByteCount = 0;
	vFdvUart_SetRXCallback(vFlibUart_SetRXByte);
	AT91F_US_EnableIt(AT91C_BASE_US0,AT91C_US_RXRDY);
}

char cFlibUart_SendBuff( unsigned char *pucBuffOut , unsigned int uiLength)
{
	vFdvUart_WaitTXFree();
	if( pucBuffOut !=0 && uiLength )
		mSEND_BUFF( pucBuffOut , uiLength );
	vFdvUart_WaitTXFree();
	return 0;
}

char cflibUart_SendReady( void )
{
	return 0;
}

void vFlibUart_SetRXByte( unsigned char *pu8Byte)
{
	unsigned short uiIndex;
	
	uiIndex = tLibUartData.uiIndexIn+1;
	
	if( uiIndex >= kLIBUART_INPUT_BUFFER_SIZE )
		uiIndex = 0;
	if( tLibUartData.uiIndexOut != uiIndex )
	{
		tLibUartData.tucInputBuffer[ tLibUartData.uiIndexIn ] = *pu8Byte;
		tLibUartData.uiIndexIn = uiIndex;
		tLibUartData.uiByteCount++;
	}

}

char cFlibUart_GetRXByte( unsigned char *pu8Byte)
{
	if(tLibUartData.uiByteCount)
	{
		mDISABLE_UART_RX_IT();
		if( tLibUartData.uiIndexOut != tLibUartData.uiIndexIn )
		{
			*pu8Byte = tLibUartData.tucInputBuffer[ tLibUartData.uiIndexOut ];
			tLibUartData.uiIndexOut++;
			if( tLibUartData.uiIndexOut >= kLIBUART_INPUT_BUFFER_SIZE)
				tLibUartData.uiIndexOut = 0;
			tLibUartData.uiByteCount--;
			mENABLE_UART_RX_IT();
			return 1;
		}
		mENABLE_UART_RX_IT();
	}
	return 0;
}
