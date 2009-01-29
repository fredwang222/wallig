/*
 * Copyright (c) 2006, Ari Suutari, ari@suutari.iki.fi.
 * Copyright (c) 2004, Jun Li, lj_sourceforge@users.sourceforge.net.
 * Copyright (c) 2004, Dennis Kuschel. All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer. 2. Redistributions
 * in binary form must reproduce the above copyright notice, this list of
 * conditions and the following disclaimer in the documentation and/or other
 * materials provided with the distribution. 3. The name of the author may
 * not be used to endorse or promote products derived from this software
 * without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,  INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 */

/*
 * This file is originally from the pico]OS realtime operating system
 * (http://picoos.sourceforge.net).
 * 
 * CVS-ID $Id: cpu_c.c,v 1.1 2006/04/29 15:30:30 dkuschel Exp $
 */

#define NANOINTERNAL
#include "pos_nano.h"
#include <picoos.h>
//#include "lpc_reg.h"

static void	armCpuTimerIrqHandler(void);

#if NOSCFG_FEATURE_CONOUT == 1 || NOSCFG_FEATURE_CONIN == 1
static void	armCpuUartIrqHandler(void);
#endif

/*
 * Initialize CPU pins, clock and console.
 */

void
p_pos_initArch(void)
{
#if NOSCFG_FEATURE_CONOUT == 1 || NOSCFG_FEATURE_CONIN == 1
  unsigned short consoleDll;
#endif

  vFsysManager_Init( );
  
#if NOSCFG_FEATURE_CONOUT == 1 || NOSCFG_FEATURE_CONIN == 1

  vFdvUart0_init();                             /* Initialize Serial Interface */

#endif

/*
 * Configure timer so that it ticks every 1 ms.
 */

  T0_PR = 0x0;

  T0_MR0 = CRYSTAL_CLOCK / HZ;
  T0_MCR = 0x2 | 0x1;		/* Reset & interrupt on match */

/*
 * Configure VIC to handle timer interrupts.
 */

  VICIntSelect &= ~(0x10);
  VICIntEnable |= 0x10;
  VICVectAddr0 = (unsigned long)armCpuTimerIrqHandler;
  VICVectCntl0 = 0x4 | 0x20;

/* 
 * Start timer !
 */

  T0_TCR = 1;
}

/*
 * Timer interrupt handler. Just call generic pico]OS timer function
 * and enable the timer again.
 */

static void
armCpuTimerIrqHandler()
{
  c_pos_timerInterrupt();
  T0_IR = 1;
}

void armSetDefaultIrqHandler(ArmIrqHandlerFunction func)
{
  VICDefVectAddr = (unsigned long)func;
}

#if NOSCFG_FEATURE_CONOUT == 1 || NOSCFG_FEATURE_CONIN == 1

/*
 * Uart interrupt handler.
 */

static void
armCpuUartIrqHandler()
{
  int reason;
  unsigned char ch;

  reason = UART0_IIR & 0xf;
  switch (reason) {
  case 0x2: // TX
#if NOSCFG_FEATURE_CONOUT == 1
    c_nos_putcharReady();
#endif
    break;

  case 0x4: // RX
    ch = UART0_RBR;
#if NOSCFG_FEATURE_CONIN == 1
    c_nos_keyinput(ch);
#endif
    break;

  case 0x6: // err
    ch= UART0_LSR;
    break;
  }
}

#endif

#if NOSCFG_FEATURE_CONOUT == 1
/*
 * Console output.
 */

UVAR_t
p_putchar(char c)
{
  if (!(UART0_LSR & 0x20))
    return 0;

  UART0_THR = c;
  return 1;
}
#endif
