#include <stdlib.h>
#include "libTimer.h"
#include "cfg/cfg_libTime/libTimerConfig.h"

static struct
{
	int iHandlerCount;
	tPeriodicTimer **ptTimerArray;
} tlibTimerData;


/// \brief init the lib
void vFlibTimer_Iinit( void )
{
	tlibTimerData.iHandlerCount = 0;
	tlibTimerData.ptTimerArray = ( tPeriodicTimer** ) malloc(sizeof(tPeriodicTimer*));
}

/// \brief Timer Tick
void vFlibTimer_Tick(void)
{
	unsigned char ucTimerCount;
	tPeriodicTimer *pTimer;
	
	for( ucTimerCount = 0 ; ucTimerCount < tlibTimerData.iHandlerCount ; ucTimerCount++ )
	{
		
		pTimer = tlibTimerData.ptTimerArray[ucTimerCount]; //newt timer
		pTimer->uiCount--; //decrement the timer count
		if(!pTimer->uiCount)
		{	//new periodextern 
			pTimer->uiCount = pTimer->uiPeriod;
			pTimer->uiPeriodCount++;
		}
	}
}

/// \brief Creat a new periodic Timer
/// \param uiPeiod period timer in ms
/// \return Pointer to the new timer
tPeriodicTimer *ptFlibTimer_NewPeriodicTimer( unsigned int uiPeriod )
{
	tPeriodicTimer *pNewTimer=NULL;
	
	if( tlibTimerData.iHandlerCount < LIBTIMER_MAX_TIMER_NUMBER )
	{
		//allocate memory for the new timer
		pNewTimer = (tPeriodicTimer*) malloc( sizeof( tPeriodicTimer)  );
		if( pNewTimer != NULL)
		{
			//add the new timer in the timer array
			if( realloc( tlibTimerData.ptTimerArray , (tlibTimerData.iHandlerCount+1)*sizeof(tPeriodicTimer*)) != NULL )
			{
				tlibTimerData.ptTimerArray[tlibTimerData.iHandlerCount] = pNewTimer;
				//update handler count
				kLIBTIMER_DISABLE_IT();
				tlibTimerData.iHandlerCount++;
				kLIBTIMER_ENABLE_IT();
				//init the new timer data
				pNewTimer->iHanler = tlibTimerData.iHandlerCount;
				pNewTimer->uiPeriod = uiPeriod;
				pNewTimer->uiPeriodCount= 0;
				pNewTimer->uiCount = uiPeriod;
			}
			else
			{
				free( pNewTimer );
				pNewTimer = NULL;
			}
		}
	}
	return pNewTimer;
}

void vFlibTimer_DeletPeriodicTimer( tPeriodicTimer *ptTimer)
{
	free( ptTimer );
	tlibTimerData.iHandlerCount--;
	realloc( tlibTimerData.ptTimerArray , (tlibTimerData.iHandlerCount)*sizeof(tPeriodicTimer*));
}

unsigned int  vFlibTimer_GetCount(tPeriodicTimer *ptTimer )
{
	unsigned int uiLocalCount=0;
	
	if( ptTimer->uiPeriodCount )
	{
		kLIBTIMER_DISABLE_IT();
		uiLocalCount = ptTimer->uiPeriodCount;
		ptTimer->uiPeriodCount = 0;
		kLIBTIMER_ENABLE_IT();
	}
	
	return uiLocalCount;
}
