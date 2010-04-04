typedef unsigned int  DRV_Time_TimeOut;
typedef unsigned int  DRV_Time_Count;

#define  mDRV_TIME_TIMEOUT_INIT( TimeOut , uiValue)  TimeOut =DRV_Timer_SysClock_Get()+uiValue
#define  mDRV_TIME_TIMEOUT_END( TimeOut ) (TimeOut<DRV_Timer_SysClock_Get())
#define  mDRV_TIME_COUNT_INIT( Count ) Count=DRV_Timer_SysClock_Get()
#define  mDRV_TIME_COUNT_GET( Count ) (DRV_Timer_SysClock_Get() - Count)
void DRV_Timer_SysClock_Init( void );
unsigned int DRV_Timer_SysClock_Get( void );
