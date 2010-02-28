/**
	\file LIB_Trace.c

	\author Gwendal Le Gall

	\date 27/02/2010
*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "LIB_Trace.h"

/**************************************************************
					Define
***************************************************************/

/**************************************************************
					Typedef
***************************************************************/
typedef struct
{
	int iLen;
	char tcBuffer[kTRACE_MAX_MESSAGE_LENGTH];
} LIB_TRACE_Message;

/**************************************************************
					Local variables
***************************************************************/
struct
{
	struct pt  TraceThead;
	LIB_TRACE_Message tMessages[kTRACE_MAX_MESSAGE_COUNT];
	int iMessageInBufferCount;
	int iNextMessageToSend;
	int iDebugLevel;
#if kTRACE_UDP
	struct uip_udp_conn *pUDP_Conn;
	LIB_TRACE_Message tTRACE_UDP_Message;
	int iRemotePort;
#endif
#if kTRACE_SERIAL
	DRV_Uart_Handle hTraceUart;
#endif
} tTraceData;

/**************************************************************
                 local Functions declaration
***************************************************************/

/**************************************************************
		 Public Functions
***************************************************************/
void LIBTrace_Init( int iDebugLevel , LIB_Trace_InitPAram *pInitParam)
{
#if kTRACE_UDP
	uip_ipaddr_t addr;
#endif
	PT_INIT(&tTraceData.TraceThead);
	memset(tTraceData.tMessages,0,sizeof(tTraceData.tMessages));
	tTraceData.iDebugLevel=iDebugLevel;
	tTraceData.iMessageInBufferCount=0;
	tTraceData.iNextMessageToSend=0;
#if kTRACE_UDP
	uip_ipaddr(&addr, pInitParam->tRemoteAddress[3] ,pInitParam->tRemoteAddress[2],pInitParam->tRemoteAddress[1],pInitParam->tRemoteAddress[0]);
	tTraceData.pUDP_Conn = uip_udp_new(&addr, HTONS(pInitParam->iRemotePort));
	tTraceData.iRemotePort = pInitParam->iRemotePort;
#endif
#if kTRACE_SERIAL

#endif
	LIB_Trace_Printf(1,"[Trace] Init Ok");
}

PT_THREAD ( LIBTrace_Thread( void ))
{

	PT_BEGIN(&tTraceData.TraceThead);
	PT_WAIT_UNTIL(&tTraceData.TraceThead, tTraceData.iMessageInBufferCount);
#if kTRACE_UDP
	memcpy( &tTraceData.tTRACE_UDP_Message , &tTraceData.tMessages[tTraceData.iNextMessageToSend] , sizeof(LIB_TRACE_Message));
#endif
#if kTRACE_SERIAL
	PT_WAIT_WHILE(&tTraceData.TraceThead,  DRV_Uart_TXBusy( tTraceData.hTraceUart ))
	DRV_Uart_Send(tTraceData.hTraceUart , tTraceData.tMessages.tcBuffer[tTraceData.iNextMessageToSend] , tTraceData.tMessages[tTraceData.iNextMessageToSend].iLen);
#endif
#if kTRACE_UDP
	PT_WAIT_WHILE(&tTraceData.TraceThead, tTraceData.tTRACE_UDP_Message.iLen );
#endif
	tTraceData.iMessageInBufferCount--;
	if(++tTraceData.iNextMessageToSend >= kTRACE_MAX_MESSAGE_COUNT )
		tTraceData.iNextMessageToSend=0;
	PT_RESTART(&tTraceData.TraceThead);
	PT_END(&tTraceData.TraceThead);
}

void LIB_Trace_Printf( int iLevel , char *pcFormat, ... )
{
	int iMessageIndex;
	LIB_TRACE_Message *pMessage;
	va_list args;
	va_start (args, pcFormat);

	if( iLevel <= tTraceData.iDebugLevel && tTraceData.iMessageInBufferCount < kTRACE_MAX_MESSAGE_COUNT)
	{
		iMessageIndex = tTraceData.iNextMessageToSend+tTraceData.iMessageInBufferCount;
		if( iMessageIndex >= kTRACE_MAX_MESSAGE_COUNT )
			iMessageIndex-=kTRACE_MAX_MESSAGE_COUNT;
		pMessage = &tTraceData.tMessages[iMessageIndex];
		vsnprintf ( pMessage->tcBuffer , kTRACE_MAX_MESSAGE_LENGTH ,pcFormat, args);
		pMessage->tcBuffer[kTRACE_MAX_MESSAGE_LENGTH-1]=0;
		pMessage->iLen = strlen(pMessage->tcBuffer);
		if( pMessage->iLen > (kTRACE_MAX_MESSAGE_LENGTH-1))
			pMessage->iLen--;
		pMessage->tcBuffer[pMessage->iLen-1]='\n';
		pMessage->tcBuffer[pMessage->iLen]=0;
		pMessage->iLen++;
		tTraceData.iMessageInBufferCount++;
	}

	va_end (args);

}
void LIB_Trace_Udp_APPCALL( u16_t rPort)
{
	if( rPort == HTONS(tTraceData.iRemotePort))
	{
		if(uip_poll())
		{
			if( tTraceData.tTRACE_UDP_Message.iLen)
			{
				memcpy( uip_appdata , tTraceData.tTRACE_UDP_Message.tcBuffer , tTraceData.tTRACE_UDP_Message.iLen);
				uip_udp_send(tTraceData.tTRACE_UDP_Message.iLen);
				tTraceData.tTRACE_UDP_Message.iLen=0;
			}
		}
	}
}
