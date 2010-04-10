#include <stdint.h>

typedef enum
{
	SdCard_No_Error,    //!< OK
	SdCard_Failed,      //!< generic error
	SdCard_TimeOut,
	SdCard_Init_Error,
	SdCard_Bad_Param,
	SdCard_No_Card,
	SdCard_Not_Ready,
	SdCard_WriteProtected,
	SdCard_Err_Unknown      //!< unexpected error
} LIB_SdCard_Error;

LIB_SdCard_Error LIB_SdCard_Init( void );
LIB_SdCard_Error LIB_SdCard_Disk_initialize (	uint8_t ucDrv );
LIB_SdCard_Error LIB_SdCard_Disk_read (uint8_t ucDrv, uint8_t *pucBuff, uint32_t uiSector,uint8_t ucCount);
LIB_SdCard_Error LIB_SdCard_Disk_write (uint8_t ucDrv, uint8_t *pucBuff, uint32_t uiSector,	 uint8_t ucCount );
LIB_SdCard_Error LIB_SdCard_Disk_ioctl ( uint8_t ucDrv,	 uint8_t ctrl, void *pBuff );
