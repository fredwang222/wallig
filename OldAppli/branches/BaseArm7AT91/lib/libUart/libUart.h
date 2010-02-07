#ifndef LIBUART_H_
#define LIBUART_H_

extern void vFlibUart_Init( void );
extern char cFlibUart_SendBuff( unsigned char *, unsigned int );
extern char cflibUart_SendReady( void );
extern char cFlibUart_GetRXByte( unsigned char *);

#endif /*LIBUART_H_*/
