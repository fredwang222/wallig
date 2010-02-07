#ifndef LIBFATCONFIG_H_
#define LIBFATCONFIG_H_

#define CARD_WP_PIN      AT91C_PIO_PA16
#define CARD_INS_PIN     AT91C_PIO_PA15
#define CARD_SELECT_PIN  AT91C_PA11_NPCS0
/* Control signals (Platform dependent) */
#define SELECT()	(AT91C_BASE_PIOA->PIO_CODR = CARD_SELECT_PIN) /* MMC CS = L */
#define	DESELECT()	(AT91C_BASE_PIOA->PIO_SODR = CARD_SELECT_PIN) /* MMC CS = H */
#define SOCKWP		CARD_WP_PIN			/* Write protect switch (PB5) */
#define SOCKINS		CARD_INS_PIN		/* Card detect switch (PB4) */
#define POWER_ON()
#define POWER_OFF()

#endif /*LIBFATCONFIG_H_*/
