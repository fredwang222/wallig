#include <stdint.h>
#include <stdlib.h>
#include "LIB_Graph_Touch.h"
#include "DRV_Gpio.h"
#include "DRV_Timer.h"
#include "DRV_Spi.h"
#include "tft_lcd.h"

#define XMAX 400
#define RADIUS 3
#define XREF1 13
#define XREF2 (XMAX-13)
#define YMAX 240
#define YREF1 13
#define YREF2 (YMAX-13)

#define mTOUCH_GET_XPOS( XMes ) ((16*XMes)/tTouchData.tCalibration.Ax+tTouchData.tCalibration.Bx)
#define mTOUCH_GET_YPOS( YMes ) ((16*YMes)/tTouchData.tCalibration.Ay+tTouchData.tCalibration.By)

static struct
{
	DRV_Gpio_Handle hBusy;
	DRV_Gpio_Handle hCsn;
	DRV_Gpio_Handle hIrq;
	DRV_Spi_Handle hSpi;
	struct
	{
		int16_t Ax,Bx;
		int16_t Ay,By;
	} tCalibration;
	TouchEvent_t tLastEvent;
} tTouchData;


void SpiDelay(unsigned int DelayCnt);
u16 TPReadX(void);
u16 TPReadY(void);
static uint16_t Touch_MeasureX(void);
static uint16_t Touch_MeasureY(void);
static uint16_t Touch_IsPenDown( void );
void LIB_Graph_Touch_Init( void )
{

	DRV_Gpio_Open("TOUCH_BUSY" , &tTouchData.hBusy , NULL);
	DRV_Gpio_Open("TOUCH_CSN" , &tTouchData.hCsn , NULL);
	DRV_Gpio_Open("TOUCH_IRQ" , &tTouchData.hIrq , NULL);
	DRV_Spi_Open("SPI1",&tTouchData.hSpi);

}



unsigned int LIB_Graph_Calibrate( unsigned char *pucState )
{
	static int x,y;
	static 	int x1,x2,y1,y2;
	float fAx,fBx;
	float fAy,fBy;

	switch( *pucState )
	{
		case 0:
			LCD_SetBackColor(Blue);
			LCD_SetTextColor(Yellow);
			LCD_Clear(Blue);
			LCD_DrawCircle(XREF1-RADIUS,YREF1-RADIUS,RADIUS*2);
			(*pucState)++;
			break;
		case 1:
			if( !Touch_IsPenDown() )
				break;
			x1=Touch_MeasureX();
			y1=Touch_MeasureY();
			(*pucState)++;
			break;
		case 2:
			if( Touch_IsPenDown() )
				break;
			mDRV_TIME_DELAY(100);
			LCD_DrawCircle(XREF2-RADIUS,YREF1-RADIUS,RADIUS*2);
			(*pucState)++;
			break;
		case 3:
			if( !Touch_IsPenDown() )
				break;
			x2=Touch_MeasureX();
			y=Touch_MeasureY();
			y1=(y1+y)/2;
			(*pucState)++;
			break;
		case 4:
			if( Touch_IsPenDown() )
				break;
			mDRV_TIME_DELAY(100);
			LCD_DrawCircle(XREF1-RADIUS,YREF2-RADIUS,RADIUS*2);
			(*pucState)++;
			break;
		case 5:
			if( !Touch_IsPenDown() )
				break;
			x=Touch_MeasureX();
			y2=Touch_MeasureY();
			x1=(x1+x)/2;
			(*pucState)++;

			break;
		case 6:
			if( Touch_IsPenDown() )
				break;
			mDRV_TIME_DELAY(100);
			LCD_DrawCircle(XREF2-RADIUS,YREF2-RADIUS,RADIUS*2);
			(*pucState)++;
			break;
		case 7:
			if( !Touch_IsPenDown() )
				break;
			x=Touch_MeasureX();
			y=Touch_MeasureY();
			x2=(x2+x)/2;
			y2=(y2+y)/2;
			(*pucState)++;
			break;
		case 8:
			fAx=((float)XREF1-(float)XREF2)/((float)x1-(float)x2);
			fBx=(float)XREF1-(float)x1*(float)fAx;
			fAy=((float)YREF1-(float)YREF2)/((float)y1-(float)y2);
			fBy=(float)YREF1-(float)y1*(float)fAy;
			if( fAx != 0.0)
				tTouchData.tCalibration.Ax =(int16_t)(16.0/fAx+0.5);
			else
				tTouchData.tCalibration.Ax = 10;
			if( fAy != 0.0)
				tTouchData.tCalibration.Ay =(int16_t)(16.0/fAy+0.5);
			else
				tTouchData.tCalibration.Ay =10;
			tTouchData.tCalibration.Bx =(int16_t)fBx+0.5;
			tTouchData.tCalibration.By =(int16_t)fBy+0.5;
			return 1;
		default:
			return 1;
	}
	return 0;
}

