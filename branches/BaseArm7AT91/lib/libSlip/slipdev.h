#ifndef SLIPDEV_H_
#define SLIPDEV_H_
extern u8_t  slipdev_read(void);
void  slipdev_send(void);
void  slipdev_init(void);
extern void vFlibSlip_Read_Init( void );
extern int iFlibSlip_Read( void );
#endif /*SLIPDEV_H_*/
