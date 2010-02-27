#include <stdlib.h>
#include <string.h>
#include "DRV_Uart.h"
#include "DRV_SoftTimer.h"
#include "uip.h"
#include "LIB_Trace.h"

#define kETAPE_Init 				0
#define kETAPE_Wait_Input_Packet 		1
#define kETAPE_Periodic_Uip				2
#define kETAPE_Periodic_Uip_Send		3
#define kETAPE_Periodic_Udp_Uip			4
#define kETAPE_Periodic_Udp_Uip_Send	5
#define kETAPE_Packet_Send				6

static struct
{
	int iEtape;
	int iPrevious;
	int iConnectionCount;
	DRV_Uart_Handle  hUart;
	DRV_SoftTimer_Handle hTimer;
	uip_ipaddr_t ipaddr;
}IpStack_Thread_Data;

void IpStack_Thread_init( void )
{
	DRV_Uart_Error eUart_Error;
	DRV_Uart_Cfg tUart0Settings;
	DRV_SoftTimer_Error eSofTimerError;
	DRV_SoftTimer_Cfg sParam;
	uip_ipaddr_t addr;
	 struct uip_udp_conn *c;

	IpStack_Thread_Data.iEtape =  kETAPE_Init;

	//Uart open
	tUart0Settings.RXCallBack=NULL;
	tUart0Settings.eBaudRate = BR115200;
	tUart0Settings.cEndOfBuff='\r';
	tUart0Settings.eParity = PARNone;
	tUart0Settings.eSLIPModeEnable=1;
	tUart0Settings.eStopBit = SB_1;
	tUart0Settings.iRXNbChar = 0;
	eUart_Error = DRV_Uart_Open("/dev/ttyUSB0",&IpStack_Thread_Data.hUart,&tUart0Settings);

	//Soft Timer open
    sParam.tType=TimeOut;
    sParam.uiValue = 10;
    eSofTimerError = DRV_SoftTimer_Open("timer1PStack",&IpStack_Thread_Data.hTimer,&sParam);
    DRV_SoftTimer_Start(IpStack_Thread_Data.hTimer);

    //uiP Stack init
    uip_init();
	uip_ipaddr(IpStack_Thread_Data.ipaddr, 192,168,2,3);
	uip_sethostaddr(IpStack_Thread_Data.ipaddr);

	//telnetd_init();
	httpd_init();
	hello_world_init();
	telnetd_init();

	uip_ipaddr(&addr, 192,168,2,1);
	 c = uip_udp_new(&addr, HTONS(12345));
	 LIB_Trace_Printf(1,"IpStack init Done\n");

}



int IpStack_Thread_Call( void )
{
	static int iDataLen=0;

	switch( IpStack_Thread_Data.iEtape )
	{
		default:
			IpStack_Thread_Data.iEtape=kETAPE_Init;
		case kETAPE_Init:
			IpStack_Thread_Data.iEtape = kETAPE_Wait_Input_Packet;
			break;
		case kETAPE_Wait_Input_Packet:
			if(DRV_Uart_RXDataReceived(IpStack_Thread_Data.hUart))
			{
				iDataLen=UIP_CONF_BUFFER_SIZE;
				DRV_Uart_Receive(IpStack_Thread_Data.hUart ,uip_buf , &iDataLen);
				uip_len = iDataLen;
				uip_input();
				 /* If the above function invocation resulted in data that
					 should be sent out on the network, the global variable
					 uip_len is set to a value > 0. */
				IpStack_Thread_Data.iPrevious =IpStack_Thread_Data.iEtape;
				IpStack_Thread_Data.iEtape=kETAPE_Packet_Send;
			}
			else
			{
				if( !DRV_SoftTimer_GetValue(IpStack_Thread_Data.hTimer))
				{
					DRV_SoftTimer_Reset(IpStack_Thread_Data.hTimer);
				  IpStack_Thread_Data.iEtape=kETAPE_Periodic_Uip;
				  IpStack_Thread_Data.iConnectionCount=0;
				}
			}
			break;
		case kETAPE_Periodic_Uip:
			for(;IpStack_Thread_Data.iConnectionCount<UIP_CONNS;IpStack_Thread_Data.iConnectionCount++)
			{
				uip_periodic(IpStack_Thread_Data.iConnectionCount++);
				/* If the above function invocation resulted in data that
				   should be sent out on the network, the global variable
				   uip_len is set to a value > 0. */
				if(uip_len > 0)
				{
					IpStack_Thread_Data.iPrevious =IpStack_Thread_Data.iEtape;
					IpStack_Thread_Data.iEtape = kETAPE_Packet_Send;
					break;
				}
			}
			if(IpStack_Thread_Data.iConnectionCount>=UIP_CONNS)
			{
#if UIP_CONF_UDP
				IpStack_Thread_Data.iEtape = kETAPE_Periodic_Udp_Uip;
#else
				IpStack_Thread_Data.iEtape = kETAPE_Wait_Input_Packet;
#endif
				IpStack_Thread_Data.iConnectionCount=0;
			}
			break;
#if UIP_CONF_UDP
		case kETAPE_Periodic_Udp_Uip:
			for(;IpStack_Thread_Data.iConnectionCount<UIP_CONNS;IpStack_Thread_Data.iConnectionCount++)
			{
				uip_udp_periodic(IpStack_Thread_Data.iConnectionCount);
				/* If the above function invocation resulted in data that
				   should be sent out on the network, the global variable
				   uip_len is set to a value > 0. */
				if(uip_len > 0)
				{
					IpStack_Thread_Data.iPrevious =IpStack_Thread_Data.iEtape;
					IpStack_Thread_Data.iEtape = kETAPE_Packet_Send;
					break;
				}
			}
			if(IpStack_Thread_Data.iConnectionCount>=UIP_CONNS)
			{
				IpStack_Thread_Data.iEtape = kETAPE_Wait_Input_Packet;
			}
			break;
#endif
		case kETAPE_Packet_Send:
			if( !DRV_Uart_TXBusy(IpStack_Thread_Data.hUart))
			{
				DRV_Uart_Send(IpStack_Thread_Data.hUart , uip_buf , uip_len);
				IpStack_Thread_Data.iEtape=IpStack_Thread_Data.iPrevious;
			}
			break;

	}

	return 1;
}

void  uip_log( char *msg )
{
	LIB_Trace_Printf(1,msg);
}

void uip_router_tcp_appcall(void)
{

  switch(uip_conn->lport)
  {
	case HTONS(1000):
		hello_world_appcall();
		break;
	case HTONS(80):
		httpd_appcall();
		break;
	case HTONS(23):
		telnetd_appcall();
		break;

  }
}
void uip_router_udp_APPCALL( void )
{
	LIB_Trace_Udp_APPCALL(uip_udp_conn->rport);

}
