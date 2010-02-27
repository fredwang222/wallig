/**
*	\file DRV_Uart.h
*
*	\author Gwendal Le Gall
*
*	\date 02/02/2010
*	\brief Lib Trace
*
*/
/*!
 * \if standalone_LIB_Trace
*	\mainpage Trace Library
*    \else
*    \page LIBTrace Trace Library
*    \endif
*   \author Gwendal Le Gall
*
*	\date 27/02/2010
*
*
*/
#include "LIB_Trace_CFG.h"
#include "pt.h"
#if kTRACE_UDP
#include "uip.h"
#endif

typedef struct
{
#if kTRACE_UDP
	char tRemoteAddress[4];
	int iRemotePort;
#endif
#if kTRACE_SERIAL
	DRV_Uart_Cfg tUartCfg;
	char *pcUartNAme;
#endif
} LIB_Trace_InitPAram;

void LIBTrace_Init( int iDebugLevel , LIB_Trace_InitPAram *pInitParam);
void LIB_Trace_Printf( int iLevel , char *pcFormat, ... );
PT_THREAD ( LIBTrace_Thread( void ));
#if kTRACE_UDP
void LIB_Trace_Udp_APPCALL( u16_t rPort);
#endif

