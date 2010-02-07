#ifndef DVTIMER_H_
#define DVTIMER_H_

extern volatile unsigned long timeval;      /* Current Time Tick */

extern void vFdvTimer_init(void);             /* Initialize Timer */
extern void dvTimerwait(unsigned long );

#endif /*DVTIMER_H_*/
