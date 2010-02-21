/**
	\file DRV_SoftTimer.c

	\author Gwendal Le Gall

	\date 02/02/2010
*/
#include <stdio.h>
#include <string.h>
#include <DRV_SoftTimer.h>
#include "Inc/DRV_SoftTimer_private.h"
/**************************************************************
					Define
***************************************************************/
#define kDRV_SofTimer_MaxDevices 5
/**************************************************************
					Local variables
***************************************************************/
/*!
 * 	Timers Data
 */
static struct
{
    DRV_Softimer_Devicedata tsSoftTimers[kDRV_SofTimer_MaxDevices];
    unsigned char ucTimerCount;
} sDRV_SoftTimer_MainData;

/**************************************************************
                 local Functions declaration
***************************************************************/
static void DRV_SoftTimer_ResetValues( DRV_Softimer_Devicedata *pTimer );

/**************************************************************
		 Public Functions
***************************************************************/
DRV_SoftTimer_Error DRV_SoftTimer_Init( void )
{
        unsigned char ucTimerIndex;

        //init timers
        memset(sDRV_SoftTimer_MainData.tsSoftTimers , 0 , sizeof(sDRV_SoftTimer_MainData.tsSoftTimers));
        for( ucTimerIndex=0 ; ucTimerIndex < kDRV_SofTimer_MaxDevices; ucTimerIndex++)
        {
            sDRV_SoftTimer_MainData.tsSoftTimers[ucTimerIndex].eState = Closed;
        }
        //reset the number of used timers
        sDRV_SoftTimer_MainData.ucTimerCount=0;
        //Architecture dependent init of the main timer
        DRV_SofTimer_MainInit();

	return SOFTTIMER_No_Error;
}

DRV_SoftTimer_Error DRV_SoftTimer_Terminate( void )
{
	unsigned char ucTimerIndex;

	//Stop the main timer
	DRV_SofTimer_MainTerminate();

        //init timers
        memset(sDRV_SoftTimer_MainData.tsSoftTimers , 0 , sizeof(sDRV_SoftTimer_MainData.tsSoftTimers));
        for( ucTimerIndex=0 ; ucTimerIndex < kDRV_SofTimer_MaxDevices; ucTimerIndex++)
        {
            sDRV_SoftTimer_MainData.tsSoftTimers[ucTimerIndex].eState = Closed;
        }
        //reset the number of used timers
        sDRV_SoftTimer_MainData.ucTimerCount=0;



	return SOFTTIMER_No_Error;
}

DRV_SoftTimer_Error DRV_SoftTimer_Open( const char * pcDeviceName , DRV_SoftTimer_Handle *phDeviceHandle , DRV_SoftTimer_Cfg *ptParam)
{
        DRV_Softimer_Devicedata *pTimer;

        if( sDRV_SoftTimer_MainData.ucTimerCount >= kDRV_SofTimer_MaxDevices )
          return SOFTTIMER_Failed;

        //Get the new timer pointer
        pTimer= &sDRV_SoftTimer_MainData.tsSoftTimers[sDRV_SoftTimer_MainData.ucTimerCount];
        //Set the conf parameters
        pTimer->cfg = *ptParam;
        pTimer->eState =  stopped;
        DRV_SoftTimer_ResetValues( pTimer );
        DRV_SoftTimer_SafeEnter();
        //update the number of opened timer
        sDRV_SoftTimer_MainData.ucTimerCount++;
        DRV_SoftTimer_SafeLeave();

        *phDeviceHandle = (DRV_SoftTimer_Handle) pTimer;
	return SOFTTIMER_No_Error;
}

DRV_SoftTimer_Error DRV_SoftTimer_Close( DRV_SoftTimer_Handle hDeviceHandle )
{
        DRV_Softimer_Devicedata *pTimer = (DRV_Softimer_Devicedata *) hDeviceHandle;

        if( pTimer == NULL )
           return SOFTTIMER_Input_Null;

        DRV_SoftTimer_SafeEnter();
        pTimer->eState = Closed;
        DRV_SoftTimer_SafeLeave();

	return SOFTTIMER_No_Error;
}

