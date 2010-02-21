/**
	\file DRV_SoftTimer_Linux_test.c

	\author Gwendal Le Gall

	\date 02/02/2010
*/
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <DRV_SoftTimer.h>

void *print_message_function( void *ptr );

int main()
{
    DRV_SoftTimer_Cfg sParam;
    DRV_SoftTimer_Handle hTimer1=NULL;

    int icount;

    DRV_SoftTimer_Init();

    sParam.tType=TimeOut;
    sParam.uiValue = 10;
    if( DRV_SoftTimer_Open("timer1",&hTimer1,&sParam)==SOFTTIMER_No_Error)
    {
      DRV_SoftTimer_Start(hTimer1);
      for (icount=0;icount< 20;icount++)
      {
        while( DRV_SoftTimer_GetValue(hTimer1) )
          usleep(1000*10);
        printf("Timeout!\n");
        DRV_SoftTimer_Reset(hTimer1);
      }
      DRV_SoftTimer_Close(hTimer1);
    }
    else
      printf("error at opening");

    DRV_SoftTimer_Terminate();
    exit(0);
}
