#include <stdlib.h>
#include <stdio.h>                          /* I/O Functions */
#include <string.h>
#include "sys/AT91SAM7S64.h"                    /* AT91SAMT7S64 definitions */
#include "sys/lib_AT91SAM7S64.h"                    /* AT91SAMT7S64 definitions */

#include "dv/dvTimer/dvTimer.h"
#include "dv/dvDBGU/dvDBGU.h"
#include "lib/libTime/libTimer.h"
#include "lib/libUart/libUart.h"
#include "lib/libFat/ff.h"


void vFPrintString( const char *pucString)
{
	cFlibUart_SendBuff((unsigned char*)pucString,strlen((const char*)pucString));

}

unsigned char tucBufferLecture[512];



void InitADC(void) {

	AT91C_BASE_PMC->PMC_PCER = 1 << AT91C_ID_ADC;
	AT91C_BASE_ADC->ADC_CR = 0x2;   // set Start Bit
	AT91C_BASE_ADC->ADC_MR = 0x3f00;   // set mode register, 10bit chanel, MCK/128
	AT91C_BASE_ADC->ADC_CHER = AT91C_ADC_CH4|AT91C_ADC_CH7; // enable chanel 4 and 7

}


unsigned int GetValue_chanel4() {
	AT91C_BASE_ADC->ADC_CR = 0x2; // set Start Bit
  while(!(AT91C_BASE_ADC->ADC_SR&0x10)); //wait until conversion complete
  return AT91C_BASE_ADC->ADC_CDR4;
}

unsigned int GetValue_chanel5() {
	AT91C_BASE_ADC->ADC_CR = 0x2; // set Start Bit
  while(!(AT91C_BASE_ADC->ADC_SR&0x20)); //wait until conversion complete
  return AT91C_BASE_ADC->ADC_CDR5;
}


void vFappTest_init( void )
{
	InitADC();
	DBGU_Configure();

}

void vFappTest( void )
{
	unsigned int uiAdc,uiAdcPred=0xffff,uiGyro;
	int iCorrection;
	char tcBuffOut[32];
	tPeriodicTimer *tTImer1;

	DBGU_PutString("debug\n");

	tTImer1 = ptFlibTimer_NewPeriodicTimer(1000);
	while(1)
	{
		AT91F_ADC_StartConversion((AT91PS_ADC)AT91C_BASE_ADC);
		if(AT91F_ADC_GetStatus((AT91PS_ADC)AT91C_BASE_ADC)&(AT91C_ADC_EOC4|AT91C_ADC_EOC7))
		{

			uiAdc=AT91F_ADC_GetConvertedDataCH4((AT91PS_ADC)AT91C_BASE_ADC);
			uiGyro=AT91F_ADC_GetConvertedDataCH7((AT91PS_ADC)AT91C_BASE_ADC);
			// Initialize the Update register write only
			if ( (uiAdc&0xfff0) != (uiAdcPred&0xfff0) )
			{
				uiAdcPred = uiAdc;
				iCorrection = 512 - uiAdc;
				iCorrection /=4;
				AT91F_PWMC_UpdateChannel( AT91C_BASE_PWMC , 0 , (unsigned int)((int)512-iCorrection));
				AT91F_PWMC_UpdateChannel( AT91C_BASE_PWMC , 1 , (unsigned int)((int)512+iCorrection));
				sprintf(tcBuffOut,"Dac Value: 0x%X 0x%X\n",uiAdc,uiGyro);
				DBGU_PutString(tcBuffOut);
			}
		}
		if( vFlibTimer_GetCount(tTImer1))
		{
			DBGU_PutString("TicTac\n");
		}

	}
		;
}
