
#include "DRV_Spi_Cfg.h"
typedef void * DRV_Spi_Handle;

#define DRV_SPI_SLOW 0
#define DRV_SPI_FAST 1

typedef enum
{
	Spi_No_Error,    //!< OK
	Spi_Failed,      //!< generic error
	Spi_Device_Not_Found,
	Spi_AlreadyOpened,
	Spi_Init_Error,
	Spi_Bad_Param,
	Spi_Err_Unknown      //!< unexpected error
} DRV_Spi_Error;

typedef enum
{
	Spi_Device_Close,
	Spi_Device_Open,
} DRV_Spi_Device_State;

void DRV_Spi_Init(void );
DRV_Spi_Error DRV_Spi_Open( char *pcName , DRV_Spi_Handle *pHandle);
void DRV_Spi_Close( DRV_Spi_Handle Handle);
DRV_Spi_Error DRV_Spi_Speed_Set( DRV_Spi_Handle Handle , uint16_t u16Speed);
unsigned short DRV_Spi_RW_Byte( DRV_Spi_Handle Handle , unsigned char ucOut);
unsigned short DRV_Spi_Read_Buffer( DRV_Spi_Handle Handle , unsigned char *pcBuffer , unsigned int uiLen);
unsigned short DRV_Spi_Write_Buffer( DRV_Spi_Handle Handle , unsigned char *pcBuffer , unsigned int uiLen);

