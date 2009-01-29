#ifndef serial_h_
#define serial_h_

extern void vFdvUart_init (void);
extern void vFdvUart_SetRXCallback( void (*callback)(unsigned char*));
extern void vFdvUart_WaitTXFree(void);
int uart0_putc(int ch); 
int uart0_putchar (int ch); /* replaces \n with \r\n */

int uart0_puts   ( char *s ); /* uses putc */
int uart0_prints ( char* s ); /* uses putchar */

int iFdvUart_kbhit( void );
int uart0_getc ( void );


unsigned char ucFdvUart_GetByte(void);

#endif
