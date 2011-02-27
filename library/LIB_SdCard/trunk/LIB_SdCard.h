#include <stdint.h>

/* Generic command */
#define CTRL_SYNC			0	/* Mandatory for write functions */
#define GET_SECTOR_COUNT	1	/* Mandatory for only f_mkfs() */
#define GET_SECTOR_SIZE		2
#define GET_BLOCK_SIZE		3	/* Mandatory for only f_mkfs() */
#define CTRL_POWER			4
#define CTRL_LOCK			5
#define CTRL_EJECT			6
/* MMC/SDC command */
#define MMC_GET_TYPE		10
#define MMC_GET_CSD			11
#define MMC_GET_CID			12
#define MMC_GET_OCR			13
#define MMC_GET_SDSTAT		14
/* ATA/CF command */
#define ATA_GET_REV			20
#define ATA_GET_MODEL		21
#define ATA_GET_SN			22

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
