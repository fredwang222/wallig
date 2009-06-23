#include "sys/AT91SAM7S64.h"                    /* AT91SAMT7S64 definitions */
#include "sys/lib_AT91SAM7S64.h"                    /* AT91SAMT7S64 definitions */
#include "sys/Board.h"

#define PWM_DUTY_MAX 1024

/*void vFdvPwm_Start( void )
{
	
}*/

void vFdvPWM_init( void )
{
	// disable PIO from PWM0
	//AT91F_PIO_CfgPeriph(AT91C_BASE_PIOA,AT91C_PA23_PWM0,0);
	//AT91F_PWMC_CH0_CfgPIO();
	
	AT91C_BASE_PIOA->PIO_PDR=AT91C_PA23_PWM0;
	AT91C_BASE_PIOA->PIO_BSR=AT91C_PA23_PWM0;
	// enable peripheral clock for PWM( PID Bit 10 )
	AT91F_PWMC_CfgPMC();
	// Initially they're stopped
	AT91C_BASE_PWMC->PWMC_DIS =  AT91C_PWMC_CHID0;
	// Set the Clock A divider
	AT91C_BASE_PWMC->PWMC_MR = (( 4 << 8 ) | 0x08 );  // MCK selection or'ed with Divider
	//AT91C_BASE_PWMC->PWMC_MR = 0; //don't use clock A & B
	  // Set the Clock
	AT91C_BASE_PWMC->PWMC_CH[ 0 ].PWMC_CMR =AT91C_PWMC_CPRE_MCKA |AT91C_PWMC_CPOL;     //Divider Clock A | Channel Polarity Invert
	//AT91C_BASE_PWMC->PWMC_CH[ 0 ].PWMC_CMR = 0xa;     //Use MCK/1024
	//AT91C_BASE_PWMC->PWMC_CH[ 0 ].PWMC_CMR =0x8|AT91C_PWMC_CPOL;     //Divider Clock A | Channel Polarity Invert
    // Set the Period register (sample size bit fied )
	AT91C_BASE_PWMC->PWMC_CH[ 0 ].PWMC_CPRDR = 1024; //
	// Set the duty cycle register (output value)
	AT91C_BASE_PWMC->PWMC_CH[ 0 ].PWMC_CDTYR = 512; //50%
	// Initialise the Update register write only
	AT91C_BASE_PWMC->PWMC_CH[ 0 ].PWMC_CUPDR = 512 ;
	//Enable PWM0
	AT91C_BASE_PWMC->PWMC_ENA =  AT91C_PWMC_CHID0;

}
