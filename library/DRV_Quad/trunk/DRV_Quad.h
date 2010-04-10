#include "DRV_Quad_Cfg.h"
typedef void * DRV_Quad_Handle;

typedef enum
{
	Quad_No_Error,    //!< OK
	Quad_Failed,      //!< generic error
	Quad_Device_Not_Found,
	Quad_AlreadyOpened,
	Quad_Init_Error,
	Quad_Bad_Param,
	Quad_Err_Unknown      //!< unexpected error
} DRV_Quad_Error;

typedef enum
{
	Quad_Device_Close,
	Quad_Device_Open,
} DRV_Quad_Device_State;

void DRV_Quad_Init(void );
DRV_Quad_Error DRV_Quad_Open( char *pcName , DRV_Quad_Handle *pHandle);
void DRV_Quad_Close( DRV_Quad_Handle Handle);
DRV_Quad_Error DRV_Quad_Position_Get( DRV_Quad_Handle Handle , int *piPosition);
DRV_Quad_Error DRV_Quad_Position_Reset( DRV_Quad_Handle Handle );