void LIB_Graph_Event_Thread( void )
{
	static unsigned char ucState =0;
	static DRV_Time_TimeOut tPeriod;

	if( ucState > 0 )
	{
		if( mDRV_TIME_TIMEOUT_END(tPeriod))
		{
			mDRV_TIME_TIMEOUT_INIT(tPeriod,5);
		}
		else
			return;
	}
	switch( ucState )
	{
		case 0:
			tTouchData.tLastEvent.eType = eEvent_none;
			tTouchData.tLastEvent.usXpos = 0;
			tTouchData.tLastEvent.usXpos = 0;
			mDRV_TIME_TIMEOUT_INIT(tPeriod,5);
			ucState++;
			break;
		case 1: //wait press
			if( !Touch_IsPenDown() )
				break;
			/* Pen is down first so it 's press event*/
			tTouchData.tLastEvent.eType = eEvent_Pressed;
			tTouchData.tLastEvent.usXpos = mTOUCH_GET_XPOS(Touch_MeasureX());
			tTouchData.tLastEvent.usYpos = mTOUCH_GET_YPOS(Touch_MeasureY());
			ucState++;
		case 2: // wait released
			if( Touch_IsPenDown() )
			{
				tTouchData.tLastEvent.eType = eEvent_Pressed;
				tTouchData.tLastEvent.usXpos = mTOUCH_GET_XPOS(Touch_MeasureX());
				tTouchData.tLastEvent.usYpos = mTOUCH_GET_YPOS(Touch_MeasureY());
				break;
			}
			tTouchData.tLastEvent.eType = eEvent_Releasd;
			tTouchData.tLastEvent.usXpos = mTOUCH_GET_XPOS(Touch_MeasureX());
			tTouchData.tLastEvent.usYpos = mTOUCH_GET_YPOS(Touch_MeasureY());
			ucState=1;
			break;
		default:
			break;
	}
}

void LIB_Graph_Get_Event( TouchEvent_t *ptEvent )
{
	*ptEvent =tTouchData.tLastEvent;
	tTouchData.tLastEvent.eType = eEvent_none;
}

void SpiDelay(unsigned int DelayCnt)
{
	unsigned int i;
	for(i=0;i<DelayCnt;i++);
}

u16 TPReadX(void)
{
   u16 x=0;

   DRV_Gpio_ValueSet(tTouchData.hCsn,0);

   while(DRV_Gpio_ValueGet(tTouchData.hBusy))
	   ;

   SpiDelay(10);
   DRV_Spi_RW_Byte(tTouchData.hSpi , 0x90 );
   SpiDelay(10);
   x=DRV_Spi_RW_Byte(tTouchData.hSpi , 0x00 );
   x<<=8;
   x+=DRV_Spi_RW_Byte(tTouchData.hSpi , 0x00 );
   SpiDelay(10);
   DRV_Gpio_ValueSet(tTouchData.hCsn,1);
   x = x>>3;
   return (x);
}

u16 TPReadY(void)
{
 u16 y=0;

  DRV_Gpio_ValueSet(tTouchData.hCsn,0);
  SpiDelay(10);

  while(DRV_Gpio_ValueGet(tTouchData.hBusy))
  	   ;

  DRV_Spi_RW_Byte(tTouchData.hSpi , 0xD0 );
  SpiDelay(10);
  y=DRV_Spi_RW_Byte(tTouchData.hSpi , 0x00 );
  y<<=8;
  y+=DRV_Spi_RW_Byte(tTouchData.hSpi , 0x00 );
  SpiDelay(10);
  DRV_Gpio_ValueSet(tTouchData.hCsn,1);
  y = y>>3;
  return (y);
}


static uint16_t Touch_IsPenDown( void )
{
	unsigned char ucPioState;
	ucPioState = DRV_Gpio_ValueGet(tTouchData.hIrq);

	return !ucPioState;
}

static uint16_t Touch_MeasureX(void)
{
	unsigned char t=0,t1,count=0;
	unsigned short int databuffer[10]={5,7,9,3,2,6,4,0,3,1};//ÊýŸÝ×é
	uint16_t temp=0,X=0;

	while(count<10)//Ñ­»·¶ÁÊý10ŽÎ
	{
		databuffer[count]=TPReadX();
		count++;
	}
	if(count==10)//Ò»¶šÒª¶Áµœ10ŽÎÊýŸÝ,·ñÔò¶ªÆú
	{
	    do//œ«ÊýŸÝXÉýÐòÅÅÁÐ
		{
			t1=0;
			for(t=0;t<count-1;t++)
			{
				if(databuffer[t]>databuffer[t+1])//ÉýÐòÅÅÁÐ
				{
					temp=databuffer[t+1];
					databuffer[t+1]=databuffer[t];
					databuffer[t]=temp;
					t1=1;
				}
			}
		}while(t1);
		X=(databuffer[3]+databuffer[4]+databuffer[5])/3;
	}
	return(X);
}

static uint16_t Touch_MeasureY(void)
{
  	unsigned char t=0,t1,count=0;
	unsigned short int databuffer[10]={5,7,9,3,2,6,4,0,3,1};//ÊýŸÝ×é
	uint16_t temp=0,Y=0;

    while(count<10)	//Ñ­»·¶ÁÊý10ŽÎ
	{
		databuffer[count]=TPReadY();
		count++;
	}
	if(count==10)//Ò»¶šÒª¶Áµœ10ŽÎÊýŸÝ,·ñÔò¶ªÆú
	{
	    do//œ«ÊýŸÝXÉýÐòÅÅÁÐ
		{
			t1=0;
			for(t=0;t<count-1;t++)
			{
				if(databuffer[t]>databuffer[t+1])//ÉýÐòÅÅÁÐ
				{
					temp=databuffer[t+1];
					databuffer[t+1]=databuffer[t];
					databuffer[t]=temp;
					t1=1;
				}
			}
		}while(t1);
		Y=(databuffer[3]+databuffer[4]+databuffer[5])/3;
	}
	return(Y);
}
