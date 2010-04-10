#include "DRV_Pwm_Cfg.h"
typedef void * DRV_Pwm_Handle;

typedef enum
{
	Pwm_No_Error,    //!< OK
	Pwm_Failed,      //!< generic error
	Pwm_Device_Not_Found,
	Pwm_AlreadyOpened,
	Pwm_Init_Error,
	Pwm_Bad_Param,
	Pwm_Err_Unknown      //!< unexpected error
} DRV_Pwm_Error;

typedef enum
{
	Pwm_Device_Close,
	Pwm_Device_Open,
} DRV_Pwm_Device_State;

void DRV_Pwm_Init(void );
DRV_Pwm_Error DRV_Pwm_Open( const char *pcName , DRV_Pwm_Handle *pHandle ,const  unsigned short ucDutycycle);
void DRV_Pwm_Close( DRV_Pwm_Handle Handle);
void DRV_Pwm_Enable( DRV_Pwm_Handle Handle );
void DRV_Pwm_Disable( DRV_Pwm_Handle Handle );
DRV_Pwm_Error DRV_Pwm_DutyCycleSet(const DRV_Pwm_Handle Handle , const unsigned char ucDutycycle);

