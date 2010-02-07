#ifdef kUSE_IP_STACK
#include "lib/libIpStack/uip/uip.h"
#include "lib/libIpStack/uip/uip_arp.h"
#include "lib/libSlip/slipdev.h"
#include "lib/libIpStack/uip/timer.h"
#include "lib/libHttpd/httpd.h"
#include "dv/dvTimer/dvTimer.h"


static   int i;
static   uip_ipaddr_t ipaddr;
static   struct timer periodic_timer;
static 	unsigned int uiAppIpStackState;

/*---------------------------------------------------------------------------*/

void vFappIpStack_Init( void )
{
	timer_set(&periodic_timer, CLOCK_SECOND / 2);
	  
	  slipdev_init();
	  uip_init();

	  uip_ipaddr(ipaddr, 192,168,0,2);
	  uip_sethostaddr(ipaddr);

	  //telnetd_init();
	  httpd_init();
	  uiAppIpStackState=0;
}

void vFappIpStack(void)
{
  
  switch(uiAppIpStackState)
  {
  	default:
  		uiAppIpStackState=0;
  	case 0: //check input frame
  		vFlibSlip_Read_Init();
  		uiAppIpStackState=1;
  	case 1:
  		//dvTimerwait(2);
  		if( iFlibSlip_Read() )
  		{	//new input frame
	  		if(uip_len > 0) 
	  			uiAppIpStackState=3;
	  		else
	  			uiAppIpStackState=2;
	  		break;
  		}
  		break; //wait for thr end of frame
  	case 2: //check periodic timer
  		if(timer_expired(&periodic_timer)) 
  		{
  			i=0;
  		  	timer_reset(&periodic_timer);
  		  	uiAppIpStackState=4;
  		}
  		else
  			uiAppIpStackState=0;
  		break;
  	case 3://read input frame
  		uip_input();
  		if(uip_len > 0)
  			slipdev_send();
  		uiAppIpStackState=0;
  		break;
  	case 4: //
  		if( i < UIP_CONNS )
  		{
  			i++;
  			uip_periodic(i);
  			/* If the above function invocation resulted in data that
  			should be sent out on the network, the global variable
  			uip_len is set to a value > 0. */
 			if(uip_len > 0) 
		  		slipdev_send();
  		}
  		else
  			uiAppIpStackState=0;	
  		break;
  }
	
/*		uip_len = slipdev_read();
		if(uip_len > 0) 
		{
			uip_input();
			if(uip_len > 0) 
			{
				slipdev_send();
			}
		} 
		else if(timer_expired(&periodic_timer)) 
		{
			timer_reset(&periodic_timer);
			for(i = 0; i < UIP_CONNS; i++) 
			{
				uip_periodic(i);
				if(uip_len > 0) 
				{
					slipdev_send();
				}
			}
		}*/
}
/*---------------------------------------------------------------------------*/
#endif
