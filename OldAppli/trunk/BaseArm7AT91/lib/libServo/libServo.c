
#define kPIO_SERVO AT91C_PIO_PA23

void vFlibServo_Init( void )
{
	//PIO configuration
	//configure pio as output
	/*AT91F_PIO_CfgOutput( AT91C_BASE_PIOA, AT91C_PIO_PA23);
	//open drain
	AT91C_BASE_PIOA->PIO_MDER = AT91C_PIO_PA23;
	//disable pull up
	AT91C_BASE_PIOA->PIO_PUDR = AT91C_PIO_PA23;
	//low level at start up
	AT91C_BASE_PIOA->PIO_CODR= AT91C_PIO_PA23;*/
	//Fast timerconfig
}
