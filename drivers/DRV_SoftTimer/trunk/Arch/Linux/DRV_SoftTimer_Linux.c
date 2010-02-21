/**
	\file DRV_SoftTimer_Linux.c

	\author Gwendal Le Gall

	\date 02/02/2010

	\section SoftTimerLinux Linux implementation of the main timer

	This implementation is a simple thread which use usleep()

*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <DRV_SoftTimer.h>
#include "Inc/DRV_SoftTimer_private.h"

/**************************************************************
                                        Local variables
***************************************************************/
static pthread_t MainTimerThread;
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;

/**************************************************************
                 private Functions
***************************************************************/
void *MaintTimer( void * arg );

void DRV_SoftTimer_SafeEnter( void )
{
  pthread_mutex_lock( &mutex1 );
}
void DRV_SoftTimer_SafeLeave( void )
{
  pthread_mutex_unlock( &mutex1 );

}

DRV_SoftTimer_Error DRV_SofTimer_MainInit(void )
{
  if( pthread_create( &MainTimerThread, NULL, MaintTimer, NULL ) < 0 )
    return SOFTTIMER_Failed;

  return SOFTTIMER_No_Error;
}

DRV_SoftTimer_Error DRV_SofTimer_MainTerminate(void )
{
    void *ret;

    //kill the main timer thread
    if (pthread_cancel (MainTimerThread) != 0)
    {
      fprintf (stderr, "pthread_cancel error for thread 1\n");
      return SOFTTIMER_Failed;
    }

     //wait the end of the called thread
    (void)pthread_join (MainTimerThread, &ret);

    return SOFTTIMER_No_Error;
}

void *MaintTimer( void * arg )
{
    //Allow the application to kill the thread
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);

    while(1)
    {
      usleep(1000*100);
      //test the end of the thread
      pthread_testcancel ();
      pthread_mutex_lock( &mutex1 );
      DRV_Softimer_TimerCallBack();
      pthread_mutex_unlock( &mutex1 );


    }

}
