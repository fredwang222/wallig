#include "libIpStack/uip/uip.h"
#include "libUart/libUart.h"
#include "dvUart/dvUart.h"
// sudo slattach -p slip -L /dev/ttyUSB1  -d -v -s 115200
// sudo ifconfig sl0 192.168.0.1 pointopoint 192.168.0.2 up


#define SLIP_END     0300  
#define SLIP_ESC     0333  
#define SLIP_ESC_END 0334  
#define SLIP_ESC_ESC 0335  
  
  
#define SIO_RECV(c)  c=uart0_getc() 
#define SIO_SEND(c) uart0_putc(c)  
  
#define MAX_SIZE UIP_BUFSIZE  
  
static const unsigned char slip_end = SLIP_END,   
                           slip_esc = SLIP_ESC,   
                           slip_esc_end = SLIP_ESC_END,   
                           slip_esc_esc = SLIP_ESC_ESC;  
  
static unsigned char tucSlipOutputBuff[UIP_BUFSIZE];

/*-----------------------------------------------------------------------------------*/  
void  
slipdev_send(void)  
{  
  /*u8_t i;  
  u8_t *ptr;  
  u8_t c;  
 */ 
 unsigned int uiIndexIn=0;
  unsigned int uiIndexOut=0;
 
  tucSlipOutputBuff[ uiIndexOut++ ]= SLIP_END;
  for ( uiIndexIn = 0; uiIndexIn < uip_len ; uiIndexIn++)
  {
	  switch( uip_buf[uiIndexIn] )
	  {
	  	case SLIP_END:
		  tucSlipOutputBuff[ uiIndexOut++ ]= SLIP_ESC;
		  tucSlipOutputBuff[ uiIndexOut++ ]= SLIP_ESC_END;
		  break;
	  	case SLIP_ESC:
		  tucSlipOutputBuff[ uiIndexOut++ ]= SLIP_ESC;
		  tucSlipOutputBuff[ uiIndexOut++ ]= SLIP_ESC_ESC;
		  break;  
		default:
			tucSlipOutputBuff[ uiIndexOut++ ]= uip_buf[uiIndexIn];
	  }
  }
  tucSlipOutputBuff[ uiIndexOut++ ] = SLIP_END;
  cFlibUart_SendBuff( tucSlipOutputBuff , uiIndexOut );

  
  /*SIO_SEND(slip_end);  
  
  ptr = uip_buf;  
  for(i = 0; i < uip_len; i++) {  
    c = *ptr++;  
    switch(c) {  
    case SLIP_END:  
      SIO_SEND(slip_esc);  
      SIO_SEND(slip_esc_end);  
      break;  
    case SLIP_ESC:  
      SIO_SEND(slip_esc);  
      SIO_SEND(slip_esc_esc);  
      break;  
    default:  
      SIO_SEND(c);  
      break;  
    }  
  }  
  SIO_SEND(slip_end);  */
}  
/*-----------------------------------------------------------------------------------*/

static unsigned int uiLibSlip_Read_State;
static unsigned char ucIn;

void vFlibSlip_Read_Init( void )
{
	uip_len = 0;
	uiLibSlip_Read_State = 0;
}

int iFlibSlip_Read( void )
{
	switch(uiLibSlip_Read_State)
	{
		default:
			uiLibSlip_Read_State=0;
		
		case 0:
			if(uip_len >= MAX_SIZE)
				uip_len=0;
			if( !cFlibUart_GetRXByte(&ucIn) )
				break;
			else
				uiLibSlip_Read_State=1;
		case 1:
			switch(ucIn) 
			{
				case SLIP_END:
					uiLibSlip_Read_State=0;
					if(uip_len > 0)
				    	return 1;//new input frame
		   			break;
				case SLIP_ESC:
					uiLibSlip_Read_State=2; //wait next char 
					break;
				default:
					uiLibSlip_Read_State=3; //store char
					break;
			}
			break;
		case 2:
			if( !cFlibUart_GetRXByte(&ucIn) )
				break;
			else
			{
				switch(ucIn) 
	   			{  
	   				case SLIP_ESC_END:  
	   					ucIn = SLIP_END;  
	   					break;  
	   				case SLIP_ESC_ESC:  
	   					ucIn = SLIP_ESC;  
	   					break;  
	   			}
				uiLibSlip_Read_State=3; //store char
			}
		case 3:
			if(uip_len < MAX_SIZE) 
			{  
			    uip_buf[uip_len] = ucIn;  
			    uip_len++; 
			} 
			uiLibSlip_Read_State=0;
			break;
	}
	return 0;
}


u8_t slipdev_read(void)  
{  
  u8_t c;  
  
 start:  
  uip_len = 0;  
  while(1) 
  {  
    if(uip_len >= MAX_SIZE) {  
      goto start;  
    }  
    SIO_RECV(c);  
    switch(c) 
    {  
   		case SLIP_END:  
   			if(uip_len > 0) 
   			{  
   				return uip_len;  
   			} 
   			else 
   			{  
   				goto start;  
   			}  
   			break;  
   		case SLIP_ESC:  
   			SIO_RECV(c);  
   			switch(c) 
   			{  
   				case SLIP_ESC_END:  
   					c = SLIP_END;  
   					break;  
   				case SLIP_ESC_ESC:  
   					c = SLIP_ESC;  
   					break;  
   			}  
   		/* FALLTHROUGH */  
    	default:  
    		if(uip_len < MAX_SIZE) 
    		{  
    			uip_buf[uip_len] = c;  
    			uip_len++;  
    		}  
    		break;  
    }  
      
  }  
  return 0;  
}  
/*-----------------------------------------------------------------------------------*/  
void  
slipdev_init(void)  
{  
}  

void
uip_log(char *m)
{
 
}

/*-----------------------------------------------------------------------------------*/  
