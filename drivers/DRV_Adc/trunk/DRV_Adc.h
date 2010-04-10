#include "DRV_Adc_Cfg.h"
typedef void * DRV_Adc_Handle;

typedef enum
{
	Adc_No_Error,    //!< OK
	Adc_Failed,      //!< generic error
	Adc_Device_Not_Found,
	Adc_AlreadyOpened,
	Adc_Init_Error,
	Adc_Bad_Param,
	Adc_Err_Unknown      //!< unexpected error
} DRV_Adc_Error;

typedef enum
{
	Adc_Device_Close,
	Adc_Device_Open,
} DRV_Adc_Device_State;

void DRV_Adc_Init(void );
DRV_Adc_Error DRV_Adc_Open( const char *pcName , DRV_Adc_Handle *pHandle);
void DRV_Adc_Close( DRV_Adc_Handle Handle);
unsigned short DRV_Adc_Read( DRV_Adc_Handle Handle );

