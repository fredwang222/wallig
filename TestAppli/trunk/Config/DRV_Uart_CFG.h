#ifdef ARCH_LINUX
#define kDRV_Uart_RXBuffSize 512
#define kDRV_Uart_TXBuffSize 512
#define kDRV_Uart_DeviceNameList "/dev/ttyUSB0","COM2"
#define kDRV_Uart_Devcdi_Count 2
#endif

#ifdef ARCH_STM32
#define kDRV_Uart_USE_FIFO 1
#define kDRV_Uart_RXBuffSize 512
#define kDRV_Uart_TXBuffSize 512
#define kDRV_Uart_DeviceNameList "UART1","UART2"
#endif
