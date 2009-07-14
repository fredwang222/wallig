#include "sys/AT91SAM7S64.h"                    /* AT91SAMT7S64 definitions */
#include "sys/lib_AT91SAM7S64.h"                    /* AT91SAMT7S64 definitions */
#include "sys/Board.h"

#define PWM_DUTY_MAX 1024

/*void vFdvPwm_Start( void )
{
	
}*/

void vFdvPWM_init( void )
{
	// enable peripheral clock for PWM( PID Bit 10 )
	AT91F_PWMC_CfgPMC();
	// disable PIO from PWM0
	AT91F_PIO_CfgPeriph(AT91C_BASE_PIOA,AT91C_PA0_PWM0,0);
	// Initially they're stopped
	AT91F_PWMC_StopChannel( AT91C_BASE_PWMC , AT91C_PWMC_CHID0);
	// Set the Clock A divider
	AT91C_BASE_PWMC->PWMC_MR = (( 4 << 8 ) | 0x02 );  // MCK selection or'ed with Divider
	//AT91C_BASE_PWMC->PWMC_MR = 0; //don't use clock A & B
	  // Set the Clock
	AT91C_BASE_PWMC->PWMC_CH[ 0 ].PWMC_CMR =AT91C_PWMC_CPRE_MCKA |AT91C_PWMC_CPOL;     //Divider Clock A | Channel Polarity Invert
	//AT91C_BASE_PWMC->PWMC_CH[ 0 ].PWMC_CMR = 0xa;     //Use MCK/1024
	//AT91C_BASE_PWMC->PWMC_CH[ 0 ].PWMC_CMR =0x8|AT91C_PWMC_CPOL;     //Divider Clock A | Channel Polarity Invert
    // Set the Period register (sample size bit fied )
	AT91C_BASE_PWMC->PWMC_CH[ 0 ].PWMC_CPRDR = 1024; //
	// Set the duty cycle register (output value)
	AT91C_BASE_PWMC->PWMC_CH[ 0 ].PWMC_CDTYR = 256; //50%
	// Initialise the Update register write only
	AT91C_BASE_PWMC->PWMC_CH[ 0 ].PWMC_CUPDR = 256 ;
	//Enable PWM0
	AT91C_BASE_PWMC->PWMC_ENA =  AT91C_PWMC_CHID0;

	// disable PIO from PWM1
		AT91F_PIO_CfgPeriph(AT91C_BASE_PIOA,AT91C_PA1_PWM1,0);
		// Initially they're stopped
		AT91F_PWMC_StopChannel( AT91C_BASE_PWMC , AT91C_PWMC_CHID1);
	  // Set the Clock
		AT91C_BASE_PWMC->PWMC_CH[ 1 ].PWMC_CMR =AT91C_PWMC_CPRE_MCKA |AT91C_PWMC_CPOL;     //Divider Clock A | Channel Polarity Invert
	    // Set the Period register (sample size bit fied )
		AT91C_BASE_PWMC->PWMC_CH[ 1 ].PWMC_CPRDR = 1024; //
		// Set the duty cycle register (output value)
		AT91C_BASE_PWMC->PWMC_CH[ 1 ].PWMC_CDTYR = 256; //50%
		// Initialise the Update register write only
		AT91C_BASE_PWMC->PWMC_CH[ 1 ].PWMC_CUPDR = 256 ;
		//Enable PWM0
		AT91C_BASE_PWMC->PWMC_ENA =  AT91C_PWMC_CHID1;


}
