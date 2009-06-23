#ifndef LIBTIMER_H_
#define LIBTIMER_H_

typedef struct 
{
	int iHanler;
	unsigned int uiPeriod;
	unsigned int uiPeriodCount;
	unsigned int uiCount;
} tPeriodicTimer;

extern unsigned int  vFlibTimer_GetCount(tPeriodicTimer *);
extern void vFlibTimer_DeletPeriodicTimer( tPeriodicTimer *);
extern tPeriodicTimer *ptFlibTimer_NewPeriodicTimer( unsigned int );
extern void vFlibTimer_Tick(void);
extern void vFlibTimer_Iinit( void );

#endif /*LIBTIMER_H_*/