DRV_SoftTimer_Error DRV_SoftTimer_Start( DRV_SoftTimer_Handle hDeviceHandle )
{
        DRV_Softimer_Devicedata *pTimer = (DRV_Softimer_Devicedata *) hDeviceHandle;

        if( pTimer == NULL )
          return SOFTTIMER_Input_Null;

        if( pTimer->eState != Closed )
        {
            DRV_SoftTimer_SafeEnter();
            pTimer->eState =  started;
            DRV_SoftTimer_SafeLeave();
        }
        return SOFTTIMER_No_Error;

}

DRV_SoftTimer_Error DRV_SoftTimer_Stop( DRV_SoftTimer_Handle hDeviceHandle )
{
        DRV_Softimer_Devicedata *pTimer = (DRV_Softimer_Devicedata *) hDeviceHandle;

        if( pTimer == NULL )
          return SOFTTIMER_Input_Null;

        if( pTimer->eState != Closed )
        {
            DRV_SoftTimer_SafeEnter();
            pTimer->eState =  stopped;
            DRV_SoftTimer_SafeLeave();
        }
        return SOFTTIMER_No_Error;
}

unsigned int DRV_SoftTimer_GetValue( DRV_SoftTimer_Handle hDeviceHandle )
{
        DRV_Softimer_Devicedata *pTimer = (DRV_Softimer_Devicedata *) hDeviceHandle;
        unsigned int uiReturnedValue;

        if( pTimer == NULL )
          return 0;
        if( pTimer->eState != Closed )
        {
            DRV_SoftTimer_SafeEnter();
            uiReturnedValue = pTimer->uiReturnedValue;
            DRV_SoftTimer_SafeLeave();
        }
        return  uiReturnedValue;
}

void DRV_SoftTimer_Reset( DRV_SoftTimer_Handle hDeviceHandle )
{
        DRV_Softimer_Devicedata *pTimer = (DRV_Softimer_Devicedata *) hDeviceHandle;

        if( pTimer == NULL )
          return ;

        if( pTimer->eState != Closed )
        {
            DRV_SoftTimer_SafeEnter();
            DRV_SoftTimer_ResetValues( pTimer );
            DRV_SoftTimer_SafeLeave();
        }

}

/**************************************************************
                 private Functions
***************************************************************/

void DRV_Softimer_TimerCallBack( void )
{
	unsigned char ucTimerIndex;

	//update timers
	for( ucTimerIndex=0 ; ucTimerIndex < kDRV_SofTimer_MaxDevices; ucTimerIndex++)
	{

		if( sDRV_SoftTimer_MainData.tsSoftTimers[ucTimerIndex].eState == started )
		{
			switch( sDRV_SoftTimer_MainData.tsSoftTimers[ucTimerIndex].cfg.tType )
			{
                                case Counter:
                                  if( ( ++sDRV_SoftTimer_MainData.tsSoftTimers[ucTimerIndex].uiCountValue) >= sDRV_SoftTimer_MainData.tsSoftTimers[ucTimerIndex].cfg.uiValue)
                                    sDRV_SoftTimer_MainData.tsSoftTimers[ucTimerIndex].uiReturnedValue++;
                                  break;
                                case TimeOut:
                                  if(sDRV_SoftTimer_MainData.tsSoftTimers[ucTimerIndex].uiCountValue)
                                  {
                                      sDRV_SoftTimer_MainData.tsSoftTimers[ucTimerIndex].uiCountValue--;
                                      sDRV_SoftTimer_MainData.tsSoftTimers[ucTimerIndex].uiReturnedValue = sDRV_SoftTimer_MainData.tsSoftTimers[ucTimerIndex].uiCountValue;
                                  }
                                  break;
                                case Periodic:
                                  break;
                                default:
                                  break;
			}
		}
	}
}

/**
 * \brief Reset the timers values ( user and internal )
 */
static void DRV_SoftTimer_ResetValues( DRV_Softimer_Devicedata *pTimer )
{
    pTimer->uiCountValue = pTimer->cfg.uiValue ;
    if( pTimer->cfg.tType !=  TimeOut )
      pTimer->uiReturnedValue = 0;
    else
      pTimer->uiReturnedValue = pTimer->cfg.uiValue ;
}

