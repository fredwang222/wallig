typedef unsigned int  DRV_Time_TimeOut;
typedef unsigned int  DRV_Time_Count;
typedef void * DRV_Timer_PeriodicTimer;

typedef enum
{
	Timer_No_Error,    //!< OK
	Timer_Failed,      //!< generic error
	Timer_Device_Not_Found,
	Timer_AlreadyOpened,
	Timer_Init_Error,
	Timer_Bad_Param,
	Timer_Err_Unknown      //!< unexpected error
} DRV_Timer_Error;

typedef enum
{
	Timer_Device_Close,
	Timer_Device_Open,
} DRV_Timer_Device_State;

typedef struct
{
	unsigned short usPeriod;
	void (*CallBack)(void);
} DRV_Timer_Periodic_Cfg;

#define  mDRV_TIME_TIMEOUT_INIT( TimeOut , uiValue)  TimeOut =DRV_Timer_SysClock_Get()+uiValue
#define  mDRV_TIME_TIMEOUT_END( TimeOut ) (TimeOut<DRV_Timer_SysClock_Get())
#define  mDRV_TIME_COUNT_INIT( Count ) Count=DRV_Timer_SysClock_Get()
#define  mDRV_TIME_COUNT_GET( Count ) (DRV_Timer_SysClock_Get() - Count)
void DRV_Timer_SysClock_Init( void );
unsigned int DRV_Timer_SysClock_Get( void );
void DRV_Timer_Periodic_Init( void );
DRV_Timer_Error DRV_Timer_Periodic_Open( char *pcName , DRV_Timer_PeriodicTimer *pHandle,DRV_Timer_Periodic_Cfg *pCfg );
DRV_Timer_Error DRV_Timer_Periodic_Start(  DRV_Timer_PeriodicTimer Handle );
DRV_Timer_Error DRV_Timer_Periodic_Stop(  DRV_Timer_PeriodicTimer Handle );
