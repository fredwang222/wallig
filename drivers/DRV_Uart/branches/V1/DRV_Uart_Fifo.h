#include "LIB_Fifo.h"
DRV_Uart_Error DRV_Uart_Fifo_Open( const char * pcDeviceName , DRV_Uart_Handle *phDeviceHandle , DRV_Uart_Cfg *ptSettings , LIB_Fifo_Type *pRX_Fifo , LIB_Fifo_Type *pTX_Fifo );
DRV_Uart_Error DRV_Uart_Fifo_Push( DRV_Uart_Handle hDeviceHandle ,unsigned  char *pucBuffer );
DRV_Uart_Error DRV_Uart_Fifo_Pop( DRV_Uart_Handle hDeviceHandle ,unsigned  char *pucBuffer );
int  DRV_Uart_Fifo_TX_Full( DRV_Uart_Handle hDeviceHandle );
int  DRV_Uart_Fifo_RX_Epmty( DRV_Uart_Handle hDeviceHandle );
